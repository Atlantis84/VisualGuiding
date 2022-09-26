#include "calibrationwidget.h"
#include "TitleBar.h"
#include <QPainter>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include "imessagebox.h"
#include <QSqlQuery>
#include "gdatafactory.h"
#include "QsLog.h"
#include <memory>
#include <QEventLoop>
#include <QTimer>
#include <QDir>
#include <QApplication>
CalibrationWidget::CalibrationWidget(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() | Qt::Widget);
    setWindowIcon(QIcon(":/icon/24x24.png"));
    this->setStyleSheet("QWidget{background-color:#FFFFFF}");
    resize(1200,700);
    TitleBar *pTitleBar = new TitleBar(this);
    pTitleBar->setTitle(u8"视觉标定");
    pTitleBar->setFixedWidth(1200);
    installEventFilter(pTitleBar);

    QVBoxLayout* vAll = new QVBoxLayout();
    QVBoxLayout* vBox1 = new QVBoxLayout();
    vBox1->addWidget(createTableWidget());
    connect(tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(slot_tablewgt_cell_changed(int,int)));
    connect(tableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(slot_item_changed(QTableWidgetItem*)));
    QHBoxLayout* hBox1 = new QHBoxLayout();
//    QCheckBox* ckBox = new QCheckBox(u8"二维码");
//    ckBox->setStyleSheet("QCheckBox{background-color:rgba(0,0,0,0);}");
    combCamera = new QComboBox();
    connect(combCamera,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_comb_index_changed(int)));
    combCamera->setStyleSheet("QComboBox{border:1px solid rgba(0,0,0,100);font-family:KaiTi;"
                                           "color:rgba(0,0,0,150);background-color:rgb(235,236,240);min-width:120px;"
                                            "max-width:120px;min-height:30px;}"
                                           "QComboBox:hover{border:2px solid rgba(0,0,0,100);}");
    combCamera->insertItem(0,u8"上相机1");
    combCamera->insertItem(1,u8"下相机1");
    combCamera->insertItem(2,u8"上相机2");
    combCamera->insertItem(3,u8"下相机2");
    combCamera->setCurrentIndex(0);

    btnSave = new QPushButton(u8"  保 存 数 据  ");
    connect(btnSave,SIGNAL(clicked()),this,SLOT(slot_SaveCalibrationData()));
    btnSave->setStyleSheet("QPushButton{background-color:rgba(0,0,0,0);} QPushButton:hover{border:1px solid;border-radius:5px;border-color:rgba(0,0,0,50);background-color:rgb(207,207,214);}");
    btnClear = new QPushButton(u8"  清 空 数 据  ");
    connect(btnClear,SIGNAL(clicked()),this,SLOT(slot_clear_data()));
    btnClear->setStyleSheet("QPushButton{background-color:rgba(0,0,0,0);} QPushButton:hover{border:1px solid;border-radius:5px;border-color:rgba(0,0,0,50);background-color:rgb(207,207,214);}");
    btnStart = new QPushButton(u8"  开 始 标 定  ");
    btnStart->setStyleSheet("QPushButton{background-color:rgba(0,0,0,0);} QPushButton:hover{border:1px solid;border-radius:5px;border-color:rgba(0,0,0,50);background-color:rgb(207,207,214);}");
    connect(btnStart,SIGNAL(clicked()),this,SLOT(slot_start_calibration()));
    btnStop = new QPushButton(u8"  结 束 标 定  ");
    connect(btnStop,SIGNAL(clicked()),this,SLOT(slot_stop_calibration()));
    btnStop->setStyleSheet("QPushButton{background-color:rgba(0,0,0,0);} QPushButton:hover{border:1px solid;border-radius:5px;border-color:rgba(0,0,0,50);background-color:rgb(207,207,214);}");
    hBox1->addStretch(3);
    hBox1->addWidget(combCamera);
    hBox1->addStretch(1);
    hBox1->addWidget(btnStart);
    hBox1->addStretch(1);
    hBox1->addWidget(btnStop);
    hBox1->addStretch(1);
    hBox1->addWidget(btnSave);
    hBox1->addStretch(1);
    hBox1->addWidget(btnClear);
    hBox1->addStretch(5);
    QGroupBox* grpBox = new QGroupBox(u8"实时标定数据");
    grpBox->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");
    grpBox->setLayout(vBox1);
    vAll->addSpacing(pTitleBar->height());
    vAll->addWidget(grpBox);
    vAll->addLayout(hBox1);
    vAll->setStretch(0,5);
    vAll->setStretch(1,5);
    this->setLayout(vAll);

    //init some para
    currentCalibrationCameraID= 0;
    currentCalibrationEntry= 0;
}

void CalibrationWidget::slot_comb_index_changed(int index)
{
//    if(index == 0)
        currentCalibrationCameraID = index+1;
//    else
//        currentCalibrationCameraID = index+2;
//    QLOG_INFO()<<"current camera id is:"<<QString::number(currentCalibrationCameraID);
    set_table_data(currentCalibrationCameraID);
}

void CalibrationWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(this->rect(),QBrush(QColor(235,236,240)));
    painter.setPen(QPen(QColor(0,0,0,100),2));
    painter.drawRect(this->rect());
}

void CalibrationWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    try {
            GDataFactory::get_factory()->load_json_config(QByteArray(QApplication::applicationDirPath().append("/config.json").toLatin1()).data());
            this->combCamera->setCurrentIndex(0);
            set_table_data(1);
            currentCalibrationEntry= 0;
        } catch (...) {
            QLOG_WARN()<<"load json config file failed!";
        }
}

void CalibrationWidget::slot_SaveCalibrationData()
{
    IMessageBox* msgBox = new IMessageBox(3);

    for (int i=0;i<12;i++)
    {
        for (int j=0;j<6;j++)
        {
            if(tableWidget->item(i,j) == nullptr)
            {
                msgBox->warning("UNSAVE! some item is NULL");
                return;
            }
            else
            {
                if(tableWidget->item(i,j)->text().trimmed() == "")
                {
                    msgBox->warning("UNSAVE! some data is NULL");
                    return;
                }
            }
        }
    }

    QString strSql = QString("select * from public.%1 order by \"ID\" desc").arg(constCalibrationEntryTable);
    QSqlQuery queryResult;
    int id = -1;
    if(GDataFactory::get_pgsql()->GetQueryResult(strSql,queryResult))
    {
        QLOG_TRACE()<<u8"query database success!";
        while(queryResult.next())
        {
            if(id > queryResult.value(0).toInt())
                continue;
            else
                id = queryResult.value(0).toInt();
        }
        if(id == -1)
        {
            id = 1;
            QString currentCameraId = QString::number(currentCalibrationCameraID);
            strSql = QString("insert into %1 values(%2,'%3','%4','%5')").arg(constCalibrationEntryTable).
                    arg(id).arg(currentCameraId).arg(QDateTime::currentDateTime().toString("yyyy/MM/dd/hh:mm:ss")).arg("calibration");
            QString strError;
            if(GDataFactory::get_pgsql()->ExecSql(strSql,strError))
                QLOG_TRACE()<<u8"insert into db success!";
            else
            {
                QLOG_WARN()<<u8"insert into db failed!";
                return;
            }
        }
        else
        {
            id += 1;
            QString currentCameraId = QString::number(currentCalibrationCameraID);
            strSql = QString("insert into %1 values(%2,'%3','%4','%5')").arg(constCalibrationEntryTable).
                    arg(id).arg(currentCameraId).arg(QDateTime::currentDateTime().toString("yyyy/MM/dd/hh:mm:ss")).arg("test");
            QString strError;
            if(GDataFactory::get_pgsql()->ExecSql(strSql,strError))
                QLOG_TRACE()<<"insert into db success!";
            else
            {
                QLOG_WARN()<<"insert into db failed!";
                return;
            }
        }
    }
    else
    {
        QLOG_WARN()<<u8"query database failed!";
        return;
    }

    //insert details to database
    bool calculate_vision_sign = false;
    for (int i=0;i<12;i++)
    {
        strSql = QString("insert into %1 values(%2,'%3','%4','%5','%6','%7','%8')").
                arg(constCalibrationDetailsTable).
                arg(id).
                arg(tableWidget->item(i,0)->text()).
                arg(tableWidget->item(i,1)->text()).
                arg(tableWidget->item(i,2)->text()).
                arg(tableWidget->item(i,3)->text()).
                arg(tableWidget->item(i,4)->text()).
                arg(tableWidget->item(i,5)->text());
        QString strError;
        if(GDataFactory::get_pgsql()->ExecSql(strSql,strError))
        {
            QLOG_TRACE()<<"insert into db success!";
            calculate_vision_sign = true;
        }
        else
        {
            QLOG_WARN()<<"insert into db failed!";
            return;
        }
    }
    if(calculate_vision_sign == true)
    {
        calculate_and_save_vision_info();

        QString strX = "CAMERA"+QString::number(currentCalibrationCameraID)+"_X";
        QString strY = "CAMERA"+QString::number(currentCalibrationCameraID)+"_Y";
        QString strA = "CAMERA"+QString::number(currentCalibrationCameraID)+"_A";
        GDataFactory::get_factory()->set_config_para(strX,tableWidget->item(4,3)->text());
        GDataFactory::get_factory()->set_config_para(strY,tableWidget->item(4,4)->text());
        GDataFactory::get_factory()->set_config_para(strA,tableWidget->item(4,5)->text());
    }
}

void CalibrationWidget::calculate_and_save_vision_info()
{
    double mX[12],mY[12],pX[12],pY[12];
    for (int i=0;i<12;i++)
    {
        mX[i] = tableWidget->item(i,0)->text().toDouble();
        mY[i] = tableWidget->item(i,1)->text().toDouble();
        pX[i] = tableWidget->item(i,3)->text().toDouble();
        pY[i] = tableWidget->item(i,4)->text().toDouble();
    }
    GDataFactory::get_vision_obj()->calibrate(currentCalibrationCameraID,mX,mY,pX,pY);
}

void CalibrationWidget::slot_rev_vision_rotate_center(double centerX, double centerY)
{
    if((currentCalibrationCameraID >= 1) && (currentCalibrationCameraID <= 4))
        {
            QString strSql = QString("update public.%1 set \"Rotate_X\"=%2,\"Rotate_Y\"=%3 where \"ID\"=%4").
                    arg(constVisionRotateTable).
                    arg(QString::number(centerX)).
                    arg(QString::number(centerY)).
                    arg(currentCalibrationCameraID);

            QString strError;
            if(GDataFactory::get_pgsql()->ExecSql(strSql,strError))
                QLOG_INFO()<<u8"update rotate center table success!";
            else
            {
                QLOG_INFO()<<u8"insert into db failed!"<<strError;
                return;
            }
        }
        else
            QLOG_INFO()<<"currentCalibrationCameraID is ABNORMAL";
}

void CalibrationWidget::slot_clear_data()
{
    this->tableWidget->clearContents();
}

void CalibrationWidget::slot_tablewgt_cell_changed(int row,int column)
{
    if(tableWidget->item(row,column)->text() == "")
        ;
    else {
        tableWidget->item(row,column)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

void CalibrationWidget::slot_start_calibration()
{

    QByteArray clearData;
    clearData.append(static_cast<char>(0x00));
    clearData.append(static_cast<char>(0x00));
    clearData.append(static_cast<char>(0x00));
    clearData.append(static_cast<char>(0x00));
    QLOG_INFO()<<"clear the take picture sign";
    GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D100,0x04,clearData);
//    QEventLoop loop;
//    QTimer::singleShot(1000, &loop, SLOT(quit()));
//    loop.exec();
    //check manual enter mechanical data exist null value or item itself is null
    IMessageBox* msgBox = new IMessageBox(3);

    for (int i=0;i<12;i++)
    {
        for (int j=0;j<3;j++)
        {
            if(tableWidget->item(i,j) == nullptr)
            {
                msgBox->warning("Warning! some item is NULL!");
                return;
            }
            else
            {
                if(tableWidget->item(i,j)->text().trimmed() == "")
                {
                    msgBox->warning("Warning! some data is NULL!");
                    return;
                }
            }
        }
    }

    if(currentCalibrationEntry >= 12)//clear last time calibration entry,restart calibration
        currentCalibrationEntry = 0;

    for (int i=0;i<12;i++)
    {
        if(tableWidget->item(i,3) == nullptr)
        {
            currentCalibrationEntry = i;
            break;
        }
        else {
            if(tableWidget->item(i,3)->text().trimmed() == "")
            {
                currentCalibrationEntry = i;
                break;
            }
        }
    }

    currentCalibrationCameraID = combCamera->currentIndex()+1;//start calibrate,set the camera ID

    calibration_process();
    m_timer_id = startTimer(1000);
    this->btnStart->setEnabled(false);
    this->combCamera->setEnabled(false);
    this->btnSave->setEnabled(false);
    this->btnClear->setEnabled(false);
}

void CalibrationWidget::slot_item_changed(QTableWidgetItem *item)
{
    QString Pattern("(-?[1-9][0-9]+)|(-?[0-9])|(-?[1-9]\\d+\\.\\d+)|(-?[0-9]\\.\\d+)");
    QRegExp  reg(Pattern);

    QString str=item->text();
    if(reg.exactMatch(str))
    {
//        qDebug()<<"data is valid";
        ;
    }
    else {
//        IMessageBox* msgBox = new IMessageBox(3);
//        msgBox->warning("The data format is forbidden!");
        item->setText("");
    }
}

void CalibrationWidget::slot_stop_calibration()
{
    GDataFactory::get_factory()->set_system_state(NULL_SYSTEM_STATE);
    killTimer(m_timer_id);
    this->btnStart->setEnabled(true);
    this->combCamera->setEnabled(true);
    this->btnSave->setEnabled(true);
    this->btnClear->setEnabled(true);
    //    GDataFactory::get_udp_service()->send_message_to_plc(1,1,6,nullptr);
}

void CalibrationWidget::timerEvent(QTimerEvent *event)
{
    //commit the timeout procedure for test---lilei20211230
//    if(timeOutCount == 6)//if reading take picutre sign times was over 5,kill timer
//    {
//        QLOG_WARN()<<"reading take picture sign time out!";
//        killTimer(m_timer_id);
//        return;
//    }
    Q_UNUSED(event);
//    QLOG_INFO()<<"Calibration Timer Process:after PLC write back msg,"
//                 "wait for mechanical device to be ready,vision take picture and calculate the para!";

    //start timer to read plc taking picture sign
    read_take_picture_sign_of_four_camera();
    timeOutCount++;
}

void CalibrationWidget::slot_rev_plc_calibration()
{
    //trigger camera to take photo，the vision get the pixel para to update the table
//    if(currentCalibrationEntry >= 12)
//    {
//        killTimer(m_timer_id);
//        timeOutCount = 0;
//        return;
//    }
//    timeOutCount = 0;
//    m_timer_id = startTimer(1000);
//    return;
}

void CalibrationWidget::slot_rev_take_picture_sign_from_plc(QByteArray takepicturesign)
{
    //the order of data byte is reverse,so we have to reverse the data
    QByteArray aa;
    aa.append(takepicturesign[0]);
//    QLOG_INFO()<<"takepicturesign[0] is:"<<GDataFactory::get_factory()->bytes_to_str(aa);
    takepicturesign = GDataFactory::get_factory()->reverse_array_order(takepicturesign);
    QLOG_WARN()<<"take picture cameraID is:"<<QString::number(currentCalibrationCameraID);
    if(takepicturesign.length() == 4)
    {
        switch (currentCalibrationCameraID)
        {
        case 1:
        {
            //upper1 camera take picture
            if((takepicturesign[0] & 0xFF) == 0x01)
            {
                killTimer(m_timer_id);//after rev take picture sign from plc,stop the timer to read take picture sign
                timeOutCount = 0;
                /*in order to clear the take picture sign in the PLC,when we read the sign,we have to*/
                /*set the system state as NULL_SYSTEM_STATE,because the current system state is CLIBRATION_SYSTEM_STATE,we*/
                /*clear the take picture sign by write PLC,when the write back msg is arrived,if the current system state is*/
                /*still CLIBRATION_SYSTEM_STATE,the software will take the next calibration*/
                GDataFactory::get_factory()->set_system_state(NULL_SYSTEM_STATE);
                GDataFactory::get_vision_obj()->get_image(currentCalibrationCameraID);//add vision code here
                QByteArray clearData;
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                QLOG_INFO()<<"clear the take picture sign";
                GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D100,0x04,clearData);
//                QEventLoop loop;
//                QTimer::singleShot(1000, &loop, SLOT(quit()));
//                loop.exec();

                if(currentCalibrationEntry>=12)
                {
                    slot_stop_calibration();
                    return;
                }

                //after vision calculation,call function calibration_process() to start next entry
                calibration_process();
                m_timer_id = startTimer(1000);
            }
         }
            break;
        case 2:
        {
            //down1 camera take picture
            if((takepicturesign[0] & 0xFF) == 0x02)
            {
                killTimer(m_timer_id);
                timeOutCount = 0;

                GDataFactory::get_factory()->set_system_state(NULL_SYSTEM_STATE);
                //add vision code here
                //after vision calculation,clear the take picture sign,call function calibration_process() to start next entry
                GDataFactory::get_vision_obj()->get_image(currentCalibrationCameraID);//add vision code here
                QByteArray clearData;
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                QLOG_INFO()<<"clear the take picture sign";
                GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D100,0x04,clearData);
//                QEventLoop loop;
//                QTimer::singleShot(1000, &loop, SLOT(quit()));
//                loop.exec();
                if(currentCalibrationEntry>=12)
                {
                    slot_stop_calibration();
                    return;
                }

                calibration_process();
                m_timer_id =startTimer(1000);
            }
        }
            break;
        case 3:
        {
            //upper2 camera take picture
            if((takepicturesign[0] & 0xFF) == 0x01)
            {
                killTimer(m_timer_id);
                timeOutCount = 0;

                GDataFactory::get_factory()->set_system_state(NULL_SYSTEM_STATE);
                GDataFactory::get_vision_obj()->get_image(currentCalibrationCameraID);

                QByteArray clearData;
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D200,0x04,clearData);
                //add vision code here
                //after vision calculation,call function calibration_process() to start next entry

                if(currentCalibrationEntry>=12)
                {
                    slot_stop_calibration();
                    return;
                }

                calibration_process();
                m_timer_id =startTimer(1000);
            }
        }
            break;
        case 4:
        {
            //down2 camera take picture
            if((takepicturesign[0] & 0xFF) == 0x02)
            {
                killTimer(m_timer_id);
                timeOutCount = 0;

                GDataFactory::get_factory()->set_system_state(NULL_SYSTEM_STATE);
                GDataFactory::get_vision_obj()->get_image(currentCalibrationCameraID);
                QByteArray clearData;
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                clearData.append(static_cast<char>(0x00));
                GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,ADDRESS_D200,0x04,clearData);
                //add vision code here
                //after vision calculation,call function calibration_process() to start next entry
                if(currentCalibrationEntry>=12)
                {
                    slot_stop_calibration();
                    return;
                }

                calibration_process();
                m_timer_id =startTimer(1000);
            }
        }
            break;
        default:
            killTimer(m_timer_id);
            timeOutCount = 0;
            QLOG_WARN()<<"camera id is not in 1\3!";
            break;
        }
    }
    else
        QLOG_WARN()<<"calibration take picture sign data is ABNORMAL!";
}

void CalibrationWidget::slot_rev_camera_pix_para(QList<double> pixlst)
{
    int entryNum = currentCalibrationEntry-1;
    if(entryNum <0)
    {
        QLOG_INFO()<<"current entryNum is:"<<QString::number(entryNum);
        return;
    }
    tableWidget->setItem(entryNum,3,new QTableWidgetItem(QString::number(pixlst[0])));
    tableWidget->item(entryNum,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    tableWidget->setItem(entryNum,4,new QTableWidgetItem(QString::number(pixlst[1])));
    tableWidget->item(entryNum,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    tableWidget->setItem(entryNum,5,new QTableWidgetItem(QString::number(pixlst[2])));
    tableWidget->item(entryNum,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//    tableWidget->item(entryNum,3)->setText(QString::number(pixlst[0]));
//    tableWidget->item(entryNum,4)->setText(QString::number(pixlst[1]));
//    tableWidget->item(entryNum,5)->setText(QString::number(pixlst[2]));
}

QWidget *CalibrationWidget::createTableWidget()
{
    QFont* font = new QFont("KaiTi", 15, QFont::StyleNormal);
    tableWidget = new QTableWidget(12,6);

//    for(int i = 0; i < 12; i++)
//    {
//        QTableWidgetItem *item = new QTableWidgetItem();
//        QTableWidgetItem *item1 = new QTableWidgetItem();
//        QTableWidgetItem *item2 = new QTableWidgetItem();
//        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
//        item1->setFlags(item->flags() & (~Qt::ItemIsEditable));
//        item2->setFlags(item->flags() & (~Qt::ItemIsEditable));
//        tableWidget->setItem(i, 3, item);
//        tableWidget->setItem(i, 4, item1);
//        tableWidget->setItem(i, 5, item2);
//     }

    tableWidget->setFont(*font);
    tableWidget->setWindowTitle("QTableWidget & Item");
    QStringList header;
    header<<u8"机械X"<<u8"机械Y"<<u8"机械角度α"<<u8"像素X"<<u8"像素Y"<<u8"相机角度θ";
    tableWidget->setHorizontalHeaderLabels(header);
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{border-color:rgb(0,0,0);color:rgb(0,0,0);background-color:rgba(216,217,222,255);}");
    tableWidget->setStyleSheet("QTableWidget{color:rgb(0,0,0);"
                               "background-color:rgba(0,0,0,0);}"
                               "QTableWidget::item::selected{color:rgb(0,0,0);"
                               "background-color:rgb(207,207,217);}");
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{border-color:rgb(0,0,0);color:rgb(0,0,0);background-color:rgba(216,217,222,255);}");
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //upper1
//    int mX[12] = {462,466,470,462,466,470,462,466,470,466,466,466};
//    int mY[12] = {-138,-138,-138,-153,-153,-153,-168,-168,-168,-153,-153,-153};
//    int mA[12] = {0,0,0,0,0,0,0,0,0,-20,0,20};
    //down1
//    int mX[12] = {318,323,328,318,323,328,318,323,328,323,323,323};
//    int mY[12] = {0,0,0,25,25,25,50,50,50,25,25,25};
//    int mA[12] = {-134,-134,-134,-134,-134,-134,-134,-134,-134,-128,-134,-140};

    //upper2
//    int mX[12] = {416,426,436,416,426,436,416,426,436,426,426,426};
//    int mY[12] = {-135,-135,-135,-160,-160,-160,-185,-185,-185,-160,-160,-160};
//    int mA[12] = {-134,-134,-134,-134,-134,-134,-134,-134,-134,-128,-134,-140};

    //down2
//    int mX[12] = {330,325,320,330,325,320,330,325,320,325,325,325};
//    int mY[12] = {40,40,40,10,10,10,-20,-20,-20,10,10,10};
//    int mA[12] = {-134,-134,-134,-134,-134,-134,-134,-134,-134,-128,-134,-140};
//    for (int i=0;i<12;i++)
//    {
//        int seed = QDateTime::currentDateTime().time().msec();
//        srand(static_cast<unsigned int>(seed));
//        double fx = 0-((rand()%(51))+i)/10.0;
//        double fy = ((rand()%(this->height()-88))+88+i)/10.0;
//        double fcita = ((rand()%(this->height()-180))+180+i)/10.0;

//        double fx = 248+i*10;
//        double fy = -116+i*10;
//        double fcita = -131+i;
//        tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(mX[i])));
//        tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//        tableWidget->setItem(i,1,new QTableWidgetItem(QString::number(mY[i])));
//        tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//        tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(mA[i])));
//        tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//    }

    return tableWidget;
}

void CalibrationWidget::calibration_process()
{

    //start calibration,set the system state
    GDataFactory::get_factory()->set_system_state(CLIBRATION_SYSTEM_STATE);

    QByteArray dataArray;
    //for calibration,the board-type is 0x0000000000
    dataArray.append(static_cast<char>(0x02));
    dataArray.append(static_cast<char>(0x09));
    dataArray.append(static_cast<char>(0x02));
    dataArray.append(static_cast<char>(0x09));
    dataArray.append(static_cast<char>(0x07));
    //for calibration,the half-material-number is the following
    dataArray.append(static_cast<char>(0x06));
    dataArray.append(static_cast<char>(0x07));
    dataArray.append(static_cast<char>(0x04));
    dataArray.append(static_cast<char>(0x04));
    dataArray.append(static_cast<char>(0x06));
    dataArray.append(static_cast<char>(0x06));

    //for calibration,calibration-sign is 0x01
    dataArray.append(static_cast<char>(0x01));

    //upper or down camera sign
    if((currentCalibrationCameraID == 1) || (currentCalibrationCameraID == 3))
        dataArray.append(static_cast<char>(0x01));
    else if ((currentCalibrationCameraID == 2) || (currentCalibrationCameraID == 4))
        dataArray.append(static_cast<char>(0x02));
    else
    {
        QLOG_WARN()<<"Current calibration camera id is abnormal first!";
        return;
    }

    QByteArray tmpdata;
    tmpdata.append(tableWidget->item(currentCalibrationEntry,0)->text().toLatin1());
    QLOG_ERROR()<<"Current Entry is:"<<currentCalibrationEntry;
    QLOG_ERROR()<<"X value is:"<<tableWidget->item(currentCalibrationEntry,0)->text();
    tmpdata.append(static_cast<char>(','));
    tmpdata.append(tableWidget->item(currentCalibrationEntry,1)->text().toLatin1());
    tmpdata.append(static_cast<char>(','));
    tmpdata.append(tableWidget->item(currentCalibrationEntry,2)->text().toLatin1());
    tmpdata.append(static_cast<char>(','));

    if(tmpdata.length()<30)
    {
        for(int i=tmpdata.length();i<30;i++)
            tmpdata.append(static_cast<char>(0x00));
    }

    dataArray.append(tmpdata);
    dataArray.append(static_cast<char>(0x00));//append the data length to 38,because the request of PLC agreement---20211124

    if((currentCalibrationCameraID == 1) || (currentCalibrationCameraID == 2))
        GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,
                ADDRESS_D150,static_cast<short>(dataArray.length()),dataArray);
    else if ((currentCalibrationCameraID == 3) || (currentCalibrationCameraID == 4))
        GDataFactory::get_udp_service()->send_message_to_plc(WRITE_PLC,
                ADDRESS_D250,static_cast<short>(dataArray.length()),dataArray);
    else
        QLOG_WARN()<<"Current calibration camera id is abnormal second!";
    currentCalibrationEntry++;
    QLOG_INFO()<<"currentCalibrationEntry value is:"<<currentCalibrationEntry;
}

void CalibrationWidget::read_take_picture_sign_of_four_camera()
{
    if(GDataFactory::get_factory()->get_system_state() == NULL_SYSTEM_STATE)
        return;
    switch (currentCalibrationCameraID)
    {
    case 1:
    case 2:
            GDataFactory::get_udp_service()->send_message_to_plc(READ_PLC,ADDRESS_D100,0x04,nullptr);
        break;
    case 3:
    case 4:
            GDataFactory::get_udp_service()->send_message_to_plc(READ_PLC,ADDRESS_D200,0x04,nullptr);
        break;
    default:
        break;
    }
}

void CalibrationWidget::set_table_data(int cameraid)
{
    QList<QString> lstAData;
    QList<QString> lstXData;
    QList<QString> lstYData;

    if(cameraid == 1)
    {
        lstAData = GDataFactory::get_factory()->get_calibration_data("Camera1_A");
        lstXData = GDataFactory::get_factory()->get_calibration_data("Camera1_X");
        lstYData = GDataFactory::get_factory()->get_calibration_data("Camera1_Y");
    }
    else if(cameraid == 2)
    {
        lstAData = GDataFactory::get_factory()->get_calibration_data("Camera2_A");
        lstXData = GDataFactory::get_factory()->get_calibration_data("Camera2_X");
        lstYData = GDataFactory::get_factory()->get_calibration_data("Camera2_Y");
    }
    else if(cameraid == 3)
    {
        lstAData = GDataFactory::get_factory()->get_calibration_data("Camera3_A");
        lstXData = GDataFactory::get_factory()->get_calibration_data("Camera3_X");
        lstYData = GDataFactory::get_factory()->get_calibration_data("Camera3_Y");
    }
    else if(cameraid == 4)
    {
        lstAData = GDataFactory::get_factory()->get_calibration_data("Camera4_A");
        lstXData = GDataFactory::get_factory()->get_calibration_data("Camera4_X");
        lstYData = GDataFactory::get_factory()->get_calibration_data("Camera4_Y");
    }
    else {
        QLOG_WARN()<<"the camera id is incorrect for setting table data!";
        return;
    }

    if((lstAData.size()==12)&&(lstXData.size()==12)&&(lstYData.size()==12))
    {
        for (int i=0;i<12;i++)
        {
            tableWidget->setItem(i,0,new QTableWidgetItem(lstXData[i]));
            tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            tableWidget->setItem(i,1,new QTableWidgetItem(lstYData[i]));
            tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            tableWidget->setItem(i,2,new QTableWidgetItem(lstAData[i]));
            tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
    }
    else {
        QLOG_WARN()<<"read camera1 calibration data from file exist ERROR!";
    }
}
