#ifndef UDPSERVICE_H
#define UDPSERVICE_H

#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include <QMutex>
class UdpService : public QThread
{
    Q_OBJECT
public:
    UdpService();
    ~UdpService();
    void release_res();
    void send_message_to_plc(const uchar function_code, const short data_address,const short data_length/*length is word length*/,const QByteArray data_array);
private:
    QUdpSocket *m_pUdpSocket;
    QUdpSocket *m_pCentralControlUdpSocket;
    QByteArray make_plc_message(const uchar function_code, const short data_address,const short data_length/*length is word length*/,const QByteArray data_array);
    void process_plc_rev_data(const QByteArray data_array);
    void timerEvent(QTimerEvent *event);
    QMutex m_mutex;
signals:
    void signal_plc_rev_calibration();
    void signal_clibration_take_picture_msg(QByteArray takepicturesign);
    void signal_normal_test_take_picture_msg(QByteArray takepicturesign);
    void signal_proc_plc_msg(const QByteArray data_array);
    void signal_proc_central_control_msg(const QByteArray data_array);
public slots:
    void slot_receive_message();
    void slot_receive_central_control_message();
    void slot_proc_plc_msg(const QByteArray data_array);
    void slot_proc_central_control_msg(const QByteArray data_array);
};

#endif // UDPSERVICE_H
