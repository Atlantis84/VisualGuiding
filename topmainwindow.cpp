#include "topmainwindow.h"
#include "gdatafactory.h"
#include <qdesktopwidget.h>
#include <QApplication>
#include <QPainter>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QLabel>
#include <QToolButton>
#include "QsLog.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "imessagebox.h"
#include <QJsonArray>
#include <QTimer>
TopMainWindow::TopMainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() | Qt::Widget);
    setWindowIcon(QIcon(":/icon/24x24.png"));
    this->setStyleSheet("QMainWindow{background-color:#000000}");
    createActions();
    createDockWindows();
    createStatusBar();

    this->setCentralWidget(GDataFactory::get_top_central_wgt());
}

void TopMainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(QPen(QColor(0,0,0,100),2));
    painter.fillRect(this->rect(),QBrush(QColor(235,236,240)));
    painter.drawRect(this->rect());
}

void TopMainWindow::createActions()
{
    QWidget *wtSpace[10];
    for (int i=0;i<10;i++) {
        wtSpace[i] = new QWidget();
        wtSpace[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    }
    menuBar()->setStyleSheet("QMenuBar{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 rgb(235,236,240),stop:0.5 rgb(207,207,214),stop:1 rgb(235,236,240));}");
    QMenu *fileMenu = menuBar()->addMenu(u8"&用户操作");
    QToolBar *fileToolBar = addToolBar(u8"..");
    fileToolBar->setStyleSheet("QToolBar{border:solid rgba(0,0,0,150);background-color:rgb(235,236,240);}\
                               QToolButton{background-color:rgba(22,194,194,0);\
                               color:rgb(0,0,0);}");
    menuBar()->setFont(QFont (u8"KaiTi", 15, QFont::StyleNormal));
    fileMenu->setFont(QFont (u8"KaiTi", 15, QFont::StyleNormal));
    const QIcon userIcon = QIcon::fromTheme("document-new", QIcon(":/icon/user.png"));
    const QIcon startIcon = QIcon::fromTheme("",QIcon(":/icon/start.png"));
    const QIcon userManageIcon = QIcon::fromTheme("document-save", QIcon(":/icon/user_manage.png"));

    calibrationAct = new QAction(userIcon, u8"&视觉标定", this);
    startRunAct = new QAction(startIcon, u8"开始引导",this);
    stopRunAct = new QAction(userManageIcon, u8"&结束引导", this);
//    newLetterAct->setShortcuts(QKeySequence::New);
    calibrationAct->setStatusTip(u8"视觉标定");
    startRunAct->setStatusTip(u8"开始引导");
    stopRunAct->setStatusTip(u8"结束引导");
    connect(stopRunAct,SIGNAL(triggered()), this, SLOT(slot_stop_run()));
    connect(calibrationAct,SIGNAL(triggered()), this, SLOT(slot_calibration()));
    connect(startRunAct,SIGNAL(triggered()), this, SLOT(slot_start_run()));
    fileMenu->addAction(calibrationAct);
    fileMenu->addAction(startRunAct);
    fileMenu->addAction(stopRunAct);
    fileToolBar->addAction(calibrationAct);
//    fileToolBar->addWidget(wtSpace[0]);
    fileToolBar->addAction(startRunAct);
//    fileToolBar->addWidget(wtSpace[1]);
    fileToolBar->addAction(stopRunAct);
//    fileToolBar->addWidget(wtSpace[2]);
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    fileToolBar->setIconSize(QSize(50,50));

    fileMenu->addSeparator();

    QAction *quitAct = new QAction(u8"&退出",this);
    fileMenu->addAction(quitAct);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(u8"退出应用程序");
    connect(quitAct,SIGNAL(triggered()),this,SLOT(exitApp()));

    QMenu *orderMenu = menuBar()->addMenu(u8"&数据浏览");
    orderMenu->setFont(QFont (u8"KaiTi", 15, QFont::StyleNormal));
    const QIcon boardNumberIcon = QIcon::fromTheme("", QIcon(":/icon/order_load.ico"));
    const QIcon portSetIcon = QIcon::fromTheme("",QIcon(":/icon/order_edit.ico"));

    historyCalibrationAct = new QAction(boardNumberIcon,u8"历史标定",this);
    portSetAct = new QAction(portSetIcon,u8"参数配置",this);
    QLabel* hisenseLogo = new QLabel();
    hisenseLogo->setStyleSheet("QLabel{min-width:350px;min-height:100px;background-color:rgba(0,0,0,0);border:none;}");
    QPixmap *hisensePixmap = new QPixmap(":/icon/2.png");
    hisensePixmap->scaled(hisenseLogo->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
    hisenseLogo->setPixmap(*hisensePixmap);

    historyCalibrationAct->setStatusTip(u8"历史标定");
    connect(historyCalibrationAct,SIGNAL(triggered()), this, SLOT(slot_history_calibration()));
    portSetAct->setStatusTip(u8"参数配置");
    connect(portSetAct,SIGNAL(triggered()), this, SLOT(slot_para_set()));
    orderMenu->addAction(historyCalibrationAct);
    orderMenu->addAction(portSetAct);
//    fileToolBar->addSeparator();
    fileToolBar->addAction(historyCalibrationAct);
    fileToolBar->addAction(portSetAct);
    fileToolBar->addWidget(wtSpace[3]);
    fileToolBar->addWidget(hisenseLogo);

    viewMenu = menuBar()->addMenu(u8"&视图");
    viewMenu->setFont(QFont (u8"KaiTi", 15, QFont::StyleNormal));

    QMenu *helpMenu = menuBar()->addMenu(u8"&帮助");
    QAction *aboutAct = new QAction(u8"&关于",this);
    helpMenu->addAction(aboutAct);
    helpMenu->setFont(QFont (u8"KaiTi", 15, QFont::StyleNormal));
    aboutAct->setStatusTip(u8"显示应用程序关于对话框");
    connect(aboutAct,SIGNAL(triggered()),this,SLOT(about()));
}

void TopMainWindow::createDockWindows()
{
    //右侧显示
    QDockWidget *dock1 = new QDockWidget(u8"当前测试板信息", this);
    GDataFactory::get_top_right_wgt()->setParent(dock1);
    GDataFactory::get_top_right_wgt()->setFixedWidth(350);
    dock1->setWidget(GDataFactory::get_top_right_wgt());
    addDockWidget(Qt::RightDockWidgetArea, dock1);
    viewMenu->addAction(dock1->toggleViewAction());

    //底部信息显示
    QDockWidget *dock2 = new QDockWidget(u8"实时数据及日志", this);
//    QDockWidget *dock3 = new QDockWidget(u8"日志信息", this);
    dock2->setAllowedAreas(Qt::BottomDockWidgetArea);
    dock2->setAutoFillBackground(true);
//    dock3->setAllowedAreas(Qt::BottomDockWidgetArea);
//    dock3->setAutoFillBackground(true);
    GDataFactory::get_top_bottom_wgt()->setFixedHeight(250);
    dock2->setWidget(GDataFactory::get_top_bottom_wgt());
//    dock3->setWidget(GDataFactory::GetBottomEventWidget());
    addDockWidget(Qt::BottomDockWidgetArea,dock2);
//    addDockWidget(Qt::BottomDockWidgetArea,dock3);
    viewMenu->addAction(dock2->toggleViewAction());
//    viewMenu->addAction(dock3->toggleViewAction());
}

void TopMainWindow::createStatusBar()
{
    QFont font("LiSu", 18, QFont::StyleNormal);
    statusBar()->setFont(font);
    statusBar()->setStyleSheet("QStatusBar{color:rgb(0,0,0);border:1px solid rgb(0,0,0,100);"
                               "background-color:rgb(216,217,222);}");
    statusBar()->showMessage(u8"准备好");
    QLabel *lb1 = new QLabel(u8"Hisense|",this);
    lb1->setStyleSheet("color:rgb(0,0,0);");
    QLabel *lb2 = new QLabel(u8"copyright@2021智动精工---智能制造部",this);
    lb2->setStyleSheet("color:rgb(0,0,0);");
    lb1->setFont(font);
    lb2->setFont(font);
    statusBar()->addPermanentWidget(lb1);
    statusBar()->addPermanentWidget(lb2);
}

void TopMainWindow::make_plc_msg_and_send(short errorcode,short updownsign,QByteArray userdata,short plcaddress,QString boardtype)
{
    QByteArray dataArray;
    //for normal test,the board-type is from central control system
//    if(updownsign == 1)
//        dataArray.append(boardtype.toLatin1());
//    else
//    {
//        QString strBdType = "12345";
//        dataArray.append(strBdType.toLatin1());
//    }
    //该处写死，固定版型12345
    QString strBdType /*= "11117"*/;
    QString strHfNum /*= "293763"*/;
    GDataFactory::get_factory()->get_current_board_info(strBdType,strHfNum);
    if(boardtype == "naked-board")
    {
        strBdType = "00000";
        strHfNum = "000000";
    }

    if((strBdType == "") || (strHfNum == ""))
    {
        strBdType = "00000";
        strHfNum = "000000";
    }

    dataArray.append(strBdType.toLatin1());
    dataArray.append(strHfNum.toLatin1());
    //for normal test,test-sign is 0x00
    if(errorcode != 0)
        dataArray.append(static_cast<char>(errorcode));
    else
        dataArray.append(static_cast<char>(0x00));

    //upper or down camera sign
    if((updownsign == 1) || (updownsign == 2))
        dataArray.append(static_cast<char>(updownsign));
    else {
        QLOG_WARN()<<"the up-down camera sign is ABNORMAL!";
        return;
    }

    if(userdata.length()<31)
    {
        for(int i=userdata.length();i<31;i++)
            userdata.append(static_cast<char>(0x00));
    }
    dataArray.append(userdata);
//    dataArray.append(static_cast<char>(0x00));

    if(plcaddress == ADDRESS_D150)
        GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,
                ADDRESS_D150,static_cast<short>(dataArray.length()),dataArray);
    else if (plcaddress == ADDRESS_D250)
        GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,
                ADDRESS_D250,static_cast<short>(dataArray.length()),dataArray);
    else
        QLOG_WARN()<<"PLC address is ABNORMAL!";
}

void TopMainWindow::slot_calibration()
{
    GDataFactory::get_calibration_wgt()->show();
}

void TopMainWindow::slot_history_calibration()
{
    GDataFactory::get_database_wgt()->show();
}

void TopMainWindow::slot_para_set()
{
    GDataFactory::get_paraconfig_wgt()->show();
}

void TopMainWindow::slot_start_run()
{
    GDataFactory::get_vision_obj()->set_normal_test_para();
    GDataFactory::get_factory()->set_system_state(NORMALTEST_SYSTEM_STATE);

    m_D100_timeout = 0;
    m_pRevSign1 = false;
    m_timer_D100 = startTimer(1000);

    m_D200_timeout = 0;
    m_pRevSign2 = false;
    m_timer_D200 = startTimer(1000);

    m_timer_TestID = startTimer(5000);//---for monitor

    calibrationAct->setEnabled(false);
    startRunAct->setEnabled(false);
    stopRunAct->setEnabled(true);
    historyCalibrationAct->setEnabled(false);
    portSetAct->setEnabled(false);

}

void TopMainWindow::timerEvent(QTimerEvent *event)
{
//    QDateTime current_date_time =QDateTime::currentDateTime();
//    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
//    QLOG_INFO()<<current_date;
    if(event->timerId() == m_timer_D100)
    {
        m_pRevSign1 = false;
        m_D100_timeout = 0;
        GDataFactory::get_udp_service()->send_message_to_plc(READ_PLC,ADDRESS_D100,0x04,nullptr);
        killTimer(m_timer_D100);
    }
    else if(event->timerId() == m_timer_D200)
    {
        m_pRevSign2 = false;
        m_D200_timeout = 0;
        GDataFactory::get_udp_service()->send_message_to_plc(READ_PLC,ADDRESS_D200,0x04,nullptr);
        killTimer(m_timer_D200);
    }
    else if(event->timerId() == m_timer_TestID)
    {
        m_D100_timeout++;
        m_D200_timeout++;
        if((m_D100_timeout%2 == 0) && (m_pRevSign1 == false))
        {
            QLOG_WARN()<<"trigger to restart timer1";
            m_timer_D100 = startTimer(1000);
            m_D100_timeout = 0;
        }
        else if((m_D200_timeout%2 == 0) && (m_pRevSign2 == false))
        {
            QLOG_WARN()<<"trigger to restart timer2";
            m_timer_D200 = startTimer(1000);
            m_D200_timeout = 0;
        }
    }
    else
        ;
}

void TopMainWindow::slot_stop_run()
{
    GDataFactory::get_factory()->set_system_state(NULL_SYSTEM_STATE);
    killTimer(m_timer_TestID);
    killTimer(m_timer_D100);
    killTimer(m_timer_D200);
    m_D100_timeout = 0;
    m_D200_timeout = 0;

    QList<double> lst;
    lst.push_back(1.0);
    lst.push_back(1.0);
    lst.push_back(1.0);
    GDataFactory::get_top_bottom_wgt()->slot_rev_vision_info(1,lst);
    GDataFactory::get_image_wgtA()->slot_rev_halcon_image(0,"");
    GDataFactory::get_image_wgtB()->slot_rev_halcon_image(0,"");
    GDataFactory::get_factory()->delete_udp_obj();
    GDataFactory::get_udp_service();
    GDataFactory::get_factory()->re_connect_udp_service();

    calibrationAct->setEnabled(true);
    startRunAct->setEnabled(true);
    stopRunAct->setEnabled(false);
    historyCalibrationAct->setEnabled(true);
    portSetAct->setEnabled(true);

}

void TopMainWindow::slot_rev_normal_take_picture_msg(QByteArray datasign)
{
    datasign = GDataFactory::get_factory()->reverse_array_order(datasign);
    if(datasign.length() == 4)
    {
        if(((datasign[3] & 0xFF) == 0x13)&&((datasign[2] & 0xFF) == 0x88))
        {
            m_pRevSign1 = true;
            if((datasign[0]&0xFF)==0x01)//upper1 camera take picture
            {
                GDataFactory::get_vision_obj()->set_current_take_picture_camera_id(1);
                return;
            }
            else if((datasign[0]&0xFF)==0x02)//down1 camera take picuture
            {
                GDataFactory::get_vision_obj()->set_current_take_picture_camera_id(2);
                return;
            }
            else
            {
//                GDataFactory::get_udp_service()->send_message_to_plc(READ_PLC,ADDRESS_D200,0x04,nullptr);
                m_timer_D100 = startTimer(1000);
                QLOG_WARN()<<"ADDRESS_D100 take picture sign is ZERO!";
            }
        }
        else if (((datasign[3] & 0xFF) == 0x14)&&((datasign[2] & 0xFF) == 0x50))
        {
            m_pRevSign2 = true;
            if((datasign[0]&0xFF)==0x01)//upper2 camera take picture
            {
                GDataFactory::get_vision_obj()->set_current_take_picture_camera_id(3);
                return;
            }
            else if((datasign[0]&0xFF)==0x02)
            {
                GDataFactory::get_vision_obj()->set_current_take_picture_camera_id(4);
                return;
            }
            else
            {
                m_timer_D200 = startTimer(1000);
                QLOG_WARN()<<"ADDRESS_D200 take picture sign is ZERO!";
            }
        }
        else
        {
            QLOG_WARN()<<"take picture sign address is ABNORMAL!";
        }
    }
    else
    {
        QLOG_WARN()<<"normal testing take picture sign msg is abnormal!";
    }
}

void TopMainWindow::slot_rev_result_from_vision(int camera_id,QList<double> lstM,QString tmpBdType)
{
    if(camera_id == 1)
    {
        //第一个if判断当定位失败时
        if((lstM.length() != 3) && (GDataFactory::get_factory()->get_bar_code_is_read_or_not() == true) && (tmpBdType != "naked-board"))
        {
            lstM.push_back(0.0);
            lstM.push_back(0.0);
            lstM.push_back(0.0);

            QByteArray tmpdata1;
            tmpdata1.append(QString::number(lstM[0],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[1],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[2],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));

            if(tmpdata1.length()<30)
            {
                for(int i=tmpdata1.length();i<30;i++)
                    tmpdata1.append(static_cast<char>(0x00));
            }

            make_plc_msg_and_send(2,1,tmpdata1,ADDRESS_D150,tmpBdType);

            m_timer_D100 = startTimer(1000);
            emit signal_send_vision_info(1,lstM);//UI display
            return;
        }
        if((lstM.length() != 3) && (GDataFactory::get_factory()->get_bar_code_is_read_or_not() == false)&&(tmpBdType == ""))
        {
            lstM.push_back(0.0);
            lstM.push_back(0.0);
            lstM.push_back(0.0);

            QByteArray tmpdata1;
            tmpdata1.append(QString::number(lstM[0],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[1],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[2],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));

            if(tmpdata1.length()<30)
            {
                for(int i=tmpdata1.length();i<30;i++)
                    tmpdata1.append(static_cast<char>(0x00));
            }

//            make_plc_msg_and_send(2,1,tmpdata1,ADDRESS_D150,tmpBdType);

            m_timer_D100 = startTimer(1000);
            emit signal_send_vision_info(1,lstM);//UI display
            return;
        }

        if((lstM.length() != 3) && (GDataFactory::get_factory()->get_bar_code_is_read_or_not() == false)&&(tmpBdType == "naked-board"))
        {
            lstM.push_back(0.0);
            lstM.push_back(0.0);
            lstM.push_back(0.0);

            QByteArray tmpdata;
            tmpdata.append(QString::number(lstM[0],'f',3).toLatin1());
            tmpdata.append(static_cast<char>(','));
            tmpdata.append(QString::number(lstM[1],'f',3).toLatin1());
            tmpdata.append(static_cast<char>(','));
            tmpdata.append(QString::number(lstM[2],'f',3).toLatin1());
            tmpdata.append(static_cast<char>(','));

            if(tmpdata.length()<30)
            {
                for(int i=tmpdata.length();i<30;i++)
                    tmpdata.append(static_cast<char>(0x00));
            }

            make_plc_msg_and_send(4,1,tmpdata,ADDRESS_D150,tmpBdType);
            QByteArray clearData;
            clearData.append(static_cast<char>(0x00));
            clearData.append(static_cast<char>(0x00));
            clearData.append(static_cast<char>(0x00));
            clearData.append(static_cast<char>(0x00));
            GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D100,0x04,clearData);

            m_timer_D100 = startTimer(1000);
            emit signal_send_vision_info(1,lstM);//UI display
            return;
        }

        if(GDataFactory::get_factory()->get_board_on_off_sign())
            ;
        else
        {
            QByteArray tmpdata1;
            tmpdata1.append(QString::number(lstM[0],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[1],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[2],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));

            if(tmpdata1.length()<30)
            {
                for(int i=tmpdata1.length();i<30;i++)
                    tmpdata1.append(static_cast<char>(0x00));
            }

            make_plc_msg_and_send(3,1,tmpdata1,ADDRESS_D150,tmpBdType);

            m_timer_D100 = startTimer(1000);
            emit signal_send_vision_info(1,lstM);//UI display
            QLOG_WARN()<<"the piano button of the current board is ON!";
            return;
        }

        emit signal_send_vision_info(1,lstM);//UI display
        QByteArray tmpdata;
        tmpdata.append(QString::number(lstM[0],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));
        tmpdata.append(QString::number(lstM[1],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));
        tmpdata.append(QString::number(lstM[2],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));

        if(tmpdata.length()<30)
        {
            for(int i=tmpdata.length();i<30;i++)
                tmpdata.append(static_cast<char>(0x00));
        }

        if(tmpBdType == "naked-board")
            make_plc_msg_and_send(4,1,tmpdata,ADDRESS_D150,tmpBdType);
        else
            make_plc_msg_and_send(0,1,tmpdata,ADDRESS_D150,tmpBdType);
        QByteArray clearData;
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D100,0x04,clearData);
        m_timer_D100 = startTimer(1000);
    }
    else if(camera_id == 2)
    {
        if(lstM.length() != 3)
        {
            lstM.push_back(0.0);
            lstM.push_back(0.0);
            lstM.push_back(0.0);

            QByteArray tmpdata1;
            tmpdata1.append(QString::number(lstM[0],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[1],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[2],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));

            if(tmpdata1.length()<30)
            {
                for(int i=tmpdata1.length();i<30;i++)
                    tmpdata1.append(static_cast<char>(0x00));
            }

            QLOG_WARN()<<"down1 jingdingwei data exist ERROR!";
            make_plc_msg_and_send(2,2,tmpdata1,ADDRESS_D150,tmpBdType);

            m_timer_D100 = startTimer(1000);
            emit signal_send_vision_info(2,lstM);//UI display
            return;
        }

        emit signal_send_vision_info(2,lstM);//UI display
        QByteArray tmpdata;
        tmpdata.append(QString::number(lstM[0],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));
        tmpdata.append(QString::number(lstM[1],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));
        tmpdata.append(QString::number(lstM[2],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));

        if(tmpdata.length()<30)
        {
            for(int i=tmpdata.length();i<30;i++)
                tmpdata.append(static_cast<char>(0x00));
        }

        make_plc_msg_and_send(0,2,tmpdata,ADDRESS_D150,tmpBdType);
        QByteArray clearData;
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D100,0x04,clearData);
        m_timer_D100 = startTimer(1000);
    }
    else if(camera_id == 4)
    {
        if(lstM.length() != 3)
        {
            lstM.push_back(0.0);
            lstM.push_back(0.0);
            lstM.push_back(0.0);

            QByteArray tmpdata1;
            tmpdata1.append(QString::number(lstM[0],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[1],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[2],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));

            if(tmpdata1.length()<30)
            {
                for(int i=tmpdata1.length();i<30;i++)
                    tmpdata1.append(static_cast<char>(0x00));
            }

            make_plc_msg_and_send(2,2,tmpdata1,ADDRESS_D250,tmpBdType);

            emit signal_send_vision_info(4,lstM);//UI display
            m_timer_D200 = startTimer(1000);
            QLOG_WARN()<<"down2 jingdingwei data is ABNORMAL!";
            return;
        }

        emit signal_send_vision_info(4,lstM);//UI display
        QByteArray tmpdata;
        tmpdata.append(QString::number(lstM[0],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));
        tmpdata.append(QString::number(lstM[1],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));
        tmpdata.append(QString::number(lstM[2],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));

        if(tmpdata.length()<30)
        {
            for(int i=tmpdata.length();i<30;i++)
                tmpdata.append(static_cast<char>(0x00));
        }
        make_plc_msg_and_send(0,2,tmpdata,ADDRESS_D250,tmpBdType);
        QByteArray clearData;
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D200,0x04,clearData);
        m_timer_D200 = startTimer(1000);
    }
    else
    {
        if((lstM.length() != 3) && (GDataFactory::get_factory()->get_bar_code_is_read_or_not() == true) && (tmpBdType != "naked-board"))
        {
            lstM.push_back(0.0);
            lstM.push_back(0.0);
            lstM.push_back(0.0);

            QByteArray tmpdata1;
            tmpdata1.append(QString::number(lstM[0],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[1],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[2],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));

            if(tmpdata1.length()<30)
            {
                for(int i=tmpdata1.length();i<30;i++)
                    tmpdata1.append(static_cast<char>(0x00));
            }

            make_plc_msg_and_send(2,1,tmpdata1,ADDRESS_D250,tmpBdType);

            m_timer_D200 = startTimer(1000);
            emit signal_send_vision_info(1,lstM);//UI display
            return;
        }
        if((lstM.length() != 3) && (GDataFactory::get_factory()->get_bar_code_is_read_or_not() == false)&&(tmpBdType == ""))
        {
            lstM.push_back(0.0);
            lstM.push_back(0.0);
            lstM.push_back(0.0);

            QByteArray tmpdata1;
            tmpdata1.append(QString::number(lstM[0],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[1],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[2],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));

            if(tmpdata1.length()<30)
            {
                for(int i=tmpdata1.length();i<30;i++)
                    tmpdata1.append(static_cast<char>(0x00));
            }

//            make_plc_msg_and_send(2,1,tmpdata1,ADDRESS_D250,tmpBdType);

            emit signal_send_vision_info(3,lstM);//UI display
            m_timer_D200 = startTimer(1000);
            QLOG_WARN()<<"upper2 cudingwei data is ABNORMAL!";
            return;
        }

        if((lstM.length() != 3) && (GDataFactory::get_factory()->get_bar_code_is_read_or_not() == false)&&(tmpBdType == "naked-board"))
        {
            lstM.push_back(0.0);
            lstM.push_back(0.0);
            lstM.push_back(0.0);

            QByteArray tmpdata1;
            tmpdata1.append(QString::number(lstM[0],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[1],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));
            tmpdata1.append(QString::number(lstM[2],'f',3).toLatin1());
            tmpdata1.append(static_cast<char>(','));

            if(tmpdata1.length()<30)
            {
                for(int i=tmpdata1.length();i<30;i++)
                    tmpdata1.append(static_cast<char>(0x00));
            }

            make_plc_msg_and_send(4,1,tmpdata1,ADDRESS_D250,tmpBdType);

            emit signal_send_vision_info(3,lstM);//UI display
            QByteArray clearData;
            clearData.append(static_cast<char>(0x00));
            clearData.append(static_cast<char>(0x00));
            clearData.append(static_cast<char>(0x00));
            clearData.append(static_cast<char>(0x00));
            GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D200,0x04,clearData);
            m_timer_D200 = startTimer(1000);
            return;
        }


        emit signal_send_vision_info(3,lstM);//UI display
        QByteArray tmpdata;
        tmpdata.append(QString::number(lstM[0],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));
        tmpdata.append(QString::number(lstM[1],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));
        tmpdata.append(QString::number(lstM[2],'f',3).toLatin1());
        tmpdata.append(static_cast<char>(','));

        if(tmpdata.length()<30)
        {
            for(int i=tmpdata.length();i<30;i++)
                tmpdata.append(static_cast<char>(0x00));
        }

        make_plc_msg_and_send(0,1,tmpdata,ADDRESS_D250,tmpBdType);
        QByteArray clearData;
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        clearData.append(static_cast<char>(0x00));
        GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D200,0x04,clearData);
        m_timer_D200 = startTimer(1000);
    }
}

void TopMainWindow::about()
{
    GDataFactory::get_about_wgt()->show();
}

void TopMainWindow::exitApp()
{
    GDataFactory::get_vision_obj()->close_some_camera(1);
    GDataFactory::get_vision_obj()->close_some_camera(2);
    GDataFactory::get_vision_obj()->close_some_camera(3);
    GDataFactory::get_vision_obj()->close_some_camera(4);
    GDataFactory::get_vision_obj()->clear_some_model();
    exit(0);
}

