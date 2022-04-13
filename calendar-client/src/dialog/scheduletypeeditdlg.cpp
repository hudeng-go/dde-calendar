/*
* Copyright (C) 2019 ~ 2019 UnionTech Software Technology Co.,Ltd.
*
* Author:     leilong <leilong@uniontech.com>
*
* Maintainer: leilong <leilong@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "scheduletypeeditdlg.h"
#include "scheduledatamanage.h"
#include "cscheduleoperation.h"
#include "configsettings.h"

#include <DFrame>
#include <DTitlebar>
#include <DIconButton>

#include <QLabel>
#include <QRadioButton>

ScheduleTypeEditDlg::ScheduleTypeEditDlg(QWidget *parent)
    : DDialog(parent)
    , m_title(tr("New event type"))
    , m_dialogType(DialogNewType)
{
    init();
}

ScheduleTypeEditDlg::ScheduleTypeEditDlg(const JobTypeInfo &jobTypeOld, QWidget *parent)
    : DDialog(parent)
    , m_jobTypeOld(jobTypeOld)
    , m_jobTypeNew(jobTypeOld)
    , m_title(tr("Edit event type"))
    , m_dialogType(DialogEditType)

{
    init();
}

void ScheduleTypeEditDlg::init()
{
    initView();
    initData();
    //默认焦点在日程名称输入框中
    m_lineEdit->setFocus();
    //先初始化数据再关联信号，初始状态下不提示
    connect(m_lineEdit, &DLineEdit::textChanged, this, &ScheduleTypeEditDlg::slotEditTextChanged);
}

void ScheduleTypeEditDlg::initView()
{
    setFixedSize(QSize(400, 220));

    m_titleLabel = new QLabel(this);
    QFont titlelabelF;
    titlelabelF.setWeight(QFont::DemiBold);
    titlelabelF.setPixelSize(17);
    m_titleLabel->setFixedSize(400, 25);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->move(0, 19);
    m_titleLabel->setFont(titlelabelF);

    QVBoxLayout *maintlayout = new QVBoxLayout;
    maintlayout->setMargin(0);
    maintlayout->setSpacing(10);

    QHBoxLayout *eLayout = new QHBoxLayout;
    QLabel *eName = new QLabel(tr("Name:"));
    eName->setFixedWidth(42);

    m_lineEdit = new DLineEdit();
    m_lineEdit->setClearButtonEnabled(false); //不显示按钮
    eLayout->addWidget(eName);
    eLayout->addWidget(m_lineEdit, 1);

    QHBoxLayout *cLayout = new QHBoxLayout;
    QLabel *cName = new QLabel(tr("Color:"));
    cName->setFixedWidth(42);
    m_colorSeletor = new ColorSeletorWidget();

    cLayout->addWidget(cName);
    cLayout->addWidget(m_colorSeletor);
    cLayout->addStretch(1);

    maintlayout->addLayout(eLayout);
    maintlayout->addLayout(cLayout);

    eLayout->setSpacing(10);
    eLayout->setMargin(0);
    cLayout->setSpacing(10);
    cLayout->setMargin(0);
    maintlayout->setSpacing(20);
    maintlayout->setContentsMargins(0, 10, 0, 0);

    DFrame *gwi = new DFrame(this);
    gwi->setFrameShape(QFrame::NoFrame);
    gwi->setLayout(maintlayout);
    gwi->setMinimumWidth(360);
    addContent(gwi, Qt::AlignCenter);

    //添加按钮
    addButton(tr("Cancel", "button"));
    addButton(tr("Save", "button"), false, DDialog::ButtonRecommend);
    for (int i = 0; i < buttonCount(); i++) {
        QAbstractButton *button = getButton(i);
        button->setFixedSize(170, 36);
    }
    connect(this->getButton(0), &QPushButton::clicked, this, &ScheduleTypeEditDlg::slotBtnCancel);
    connect(this->getButton(1), &QPushButton::clicked, this, &ScheduleTypeEditDlg::slotBtnNext);
}

void ScheduleTypeEditDlg::initData()
{
    m_titleLabel->setText(m_title);
    m_lineEdit->setText(m_jobTypeOld.getJobTypeName());
    this->getButton(1)->setEnabled(!m_jobTypeOld.getJobTypeName().isEmpty());//如果是新增，则保存按钮默认不可用
    switch (m_dialogType) {
    case DialogEditType: {
        //编辑日程类型
        m_colorSeletor->setSelectedColorByIndex(m_jobTypeOld.getColorInfo());
    } break;
    default: {
        //默认新建日程类型
        if (CConfigSettings::getInstance()->contains("LastUserColor")) {
            QString colorName = CConfigSettings::getInstance()->value("LastUserColor").toString();
            if (!colorName.isEmpty()) {
                m_colorSeletor->setUserColor(JobTypeColorInfo(0, colorName, 7));
            }
        }

        if (CConfigSettings::getInstance()->contains("LastSysColorTypeNo")) {
            int colorId = CConfigSettings::getInstance()->value("LastSysColorTypeNo").toInt();
            if (colorId > 0) {
                m_colorSeletor->setSelectedColorById(colorId);
            }
        }
    } break;
    }
}

void ScheduleTypeEditDlg::slotEditTextChanged(const QString &strName)
{
    //文本编辑框中文本改变事件
    //1不能为空，2不能全空格，3不能重名
    if (strName.isEmpty()) {
        //名称为空，返回
        m_lineEdit->showAlertMessage(tr("Enter a name please"));
        m_lineEdit->setAlert(true);
        this->getButton(1)->setEnabled(false);
        return;
    }
    if (strName.trimmed().isEmpty()) {
        //名称为全空格，返回
        m_lineEdit->showAlertMessage(tr("The name can not only contain whitespaces"));
        m_lineEdit->setAlert(true);
        this->getButton(1)->setEnabled(false);
        return;
    }
    m_jobTypeNew.setJobTypeName(strName);
    //在编辑日程状态下不对编辑的日程类型名做重名处理
    bool isUsed = m_dialogType == DialogEditType ? JobTypeInfoManager::instance()->isJobTypeNameUsed(m_jobTypeNew)
                                                 : JobTypeInfoManager::instance()->isJobTypeNameUsed(strName);

    if (isUsed) {
        //重名，返回
        m_lineEdit->showAlertMessage(tr("The name already exists"));
        m_lineEdit->setAlert(true);
        this->getButton(1)->setEnabled(false);
        return;
    }
    m_lineEdit->setAlert(false);
    this->getButton(1)->setEnabled(true);
    return;
}

void ScheduleTypeEditDlg::slotBtnCancel()
{
    this->close();
}

void ScheduleTypeEditDlg::slotBtnNext()
{
    //先修改颜色
    m_jobTypeNew.setColorInfo(m_colorSeletor->getSelectedColorInfo());

    CScheduleOperation so;
    //更新或创建日程类型
    so.updateJobType(m_jobTypeOld, m_jobTypeNew);
}

