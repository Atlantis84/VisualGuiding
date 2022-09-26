#include "mesprocess.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include "QsLog.h"
#include "imessagebox.h"
MesProcess::MesProcess(QObject *parent) : QObject(parent)
{
    accessManager = new QNetworkAccessManager();
    connect(accessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(slot_urlReply(QNetworkReply*)));
}

void MesProcess::exec_http_post()
{
    QNetworkRequest request;
    request.setUrl(QUrl("http://127.0.0.1:8080/post"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    request.setRawHeader("appId","CenterController");
    request.setRawHeader("publicKey","MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqyHeiI"
                                     "AoTZ1I3Nb9KE8DoSMPWEMdA4ilQGh0yGdUzqvbU2XfpK5aNIen"
                                     "9aWqDUb7hV5rjSc6ab/sw5Zp2EJtGKwZB/JzVaLIwlx7Dvijx9"
                                     "FojiQ1Ca+7qJF74b9CnDkIWrIsKBiHvFQQfgxKUACfXxwsLirf"
                                     "E8pcE309wcXlldBcGFY/UgyYi6MJZcP7Es9a44Fb3cH8iluEBw"
                                     "3x85BvGhAsFMp/1cx4FcUflgSr/L7qWIJbH90yuSaQEFJd8xMb"
                                     "2sJViZrogknD4KQzMd7gmdygaDtyxujh517U/vpgNGbxgpaCLt"
                                     "4u8EZDyP+Pbyxu4TI1LZlv3RJry95M7dn2mQIDAQAB");

    //register operation
    QJsonObject reJson;
    //register test
//    reJson.insert("reType","MACHINE_REGISTER");
//    reJson.insert("lineName","T104");
//    reJson.insert("macAddress","D8-F2-CA-54-E3-B0");
//    reJson.insert("machineStyle","1");

//    reJson.insert("reType","VISION_LEAD");
//    reJson.insert("productSN","TG21AM3B80");

//    reJson.insert("reType","TCON_RESULT");
//    reJson.insert("machineName",u8"机台1");
//    reJson.insert("lineName","T101");

    reJson.insert("reType","VISION_LEAD");
    reJson.insert("productSN","ZTP2053E5AE");

    QJsonDocument doc_data1(reJson);
    QByteArray response_data = doc_data1.toJson(QJsonDocument::Compact);
    qDebug()<<response_data;
    accessManager->post(request, response_data);
    return;
}

void MesProcess::slot_urlReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray responseByte = reply->readAll();
        QLOG_DEBUG()<<"rev up1 camera central control reply,the info is:" << QString(responseByte);
        QJsonDocument jsonDocument = QJsonDocument::fromJson(responseByte);

        QJsonObject jsonObject = jsonDocument.object();
        QString statusString = "";
        if(jsonObject.contains("code"))
        {
            QJsonValue instructionValue = jsonObject.value(QStringLiteral("code"));
            if(instructionValue.isString())
                statusString = instructionValue.toString();
            else
            {
                QLOG_WARN()<<"central control code para data format is ABNORMAL!";
                IMessageBox* msgBox = new IMessageBox(3);
                msgBox->warning(u8"上相机1中控系统信息数据格式错误!");
                return;
            }
        }

        if(statusString == "OK")
        {
            QList<QString> lstinfo;
            if(jsonObject.contains("projectID"))
            {
                lstinfo.push_back(jsonObject.value(QStringLiteral("projectID")).toString());
            }

            if(jsonObject.contains("halfMaterialNumber"))
            {
                lstinfo.push_back(jsonObject.value(QStringLiteral("halfMaterialNumber")).toString());
            }

            if(jsonObject.contains("machineStyleName"))
            {
                lstinfo.push_back(jsonObject.value(QStringLiteral("machineStyleName")).toString());
            }

            if(jsonObject.contains("boardStyle"))
            {
                lstinfo.push_back(jsonObject.value(QStringLiteral("boardStyle")).toString());
            }

            if(jsonObject.contains("boardSize"))
            {
                lstinfo.push_back(jsonObject.value(QStringLiteral("boardSize")).toString());
            }

            if(lstinfo.length() == 5)
                emit signal_test_central_control(lstinfo);

        }
        else
        {
            QLOG_WARN()<<"central control reply ERROR!";
//            IMessageBox* msgBox = new IMessageBox(3);
//            msgBox->warning(u8"中控系统返回错误!");
            return;
        }
    }
}
