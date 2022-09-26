#include <QApplication>
#include <QFile>
#include "gdatafactory.h"
#include <QDir>
#include "QsLog.h"
#include "QsLogDest.h"
#include "imessagebox.h"
#include <QTextCodec>
#include <QDateTime>
using namespace QsLogging;
int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
    QString qssFile = ":/icon/darkgray.qss";
    QFile file(qssFile);

    Logger& logger = Logger::instance();

    QString ttl = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    QString tmpStr = u8"\Log\\log.txt";
    const QString sLogPath(QDir(QApplication::applicationDirPath()).filePath(tmpStr));
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(sLogPath, EnableLogRotation, MaxSizeBytes(512*1024), MaxOldLogCount(5000)));
    logger.addDestination(fileDestination);

    DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination);

    DestinationPtr controlDestination(DestinationFactory::MakeFunctorDestination(GDataFactory::get_top_bottom_wgt(),SLOT(slot_rev_logs(const QString))));
    logger.addDestination(controlDestination);
    QLOG_INFO()<<u8"QsLog Init Done";
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }

    GDataFactory::get_factory()->load_json_config(QByteArray(QApplication::applicationDirPath().append("/config.json").toLatin1()).data());
//    GDataFactory::get_factory()->load_json_config(QByteArray(QString(QDir::currentPath()+"/release/config.json").toLatin1()).data());
    GDataFactory::get_factory()->connections_initialization();
    if(GDataFactory::get_pgsql()->ConnectDataBase())
        QLOG_INFO()<<"connect to db success!";
    else
        QLOG_WARN()<<"connect to db failed!";
//    GDataFactory::get_welcome_dlg()->exec();

    QString logLevel = GDataFactory::get_factory()->get_config_para("LOG_LEVEL");
    if(logLevel == "0")
        logger.setLoggingLevel(QsLogging::TraceLevel);
    else if(logLevel == "1")
        logger.setLoggingLevel(QsLogging::DebugLevel);
    else if(logLevel == "2")
        logger.setLoggingLevel(QsLogging::InfoLevel);
    else if(logLevel == "3")
        logger.setLoggingLevel(QsLogging::WarnLevel);
    else if(logLevel == "4")
        logger.setLoggingLevel(QsLogging::ErrorLevel);
    else if(logLevel == "5")
        logger.setLoggingLevel(QsLogging::FatalLevel);
    else
        logger.setLoggingLevel(QsLogging::OffLevel);

    if((GDataFactory::get_vision_obj()->open_camera(1) == 1)&&(GDataFactory::get_vision_obj()->open_camera(3) == 3)&&
            (GDataFactory::get_vision_obj()->open_camera(2) == 2)&&(GDataFactory::get_vision_obj()->open_camera(4) == 4))
    {
        GDataFactory::get_vision_obj()->initiallize();
        GDataFactory::get_vision_obj()->start();
    }
    else
    {
        IMessageBox* msgBox = new IMessageBox(3);
        msgBox->warning("Open camera failed!");
        exit(1);
    }

    GDataFactory::get_top_wgt()->show();

    return a.exec();
}
