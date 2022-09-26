#include "udpservice.h"
#include "gdatafactory.h"
#include <QNetworkDatagram>
#include "QsLog.h"
UdpService::UdpService()
{
    m_pUdpSocket = new QUdpSocket(this);
    quint16 port = static_cast<quint16>(GDataFactory::get_factory()->get_config_para("SELF_UDP_REV_PORT").toInt());
    m_pUdpSocket->bind(QHostAddress("192.168.8.196"),60000);
    connect(m_pUdpSocket,SIGNAL(readyRead()),this,SLOT(slot_receive_message()));
    connect(this,SIGNAL(signal_proc_plc_msg(const QByteArray)),this,SLOT(slot_proc_plc_msg(const QByteArray)));

    m_pCentralControlUdpSocket = new QUdpSocket(this);
    m_pCentralControlUdpSocket->bind(QHostAddress("172.16.130.11"),59999);
    connect(m_pCentralControlUdpSocket,SIGNAL(readyRead()),this,SLOT(slot_receive_central_control_message()));
    connect(this,SIGNAL(signal_proc_central_control_msg(const QByteArray)),this,SLOT(slot_proc_central_control_msg(const QByteArray)));
    qRegisterMetaType<QByteArray>("QByteArray");
}

UdpService::~UdpService()
{
}

void UdpService::release_res()
{
    disconnect(m_pUdpSocket,SIGNAL(readyRead()),this,SLOT(slot_receive_message()));
    disconnect(this,SIGNAL(signal_proc_plc_msg(const QByteArray)),this,SLOT(slot_proc_plc_msg(const QByteArray)));
    if(m_pUdpSocket != nullptr)
        delete m_pUdpSocket;
    m_pUdpSocket = nullptr;
}

void UdpService::send_message_to_plc(const uchar function_code, const short data_address,const short data_length/*length is word length*/,const QByteArray data_array)
{
//    if(GDataFactory::get_factory()->get_system_state() == NULL_SYSTEM_STATE)
//        return;
    m_mutex.lock();
    QHostAddress hAddr(GDataFactory::get_factory()->get_config_para("PLC_IP"));
    int hPort = GDataFactory::get_factory()->get_config_para("PLC_PORT").toInt();
    QByteArray btArray = make_plc_message(function_code,data_address,data_length,data_array);
    QLOG_INFO()<<"Send Msg To PLC:"<<GDataFactory::get_factory()->bytes_to_str(btArray);
    m_pUdpSocket->writeDatagram(btArray,btArray.length(),hAddr,static_cast<quint16>(hPort));
    m_mutex.unlock();
}

QByteArray UdpService::make_plc_message(const uchar function_code, const short data_address,const short data_length/*length is word length*/,const QByteArray data_array)
{
    QByteArray m_data;
    m_data.append(static_cast<char>(0x80));  //ÆðÊ¼Âë
    m_data.append(static_cast<char>(0x00));
    m_data.append(static_cast<char>(0x02));
    m_data.append(static_cast<char>(0x00));  //PLC IP Address
    m_data.append(static_cast<char>(GDataFactory::get_factory()->get_last_ip(1)));
    m_data.append(static_cast<char>(0x00));
    m_data.append(static_cast<char>(0x00));  //self IP Address
    m_data.append(static_cast<char>(GDataFactory::get_factory()->get_last_ip(0)));
    m_data.append(static_cast<char>(0x00));
    m_data.append(static_cast<char>(0xFF));  //SID
    m_data.append(static_cast<char>(0x01));  //FINS

    switch (function_code)
    {
    case 1://read plc
        m_data.append(static_cast<char>(0x01));
        break;
    case 2://write plc
        m_data.append(static_cast<char>(0x02));
        break;
    }

    m_data.append(static_cast<char>(0x82));
    m_data.append(static_cast<char>(0xFF&(data_address>>8)));
    m_data.append(static_cast<char>(0xFF&data_address));
    m_data.append(static_cast<char>(0x00));
    int length = data_length/2;//plc recognize by word
    m_data.append(static_cast<char>(0xFF&(length>>8)));
    m_data.append(static_cast<char>(0xFF&length));

    if(function_code == 2)
        m_data.append(data_array);

    return m_data;
}

void UdpService::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
}

void UdpService::process_plc_rev_data(const QByteArray data_array)
{
    if(data_array.length() == 67)
    {
        QLOG_INFO()<<"REV CentralControl change production MSG:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
        if(data_array[66] == 0x79)
        {
            QByteArray tmpByteArray;
            for (int i=0;i<66;i++) {
                tmpByteArray.append(data_array[i]);
            }
            send_message_to_plc(WRITE_PLC,ADDRESS_D600,static_cast<short>(tmpByteArray.length()),tmpByteArray);
            return;
        }
    }
    QLOG_INFO()<<"REV PLC MSG:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
    if(data_array.length() > 10)//check the header length
    {
        if((data_array[10] == 0x01) && (data_array[11] == 0x01))
        {
            if((data_array[12] == 0x00)&&(data_array[13] == 0x00))
            {
//                QLOG_INFO()<<"Rev Read Back Msg From PLC:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
                QByteArray tmpArray;
                for(int i=14;i<data_array.length();i++)
                    tmpArray.append(data_array[i]);
                if(GDataFactory::get_factory()->get_system_state() == CLIBRATION_SYSTEM_STATE)
                {
                    emit signal_clibration_take_picture_msg(tmpArray);
                    QLOG_INFO()<<"CLIBRATION DATA ARRAY IS:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
                }
                else if (GDataFactory::get_factory()->get_system_state() == NORMALTEST_SYSTEM_STATE)
                {
                    emit signal_normal_test_take_picture_msg(tmpArray);//normal test procedure
                    QLOG_INFO()<<"NORMAL TEST DATA ARRAY IS:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
                }
                else
                    ;
            }
            else
            {
                QLOG_WARN()<<"PLC read back message is ABNORMAL!";
            }
        }
        else if((data_array[10] == 0x01) && (data_array[11] == 0x02))
        {
            if((data_array[12] == 0x00)&&(data_array[13] == 0x00))
            {
                QLOG_INFO()<<"Rev Write Back Msg From PLC:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
                if(GDataFactory::get_factory()->get_system_state() == CLIBRATION_SYSTEM_STATE)
                {
                    QLOG_INFO()<<"clibration write back right,start vision to take picture and calculate pix&angle";
                    emit signal_plc_rev_calibration();
                    //send_message_to_plc(READ_PLC,ADDRESS_D150,0x26,nullptr);//---test use
                }
                else if (GDataFactory::get_factory()->get_system_state() == NORMALTEST_SYSTEM_STATE)
                {
                    ;
                }
                else
                {
                    ;
                }
            }
            else
            {
                QLOG_WARN()<<"PLC write back message ABNORMAL!";
            }
        }
    }
    else
    {
        QLOG_WARN()<<"PLC message header lenght is WRONG!";
    }
}

void UdpService::slot_receive_message()
{
    m_mutex.lock();
    if(m_pUdpSocket == nullptr)
        return;
    QByteArray datagram;
    datagram.resize(static_cast<int>(m_pUdpSocket->pendingDatagramSize()));
    while(m_pUdpSocket->hasPendingDatagrams())
    {
        m_pUdpSocket->readDatagram(datagram.data(), datagram.size());
//        process_plc_rev_data(datagram);
    }
    m_mutex.unlock();
    emit signal_proc_plc_msg(datagram);
}

void UdpService::slot_receive_central_control_message()
{
    if(m_pCentralControlUdpSocket == nullptr)
        return;
    QByteArray datagram;
    datagram.resize(static_cast<int>(m_pCentralControlUdpSocket->pendingDatagramSize()));
    while(m_pCentralControlUdpSocket->hasPendingDatagrams())
    {
        m_pCentralControlUdpSocket->readDatagram(datagram.data(), datagram.size());
//        process_plc_rev_data(datagram);
    }
    emit signal_proc_central_control_msg(datagram);
}

void UdpService::slot_proc_plc_msg(const QByteArray data_array)
{
    QLOG_INFO()<<"REV PLC MSG:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
    if(data_array.length() > 10)//check the header length
    {
        if((data_array[10] == 0x01) && (data_array[11] == 0x01))
        {
            if((data_array[12] == 0x00)&&(data_array[13] == 0x00))
            {
//                QLOG_INFO()<<"Rev Read Back Msg From PLC:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
                QByteArray tmpArray;
                for(int i=14;i<data_array.length();i++)
                    tmpArray.append(data_array[i]);
                if(GDataFactory::get_factory()->get_system_state() == CLIBRATION_SYSTEM_STATE)
                {
                    GDataFactory::get_calibration_wgt()->slot_rev_take_picture_sign_from_plc(tmpArray);
//                    emit signal_clibration_take_picture_msg(tmpArray);
                    QLOG_INFO()<<"CLIBRATION DATA ARRAY IS:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
                }
                else if (GDataFactory::get_factory()->get_system_state() == NORMALTEST_SYSTEM_STATE)
                {
                    emit signal_normal_test_take_picture_msg(tmpArray);//normal test procedure
                    QLOG_INFO()<<"NORMAL TEST DATA ARRAY IS:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
                }
                else
                    ;
            }
            else
            {
                QLOG_WARN()<<"PLC read back message is ABNORMAL!";
            }
        }
        else if((data_array[10] == 0x01) && (data_array[11] == 0x02))
        {
            if((data_array[12] == 0x00)&&(data_array[13] == 0x00))
            {
                QLOG_INFO()<<"Rev Write Back Msg From PLC:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
                if(GDataFactory::get_factory()->get_system_state() == CLIBRATION_SYSTEM_STATE)
                {
                    QLOG_INFO()<<"clibration write back right,start vision to take picture and calculate pix&angle";
                    emit signal_plc_rev_calibration();
                    //send_message_to_plc(READ_PLC,ADDRESS_D150,0x26,nullptr);//---test use
                }
                else if (GDataFactory::get_factory()->get_system_state() == NORMALTEST_SYSTEM_STATE)
                {
                    ;
                }
                else
                {
                    ;
                }
            }
            else
            {
                QLOG_WARN()<<"PLC write back message ABNORMAL!";
            }
        }
    }
    else
    {
        QLOG_WARN()<<"PLC message header lenght is WRONG!";
    }
}

void UdpService::slot_proc_central_control_msg(const QByteArray data_array)
{
    if(data_array.length() == 67)
    {
        QLOG_INFO()<<"REV CentralControl change production MSG:"<<GDataFactory::get_factory()->bytes_to_str(data_array);
        if(data_array[66] == 0x79)
        {
            QByteArray tmpByteArray;
            for (int i=0;i<66;i++) {
                tmpByteArray.append(data_array[i]);
            }
            send_message_to_plc(WRITE_PLC,ADDRESS_D600,static_cast<short>(tmpByteArray.length()),tmpByteArray);
            return;
        }
    }
}
