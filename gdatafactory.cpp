#include "gdatafactory.h"
#include <QFile>
#include "QsLog.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostInfo>
#include <QtWinExtras/qwinfunctions.h>
#include <QDir>
#include <QSqlQuery>
#include <QTimer>
#include <QEventLoop>
#include <QApplication>
#include <QMetaType>
GDataFactory* GDataFactory::m_pGDataFactory = nullptr;
WelcomeDlg* GDataFactory::m_pWelcomeDlg = nullptr;
DataBaseWgt* GDataFactory::m_pDataBaseWgt = nullptr;
ParaConfigWgt* GDataFactory::m_pParaConfigWgt = nullptr;
DMDBAccessInterface* GDataFactory::m_pDMDBAccess = nullptr;
UdpService* GDataFactory::m_pUdpService = nullptr;
UdpService* GDataFactory::m_pUdpService1 = nullptr;
MesProcess* GDataFactory::m_pMesProcess = nullptr;
TopWidget* GDataFactory::m_pTopWgt=nullptr;
TopMainWindow* GDataFactory::m_pTopMainWindow=nullptr;
TopRightWidget* GDataFactory::m_pTopRightWgt=nullptr;
TopBottomWidget* GDataFactory::m_pTopBottomWgt=nullptr;
TopCentralWidget* GDataFactory::m_pTopCentralWgt=nullptr;
ImageWidget* GDataFactory::m_pImageWgtA=nullptr;
ImageWidget* GDataFactory::m_pImageWgtB=nullptr;
CalibrationWidget* GDataFactory::m_pCalibrationWgt=nullptr;
vision* GDataFactory::m_pVisionOperations=nullptr;
AboutWgt* GDataFactory::m_pAboutWgt = nullptr;
QNetworkAccessManager* GDataFactory::m_pAccessManager = nullptr;
GDataFactory::GDataFactory()
{
//    get_calibration_wgt();
//    get_paraconfig_wgt();
//    get_database_wgt();
//    get_top_bottom_wgt();

    qRegisterMetaType<QList<QString>>("QList<QString>");
    m_pABSign = "A";
    system_state = 0x03;//init system state
    m_pBoardIsOffOrOn= false;
    m_pBarCodeIsReadOrNot = false;
    m_pPickX = 0;
    m_pPickY = 0;
}

void GDataFactory::load_json_config(char *file_name)
{
    QFile file(file_name);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QLOG_INFO()<<"load json config file success!";
        JsonValue = file.readAll();
        file.close();

        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(JsonValue.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_INFO()<<"parse json file failed!";
            return;
        }
        else {
            QLOG_INFO()<<"parse json file success!";
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains("IPConfigs"))
        {
            QJsonValue arrayValue = jsonObject.value(QStringLiteral("IPConfigs"));
            if(arrayValue.isArray())
            {
                QJsonArray array = arrayValue.toArray();
                for (int i=0;i<array.size();i++)
                {
                    QJsonValue iconArray = array.at(i);
                    QJsonObject icon = iconArray.toObject();
                    ConfigInfo.insert("PLC_IP",icon["PLC_IP"].toString());
                    ConfigInfo.insert("VISUAL_IP",icon["VISUAL_IP"].toString());
                    ConfigInfo.insert("CENTRAL_CONTROL_IP",icon["CENTRAL_CONTROL_IP"].toString());
                }
            }
        }

        //parse the calibration data from file
        if(jsonObject.contains("Camera_Calibration"))
        {
            QJsonObject cameraObject =jsonObject.value(QStringLiteral("Camera_Calibration")).toObject();
            //the Angle of camera1
            if(cameraObject.contains("Camera1_A"))
            {
                QJsonValue aValue = cameraObject.value(QStringLiteral("Camera1_A"));
                if(aValue.isArray())
                {
                    QJsonArray aArray = aValue.toArray();
                    QList<QString> aList;
                    for (int i=0;i < aArray.size();i++)
                    {
                        QJsonValue tmpAValue = aArray.at(i);
                        if(tmpAValue.isObject())
                        {
                            QJsonObject aObj = tmpAValue.toObject();
                            QString tmpAStr = "A"+QString::number(i+1);
                            aList.push_back(aObj.value(tmpAStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera1_A",aList);
                }
            }
            //the X of camera1
            if(cameraObject.contains("Camera1_X"))
            {
                QJsonValue xValue = cameraObject.value(QStringLiteral("Camera1_X"));
                if(xValue.isArray())
                {
                    QJsonArray xArray = xValue.toArray();
                    QList<QString> xList;
                    for (int i=0;i < xArray.size();i++)
                    {
                        QJsonValue tmpXValue = xArray.at(i);
                        if(tmpXValue.isObject())
                        {
                            QJsonObject xObj = tmpXValue.toObject();
                            QString tmpXStr = "X"+QString::number(i+1);
                            xList.push_back(xObj.value(tmpXStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera1_X",xList);
                }
            }
            //the Y of camera1
            if(cameraObject.contains("Camera1_Y"))
            {
                QJsonValue yValue = cameraObject.value(QStringLiteral("Camera1_Y"));
                if(yValue.isArray())
                {
                    QJsonArray yArray = yValue.toArray();
                    QList<QString> yList;
                    for (int i=0;i < yArray.size();i++)
                    {
                        QJsonValue tmpYValue = yArray.at(i);
                        if(tmpYValue.isObject())
                        {
                            QJsonObject yObj = tmpYValue.toObject();
                            QString tmpYStr = "Y"+QString::number(i+1);
                            yList.push_back(yObj.value(tmpYStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera1_Y",yList);
                }
            }

            //the Angle of camera2
            if(cameraObject.contains("Camera2_A"))
            {
                QJsonValue aValue = cameraObject.value(QStringLiteral("Camera2_A"));
                if(aValue.isArray())
                {
                    QJsonArray aArray = aValue.toArray();
                    QList<QString> aList;
                    for (int i=0;i < aArray.size();i++)
                    {
                        QJsonValue tmpAValue = aArray.at(i);
                        if(tmpAValue.isObject())
                        {
                            QJsonObject aObj = tmpAValue.toObject();
                            QString tmpAStr = "A"+QString::number(i+1);
                            aList.push_back(aObj.value(tmpAStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera2_A",aList);
                }
            }
            //the X of camera2
            if(cameraObject.contains("Camera2_X"))
            {
                QJsonValue xValue = cameraObject.value(QStringLiteral("Camera2_X"));
                if(xValue.isArray())
                {
                    QJsonArray xArray = xValue.toArray();
                    QList<QString> xList;
                    for (int i=0;i < xArray.size();i++)
                    {
                        QJsonValue tmpXValue = xArray.at(i);
                        if(tmpXValue.isObject())
                        {
                            QJsonObject xObj = tmpXValue.toObject();
                            QString tmpXStr = "X"+QString::number(i+1);
                            xList.push_back(xObj.value(tmpXStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera2_X",xList);
                }
            }
            //the Y of camera2
            if(cameraObject.contains("Camera2_Y"))
            {
                QJsonValue yValue = cameraObject.value(QStringLiteral("Camera2_Y"));
                if(yValue.isArray())
                {
                    QJsonArray yArray = yValue.toArray();
                    QList<QString> yList;
                    for (int i=0;i < yArray.size();i++)
                    {
                        QJsonValue tmpYValue = yArray.at(i);
                        if(tmpYValue.isObject())
                        {
                            QJsonObject yObj = tmpYValue.toObject();
                            QString tmpYStr = "Y"+QString::number(i+1);
                            yList.push_back(yObj.value(tmpYStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera2_Y",yList);
                }
            }

            //the Angle of camera3
            if(cameraObject.contains("Camera3_A"))
            {
                QJsonValue aValue = cameraObject.value(QStringLiteral("Camera3_A"));
                if(aValue.isArray())
                {
                    QJsonArray aArray = aValue.toArray();
                    QList<QString> aList;
                    for (int i=0;i < aArray.size();i++)
                    {
                        QJsonValue tmpAValue = aArray.at(i);
                        if(tmpAValue.isObject())
                        {
                            QJsonObject aObj = tmpAValue.toObject();
                            QString tmpAStr = "A"+QString::number(i+1);
                            aList.push_back(aObj.value(tmpAStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera3_A",aList);
                }
            }
            //the X of camera3
            if(cameraObject.contains("Camera3_X"))
            {
                QJsonValue xValue = cameraObject.value(QStringLiteral("Camera3_X"));
                if(xValue.isArray())
                {
                    QJsonArray xArray = xValue.toArray();
                    QList<QString> xList;
                    for (int i=0;i < xArray.size();i++)
                    {
                        QJsonValue tmpXValue = xArray.at(i);
                        if(tmpXValue.isObject())
                        {
                            QJsonObject xObj = tmpXValue.toObject();
                            QString tmpXStr = "X"+QString::number(i+1);
                            xList.push_back(xObj.value(tmpXStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera3_X",xList);
                }
            }
            //the Y of camera3
            if(cameraObject.contains("Camera3_Y"))
            {
                QJsonValue yValue = cameraObject.value(QStringLiteral("Camera3_Y"));
                if(yValue.isArray())
                {
                    QJsonArray yArray = yValue.toArray();
                    QList<QString> yList;
                    for (int i=0;i < yArray.size();i++)
                    {
                        QJsonValue tmpYValue = yArray.at(i);
                        if(tmpYValue.isObject())
                        {
                            QJsonObject yObj = tmpYValue.toObject();
                            QString tmpYStr = "Y"+QString::number(i+1);
                            yList.push_back(yObj.value(tmpYStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera3_Y",yList);
                }
            }

            //the Angle of camera4
            if(cameraObject.contains("Camera4_A"))
            {
                QJsonValue aValue = cameraObject.value(QStringLiteral("Camera4_A"));
                if(aValue.isArray())
                {
                    QJsonArray aArray = aValue.toArray();
                    QList<QString> aList;
                    for (int i=0;i < aArray.size();i++)
                    {
                        QJsonValue tmpAValue = aArray.at(i);
                        if(tmpAValue.isObject())
                        {
                            QJsonObject aObj = tmpAValue.toObject();
                            QString tmpAStr = "A"+QString::number(i+1);
                            aList.push_back(aObj.value(tmpAStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera4_A",aList);
                }
            }
            //the X of camera4
            if(cameraObject.contains("Camera4_X"))
            {
                QJsonValue xValue = cameraObject.value(QStringLiteral("Camera4_X"));
                if(xValue.isArray())
                {
                    QJsonArray xArray = xValue.toArray();
                    QList<QString> xList;
                    for (int i=0;i < xArray.size();i++)
                    {
                        QJsonValue tmpXValue = xArray.at(i);
                        if(tmpXValue.isObject())
                        {
                            QJsonObject xObj = tmpXValue.toObject();
                            QString tmpXStr = "X"+QString::number(i+1);
                            xList.push_back(xObj.value(tmpXStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera4_X",xList);
                }
            }
            //the Y of camera4
            if(cameraObject.contains("Camera4_Y"))
            {
                QJsonValue yValue = cameraObject.value(QStringLiteral("Camera4_Y"));
                if(yValue.isArray())
                {
                    QJsonArray yArray = yValue.toArray();
                    QList<QString> yList;
                    for (int i=0;i < yArray.size();i++)
                    {
                        QJsonValue tmpYValue = yArray.at(i);
                        if(tmpYValue.isObject())
                        {
                            QJsonObject yObj = tmpYValue.toObject();
                            QString tmpYStr = "Y"+QString::number(i+1);
                            yList.push_back(yObj.value(tmpYStr).toString());
                        }
                    }
                    CalibrationInfo.insert("Camera4_Y",yList);
                }
            }
        }
        //parse the calibration data from file

        if(jsonObject.contains("PortConfigs"))
        {
            QJsonValue portValue = jsonObject.value(QStringLiteral("PortConfigs"));
            if(portValue.isObject())
            {
                QJsonObject obj = portValue.toObject();
                if(obj.contains("PLC_PORT"))
                    ConfigInfo.insert("PLC_PORT",obj.value("PLC_PORT").toString());
                if(obj.contains("VISUAL_PORT"))
                    ConfigInfo.insert("VISUAL_PORT",obj.value("VISUAL_PORT").toString());
                if(obj.contains("CENTRAL_CONTROL_PORT"))
                    ConfigInfo.insert("CENTRAL_CONTROL_PORT",obj.value("CENTRAL_CONTROL_PORT").toString());
                if(obj.contains("SELF_UDP_REV_PORT"))
                    ConfigInfo.insert("SELF_UDP_REV_PORT",obj.value("SELF_UDP_REV_PORT").toString());
                if(obj.contains("LOG_LEVEL"))
                    ConfigInfo.insert("LOG_LEVEL",obj.value("LOG_LEVEL").toString());
            }
        }

        if(jsonObject.contains("CameraConfigs"))
        {
            QJsonValue cameraValue = jsonObject.value(QStringLiteral("CameraConfigs"));
            if(cameraValue.isObject())
            {
                QJsonObject obj = cameraValue.toObject();
                //camera1
                if(obj.contains("CAMERA1_X"))
                    ConfigInfo.insert("CAMERA1_X",obj.value("CAMERA1_X").toString());
                if(obj.contains("CAMERA1_Y"))
                    ConfigInfo.insert("CAMERA1_Y",obj.value("CAMERA1_Y").toString());
                if(obj.contains("CAMERA1_A"))
                    ConfigInfo.insert("CAMERA1_A",obj.value("CAMERA1_A").toString());

                //camera2
                if(obj.contains("CAMERA2_X"))
                    ConfigInfo.insert("CAMERA2_X",obj.value("CAMERA2_X").toString());
                if(obj.contains("CAMERA2_Y"))
                    ConfigInfo.insert("CAMERA2_Y",obj.value("CAMERA2_Y").toString());
                if(obj.contains("CAMERA2_A"))
                    ConfigInfo.insert("CAMERA2_A",obj.value("CAMERA2_A").toString());

                //camera3
                if(obj.contains("CAMERA3_X"))
                    ConfigInfo.insert("CAMERA3_X",obj.value("CAMERA3_X").toString());
                if(obj.contains("CAMERA3_Y"))
                    ConfigInfo.insert("CAMERA3_Y",obj.value("CAMERA3_Y").toString());
                if(obj.contains("CAMERA3_A"))
                    ConfigInfo.insert("CAMERA3_A",obj.value("CAMERA3_A").toString());

                //camera4
                if(obj.contains("CAMERA4_X"))
                    ConfigInfo.insert("CAMERA4_X",obj.value("CAMERA4_X").toString());
                if(obj.contains("CAMERA4_Y"))
                    ConfigInfo.insert("CAMERA4_Y",obj.value("CAMERA4_Y").toString());
                if(obj.contains("CAMERA4_A"))
                    ConfigInfo.insert("CAMERA4_A",obj.value("CAMERA4_A").toString());
            }
        }

        if(jsonObject.contains("ConfigInstructions"))
        {
            QJsonValue instructionValue = jsonObject.value(QStringLiteral("ConfigInstructions"));
            if(instructionValue.isString())
                qDebug()<<instructionValue.toString();
        }
    }
    else
    {
        QLOG_INFO()<<"load json config file failed!";
        return;
    }

    file.flush();
    file.close();
}

QString GDataFactory::get_config_para(const QString keyname)
{
    return ConfigInfo[keyname];
}

void GDataFactory::set_config_para(const QString keyname, const QString keyvalue)
{
    ConfigInfo[keyname] = keyvalue;
    QFile file(QApplication::applicationDirPath().append("/config.json"));
    QJsonDocument jsonDocument;
    if(file.open(QIODevice::ReadWrite))
    {
        QLOG_INFO()<<"open json config file success for set!";
        jsonDocument = QJsonDocument::fromJson(file.readAll());
    }
    else
    {
        QLOG_INFO()<<"open json config file failed for set!";
        return;
    }
    QJsonObject rootObj = jsonDocument.object();
    QJsonObject subObj;
    QJsonArray subArray;

    if((keyname == "PLC_IP") || (keyname == "VISUAL_IP") || (keyname == "CENTRAL_CONTROL_IP"))
    {
        if(rootObj.contains("IPConfigs") && rootObj["IPConfigs"].isArray())
        {
            subArray = rootObj.value("IPConfigs").toArray();
            for (int i=0;i<subArray.size();i++)
            {
                subObj = subArray.at(i).toObject();
                if(subObj.contains(keyname) && subObj[keyname].isString())
                {
                    subObj[keyname] = keyvalue;
                    subArray.replace(i,subObj);
                    rootObj.insert("IPConfigs",subArray);
                }
            }
        }
    }

    if((keyname == "CENTRAL_CONTROL_PORT") || (keyname == "PLC_PORT") ||(keyname == "VISUAL_PORT"))
    {
        if(rootObj.contains("PortConfigs") && rootObj["PortConfigs"].isObject())
        {
            QJsonValueRef refValue = rootObj.find("PortConfigs").value();
            QJsonObject obj = refValue.toObject();
            obj[keyname] = keyvalue;
            refValue = obj;
        }
    }

    if((keyname == "CAMERA1_X") || (keyname == "CAMERA1_Y") ||(keyname == "CAMERA1_A")||
            (keyname == "CAMERA2_X") || (keyname == "CAMERA2_Y") ||(keyname == "CAMERA2_A")||
            (keyname == "CAMERA3_X") || (keyname == "CAMERA3_Y") ||(keyname == "CAMERA3_A")||
            (keyname == "CAMERA4_X") || (keyname == "CAMERA4_Y") ||(keyname == "CAMERA4_A"))
    {
        if(rootObj.contains("CameraConfigs") && rootObj["CameraConfigs"].isObject())
        {
            QJsonValueRef refValue = rootObj.find("CameraConfigs").value();
            QJsonObject obj = refValue.toObject();
            obj[keyname] = keyvalue;
            refValue = obj;
        }
    }

//    QJsonValueRef testRef = rootObj.find("ConfigInstructions").value();
//    testRef = QJsonValue("Json write test");

    jsonDocument.setObject(rootObj);
    file.seek(0);
    file.write(jsonDocument.toJson());
    file.close();
    file.flush();
}

QString GDataFactory::bytes_to_str(QByteArray data)
{
    QString str_data;
    str_data.clear();
    quint8 n_data = 0;
    for(int i=0; i<data.size(); i++)
    {
        n_data = static_cast<quint8>(data[i]) ;
        if(n_data < 16) str_data.append('0');
        str_data.append(QString::number(n_data, 16));
        str_data.append(' ');
    }
    return str_data;
}

void GDataFactory::connections_initialization()
{
    connect(get_udp_service(),SIGNAL(signal_plc_rev_calibration()),get_calibration_wgt(),SLOT(slot_rev_plc_calibration()));
    connect(get_mes_process(),SIGNAL(signal_to_ui(QByteArray*)),get_top_bottom_wgt(),SLOT(slot_rev_http_data(QByteArray*)));
    connect(get_vision_obj(),SIGNAL(signal_send_image(int,QString)),this,SLOT(slot_rev_from_halcon(int,QString)));
    connect(get_vision_obj(),SIGNAL(signal_send_calib(QList<double>)),get_calibration_wgt(),SLOT(slot_rev_camera_pix_para(QList<double>)));
    connect(get_vision_obj(),SIGNAL(signal_send_rotate_center(double,double)),get_calibration_wgt(),SLOT(slot_rev_vision_rotate_center(double,double)));
    connect(get_udp_service(),&UdpService::signal_clibration_take_picture_msg,get_calibration_wgt(),&CalibrationWidget::slot_rev_take_picture_sign_from_plc);
    connect(get_udp_service(),SIGNAL(signal_normal_test_take_picture_msg(QByteArray)),get_top_main_window(),SLOT(slot_rev_normal_take_picture_msg(QByteArray)));
    connect(this,SIGNAL(signal_send_mes_board_info_to_ui(QList<QString>)),get_top_right_wgt(),SLOT(slot_test_rev_from_central_control(QList<QString>)));
    connect(get_top_main_window(),SIGNAL(signal_send_vision_info(int,QList<double>)),get_top_bottom_wgt(),SLOT(slot_rev_vision_info(int,QList<double>)));
    connect(this,SIGNAL(signal_notify_top_widget_to_kill_the_query_timer()),get_top_wgt(),SLOT(kill_query_timer()));
    connect(get_vision_obj(),SIGNAL(signal_send_image_info_to_top_main_window(int,QList<double>,QString)),get_top_main_window(),SLOT(slot_rev_result_from_vision(int,QList<double>,QString)));
}

void GDataFactory::re_connect_udp_service()
{
    connect(get_udp_service(),SIGNAL(signal_normal_test_take_picture_msg(QByteArray)),get_top_main_window(),SLOT(slot_rev_normal_take_picture_msg(QByteArray)));
}

quint8 GDataFactory::get_last_ip(const int id)
{
    if (id == 0)
    {
        QString localHostName = QHostInfo::localHostName();
        QString localIP = "";
//        qDebug()<<"localHostName:"<<localHostName;
        QHostInfo info = QHostInfo::fromName(localHostName);
        foreach(QHostAddress addr,info.addresses())
            if(addr.protocol() == QAbstractSocket::IPv4Protocol)
            {
//                qDebug()<<"IPV4 Address:"<<addr.toString();
                localIP = addr.toString();
            }
        return process_ip_string(localIP);
    }
    else
    {
        return process_ip_string(get_config_para("PLC_IP"));
    }
}

quint8 GDataFactory::process_ip_string(const QString ipstr)
{
    int nSize = ipstr.size();
    int nCount = 0;
    QString mData;
    mData.clear();
    QChar mChar;
    for(int idx = 0; idx<nSize; idx++)
    {
        mChar = ipstr[idx];
        if(mChar == '.')
        {
            nCount++;
        }
        else if(mChar != '.' && nCount == 3 && (mChar >= '0' || mChar <= '9'))
        {
            mData.append(mChar);
        }
    }
    return static_cast<quint8>(mData.toInt());
}

void GDataFactory::slot_rev_from_halcon(int cameraID,QString imageName)
{
    if(m_pABSign == "A")
    {
        m_pABSign = "B";
        get_image_wgtA()->set_image_x_y(m_pPickX,m_pPickY);
        get_image_wgtA()->slot_rev_halcon_image(cameraID,imageName);
    }
    else
    {
        m_pABSign = "A";
        get_image_wgtB()->set_image_x_y(m_pPickX,m_pPickY);
        get_image_wgtB()->slot_rev_halcon_image(cameraID,imageName);
    }
}

QByteArray GDataFactory::float_to_byte(float data)
{
    QByteArray byte_data;

    char* data_char = reinterpret_cast<char*>(&data);
    for(int index = 3; index >= 0; index--)
    {
        byte_data.append(data_char[index]);
    }
    return byte_data;
}

float GDataFactory::byte_to_float(QByteArray byte)
{
    float result = 0;
    int size = byte.size();
    char* data_char = byte.data();
    char* p = reinterpret_cast<char*>(&result);
    for(int index = 0; index < size; index++)
    {
        *(p + index) = *(data_char + size - 1 - index);
    }
    return result;
}

QByteArray GDataFactory::reverse_array_order(QByteArray srcdata)
{
    QByteArray aa;
    for (int i=srcdata.length()-1;i>=0;i--)
        aa.append(srcdata[i]);
    return aa;
}

QList<double> GDataFactory::get_rotate_center(int camera_id)
{
    QString strSql = QString("select * from public.%1 where \"ID\" =%2").arg(constVisionRotateTable).arg(camera_id);
    QSqlQuery queryResult;
    QList<double> rotateCenter;
    if(get_pgsql()->GetQueryResult(strSql,queryResult))
    {
        while(queryResult.next())
        {
            rotateCenter.push_back(queryResult.value(1).toDouble());
            rotateCenter.push_back(queryResult.value(2).toDouble());
        }
    }
    return rotateCenter;
}

QList<QString> GDataFactory::get_calibration_data(QString keyvalue)
{
    QList<QString> lst;
        if(CalibrationInfo.contains(keyvalue))
            return CalibrationInfo[keyvalue];
        else
            return lst;
}

QString GDataFactory::exec_http_post_to_get_board_style(QString sn)
{
    //璇ュ杩涜淇敼锛屽啓姝籸eturn
//    return  "11117";
    QMutexLocker lock(&m_mutexMes);
    QNetworkReply* reply;
    QEventLoop eventloop;
    QNetworkRequest request;

    QString url = "http://";
    url.append(GDataFactory::get_factory()->get_config_para("CENTRAL_CONTROL_IP"));
    url.append(":");
    url.append(GDataFactory::get_factory()->get_config_para("CENTRAL_CONTROL_PORT"));
    url.append("/post");
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));

    QJsonObject reJson;
    reJson.insert("reType","VISION_LEAD");
    reJson.insert("productSN",sn);//the sn value is from vision

    QJsonDocument doc_data1(reJson);
    QByteArray response_data = doc_data1.toJson(QJsonDocument::Compact);

//    QNetworkAccessManager* tmpaccessManager = new QNetworkAccessManager();
    reply = get_access_manager()->post(request, response_data);
    connect(reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
    //set get request time out
    QLOG_INFO()<<"post to Central Control";
    QTimer::singleShot(3000,&eventloop,&QEventLoop::quit);
    eventloop.exec();

    if(reply->isFinished())
    {
        QByteArray responseByte = reply->readAll();
//        QLOG_WARN()<<"rev up1 camera central control reply,the info is:" << QString(responseByte);
        QLOG_INFO()<<"central control is NORMAL!";
//        emit signal_notify_top_widget_to_kill_the_query_timer();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(responseByte);

        QJsonObject jsonObject = jsonDocument.object();
        QString statusString = "";
        try{
            if(jsonObject.contains("code"))
            {
                QJsonValue instructionValue = jsonObject.value(QStringLiteral("code"));
                if(instructionValue.isString())
                {
                    statusString = instructionValue.toString();
                    if(statusString == "OK")
                    {
                        QList<QString> lstinfo;
                        m_pCurrentBoardType = jsonObject.value(QStringLiteral("boardStyle")).toString();
                        m_pCurrentHalfMaterialNumber = jsonObject.value(QStringLiteral("halfMaterialNumber")).toString();
                        lstinfo.push_back(sn);
                        lstinfo.push_back(jsonObject.value(QStringLiteral("projectID")).toString());
                        lstinfo.push_back(m_pCurrentHalfMaterialNumber);
                        lstinfo.push_back(jsonObject.value(QStringLiteral("machineStyleName")).toString());
                        lstinfo.push_back(m_pCurrentBoardType);
                        lstinfo.push_back(jsonObject.value(QStringLiteral("boardSize")).toString());
                        QLOG_INFO()<<"send to DISPLAY UI!";
                        emit signal_send_mes_board_info_to_ui(lstinfo);
                        QLOG_INFO()<<"send to DISPLAY UI SUCCESS!";
                        disconnect(reply,&QNetworkReply::finished,&eventloop,&QEventLoop::quit);
                        reply->abort();
                        reply->deleteLater();
                        return jsonObject.value(QStringLiteral("boardStyle")).toString();
                    }
                    else
                    {
                       QLOG_WARN()<<"central control code ERROR!";
//                       return "";
                    }
                }
                else
                {
                    QLOG_WARN()<<"central control code para data format is ABNORMAL!";
//                    return "";
                }
                disconnect(reply,&QNetworkReply::finished,&eventloop,&QEventLoop::quit);
                reply->abort();
                reply->deleteLater();
                return "";
            }
            else
            {
                disconnect(reply,&QNetworkReply::finished,&eventloop,&QEventLoop::quit);
                reply->abort();
                reply->deleteLater();
                return "";
            }
        }
        catch(...)
        {
            QLOG_INFO()<<"exception!";
            return "";
        }
    }
    else
    {
        QLOG_WARN()<<"vision request from central control is TIMEOUT!";
        disconnect(reply,&QNetworkReply::finished,&eventloop,&QEventLoop::quit);
        reply->abort();
        reply->deleteLater();
        return "";
    }
}



