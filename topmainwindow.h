#ifndef TOPMAINWINDOW_H
#define TOPMAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QNetworkAccessManager>
class TopMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit TopMainWindow(QWidget *parent = nullptr);
private:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void createActions();
    void createDockWindows();
    void createStatusBar();
    void make_plc_msg_and_send(short errorcode,short updownsign,QByteArray userdata,short plcaddress,QString boardtype);
    QMenu *viewMenu;
    int m_timer_D100;
    int m_timer_D200;
    int m_D100_timeout;
    int m_D200_timeout;
    int m_timer_TestID;
    bool m_pRevSign1;
    bool m_pRevSign2;

    QAction *calibrationAct;
    QAction *startRunAct;
    QAction *stopRunAct;
    QAction *historyCalibrationAct;
    QAction *portSetAct;

public:

signals:
    void signal_send_vision_info(int camera_id,QList<double> lstInfo);
public slots:
    void slot_calibration();
    void slot_history_calibration();
    void slot_para_set();
    void slot_start_run();
    void slot_stop_run();
    void slot_rev_normal_take_picture_msg(QByteArray datasign);
    void slot_rev_result_from_vision(int camera_id,QList<double> lstM,QString tmpBdType);
    void about();
    void exitApp();
};

#endif // TOPMAINWINDOW_H
