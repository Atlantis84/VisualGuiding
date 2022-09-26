#include "vision.h"
#include "QsLog.h"
#include <QCoreApplication>
#include "gdatafactory.h"
vision::vision()
{
    current_take_pictrue_camera_id = -1;
    qRegisterMetaType<QList<double>>("QList<double>");
    qRegisterMetaType<QByteArray>("QByteArray");
}
//开启相机
int vision::open_camera(int camera_id)
{
    HTuple hv_Exception;
    if(camera_id==1)
    {
        try
        {
            //开启上相机1，参数要根据不同电脑进行更改
            //相机设置为软触发模式
            OpenFramegrabber("MVision", 1, 1, 0, 0, 0, 0, "progressive", 8, "default",
                             -1, "false", "auto", "GEV:00G14696095 MV-CH120-10GM", 0, -1, &hv_AcqHandleUpper1);
            if(hv_AcqHandleUpper1!=0)
            {
                GrabImageStart(hv_AcqHandleUpper1, -1);
                QLOG_WARN()<<"Open camera1 success!";
                return 1;
            }
            else {
                return -1;
            }
        }
        catch (HException &HDevExpDefaultException)
        {
            QLOG_WARN()<<"Open camera1 failed!";
            HDevExpDefaultException.ToHTuple(&hv_Exception);
            return -1;
        }

    }
    else if(camera_id==2)
    {
        try
        {
            //开启下相机1
            OpenFramegrabber("MVision", 1, 1, 0, 0, 0, 0, "progressive", 8, "default",
                             -1, "false", "auto", "GEV:00G14696080 MV-CH120-10GM", 0, -1, &hv_AcqHandleDown1);
            if(hv_AcqHandleDown1!=0)
            {
                GrabImageStart(hv_AcqHandleDown1, -1);
                QLOG_WARN()<<"Open camera2 success!";
                return 2;
            }
            else {
                QLOG_WARN()<<"Open camera2 failed!";
                return -2;
            }
        }
        catch (HException &HDevExpDefaultException)
        {
            HDevExpDefaultException.ToHTuple(&hv_Exception);
            return -2;
        }

    }
    else if(camera_id==3)
    {
        try
        {
            //开启上相机2
            OpenFramegrabber("MVision", 1, 1, 0, 0, 0, 0, "progressive", 8, "default",
                             -1, "false", "auto", "GEV:00G14696091 MV-CH120-10GM", 0, -1, &hv_AcqHandleUpper2);
            if(hv_AcqHandleUpper2!=0)
            {
                GrabImageStart(hv_AcqHandleUpper2, -1);
                QLOG_INFO()<<"Open camera3 success!";
                return 3;
            }
            else {
                QLOG_WARN()<<"Open camera3 failed!";
                return -3;
            }
        }
        catch (HException &HDevExpDefaultException)
        {
            HDevExpDefaultException.ToHTuple(&hv_Exception);
            return -3;
        }

    }
    else if(camera_id==4)
    {
        try
        {
            //开启下相机2
            OpenFramegrabber("MVision", 1, 1, 0, 0, 0, 0, "progressive", 8, "default",
                             -1, "false", "auto", "GEV:00G14696084 MV-CH120-10GM", 0, -1, &hv_AcqHandleDown2);
//            double dd=hv_AcqHandleDown2.D();
            if(hv_AcqHandleDown2!=0)
            {
                GrabImageStart(hv_AcqHandleDown2, -1);
                QLOG_INFO()<<"Open camera4 success!";
                return 4;
            }
            else
            {
                QLOG_WARN()<<"Open camera4 failed!";
                return -4;
            }
        }
        catch (HException &HDevExpDefaultException)
        {
            HDevExpDefaultException.ToHTuple(&hv_Exception);
            return -4;
        }

    }
    else
        return 0;
}

//采集图像
void vision::get_image(int camera_id)
{
    HObject ho_empty;
    HTuple hv_n;
    //一号测试机台的上相机
    if(camera_id==1)
    {
        //取消采图延时,取消的参数需根据实际相机连接的参数确定
        SetFramegrabberParam(hv_AcqHandleUpper1,"grab_timeout",-1);
        GrabImage(&ho_ImageUpper1, hv_AcqHandleUpper1);
        GenEmptyObj(&ho_empty);
        TestEqualObj(ho_ImageUpper1,ho_empty,&hv_n);
        if((GDataFactory::get_factory()->get_system_state() == CLIBRATION_SYSTEM_STATE)||
                (GDataFactory::get_factory()->get_system_state() == NULL_SYSTEM_STATE))
        {
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyyMMddhhmmss");
            QString imageName = current_date +".jpg";
            QByteArray ba = imageName.toLatin1();
            const char* str = ba.data();
            HTuple tt(str);
            WriteImage(ho_ImageUpper1,"jpg",0,tt);
            calibrate_upper(ho_ImageUpper1,camera_id);
            emit signal_send_image(camera_id,imageName);
        }
        else
        {
            QString ss = QCoreApplication::applicationDirPath()+"/1.jpg";
            QByteArray ba = ss.toLatin1();
            const char* str = ba.data();
            HTuple tt(str);
            WriteImage(ho_ImageUpper1,"jpg",0,tt);
            calibrate_upper(ho_ImageUpper1,camera_id);
            emit signal_send_image(camera_id,ss);

            double x,y,a;
            x = 1168.22;y=2079.28;a=0.00273391;
            cudingwei_model(ho_ImageUpper1,camera_id,x,y,a);
        }

        //判断图像有无初始化
        if(!ho_ImageUpper1.IsInitialized())
        {
            qDebug()<<"机台1上图像初始化失败";
        }
        //判断图像是否为空
        else if(hv_n==1)
        {
            QLOG_WARN()<<QString::fromLocal8Bit("机台1上相机图像为空");
        }
    }
    //一号测试机台的下相机
    if(camera_id==2)
    {
        //取消采图延时
        SetFramegrabberParam(hv_AcqHandleDown1,"grab_timeout",-1);
        GrabImage(&ho_ImageDown1, hv_AcqHandleDown1);
        GenEmptyObj(&ho_empty);
        TestEqualObj(ho_ImageDown1,ho_empty,&hv_n);

        if((GDataFactory::get_factory()->get_system_state() == CLIBRATION_SYSTEM_STATE)||
                (GDataFactory::get_factory()->get_system_state() == NULL_SYSTEM_STATE))
        {
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyyMMddhhmmss");
            QString imageName = current_date +".jpg";
            QByteArray ba = imageName.toLatin1();
            const char* str = ba.data();
            HTuple tt(str);
            WriteImage(ho_ImageDown1,"jpg",0,tt);
            calibrate_down(ho_ImageDown1,camera_id);
            emit signal_send_image(camera_id,imageName);
        }
        else
        {
            QString ss = QCoreApplication::applicationDirPath()+"/2.jpg";
            QByteArray ba = ss.toLatin1();
            const char* str = ba.data();
            HTuple tt(str);
            WriteImage(ho_ImageDown1,"jpg",0,tt);
            calibrate_upper(ho_ImageDown1,camera_id);
            emit signal_send_image(camera_id,ss);

            double x,y,a;
            x = 1477.32;y=2040.94;a=-0.00090315;
            jingdingwei_model(ho_ImageDown1,camera_id,"12345",x,y,a);
        }

        //判断图像有无初始化
        if(!ho_ImageDown1.IsInitialized())
        {
            qDebug()<<"机台1下相机图像初始化失败";
        }
        //判断图像是否为空
        else if(hv_n==1)
        {
            qDebug()<<"机台1下相机图像为空";
        }
    }
    //二号测试机台的上相机
    if(camera_id==3)
    {
        //取消采图延时
        SetFramegrabberParam(hv_AcqHandleUpper2,"grab_timeout",-1);
        GrabImage(&ho_ImageUpper2, hv_AcqHandleUpper2);
        GenEmptyObj(&ho_empty);
        TestEqualObj(ho_ImageUpper2,ho_empty,&hv_n);

        if((GDataFactory::get_factory()->get_system_state() == CLIBRATION_SYSTEM_STATE)||
                (GDataFactory::get_factory()->get_system_state() == NULL_SYSTEM_STATE))
        {
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyyMMddhhmmss");
            QString imageName = current_date +".jpg";
            QByteArray ba = imageName.toLatin1();
            const char* str = ba.data();
            HTuple tt(str);
            WriteImage(ho_ImageUpper2,"jpg",0,tt);
            calibrate_upper(ho_ImageUpper2,camera_id);
            emit signal_send_image(camera_id,imageName);
        }
        else
        {
            QString ss = QCoreApplication::applicationDirPath()+"/3.jpg";
            QByteArray ba = ss.toLatin1();
            const char* str = ba.data();
            HTuple tt(str);
            WriteImage(ho_ImageUpper2,"jpg",0,tt);
            calibrate_upper(ho_ImageUpper2,camera_id);
            emit signal_send_image(camera_id,ss);

            double x,y,a;
            x = 1477.32;y=2040.94;a=-0.00090315;
            jingdingwei_model(ho_ImageUpper2,camera_id,"12345",x,y,a);
        }

        //判断图像有无初始化
        if(!ho_ImageUpper2.IsInitialized())
        {
            qDebug()<<"机台2上相机图像初始化失败";
        }
        //判断图像是否为空
        else if(hv_n==1)
        {
            qDebug()<<"机台2上相机图像为空";
        }
    }

    //二号测试机台的下相机
    if(camera_id==4)
    {
        //取消采图延时
        SetFramegrabberParam(hv_AcqHandleDown2,"grab_timeout",-1);
        GrabImage(&ho_ImageDown2, hv_AcqHandleDown2);
        GenEmptyObj(&ho_empty);
        TestEqualObj(ho_ImageDown2,ho_empty,&hv_n);

        if((GDataFactory::get_factory()->get_system_state() == CLIBRATION_SYSTEM_STATE)||
                (GDataFactory::get_factory()->get_system_state() == NULL_SYSTEM_STATE))
        {
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyyMMddhhmmss");
            QString imageName = current_date +".jpg";
            QByteArray ba = imageName.toLatin1();
            const char* str = ba.data();
            HTuple tt(str);
            WriteImage(ho_ImageDown2,"jpg",0,tt);
            calibrate_down(ho_ImageDown2,camera_id);
            emit signal_send_image(camera_id,imageName);
        }
        else
        {
            QString ss = QCoreApplication::applicationDirPath()+"/4.jpg";
            QByteArray ba = ss.toLatin1();
            const char* str = ba.data();
            HTuple tt(str);
            WriteImage(ho_ImageDown2,"jpg",0,tt);
            calibrate_upper(ho_ImageDown2,camera_id);
            emit signal_send_image(camera_id,ss);
        }

        //判断图像有无初始化
        if(!ho_ImageDown2.IsInitialized())
        {
            qDebug()<<"机台2下相机图像初始化失败";
        }
        //判断图像是否为空
        else if(hv_n==1)
        {
            qDebug()<<"机台2下相机图像为空";
        }
    }
}

int camera1_naked = 0;
int camera3_naked = 0;
void vision::get_image_normal_test_by_thread()
{
    QList<double> lstM;
    //插座检测结果
    QList<int> detect_result;
    HObject ho_empty;
    HTuple hv_n;
    //一号测试机台的上相机
    int camera_id = current_take_pictrue_camera_id;
    if(camera_id==1)
    {
        current_take_pictrue_camera_id = -1;
        //取消采图延时,取消的参数需根据实际相机连接的参数确定
        SetFramegrabberParam(hv_AcqHandleUpper1,"grab_timeout",-1);
        GrabImage(&ho_ImageUpper1, hv_AcqHandleUpper1);
        GenEmptyObj(&ho_empty);
        TestEqualObj(ho_ImageUpper1,ho_empty,&hv_n);

        //判断图像有无初始化
        if(!ho_ImageUpper1.IsInitialized())
        {
//            qDebug()<<"机台1上图像初始化失败";
            QLOG_WARN()<<u8"机台1上图像初始化失败";
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
            return;
        }
        //判断图像是否为空
        if(hv_n==1)
        {
//            qDebug()<<"机台1上相机图像为空";
            QLOG_WARN()<<u8"机台1上相机图像为空";
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
            return;
        }

        QString ss = QCoreApplication::applicationDirPath()+"/1.jpg";
        QByteArray ba = ss.toLatin1();
        const char* str = ba.data();
        HTuple tt(str);
        WriteImage(ho_ImageUpper1,"jpg",0,tt);

        if(GDataFactory::get_factory()->get_system_state() == NORMALTEST_SYSTEM_STATE)
        {
            QString barcode =  QString(read_bar_code(ho_ImageUpper1));

            QLOG_INFO()<<"camera1:the barcode from vision is:"<<barcode;
            if((barcode == "")||(barcode.length() != 10))
            {
                camera1_naked++;
                QLOG_ERROR()<<"camera1 current bar code is NULL or the length of barcode is not Correct";
                GDataFactory::get_factory()->set_bar_code_is_read_or_not(false);
                emit signal_send_image(camera_id,ss);
                if(camera1_naked == 3)
                {
                    camera1_naked = 0;
                    emit signal_send_image_info_to_top_main_window(camera_id,lstM,"naked-board");
                }
                else
                    emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
                return;
            }
            QString board_style = "";
            while (1) {
                board_style = GDataFactory::get_factory()->exec_http_post_to_get_board_style(barcode);
                if( board_style != "")
                    break;
                QThread::msleep(1000);
            }
            QLOG_INFO()<<"camera1:the boardtype from centralcontrol is:"<<board_style;
            if(board_style == "")
            {
                QLOG_ERROR()<<"**********************************ERROR***********************************";
                QLOG_ERROR()<<"the barcode from camera1 is:"<<barcode;
                QLOG_WARN()<<"the board_style from central control is:"<<board_style;
                GDataFactory::get_factory()->set_bar_code_is_read_or_not(false);
                emit signal_send_image(camera_id,ss);
                emit signal_send_image_info_to_top_main_window(camera_id,lstM,board_style);
                return;
            }
            else
            {
                QLOG_INFO()<<"camera1:the boardtype from central control is not NULL!";
                GDataFactory::get_factory()->set_bar_code_is_read_or_not(true);
                //检测插座
                GDataFactory::get_factory()->set_board_on_off_sign(detect_chazuo(ho_ImageUpper1,board_style));
            }

//            emit signal_send_image(camera_id,ss);

            double x,y,a;
            x = GDataFactory::get_factory()->get_config_para("CAMERA1_X").toDouble();
            y = GDataFactory::get_factory()->get_config_para("CAMERA1_Y").toDouble();
            a = GDataFactory::get_factory()->get_config_para("CAMERA1_A").toDouble();
            lstM = cudingwei_model(ho_ImageUpper1,camera_id,x,y,a);
            emit signal_send_image(camera_id,ss);
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,board_style);
        }
    }

    if(camera_id == 2)
    {
        current_take_pictrue_camera_id = -1;
        //取消采图延时,取消的参数需根据实际相机连接的参数确定
        SetFramegrabberParam(hv_AcqHandleDown1,"grab_timeout",-1);
        GrabImage(&ho_ImageDown1, hv_AcqHandleDown1);
        GenEmptyObj(&ho_empty);
        TestEqualObj(ho_ImageDown1,ho_empty,&hv_n);

        //判断图像有无初始化
        if(!ho_ImageDown1.IsInitialized())
        {
//            qDebug()<<"机台1上图像初始化失败";
            QLOG_WARN()<<u8"机台1下图像初始化失败";
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
            return;
        }
        //判断图像是否为空
        if(hv_n==1)
        {
//            qDebug()<<"机台1上相机图像为空";
            QLOG_WARN()<<u8"机台1下相机图像为空";
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
            return;
        }

        QString ss = QCoreApplication::applicationDirPath()+"/2.jpg";
        QByteArray ba = ss.toLatin1();
        const char* str = ba.data();
        HTuple tt(str);
        WriteImage(ho_ImageDown1,"jpg",0,tt);

        if(GDataFactory::get_factory()->get_system_state() == NORMALTEST_SYSTEM_STATE)
        {
            double x,y,a;
            x = GDataFactory::get_factory()->get_config_para("CAMERA2_X").toDouble();
            y = GDataFactory::get_factory()->get_config_para("CAMERA2_Y").toDouble();
            a = GDataFactory::get_factory()->get_config_para("CAMERA2_A").toDouble();
            lstM = jingdingwei_model(ho_ImageDown1,camera_id,"12345",x,y,a);
            emit signal_send_image(camera_id,ss);
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
        }
    }

    if(camera_id==3)
    {
        current_take_pictrue_camera_id = -1;
        //取消采图延时,取消的参数需根据实际相机连接的参数确定
        SetFramegrabberParam(hv_AcqHandleUpper2,"grab_timeout",-1);
        GrabImage(&ho_ImageUpper2, hv_AcqHandleUpper2);
        GenEmptyObj(&ho_empty);
        TestEqualObj(ho_ImageUpper2,ho_empty,&hv_n);

        //判断图像有无初始化
        if(!ho_ImageUpper2.IsInitialized())
        {
//            qDebug()<<"机台2上图像初始化失败";
            QLOG_WARN()<<u8"机台2上图像初始化失败";
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
            return;
        }
        //判断图像是否为空
        if(hv_n==1)
        {
//            qDebug()<<"机台2上相机图像为空";
            QLOG_WARN()<<u8"机台2上相机图像为空";
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
            return;
        }

        QString ss = QCoreApplication::applicationDirPath()+"/3.jpg";
        QByteArray ba = ss.toLatin1();
        const char* str = ba.data();
        HTuple tt(str);
        WriteImage(ho_ImageUpper2,"jpg",0,tt);

        if(GDataFactory::get_factory()->get_system_state() == NORMALTEST_SYSTEM_STATE)
        {
            QString barcode =  QString(read_bar_code(ho_ImageUpper2));
            QLOG_INFO()<<"camera3:the barcode from vision is:"<<barcode;
            if((barcode == "")||(barcode.length() != 10))
            {
                camera3_naked++;
                QLOG_ERROR()<<"camera3 current bar code is NULL or the length of barcode is not correct";
                GDataFactory::get_factory()->set_bar_code_is_read_or_not(false);
                emit signal_send_image(camera_id,ss);
                if(camera3_naked == 3)
                {
                    camera3_naked = 0;
                    emit signal_send_image_info_to_top_main_window(camera_id,lstM,"naked-board");
                }
                else
                    emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
                return;
            }

            QString board_style = "";
            while (1) {
                board_style = GDataFactory::get_factory()->exec_http_post_to_get_board_style(barcode);
                if( board_style != "")
                    break;
                QThread::msleep(100);
            }
            QLOG_INFO()<<"camera3:the boardtype from central control is:"<<board_style;
            if(board_style == "")
            {
                QLOG_ERROR()<<"**********************************ERROR***********************************";
                QLOG_ERROR()<<"the barcode from camera3 is:"<<barcode;
                QLOG_WARN()<<"the board_style from central control is:"<<board_style;
                GDataFactory::get_factory()->set_bar_code_is_read_or_not(false);
                emit signal_send_image(camera_id,ss);
                emit signal_send_image_info_to_top_main_window(camera_id,lstM,board_style);
                return;
            }
            else
            {
                QLOG_INFO()<<"camera3:the boardtype is not null";
                GDataFactory::get_factory()->set_bar_code_is_read_or_not(true);
                QLOG_INFO()<<"camera3:detect chazuo is over!";
            }


            double x,y,a;
            x = GDataFactory::get_factory()->get_config_para("CAMERA3_X").toDouble();
            y = GDataFactory::get_factory()->get_config_para("CAMERA3_Y").toDouble();
            a = GDataFactory::get_factory()->get_config_para("CAMERA3_A").toDouble();
            QLOG_INFO()<<"camera3 starts to cudingwei";
            lstM = cudingwei_model(ho_ImageUpper2,camera_id,x,y,a);
            QLOG_INFO()<<"camera3 ends cudingwei";
            emit signal_send_image(camera_id,ss);
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,board_style);
        }
    }

    if(camera_id == 4)
    {
        current_take_pictrue_camera_id = -1;
        //取消采图延时,取消的参数需根据实际相机连接的参数确定
        SetFramegrabberParam(hv_AcqHandleDown2,"grab_timeout",-1);
        GrabImage(&ho_ImageDown2, hv_AcqHandleDown2);
        GenEmptyObj(&ho_empty);
        TestEqualObj(ho_ImageDown2,ho_empty,&hv_n);

        //判断图像有无初始化
        if(!ho_ImageDown2.IsInitialized())
        {
//            qDebug()<<"机台1上图像初始化失败";
            QLOG_WARN()<<u8"机台2下图像初始化失败";
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
            return;
        }
        //判断图像是否为空
        if(hv_n==1)
        {
//            qDebug()<<"机台1上相机图像为空";
            QLOG_WARN()<<u8"机台2下相机图像为空";
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
            return;
        }

        QString ss = QCoreApplication::applicationDirPath()+"/4.jpg";
        QByteArray ba = ss.toLatin1();
        const char* str = ba.data();
        HTuple tt(str);
        WriteImage(ho_ImageDown2,"jpg",0,tt);

        if(GDataFactory::get_factory()->get_system_state() == NORMALTEST_SYSTEM_STATE)
        {
            double x,y,a;
            x = GDataFactory::get_factory()->get_config_para("CAMERA4_X").toDouble();
            y = GDataFactory::get_factory()->get_config_para("CAMERA4_Y").toDouble();
            a = GDataFactory::get_factory()->get_config_para("CAMERA4_A").toDouble();
            lstM = jingdingwei_model(ho_ImageDown2,camera_id,"12345",x,y,a);
            emit signal_send_image(camera_id,ss);
            emit signal_send_image_info_to_top_main_window(camera_id,lstM,"");
        }
    }
}

//相机标定，上下相机标定流程一致
//通过九点标定确定坐标转换矩阵，再通过不同旋转角度的三个点确定旋转中心
//根据坐标转换矩阵和旋转中心计算最终的机械手坐标和对应角度(弧度）

//上相机标定时，计算图像坐标点的函数
//此时输出的像素坐标为Halcon坐标系中的点，X/Y仍与实际相反
double vision::calibrate_upper(HObject ho_Image,int camera_id)
{
    double pixel_X,pixel_Y,pixel_A;
    if(camera_id==1)
    {
//        FindShapeModel(ho_Image, hv_ModelID_Upper1, HTuple(-60).TupleRad(), HTuple(120).TupleRad(), 0.4, 1, 0.5, "least_squares_high", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
        FindShapeModel(ho_Image, hv_ModelID_Upper1_jiaodu, HTuple(-180).TupleRad(), HTuple(360).TupleRad(), 0.5, 1, 0.5, "least_squares_high", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
//        int S=hv_Score.TupleLength().I();
        int S1=hv_Score.TupleLength().I();
        if(/*S==0 ||*/ S1==0)
        {
//            qDebug()<<"未找到匹配点";
            QLOG_WARN()<<u8"上相机1未找到匹配点";
            return -1;
            }
    }
    if(camera_id==3)
    {
        FindShapeModel(ho_Image, hv_ModelID_Upper2, HTuple(-60).TupleRad(), HTuple(120).TupleRad(), 0.4, 1, 0.5, "least_squares_high", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
        FindShapeModel(ho_Image, hv_ModelID_Upper2_jiaodu, HTuple(-180).TupleRad(), HTuple(360).TupleRad(), 0.2, 1, 0.5, "least_squares_high", 0,0.9, &hv_Row1, &hv_Column1, &hv_Angle1, &hv_Score1);
        int S=hv_Score.TupleLength().I();
        int S1=hv_Score.TupleLength().I();
        if(S==0 || S1==0)
        {
            QLOG_WARN()<<u8"上相机2未找到匹配点";
            return -1;
            }
    }
    //等到计算转换矩阵时，再将X/Y进行更换
//    QLOG_INFO()<<"hv_Row.D() is:"<<hv_Row.D();
    pixel_X=hv_Row.D();
    pixel_Y=hv_Column.D();
    pixel_A=hv_Angle.D();
    QList<double> tmpLst;
    tmpLst.push_back(pixel_X);
    tmpLst.push_back(pixel_Y);
    tmpLst.push_back(pixel_A);
    emit signal_send_calib(tmpLst);
    return 0.0;
}

//下相机标定
//此时输出的像素坐标为Halcon坐标系中的点，X/Y仍与实际相反
double vision::calibrate_down(HObject ho_Image,int camera_id)
{
   HTuple hv_Row1,hv_Col,hv_Column1,hv_Angle1,hv_Score1;
    HTuple hv_Row_Indices,hv_Column_Indices,hv_point_left_indices,hv_point_right_indices;

    HTuple hv_R,hv_C;
    double pixel_X,pixel_Y,pixel_A;

    //Emphasize(ho_Image, &ho_Image, 7, 7, 1);
    if(camera_id==2)
    {
         //FindShapeModel(ho_Image, hv_ModelID_Down1, 0, 3.14, 0.5, 0, 0.5, "least_squares", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
         FindShapeModel(ho_Image,hv_ModelID_Down1,HTuple(-60).TupleRad(),HTuple(120).TupleRad(),0.3, 1, 0.5, "least_squares_high", 0,0.8, &hv_Row1, &hv_Column1, &hv_Angle1, &hv_Score1);
         int S=hv_Score1.TupleLength().I();
         if(S==0)
         {
//             qDebug()<<"未找到匹配点";
             QLOG_WARN()<<u8"下相机1未找到匹配点";
             return -1;}
    }
    //寻找Mark点，便于确定中心点坐标
   if(camera_id==4)
   {
       FindShapeModel(ho_Image,hv_ModelID_Down2,HTuple(-60).TupleRad(),HTuple(120).TupleRad(),0.3, 1, 0.5, "least_squares_high", 0,0.8, &hv_Row1, &hv_Column1, &hv_Angle1, &hv_Score1);
//        FindShapeModel(ho_Image, hv_ModelID_Down2, 0, 3.14, 0.5, 0, 0.5, "least_squares", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
//        FindShapeModel(ho_Image,hv_ModelID_Down_Angle2,0,3.14,0.5, 0, 0.5, "least_squares", 0,0.9, &hv_Row1, &hv_Column1, &hv_Angle1, &hv_Score1);
       int S=hv_Score1.TupleLength().I();
       if(S==0)
       {
//           qDebug()<<"未找到匹配点";
           QLOG_WARN()<<u8"下相机2未找到匹配点";
           return -1;}
    }
    pixel_X=hv_Row1.D();
    pixel_Y=hv_Column1.D();
    pixel_A=hv_Angle1.D();

    QList<double> tmpLst;
    tmpLst.push_back(pixel_X);
    tmpLst.push_back(pixel_Y);
    tmpLst.push_back(pixel_A);
    emit signal_send_calib(tmpLst);

    return 0.0;
}

//计算标定后的转换矩阵及旋转中心
void vision::calibrate(int camera_id,double machine_x[12],double machine_y[12],double pixel_x[12],double pixel_y[12])
{
    //计算旋转中心
    //读取后三个点像素坐标，用来计算旋转中心
    //后三个点的机械坐标不变，只是旋转角度发生变化，拍摄图像后获取每个角度对应的模板中心的像素坐标
//    HTuple Columns=[pixel_x[9],pixel_x[10],pixel_x[11]];
//    HTuple Rows=[pixel_y[9],pixel_y[10],pixel_y[11]];
    HTuple hv_Rows,hv_Columns;
    //
    hv_Rows.Clear();
    hv_Rows[0]=pixel_x[9];
    QLOG_INFO()<<"hv_Rows[0]:"<<hv_Rows[0].D();
    hv_Rows[1]=pixel_x[10];
    QLOG_INFO()<<"hv_Rows[1]:"<<hv_Rows[1].D();
    hv_Rows[2]=pixel_x[11];
    QLOG_INFO()<<"hv_Rows[2]:"<<hv_Rows[2].D();
    //
    hv_Columns.Clear();
    hv_Columns[0]=pixel_y[9];
    QLOG_INFO()<<"hv_Columns[0]:"<<hv_Columns[0].D();
    hv_Columns[1]=pixel_y[10];
    QLOG_INFO()<<"hv_Columns[1]:"<<hv_Columns[1].D();
    hv_Columns[2]=pixel_y[11];
    QLOG_INFO()<<"hv_Columns[2]:"<<hv_Columns[2].D();

    //取前九个数，创建计算转换矩阵时的所需的机械坐标和对应的像素坐标（注意像素坐标的X、Y与机械坐标是反转的）
    //double a[9]={10.1,23.5,0.25,22.25,21.8,15.0,211.2,12.11,23.54};
   // memcpy(a, machine_x, sizeof(a));
    HTuple hv_pixel_x,hv_pixel_y,hv_machine_x,hv_machine_y;
    hv_pixel_x.Clear();
    hv_pixel_x[0]=pixel_x[0];
    hv_pixel_x[1]=pixel_x[1];
    hv_pixel_x[2]=pixel_x[2];
    hv_pixel_x[3]=pixel_x[3];
    hv_pixel_x[4]=pixel_x[4];
    hv_pixel_x[5]=pixel_x[5];
    hv_pixel_x[6]=pixel_x[6];
    hv_pixel_x[7]=pixel_x[7];
    hv_pixel_x[8]=pixel_x[8];
    hv_pixel_y.Clear();
    hv_pixel_y[0]=pixel_y[0];
    hv_pixel_y[1]=pixel_y[1];
    hv_pixel_y[2]=pixel_y[2];
    hv_pixel_y[3]=pixel_y[3];
    hv_pixel_y[4]=pixel_y[4];
    hv_pixel_y[5]=pixel_y[5];
    hv_pixel_y[6]=pixel_y[6];
    hv_pixel_y[7]=pixel_y[7];
    hv_pixel_y[8]=pixel_y[8];
    hv_machine_x.Clear();
    hv_machine_x[0]=machine_x[0];
    hv_machine_x[1]=machine_x[1];
    hv_machine_x[2]=machine_x[2];
    hv_machine_x[3]=machine_x[3];
    hv_machine_x[4]=machine_x[4];
    hv_machine_x[5]=machine_x[5];
    hv_machine_x[6]=machine_x[6];
    hv_machine_x[7]=machine_x[7];
    hv_machine_x[8]=machine_x[8];
    hv_machine_y.Clear();
    hv_machine_y[0]=machine_y[0];
    hv_machine_y[1]=machine_y[1];
    hv_machine_y[2]=machine_y[2];
    hv_machine_y[3]=machine_y[3];
    hv_machine_y[4]=machine_y[4];
    hv_machine_y[5]=machine_y[5];
    hv_machine_y[6]=machine_y[6];
    hv_machine_y[7]=machine_y[7];
    hv_machine_y[8]=machine_y[8];


    //确定旋转中心所需声明的参数
    HObject ho_Contour;
    HTuple hv_Row,hv_Column,hv_Radius,hv_StartPhi,hv_EndPhi,hv_PointerOrder;
//    HObject ho_ContCircle;
//    HTuple hv_DistanceMax,hv_DistanceMin;
    HTuple hv_Circle_X,hv_Circle_Y;

    GenContourPolygonXld(&ho_Contour,hv_Rows,hv_Columns);
    FitCircleContourXld(ho_Contour,"geotukey", -1, 0, 0, 3, 2, &hv_Row, &hv_Column,&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointerOrder);
     if(camera_id==1)
     {
         //计算转换矩阵
         VectorToHomMat2d(hv_pixel_y,hv_pixel_x,hv_machine_x,hv_machine_y,&hv_biaoding_tuple_Upper1);
         WriteTuple(hv_biaoding_tuple_Upper1,"E://hv_biaoding_tuple_Upper1");
         //将计算出的旋转中心转换为实际物理空间的旋转中心坐标
         AffineTransPoint2d(hv_biaoding_tuple_Upper1, hv_Column, hv_Row, &hv_Circle_X, &hv_Circle_Y);
         Rotate_X_Upper1=hv_Circle_X.D();
         Rotate_Y_Upper1=hv_Circle_Y.D();
         emit signal_send_rotate_center(Rotate_X_Upper1,Rotate_Y_Upper1);
     }
     if(camera_id==2)
     {
         //计算转换矩阵
         VectorToHomMat2d(hv_pixel_y,hv_pixel_x,hv_machine_x,hv_machine_y,&hv_biaoding_tuple_Down1);
         WriteTuple(hv_biaoding_tuple_Down1,"E://hv_biaoding_tuple_Down1");
         //将计算出的旋转中心转换为实际物理空间的旋转中心坐标
         AffineTransPoint2d(hv_biaoding_tuple_Down1, hv_Column, hv_Row, &hv_Circle_X, &hv_Circle_Y);
         Rotate_X_Down1=hv_Circle_X.D();
         Rotate_Y_Down1=hv_Circle_Y.D();
         emit signal_send_rotate_center(Rotate_X_Down1,Rotate_Y_Down1);
     }
     if(camera_id==3)
     {
         //计算转换矩阵
         VectorToHomMat2d(hv_pixel_y,hv_pixel_x,hv_machine_x,hv_machine_y,&hv_biaoding_tuple_Upper2);
         WriteTuple(hv_biaoding_tuple_Upper2,"E://hv_biaoding_tuple_Upper2");
         //将计算出的旋转中心转换为实际物理空间的旋转中心坐标
         AffineTransPoint2d(hv_biaoding_tuple_Upper2, hv_Column, hv_Row, &hv_Circle_X, &hv_Circle_Y);
         Rotate_X_Upper2=hv_Circle_X.D();
         Rotate_Y_Upper2=hv_Circle_Y.D();
         emit signal_send_rotate_center(Rotate_X_Upper2,Rotate_Y_Upper2);
     }
     if(camera_id==4)
     {
         //计算转换矩阵
         VectorToHomMat2d(hv_pixel_y,hv_pixel_x,hv_machine_x,hv_machine_y,&hv_biaoding_tuple_Down2);
         WriteTuple(hv_biaoding_tuple_Down2,"E://hv_biaoding_tuple_Down2");
         //将计算出的旋转中心转换为实际物理空间的旋转中心坐标
         AffineTransPoint2d(hv_biaoding_tuple_Down2, hv_Column, hv_Row, &hv_Circle_X, &hv_Circle_Y);
         Rotate_X_Down2=hv_Circle_X.D();
         Rotate_Y_Down2=hv_Circle_Y.D();
         emit signal_send_rotate_center(Rotate_X_Down2,Rotate_Y_Down2);
     }

    return;
}

//程序初始化，初始化时需读取的相关模板
void vision::initiallize()
{
    //条码模板创建及参数设置
    CreateBarCodeModel(HTuple(),HTuple(),&hv_BarCodeHandle);
    SetBarCodeParam(hv_BarCodeHandle,"stop_after_result_num",0);
    SetBarCodeParam(hv_BarCodeHandle,"min_code_length",10);
    SetBarCodeParam(hv_BarCodeHandle,"meas_thresh",0.2);
    //SetBarCodeParam(hv_BarCodeHandle,"num_scanlines",25);
    //各相机模板参数加载
    ReadShapeModel("E://ModelID_Upper1_xld",&hv_ModelID_Upper1);
    ReadShapeModel("E://ModelID_Upper1_jiaodu",&hv_ModelID_Upper1_jiaodu);
    ReadShapeModel("E://ModelID_Down1", &hv_ModelID_Down1);
    ReadShapeModel("E://ModelID_Upper2_xld",&hv_ModelID_Upper2);
    ReadShapeModel("E://ModelID_Upper2_jiaodu",&hv_ModelID_Upper2_jiaodu);//need to be changed,just for test
    ReadShapeModel("E://ModelID_Down2", &hv_ModelID_Down2);//need to be changed,just for test

    //各版型中检测插座的模板
    ReadShapeModel("E://ModelID_TuQi_11117",&hv_ModelID_TuQi_11117);
    ReadShapeModel("E://ModelID_TuQi_11389", &hv_ModelID_TuQi_11389);
    ReadShapeModel("E://ModelID_PingZhi_11117", &hv_ModelID_PingZhi_11117);
    ReadShapeModel("E://ModelID_TuQi_11133", &hv_ModelID_TuQi_11133);
    ReadShapeModel("E://ModelID_PingZhi_11133", &hv_ModelID_PingZhi_11133);


    return;

}

//视觉扫码
//条形码
char* vision::read_bar_code(HObject ho_Image)
{
    m_mutex.lock();
    //定义局部图像变量
    HObject ho_SymbolRegions;
    //定义局部控制变量
    HTuple hv_DecodedDataStrings;
    //读取图像并识别条码
    //ReadImage(&ho_Image,"对应相机采集到的图像，可考虑将图像设置为全局变量，直接读取即可");
    FindBarCode(ho_Image,&ho_SymbolRegions,hv_BarCodeHandle,"Code 128",&hv_DecodedDataStrings);
    int result=hv_DecodedDataStrings.TupleLength().I();
    if(result==0)
    {
//        qDebug()<<"条码未识别";
        QLOG_WARN()<<u8"条码未识别";
        m_mutex.unlock();
        return nullptr;
    }
    //输出最终条码读取结果
    //将halcon中的字符串转化为qt中的字符串格式
    //char* resultSN=new char[255];
    sprintf_s(resultSN,255,"%s",hv_DecodedDataStrings[0].S().Text());
    m_mutex.unlock();

    return resultSN;

}

//粗定位，采用两边定位孔创建模板进行定位
//其中基准点为上相机第五点对应的像素坐标，此坐标X/Y颠倒
QList<double> vision::cudingwei_model(HObject ho_Image,int camera_id,double Qx_base,double Qy_base,double Qa_base)
{
    QList<double> lstM;
    double x=-1;
    double y = -1;
    try {
        if(camera_id==1)
        {
//            FindShapeModel(ho_Image, hv_ModelID_Upper1, HTuple(-60).TupleRad(), HTuple(120).TupleRad(), 0.7, 1, 0.5, "least_squares_high", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
            FindShapeModel(ho_Image, hv_ModelID_Upper1_jiaodu, HTuple(-60).TupleRad(), HTuple(120).TupleRad(), 0.5, 1, 0.5, "least_squares_high", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
//            int S=hv_Score.TupleLength().I();
            int S1=hv_Score.TupleLength().I();
            if(/*S==0 ||*/ S1==0)
            {
                //QString msg=QStringLiteral("未找到匹配点");
                QLOG_WARN()<<u8"上相机1粗定位未找到匹配点";
                return lstM;
            }
            else{
                //获取匹配到的中心点像素坐标
                x=hv_Column.D();
                y=hv_Row.D();
                QLOG_WARN()<<"the center of the camera1 is:X:"<<QString::number(x)<<" Y:"<<QString::number(y);

//            //利用标定时的转换矩阵计算转换后的匹配点坐标
           AffineTransPoint2d(hv_biaoding_tuple_Upper1,hv_Column,hv_Row,&hv_Qx,&hv_Qy);
//           double qx = hv_Qx.D();


//            计算转换后的基准点坐标
            AffineTransPoint2d(hv_biaoding_tuple_Upper1,Qy_base,Qx_base,&hv_Qx_base,&hv_Qy_base);
//            double x = hv_Qx_base.D();
            //计算角度对应三角函数值
            HTuple hv_Del_A=hv_Angle-Qa_base;
            hv_Cos=hv_Del_A.TupleCos();
            hv_Sin=hv_Del_A.TupleSin();
            //偏差值计算
            hv_mid_X=(hv_Qx_base-Rotate_X_Upper1)*hv_Cos-(hv_Qy_base-Rotate_Y_Upper1)*hv_Sin+Rotate_X_Upper1;
            hv_mid_Y=(hv_Qx_base-Rotate_X_Upper1)*hv_Sin+(hv_Qy_base-Rotate_Y_Upper1)*hv_Cos+Rotate_Y_Upper1;

            double midx = hv_mid_X.D();
            double midy = hv_mid_Y.D();
            //计算最终输出坐标
            hv_robot_X=(hv_Qx_base-hv_mid_X)+hv_Qx;
            hv_robot_Y=(hv_Qy_base-hv_mid_Y)+hv_Qy;
            hv_robot_A=hv_Angle*180/M_PI-Qa_base;

            //直接取中心点位置
//            hv_robot_X=hv_Qx;
//            hv_robot_Y=hv_Qy;
//    //        hv_robot_A=(hv_Angle-(-3.13352))*180/M_PI;
//            hv_robot_A=hv_Angle1*180/M_PI;
            }
        }
        //相机2的计算方法未更改
        if(camera_id==3)
        {
            FindShapeModel(ho_Image, hv_ModelID_Upper2, HTuple(-60).TupleRad(), HTuple(120).TupleRad(), 0.7, 1, 0.5, "least_squares_high", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
            FindShapeModel(ho_Image, hv_ModelID_Upper2_jiaodu, HTuple(-180).TupleRad(), HTuple(360).TupleRad(), 0.3, 1, 0.5, "least_squares_high", 0,0.9, &hv_Row1, &hv_Column1, &hv_Angle1, &hv_Score1);
            int S=hv_Score.TupleLength().I();
            int S1=hv_Score1.TupleLength().I();
            if(S==0 || S1==0)
            {
                //QString msg=QStringLiteral("未找到匹配点");
                QLOG_WARN()<<u8"上相机2粗定位未找到匹配点";
                return lstM;
            }
            else{
                //获取匹配到的中心点像素坐标
                 x=hv_Column.D();
                 y=hv_Row.D();
                 QLOG_WARN()<<"the center of the camera2 is:X:"<<QString::number(x)<<" Y:"<<QString::number(y);

//            //利用标定时的转换矩阵计算转换后的匹配点坐标
            AffineTransPoint2d(hv_biaoding_tuple_Upper2,hv_Column,hv_Row,&hv_Qx,&hv_Qy);
            //计算转换后的基准点坐标
            AffineTransPoint2d(hv_biaoding_tuple_Upper2,Qy_base,Qx_base,&hv_Qx_base,&hv_Qy_base);
            //计算角度对应三角函数值
            HTuple hv_Del_A=hv_Angle;
            hv_Cos=hv_Del_A.TupleCos();
            hv_Sin=hv_Del_A.TupleSin();
            //偏差值计算
            hv_mid_X=(hv_Qx_base-Rotate_X_Upper2)*hv_Cos-(hv_Qy_base-Rotate_Y_Upper2)*hv_Sin+Rotate_X_Upper2;
            hv_mid_Y=(hv_Qx_base-Rotate_X_Upper2)*hv_Sin+(hv_Qy_base-Rotate_Y_Upper2)*hv_Cos+Rotate_Y_Upper2;
            //计算最终输出坐标
            hv_robot_X=(hv_Qx_base-hv_mid_X)+hv_Qx;
            hv_robot_Y=(hv_Qy_base-hv_mid_Y)+hv_Qy;
            hv_robot_A=hv_Angle1*180/M_PI;

//            //直接取中心点位置
//            hv_robot_X=hv_Qx;
//            hv_robot_Y=hv_Qy;
//    //        hv_robot_A=(hv_Angle-(-3.13352))*180/M_PI;
//            hv_robot_A=hv_Angle1*180/M_PI;
            }
        }
        //数值转换
        Robot_X=hv_robot_X.D();
        Robot_Y=hv_robot_Y.D();
        //角度的最终输出值需要根据机械手旋转角度基准值进行计算，和第五点时的旋转角度为基准计算偏差，以确定最终的角度输出值
        Robot_A=hv_robot_A.D();

        lstM.push_back(Robot_X);
        QLOG_INFO()<<"Robot_X:"<<Robot_X;
        lstM.push_back(Robot_Y);
        QLOG_INFO()<<"Robot_Y:"<<Robot_Y;
        lstM.push_back(Robot_A);
        QLOG_INFO()<<"Robot_A:"<<Robot_A;

        GDataFactory::get_factory()->set_pick_x_y(x,y);
        return lstM;
    }
    catch (...) {
                QLOG_ERROR()<<"cudingwei exist exception!";
                return lstM;
            }
}


//精定位：采用成对角线的Mark点进行匹配，找到板子的中心位置
//然后利用左边定位孔的模板进行匹配，确定板子的旋转角度
QList<double> vision::jingdingwei_model(HObject ho_Image,int camera_id,QString SN,double Qx_base,double Qy_base,double Qa_base)
{
    QList<double> lstM;
    //确定旋转角度的模板匹配结果
    HTuple hv_Row1,hv_Column1,hv_Angle1,hv_Score1;
    //计算对角线的Mark点时的中间过程参b数
//    HTuple hv_Column_Indices,hv_Row_Indices,hv_point_left_indices,hv_point_right_indices;
//    HTuple hv_C,hv_R;

      if(camera_id==2)
      {
          FindShapeModel(ho_Image, hv_ModelID_Down1, HTuple(-45).TupleRad(), HTuple(90).TupleRad(), 0.3, 1, 0.5, "least_squares_high",0, 0.9, &hv_Row1, &hv_Column1, &hv_Angle1, &hv_Score1);
          int S=hv_Score1.TupleLength().I();
          if(S==0)
          {
              QLOG_WARN()<<u8"下相机1精定位未找到匹配点";
              return lstM;
              }
          else
          {
              //计算角度对应三角函数值,此时的角度为弧度值
              HTuple hv_Del_A=hv_Angle1-Qa_base;
              hv_Del_A=-hv_Del_A;
              hv_Cos=hv_Del_A.TupleCos();
              hv_Sin=hv_Del_A.TupleSin();

                  //坐标转换
                  AffineTransPoint2d(hv_biaoding_tuple_Down1, hv_Column1, hv_Row1, &hv_Qx, &hv_Qy);
                  //计算转换后的九点坐标中的第五点的坐标,基准坐标转换
                  AffineTransPoint2d(hv_biaoding_tuple_Down1,Qy_base,Qx_base,&hv_Qx_base,&hv_Qy_base);
                  //偏差值计算
                  hv_mid_X=(hv_Qx_base-Rotate_X_Down1)*hv_Cos-(hv_Qy_base-Rotate_Y_Down1)*hv_Sin+Rotate_X_Down1;
                  hv_mid_Y=(hv_Qx_base-Rotate_X_Down1)*hv_Sin+(hv_Qy_base-Rotate_Y_Down1)*hv_Cos+Rotate_Y_Down1;
                  //计算最终输出坐标
                  hv_robot_X=(hv_Qx_base-hv_mid_X)+hv_Qx;
                  hv_robot_Y=(hv_Qy_base-hv_mid_Y)+hv_Qy;
                  //最终输出角度需要根据基准点和机械手之间的角度偏差，进行计算，以此为基准，计算出最终的偏转角度
                  hv_robot_A=-hv_Angle1*180/M_PI;
                  //数值转换
                  Robot_X=hv_robot_X.D();
                  Robot_Y=hv_robot_Y.D();
                  Robot_A=hv_robot_A.D();

                  double x_base=hv_Qx_base.D();
                  double y_base=hv_Qy_base.D();

                  //计算最终的输出值偏差，根据机械手的固定拍照位进行计算
                  Del_X=-(Robot_X-x_base);
                  Del_Y=-(Robot_Y-y_base);
                  Del_A=Robot_A;
          }
      }
      else if(camera_id==4)
      {
          FindShapeModel(ho_Image, hv_ModelID_Down2, HTuple(-45).TupleRad(), HTuple(90).TupleRad(), 0.3, 1, 0.5, "least_squares_high",0, 0.9, &hv_Row1, &hv_Column1, &hv_Angle1, &hv_Score1);

          int S=hv_Score1.TupleLength().I();
          if(S==0)
          {
              QLOG_WARN()<<u8"下相机2精定位未找到匹配点";
              return lstM;
              }
          else
          {
              double angle=hv_Angle1.D();
              QLOG_INFO()<<"Cal_Angle"<<angle;
              //计算角度对应三角函数值,此时的角度为弧度值
              HTuple hv_Del_A=hv_Angle1-Qa_base;
              hv_Del_A=-hv_Del_A;
              hv_Cos=hv_Del_A.TupleCos();
              hv_Sin=hv_Del_A.TupleSin();

              //坐标转换
              AffineTransPoint2d(hv_biaoding_tuple_Down2, hv_Column1, hv_Row1, &hv_Qx, &hv_Qy);
              //计算转换后的九点坐标中的第五点的坐标,基准坐标转换
              AffineTransPoint2d(hv_biaoding_tuple_Down2,Qy_base,Qx_base,&hv_Qx_base,&hv_Qy_base);
              //偏差值计算
              hv_mid_X=(hv_Qx_base-Rotate_X_Down2)*hv_Cos-(hv_Qy_base-Rotate_Y_Down2)*hv_Sin+Rotate_X_Down2;
              hv_mid_Y=(hv_Qx_base-Rotate_X_Down2)*hv_Sin+(hv_Qy_base-Rotate_Y_Down2)*hv_Cos+Rotate_Y_Down2;
              //计算最终输出坐标
              hv_robot_X=(hv_Qx_base-hv_mid_X)+hv_Qx;
              hv_robot_Y=(hv_Qy_base-hv_mid_Y)+hv_Qy;
              //最终输出角度需要根据基准点和机械手之间的角度偏差，进行计算，以此为基准，计算出最终的偏转角度
              hv_robot_A=-hv_Angle1*180/M_PI ;

              //数值转换
              Robot_X=hv_robot_X.D();
              QLOG_INFO()<<"------Robot_X------"<<Robot_X;
              Robot_Y=hv_robot_Y.D();
              QLOG_INFO()<<"------Robot_Y------"<<Robot_Y;
              Robot_A=hv_robot_A.D();
              QLOG_INFO()<<"------Robot_A------"<<Robot_A;

              double x_base=hv_Qx_base.D();
              QLOG_INFO()<<"------x_base------"<<x_base;
              double y_base=hv_Qy_base.D();
              QLOG_INFO()<<"------y_base------"<<y_base;

              //计算最终的输出值偏差，根据机械手的固定拍照位进行计算
              //都要改为负的，最终以该计算方式为准
              Del_X=-(Robot_X-x_base);
              Del_Y=-(Robot_Y-y_base);
              Del_A=-Robot_A;
      }
      }
//      int S=hv_Score1.TupleLength().I();
//      if(S==0)
//      {
//          qDebug()<<"未找到匹配点";
//          return lstM;
//          }
//      else
//      {
//          //计算角度对应三角函数值,此时的角度为弧度值
//          HTuple hv_Del_A=hv_Angle1-Qa_base;
//          hv_Del_A=-hv_Del_A;
//          hv_Cos=hv_Del_A.TupleCos();
//          hv_Sin=hv_Del_A.TupleSin();
//          if(camera_id==2)
//          {
//              //坐标转换
//              AffineTransPoint2d(hv_biaoding_tuple_Down1, hv_Column1, hv_Row1, &hv_Qx, &hv_Qy);
//              //计算转换后的九点坐标中的第五点的坐标,基准坐标转换
//              AffineTransPoint2d(hv_biaoding_tuple_Down1,Qy_base,Qx_base,&hv_Qx_base,&hv_Qy_base);
//              //偏差值计算
//              hv_mid_X=(hv_Qx_base-Rotate_X_Down1)*hv_Cos-(hv_Qy_base-Rotate_Y_Down1)*hv_Sin+Rotate_X_Down1;
//              hv_mid_Y=(hv_Qx_base-Rotate_X_Down1)*hv_Sin+(hv_Qy_base-Rotate_Y_Down1)*hv_Cos+Rotate_Y_Down1;
//              //计算最终输出坐标
//              hv_robot_X=(hv_Qx_base-hv_mid_X)+hv_Qx;
//              hv_robot_Y=(hv_Qy_base-hv_mid_Y)+hv_Qy;
//              //最终输出角度需要根据基准点和机械手之间的角度偏差，进行计算，以此为基准，计算出最终的偏转角度
//              hv_robot_A=-hv_Angle1*180/M_PI-134;
//          }
//          if(camera_id==4)
//          {
//              //坐标转换
//              AffineTransPoint2d(hv_biaoding_tuple_Down2, hv_Column1, hv_Row1, &hv_Qx, &hv_Qy);
//              //计算转换后的九点坐标中的第五点的坐标,基准坐标转换
//              AffineTransPoint2d(hv_biaoding_tuple_Down2,Qx_base,Qy_base,&hv_Qx_base,&hv_Qy_base);
//              //偏差值计算
//              hv_mid_X=(hv_Qx_base-Rotate_X_Down2)*hv_Cos-(hv_Qy_base-Rotate_Y_Down2)*hv_Sin+Rotate_X_Down2;
//              hv_mid_Y=(hv_Qx_base-Rotate_X_Down2)*hv_Sin+(hv_Qy_base-Rotate_Y_Down2)*hv_Cos+Rotate_Y_Down2;
//              //计算最终输出坐标
//              hv_robot_X=(hv_Qx_base-hv_mid_X)+hv_Qx;
//              hv_robot_Y=(hv_Qy_base-hv_mid_Y)+hv_Qy;
//              //最终输出角度需要根据基准点和机械手之间的角度偏差，进行计算，以此为基准，计算出最终的偏转角度
//              hv_robot_A=-hv_Angle*180/M_PI-134 ;
//          }
//      }
//      //数值转换
//      Robot_X=hv_robot_X.D();
//      Robot_Y=hv_robot_Y.D();
//      Robot_A=hv_robot_A.D();

//      double x_base=hv_Qx_base.D();
//      double y_base=hv_Qy_base.D();

//      //计算最终的输出值偏差，根据机械手的固定拍照位进行计算
//      Del_X=Robot_X-x_base-(-0.190959);
//      Del_Y=Robot_Y-y_base-0.0620412;
//      Del_A=Robot_A-(-134);

      lstM.push_back(Del_X);
      QLOG_INFO()<<"Del_X:"<<Del_X;
      lstM.push_back(Del_Y);
      QLOG_INFO()<<"Del_Y:"<<Del_Y;
      lstM.push_back(Del_A);
      QLOG_INFO()<<"Del_A:"<<Del_A;
      return lstM;

}

bool vision::detect_chazuo(HObject ho_Image, QString SN)
{
    QList<int> detect_result;
    int plain_number=0;
    int hump_number=0;
    //先匹配各版型都有的边缘有凸起部分的插座，查看匹配结果
    if(SN=="11117"||SN=="10545")
    {
        FindShapeModel(ho_Image, hv_ModelID_TuQi_11117, HTuple(-60).TupleRad(), HTuple(120).TupleRad(), 0.3, 1, 0.5, "least_squares", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
        hump_number=hv_Score.TupleLength().I();
    }
    else
    {
        FindShapeModel(ho_Image, hv_ModelID_TuQi_11133, HTuple(-60).TupleRad(), HTuple(120).TupleRad(), 0.3, 1, 0.5, "least_squares", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
        hump_number=hv_Score.TupleLength().I();
    }
//    FindShapeModel(ho_Image, hv_ModelID_TuQi_11117, HTuple(-180).TupleRad(), HTuple(360).TupleRad(), 0.6, 1, 0.5, "least_squares", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
//    hump_number=hv_Score.TupleLength().I();
//    if(hump_number == 0)
//    {
//        FindShapeModel(ho_Image, hv_ModelID_TuQi_11389, HTuple(-180).TupleRad(), HTuple(360).TupleRad(), 0.6, 1, 0.5, "least_squares", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
//        hump_number=hv_Score.TupleLength().I();
//    }
//    hump_number = 1;
    //检测各版型平直部分的插座
    if(SN=="11117"||SN=="11389"||SN=="11254"||SN=="10545"||SN=="11383")
    {
        FindShapeModel(ho_Image, hv_ModelID_PingZhi_11117, HTuple(-60).TupleRad(), HTuple(120).TupleRad(), 0.4, 2, 0.5, "least_squares", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
        plain_number=hv_Score.TupleLength().I();
    }
//    else if(SN=="10707")
//    {
//        FindShapeModel(ho_Image, hv_ModelID_PingZhi_10707, HTuple(-180).TupleRad(), HTuple(360).TupleRad(), 0.4, 0, 0.5, "least_squares", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
//        plain_number=hv_Score.TupleLength().I();
//    }
    else if(SN=="11133")
    {
        FindShapeModel(ho_Image, hv_ModelID_PingZhi_11133, HTuple(-60).TupleRad(), HTuple(120).TupleRad(), 0.4, 2, 0.5, "least_squares", 0,0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
        plain_number=hv_Score.TupleLength().I();
    }
    else {
        return false;
    }

    int number=plain_number+hump_number;
    if(number<3)
        number=0;
    else
        number=1;
//    detect_result.push_back(number);
    return number;

}

void vision::set_normal_test_para()
{
    ReadTuple("E://hv_biaoding_tuple_Upper1",&hv_biaoding_tuple_Upper1);
    ReadTuple("E://hv_biaoding_tuple_Down1",&hv_biaoding_tuple_Down1);
    ReadTuple("E://hv_biaoding_tuple_Upper2",&hv_biaoding_tuple_Upper2);
    ReadTuple("E://hv_biaoding_tuple_Down2",&hv_biaoding_tuple_Down2);
    QLOG_INFO()<<"load success";
    QList<double> lstCenter;
    lstCenter = GDataFactory::get_factory()->get_rotate_center(1);
    if(lstCenter.length()!=2)
    {
        QLOG_WARN()<<"load Rotate Center ERROR";
        return;
    }
    Rotate_X_Upper1 = lstCenter[0];
    Rotate_Y_Upper1 = lstCenter[1];

    lstCenter = GDataFactory::get_factory()->get_rotate_center(2);
    if(lstCenter.length()!=2)
    {
        QLOG_WARN()<<"load Rotate Center ERROR";
        return;
    }
    Rotate_X_Down1 = lstCenter[0];
    Rotate_Y_Down1 = lstCenter[1];

    lstCenter = GDataFactory::get_factory()->get_rotate_center(3);
    if(lstCenter.length()!=2)
    {
        QLOG_WARN()<<"load Rotate Center ERROR";
        return;
    }
    Rotate_X_Upper2 = lstCenter[0];
    Rotate_Y_Upper2 = lstCenter[1];

    lstCenter = GDataFactory::get_factory()->get_rotate_center(4);
    if(lstCenter.length()!=2)
    {
        QLOG_WARN()<<"load Rotate Center ERROR";
        return;
    }
    Rotate_X_Down2 = lstCenter[0];
    Rotate_Y_Down2 = lstCenter[1];
}

void vision::close_some_camera(int camera_id)
{
    if(camera_id == 1)
        CloseFramegrabber(hv_AcqHandleUpper1);
    else if(camera_id == 2)
        CloseFramegrabber(hv_AcqHandleDown1);
    else if(camera_id == 3)
        CloseFramegrabber(hv_AcqHandleUpper2);
    else if(camera_id == 4)
        CloseFramegrabber(hv_AcqHandleDown2);
    else {
        CloseAllFramegrabbers();
    }
}

void vision::clear_some_model()
{
        ClearShapeModel(hv_ModelID_Upper1);
        ClearShapeModel(hv_ModelID_Upper2);

        ClearShapeModel(hv_ModelID_Down1);
        ClearShapeModel(hv_ModelID_Down2);

        ClearShapeModel(hv_ModelID_Upper1_jiaodu);
        ClearShapeModel(hv_ModelID_Upper2_jiaodu);

        ClearShapeModel(hv_ModelID_TuQi_11117);
        ClearShapeModel(hv_ModelID_TuQi_11389);
        ClearShapeModel(hv_ModelID_PingZhi_11117);
        ClearShapeModel(hv_ModelID_PingZhi_10707);
        ClearShapeModel(hv_ModelID_PingZhi_11133);

        ClearBarCodeModel(hv_BarCodeHandle);

}

void vision::run()
{
    while (1)
    {
        get_image_normal_test_by_thread();
    }
    QLOG_ERROR()<<"the vision thread is exit!";
}


