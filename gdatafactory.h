#ifndef GDATAFACTORY_H
#define GDATAFACTORY_H

#include <QObject>
#include "welcomedlg.h"
#include "databasewgt.h"
#include "paraconfigwgt.h"
#include "dmdbaccessinterface.h"
#include <QMap>
#include "udpservice.h"
#include "mesprocess.h"
#include "topwidget.h"
#include "topmainwindow.h"
#include "toprightwidget.h"
#include "topbottomwidget.h"
#include "topcentralwidget.h"
#include "imagewidget.h"
#include "calibrationwidget.h"
#include "vision.h"
#include "aboutwgt.h"
#include <QMutex>

#define ADDRESS_D100 0x1388 //D5000
#define ADDRESS_D150 0x13EC //D5100
#define ADDRESS_D200 0x1450 //D5200
#define ADDRESS_D250 0x14B4 //D5300
#define ADDRESS_D550 0x157C //D5500 error address

#define ADDRESS_D600 0x1770 //D6000 for one button change production

#define READ_PLC  0x01 //function code of read plc
#define WRITE_PLC 0x02 //function code of write plc

#define CLIBRATION_SYSTEM_STATE 0x01 //for calibration
#define NORMALTEST_SYSTEM_STATE 0x02 //for normal test
#define NULL_SYSTEM_STATE       0x03 //for null state

class GDataFactory:public QObject
{
    Q_OBJECT
public:
    GDataFactory();

    void load_json_config(char* file_name);
    QString get_config_para(const QString keyname);
    void set_config_para(const QString keyname,const QString keyvalue);
    QString bytes_to_str(QByteArray data);
    void connections_initialization();
    quint8 get_last_ip(const int id);//id: 0 for self last ip;1 for plc last ip

    QByteArray float_to_byte(float data);
    float byte_to_float(QByteArray byte);
    QByteArray reverse_array_order(QByteArray srcdata);

    quint8 get_system_state(){return system_state;}
    void set_system_state(quint8 sys_state){system_state = sys_state;}

    QList<double> get_rotate_center(int camera_id);
    QList<QString> get_calibration_data(QString keyvalue);

    QString exec_http_post_to_get_board_style(QString sn);

    void set_board_on_off_sign(bool onoffsign)
    {
        m_pBoardIsOffOrOn = onoffsign;
    }

    bool get_board_on_off_sign()
    {
        return m_pBoardIsOffOrOn;
    }

    void set_bar_code_is_read_or_not(bool isread)
    {
        m_mutex.lock();
        m_pBarCodeIsReadOrNot = isread;
        m_mutex.unlock();
    }

    bool get_bar_code_is_read_or_not()
    {
        bool aa;
        m_mutex.lock();
        aa= m_pBarCodeIsReadOrNot;
        m_mutex.unlock();
        return aa;
    }

    void test_char(QString& aa){
        QString str = "abc";
        aa = str;
    }

    void get_current_board_info(QString& boardtype,QString& halfmaterialnum)
    {
        boardtype = m_pCurrentBoardType;
        halfmaterialnum = m_pCurrentHalfMaterialNumber;
    }

    void set_pick_x_y(int pickx,int picky)
    {
        m_pPickX = pickx;
        m_pPickY = picky;
    }

    void re_connect_udp_service();

    void delete_udp_obj()
    {
        m_pUdpService->release_res();
        if(m_pUdpService != nullptr)
            delete m_pUdpService;
        m_pUdpService = nullptr;
    }

    static GDataFactory* get_factory()
    {
        if(m_pGDataFactory == nullptr)
            m_pGDataFactory = new GDataFactory();
        return m_pGDataFactory;
    }

    static WelcomeDlg* get_welcome_dlg()
    {
        if(m_pWelcomeDlg == nullptr)
            m_pWelcomeDlg = new WelcomeDlg();
        return m_pWelcomeDlg;
    }

    static DataBaseWgt* get_database_wgt()
    {
        if(m_pDataBaseWgt == nullptr)
            m_pDataBaseWgt = new DataBaseWgt();
        return m_pDataBaseWgt;
    }

    static ParaConfigWgt* get_paraconfig_wgt()
    {
        if(m_pParaConfigWgt == nullptr)
            m_pParaConfigWgt = new ParaConfigWgt();
        return m_pParaConfigWgt;
    }

    static DMDBAccessInterface* get_pgsql()
    {
        if(m_pDMDBAccess == nullptr)
            m_pDMDBAccess = DMDBAccessInterface::GetInterface() ;
        return m_pDMDBAccess;
    }

    static UdpService* get_udp_service(){
        if(m_pUdpService == nullptr)
            m_pUdpService = new UdpService;
        return m_pUdpService;
    }

    static UdpService* get_udp_service1(){
        if(m_pUdpService1 == nullptr)
            m_pUdpService1 = new UdpService;
        return m_pUdpService1;
    }

    static MesProcess* get_mes_process()
    {
        if(m_pMesProcess == nullptr)
            m_pMesProcess = new MesProcess();
        return m_pMesProcess;
    }

    static TopWidget* get_top_wgt()
    {
        if(m_pTopWgt == nullptr)
            m_pTopWgt = new TopWidget();
        return m_pTopWgt;
    }

    static TopMainWindow* get_top_main_window()
    {
        if(m_pTopMainWindow == nullptr)
            m_pTopMainWindow = new TopMainWindow();
        return m_pTopMainWindow;
    }

    static TopRightWidget* get_top_right_wgt()
    {
        if(m_pTopRightWgt == nullptr)
            m_pTopRightWgt = new TopRightWidget();
        return m_pTopRightWgt;
    }

    static TopBottomWidget* get_top_bottom_wgt()
    {
        if(m_pTopBottomWgt == nullptr)
            m_pTopBottomWgt = new TopBottomWidget();
        return m_pTopBottomWgt;
    }

    static TopCentralWidget* get_top_central_wgt()
    {
        if(m_pTopCentralWgt == nullptr)
            m_pTopCentralWgt = new TopCentralWidget();
        return m_pTopCentralWgt;
    }

    static ImageWidget* get_image_wgtA()
    {
        if(m_pImageWgtA == nullptr)
            m_pImageWgtA = new ImageWidget();
        return m_pImageWgtA;
    }

    static ImageWidget* get_image_wgtB()
    {
        if(m_pImageWgtB == nullptr)
            m_pImageWgtB = new ImageWidget();
        return m_pImageWgtB;
    }

    static CalibrationWidget* get_calibration_wgt()
    {
        if(m_pCalibrationWgt == nullptr)
            m_pCalibrationWgt = new CalibrationWidget();
        return m_pCalibrationWgt;
    }

    static vision* get_vision_obj()
    {
        if(m_pVisionOperations == nullptr)
            m_pVisionOperations = new vision();
        return m_pVisionOperations;
    }

    static AboutWgt* get_about_wgt()
    {
        if(m_pAboutWgt == nullptr)
            m_pAboutWgt = new AboutWgt();
        return m_pAboutWgt;
    }

    static QNetworkAccessManager* get_access_manager()
    {
        if(m_pAccessManager == nullptr)
            m_pAccessManager = new QNetworkAccessManager();
        return m_pAccessManager;
    }

signals:
    void signal_send_mes_board_info_to_ui(QList<QString> lstinfo);
    void signal_notify_top_widget_to_kill_the_query_timer();
public slots:
    void slot_rev_from_halcon(int cameraID,QString imageName);

private:
    static GDataFactory* m_pGDataFactory;
    static WelcomeDlg* m_pWelcomeDlg;
    static DataBaseWgt* m_pDataBaseWgt;
    static ParaConfigWgt* m_pParaConfigWgt;
    static DMDBAccessInterface* m_pDMDBAccess;
    static UdpService* m_pUdpService;
    static UdpService* m_pUdpService1;
    static MesProcess* m_pMesProcess;
    static TopWidget* m_pTopWgt;
    static TopMainWindow* m_pTopMainWindow;
    static TopRightWidget* m_pTopRightWgt;
    static TopBottomWidget* m_pTopBottomWgt;
    static TopCentralWidget* m_pTopCentralWgt;
    static ImageWidget* m_pImageWgtA;
    static ImageWidget* m_pImageWgtB;
    static CalibrationWidget* m_pCalibrationWgt;
    static vision* m_pVisionOperations;
    static AboutWgt* m_pAboutWgt;
    static QNetworkAccessManager* m_pAccessManager;

    QString JsonValue;
    QMap<QString,QString> ConfigInfo;
    QMap<QString,QList<QString>> CalibrationInfo;
    quint8 process_ip_string(const QString ipstr);
    QString m_pABSign;
    QString m_pCurrentBoardType;
    QString m_pCurrentHalfMaterialNumber;
    quint8 system_state;
    QMutex m_mutex;
    QMutex m_mutexMes;
    int m_pPickX,m_pPickY;
    bool m_pBarCodeIsReadOrNot;
    bool m_pBoardIsOffOrOn;//the vision detect the board pushbutton on or off and return the result
};

#endif // GDATAFACTORY_H
