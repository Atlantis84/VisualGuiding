#ifndef VISION_H
#define VISION_H
#include <QObject>
#include"HalconCpp.h"
#include"HDevThread.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include<QTextStream>
#include<QDebug>
#include <QThread>
#include <QMutex>
#include <QMetaType>
using namespace HalconCpp;

class vision:public QThread
{
    Q_OBJECT
public:
    vision();
    //Halcon中的变量定义
private:
    //创建相机句柄
    HTuple hv_AcqHandleUpper1,hv_AcqHandleDown1,hv_AcqHandleUpper2,hv_AcqHandleDown2;
    //相机状态值
    QString states;
    //设置相机采图参数
    double exposure,gain;
    //创建显示窗口句柄
    HTuple hv_windowID;
    //创建图像类型
    HObject ho_ImageUpper1,ho_ImageDown1;
    HObject ho_ImageUpper2,ho_ImageDown2;
    //创建标定时的转换矩阵
    HTuple hv_biaoding_tuple_Upper1,hv_biaoding_tuple_Upper2;
    HTuple hv_biaoding_tuple_Down1,hv_biaoding_tuple_Down2;
    //创建视觉扫码相关模型
    HTuple hv_BarCodeHandle,hv_DataCodeHandle;
    //创建模板匹配所需模板
    HTuple hv_ModelID_Upper1,hv_ModelID_Upper1_jiaodu,hv_ModelID_Down1;
    HTuple hv_ModelID_Upper2,hv_ModelID_Upper2_jiaodu,hv_ModelID_Down2;
    //创建旋转中心坐标
    double Rotate_X_Upper1,Rotate_Y_Upper1;
    double Rotate_X_Down1,Rotate_Y_Down1;
    double Rotate_X_Upper2,Rotate_Y_Upper2;
    double Rotate_X_Down2,Rotate_Y_Down2;
    //创建基准点坐标及偏移角度
    HTuple hv_Qx_base,hv_Qy_base,hv_Qa_base;
    //创建定位模板匹配的数值及标定转换后的数值
    HTuple hv_Column,hv_Row,hv_Angle,hv_Score;
    HTuple hv_Column1,hv_Row1,hv_Angle1,hv_Score1;
    HTuple hv_Qx,hv_Qy;
    //计算的中间参数
    HTuple hv_Cos,hv_Sin,hv_mid_X,hv_mid_Y;
    //创建输出图像
    HObject ho_ResultImage;

    //Qt中最终输出的相关变量定义
    //创建输出条码值
    char* resultSN=new char[255];
    //创建定位点输出数值
    HTuple hv_robot_X,hv_robot_Y,hv_robot_A;
    double Robot_X,Robot_Y,Robot_A;
    //创建精定位最终输出偏差值
    double Del_X,Del_Y,Del_A;

    //创建检测插座所需的模板
    HTuple hv_ModelID_TuQi_11117,hv_ModelID_TuQi_11389,hv_ModelID_TuQi_11133;
    HTuple hv_ModelID_PingZhi_11117,hv_ModelID_PingZhi_10707,hv_ModelID_PingZhi_11133;

//private:
public:
    //连接相机
    int open_camera(int camera_id);
    //采集图像
    void get_image(int camera_id);

    //采集图像过程放入线程函数，防止软件卡顿
    void get_image_normal_test_by_thread();

    void re_init_calibration_sign();

    //标定
    //上相机标定，计算图像的像素点坐标
    double calibrate_upper(HObject ho_Image,int camera_id);
    //下相机标定，计算图像的像素点坐标
    double calibrate_down(HObject ho_Image,int camera_id);
    //计算标定时对应的转换矩阵及旋转中心
    void calibrate(int camera_id,double machine_x[12],double machine_y[12],double pixel_x[12],double pixel_y[12]);

    //正常运行时调用的程序
    //初始化程序
    void initiallize();
    //视觉扫码
    //条形码
    char* read_bar_code(HObject ho_Image);
    //二维码
    char* read_2dcode(HObject ho_Image);
    //粗定位
    QList<double> cudingwei_model(HObject ho_Image,int camera_id,double Qx_base,double Qy_base,double Qa_base);
    //精定位
    QList<double> jingdingwei_model(HObject ho_Image,int camera_id,QString SN,double Qx_base,double Qy_base,double Qa_base);
    //
    bool detect_chazuo(HObject ho_Image,QString SN);
    //设置正式测试流程各个参数
    void set_normal_test_para();

    void close_some_camera(int camera_id);

    void clear_some_model();

    QMutex m_mutex;

    void set_current_take_picture_camera_id(int id)
    {
//        m_mutex.lock();
        current_take_pictrue_camera_id = id;
//        m_mutex.unlock();
    }

private:
    int current_take_pictrue_camera_id;
    void run();

signals:
    void signal_send_image(int cameraID,QString imageName);
    void signal_send_calib(QList<double> lst);
    void signal_send_rotate_center(double centerX,double centerY);
    void signal_send_image_info_to_top_main_window(int camera_id,QList<double> lstM,QString tmpBdType);

};

#endif // VISION_H
