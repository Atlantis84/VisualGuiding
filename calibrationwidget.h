#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidgetItem>
class QTableWidget;
class QPushButton;
class CalibrationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CalibrationWidget(QWidget *parent = nullptr);
private:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *event);
    void set_table_data(int cameraid);
    void timerEvent(QTimerEvent *event);
    QWidget* createTableWidget();
    QTableWidget* tableWidget;
    QComboBox* combCamera;
    QPushButton* btnStart;
    QPushButton* btnSave;
    QPushButton* btnClear;
    QPushButton* btnStop;
    void calibration_process();
    int currentCalibrationCameraID;//current calibration camera id
    int currentCalibrationEntry;//current calibration entry
    void read_take_picture_sign_of_four_camera();
    int timeOutCount;
    int m_timer_id;
    void calculate_and_save_vision_info();
//    itemChanged(QTableWidgetItem *)
public:
    int get_current_camera_id(){return currentCalibrationCameraID;}
    void set_current_entry(int entryID){currentCalibrationEntry = entryID;}
signals:

public slots:
    void slot_SaveCalibrationData();
    void slot_clear_data();
    void slot_tablewgt_cell_changed(int row,int column);
    void slot_start_calibration();
    void slot_item_changed(QTableWidgetItem * item);
    void slot_comb_index_changed(int index);
    void slot_stop_calibration();
    void slot_rev_plc_calibration();
    void slot_rev_take_picture_sign_from_plc(QByteArray takepicturesign);
    void slot_rev_camera_pix_para(QList<double> pixlst);
    void slot_rev_vision_rotate_center(double centerX,double centerY);
};

#endif // CALIBRATIONWIDGET_H
