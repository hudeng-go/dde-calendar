// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "jobtypelistview.h"
#include "cscheduleoperation.h"
#include "scheduletypeeditdlg.h"
#include "schedulectrldlg.h"
#include "accountmanager.h"

#include <DHiDPIHelper>
#include <DStyle>
#include <DIconButton>
#include <QToolTip>
#include <QPainter>
#include <QHeaderView>

//Qt::UserRole + 1,会影响item的高度
static const int RoleJobTypeInfo = Qt::UserRole + 2;
static const int RoleJobTypeEditable = Qt::UserRole + 3;
static const int RoleJobTypeLine = Qt::UserRole + 4;

JobTypeListView::JobTypeListView(QWidget *parent) : QTableView(parent)
{
    initUI();
}

JobTypeListView::~JobTypeListView()
{
//    JobTypeInfoManager::instance()->removeFromNoticeBill(this);
}

void JobTypeListView::initUI()
{
    m_modelJobType = new QStandardItemModel(this);
    setModel(m_modelJobType);
    setFrameStyle(QFrame::NoFrame);
    setEditTriggers(QListView::NoEditTriggers);
    setSelectionMode(QListView::NoSelection);
    setFocusPolicy(Qt::NoFocus);
    setItemDelegate(new JobTypeListViewStyle(this));
    setShowGrid(false);
    setMouseTracking(true);
    verticalHeader()->setMinimumSectionSize(0);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    //
    updateJobType();

    connect(gAccountManager, &AccountManager::signalScheduleTypeUpdate, this, &JobTypeListView::updateJobType);
    connect(this, &QTableView::entered, this, [&](const QModelIndex & index) {
        if (!index.isValid()) return;
        DScheduleType info = index.data(RoleJobTypeInfo).value<DScheduleType>();
        QString displayName = info.displayName();
        if (!displayName.isEmpty()) {
            QToolTip::showText(QCursor::pos(), info.displayName());
        }
    });
}

bool JobTypeListView::viewportEvent(QEvent *event)
{
    QTableView::viewportEvent(event);

    int indexCurrentHover;

    if (QEvent::HoverLeave == event->type()) {
        QStandardItemModel *itemModel = qobject_cast<QStandardItemModel *>(model());
        if (nullptr == itemModel) {
            return true;
        }
        DStandardItem *itemJobType = dynamic_cast<DStandardItem *>(itemModel->item(m_iIndexCurrentHover));

        if (nullptr == itemJobType) {
            return true;
        }
        //typedef QList<DViewItemAction *> DViewItemActionList;
        for (DViewItemAction *a : itemJobType->actionList(Qt::Edge::RightEdge)) {
            a->setVisible(false);
        }
        m_iIndexCurrentHover = -1;
    } else if (QEvent::HoverEnter == event->type() || QEvent::HoverMove == event->type()) {
        QStandardItemModel *itemModel = qobject_cast<QStandardItemModel *>(model());
        if (nullptr == itemModel) {
            return true;
        }
        indexCurrentHover = indexAt(static_cast<QHoverEvent *>(event)->pos()).row();

        if (indexCurrentHover != m_iIndexCurrentHover) {
            DStandardItem *itemJobType;

            //隐藏此前鼠标悬浮行的图标
            if (m_iIndexCurrentHover >= 0) {
                itemJobType = dynamic_cast<DStandardItem *>(m_modelJobType->item(m_iIndexCurrentHover));
                if (nullptr != itemJobType) {
                    for (DViewItemAction *a : itemJobType->actionList(Qt::Edge::RightEdge)) {
                        a->setVisible(false);
                    }
                }
                //typedef QList<DViewItemAction *> DViewItemActionList;

            }

            if (indexCurrentHover < 0) {
                return true;
            }
            //展示此前鼠标悬浮行的图标
            m_iIndexCurrentHover = indexCurrentHover;
            itemJobType = dynamic_cast<DStandardItem *>(m_modelJobType->item(m_iIndexCurrentHover));
            if (nullptr == itemJobType) {
                return true;
            }
            if (itemJobType->actionList(Qt::Edge::RightEdge).size() > 0) {
                for (DViewItemAction *a : itemJobType->actionList(Qt::Edge::RightEdge)) {
                    a->setVisible(true);
                }
            } else {
                // 设置其他style时，转换指针为空
                if (DStyle *ds = qobject_cast<DStyle *>(style())) {
                    Q_UNUSED(ds)
                    if (!itemJobType->data(RoleJobTypeEditable).toBool())
                        return true;
                    auto actionEdit = new DViewItemAction(Qt::AlignVCenter, QSize(20, 20), QSize(20, 20), true);
                    actionEdit->setIcon(DHiDPIHelper::loadNxPixmap(":/icons/deepin/builtin/icons/dde_calendar_edit_32px.svg"));
                    actionEdit->setParent(this);
                    connect(actionEdit, &QAction::triggered, this, &JobTypeListView::slotUpdateJobType);

                    auto actionDelete = new DViewItemAction(Qt::AlignVCenter, QSize(20, 20), QSize(20, 20), true);
                    actionDelete->setIcon(DHiDPIHelper::loadNxPixmap(":/icons/deepin/builtin/icons/dde_calendar_delete_32px.svg"));
                    actionDelete->setParent(this);
                    connect(actionDelete, &QAction::triggered, this, &JobTypeListView::slotDeleteJobType);

                    itemJobType->setActionList(Qt::Edge::RightEdge, {actionEdit, actionDelete});
                }
            }
        }
    }
    return true;
}
bool JobTypeListView::updateJobType()
{
    AccountItem::Ptr account = gAccountManager->getAccountItemByAccountId(m_account_id);
    if (!account)
        return false;
    m_modelJobType->removeRows(0, m_modelJobType->rowCount());//先清理
    m_iIndexCurrentHover = -1;

    DScheduleType::List lstJobType = account->getScheduleTypeList();
    int viewHeight = 0;
    for (int i = 0; i < lstJobType.size(); i++) {
        viewHeight += addJobTypeItem(*lstJobType[i]);
    }

    setFixedHeight(viewHeight);
    emit signalAddStatusChanged(canAdd());
    return true;
}

void JobTypeListView::updateCalendarAccount(QString account_id)
{
    m_account_id = account_id;
    updateJobType();
}

void JobTypeListView::slotAddScheduleType()
{
    AccountItem::Ptr account = gAccountManager->getAccountItemByAccountId(m_account_id);
    if (!account)
        return;

    ScheduleTypeEditDlg dialog(this);
    dialog.setAccount(account);
    //按保存键退出则触发保存数据
    if (QDialog::Accepted == dialog.exec()) {
        DScheduleType::Ptr type(new DScheduleType(dialog.newJsonType()));
        account->createJobType(type);
    }
}

bool JobTypeListView::canAdd()
{
    AccountItem::Ptr account = gAccountManager->getAccountItemByAccountId(m_account_id);
    if (!account)
        return false;

    //最多20个类型
    return account->getScheduleTypeList().count() < 20;
}

void JobTypeListView::setItemEnabled(bool b)
{
    if (!m_modelJobType) return;

    for (int i = 0; i < m_modelJobType->rowCount(); ++i) {
        QStandardItem *item = m_modelJobType->item(i);
        item->setEnabled(b);
    }
}

int JobTypeListView::addJobTypeItem(const DScheduleType &info)
{
    int itemHeight = 0;
    DStandardItem *item = new DStandardItem;
    item->setData(QVariant::fromValue(info), RoleJobTypeInfo);
    //根据日程类型权限设置显示数据
    item->setData(info.privilege() > 1, RoleJobTypeEditable);
    item->setData(false, RoleJobTypeLine);

    //首个 非默认日程类型，前面 添加分割线
    if (m_modelJobType->rowCount() > 1
            && !m_modelJobType->item(m_modelJobType->rowCount() - 1)->data(RoleJobTypeEditable).toBool()
            && item->data(RoleJobTypeEditable).toBool()) {
        DStandardItem *itemLine = new DStandardItem;
        itemLine->setData(QVariant(), RoleJobTypeInfo);
        itemLine->setData(false, RoleJobTypeEditable);
        itemLine->setData(true, RoleJobTypeLine);
        m_modelJobType->appendRow(itemLine);
        setRowHeight(m_modelJobType->rowCount() - 1, 19);
        itemHeight += 19;
    }
    m_modelJobType->appendRow(item);
    setRowHeight(m_modelJobType->rowCount() - 1, 46);
    itemHeight += 46;

    return itemHeight;
}

void JobTypeListView::slotUpdateJobType()
{
    int index =  indexAt(mapFromGlobal(QCursor::pos())).row();
    if (index < 0 || index >= m_modelJobType->rowCount())
        return;
    QStandardItem *item = m_modelJobType->item(index);
    if (!item)
        return;
    AccountItem::Ptr account = gAccountManager->getAccountItemByAccountId(m_account_id);
    if (!account)
        return;

    DScheduleType info = item->data(RoleJobTypeInfo).value<DScheduleType>();
    ScheduleTypeEditDlg dialog(info, this);
    dialog.setAccount(account);
    if (QDialog::Accepted == dialog.exec()) {
        DScheduleType::Ptr type(new DScheduleType(dialog.newJsonType()));
        if (type->typeID() == "0") {
            account->createJobType(type);
        } else {
            account->updateScheduleType(type);
        }
    }
}

void JobTypeListView::slotDeleteJobType()
{
    DStandardItem *item = dynamic_cast<DStandardItem *>(m_modelJobType->item(m_iIndexCurrentHover));
    if (!item)
        return;
    AccountItem::Ptr account = gAccountManager->getAccountItemByAccountId(m_account_id);
    if (!account)
        return;

    DScheduleType info = item->data(RoleJobTypeInfo).value<DScheduleType>();
    //TODO:获取日程编号
    QString typeNo = info.typeID();

    //TODO:根据帐户获取对应信息
    if (account->scheduleTypeIsUsed(typeNo)) {
        CScheduleCtrlDlg msgBox(this);
        msgBox.setText(tr("You are deleting an event type."));
        msgBox.setInformativeText(tr("All events under this type will be deleted and cannot be recovered."));
        msgBox.addPushButton(tr("Cancel", "button"), true);
        msgBox.addWaringButton(tr("Delete", "button"), true);
        msgBox.exec();
        if (msgBox.clickButton() == 0) {
            return;
        } else if (msgBox.clickButton() == 1) {
            //删除日程类型时，后端会删除关联日程
            account->deleteScheduleTypeByID(typeNo);
        }
    } else {
        account->deleteScheduleTypeByID(typeNo);
    }
}


void JobTypeListViewStyle::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;

    //draw line
    bool isDrawLine = index.data(RoleJobTypeLine).toBool();
    if (isDrawLine) {
        int y = opt.rect.y() + opt.rect.height() / 2;
        int x = opt.rect.x();
        int w = x + opt.rect.width();
        painter->fillRect(x, y, w, 1, qApp->palette().color(QPalette::Button));
        return;
    }
    opt.rect.adjust(0, 5, 0, -5);
    DStyledItemDelegate::paint(painter, opt, index);
    DScheduleType info = index.data(RoleJobTypeInfo).value<DScheduleType>();

    //draw icon
    painter->save();
    painter->setPen(QPen(QColor(0, 0, 0, int(255 * 0.1)), 2));
    painter->setBrush(QColor(info.typeColor().colorCode()));
    painter->drawEllipse(QRect(opt.rect.x() + 12, opt.rect.y() + 10, 16, 16));

    //draw text

    painter->setPen(qApp->palette().color(QPalette::Text));
    QFontMetrics fontMetr(painter->font());

    //如果为焦点,且不为系统自带颜色
    if (opt.state & QStyle::State_HasFocus && !info.typeColor().isSysColorInfo()) {
    }

    JobTypeListView *view = qobject_cast<JobTypeListView *>(parent());
    //如果当前的index为hover状态则空出图标位置
    QString displayName = info.displayName();
    // 获取当前字体的信息
    QFontMetrics fontMetrics(opt.font);
    // 当前文字长度是否大于显示框长度
    if (fontMetrics.width(displayName) > (opt.rect.width() - 90)) {
        displayName = fontMetrics.elidedText(displayName, Qt::ElideRight, opt.rect.width() - 90); // 截取字符串长度用...代替
    }
    if (view && view->m_iIndexCurrentHover == index.row()) {
        painter->drawText(opt.rect.adjusted(38, 0, -10, 0), Qt::AlignVCenter | Qt::AlignLeft, displayName);
    } else {
        painter->drawText(opt.rect.adjusted(38, 0, -10, 0), Qt::AlignVCenter | Qt::AlignLeft, displayName);
    }

    painter->restore();
}


