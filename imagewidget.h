#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QPainter>
class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = nullptr);
    void set_image_x_y(int x,int y);
    void draw_red_cross(int center_x,int center_y);
private:
    void paintEvent(QPaintEvent *event);
    bool m_pDrawImageSign;
    int m_pCameraID;
    QString m_pImageName;
    QPixmap m_pHalconImage;
    int m_pPickPtX,m_pPickPtY;
public:

signals:

public slots:
    void slot_rev_halcon_image(int cameraID,QString imageName);
};

#endif // IMAGEWIDGET_H
