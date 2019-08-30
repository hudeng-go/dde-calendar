/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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
#ifndef MYSCHCEDULEVIEW_H
#define MYSCHCEDULEVIEW_H

#include <QObject>
#include <QWidget>
#include <dlabel.h>
#include <ddialog.h>
#include <dpushbutton.h>
#include <dtextbutton.h>
#include "schedulestructs.h"
DWIDGET_USE_NAMESPACE

class CMySchceduleView : public DDialog
{
    Q_OBJECT
public:
    explicit CMySchceduleView(QWidget *parent = nullptr);
    void setSchedules(ScheduleInfo info);
    ScheduleInfo getSchedules()
    {
        return  m_scheduleInfo;
    }
signals:
    void signalsEditorDelete(int type = 0);
public slots:
    void slotEditBt();
    void slotDeleteBt();
private:
    void initUI();
    void initConnection();
private:
    DLabel                           *m_schceduleLabel = nullptr;
    DLabel                           *m_timeLabel = nullptr;
    DTextButton                      *m_editBt = nullptr;
    DTextButton                      *m_deleteBt = nullptr;
    ScheduleInfo                     m_scheduleInfo;//日程
};

#endif // MYSCHCEDULEVIEW_H
