#include "imagewidget.h"
#include <QPainter>
#include <QCoreApplication>
#include <QDebug>
#include <QPoint>
#include "gdatafactory.h"
ImageWidget::ImageWidget(QWidget *parent) : QWidget(parent)
{
    m_pDrawImageSign = false;
    m_pCameraID = 0;
    m_pPickPtX = 2048;
    m_pPickPtY = 1500;
}

void ImageWidget::set_image_x_y(int x, int y)
{
    m_pPickPtX = x;
    m_pPickPtY = y;
}

void ImageWidget::draw_red_cross(int center_x, int center_y)
{
    if(GDataFactory::get_factory()->get_system_state() == CLIBRATION_SYSTEM_STATE)
        return;
    QPainter painter(this);
    painter.setPen(QPen(QColor(255,0,0,150),2));
    QPoint center1(center_x-3,center_y-3);
    QPoint center2(center_x+3,center_y-3);
    QPoint center3(center_x-3,center_y+3);
    QPoint center4(center_x+3,center_y+3);
    QLine lineF4[] = {QLine(center1,QPoint(center1.x()-5,center1.y())),
                     QLine(center1,QPoint(center1.x(),center1.y()-5)),
                     QLine(center2,QPoint(center2.x()+5,center2.y())),
                     QLine(center2,QPoint(center2.x(),center2.y()-5)),
                     QLine(center3,QPoint(center3.x()-5,center3.y())),
                     QLine(center3,QPoint(center3.x(),center3.y()+5)),
                     QLine(center4,QPoint(center4.x()+5,center4.y())),
                     QLine(center4,QPoint(center4.x(),center4.y()+5))};

    QLine lineF[] = {QLine(center_x-10,center_y,center_x+10,center_y),
                    QLine(center_x,center_y-10,center_x,center_y+10)};
    painter.drawLines(lineF4,8);
    painter.drawLines(lineF,2);

    QPoint pt1(center_x-20,center_y),pt2(center_x+20,center_y),
            pt3(center_x,center_y-20),pt4(center_x,center_y+20);
    QPainterPath path1;
    QPolygonF plyg,plyg1,plyg2,plyg3;
    plyg<<QPointF(pt1)<<QPointF(pt1.x()+10,pt1.y()-8)
       <<QPointF(pt1.x()+10,pt1.y()+8);
    plyg1<<QPointF(pt2)<<QPointF(pt2.x()-10,pt2.y()-8)
        <<QPointF(pt2.x()-10,pt2.y()+8);
    plyg2<<QPointF(pt3)<<QPointF(pt3.x()-8,pt3.y()+10)
        <<QPointF(pt3.x()+8,pt3.y()+10);
    plyg3<<QPointF(pt4)<<QPointF(pt4.x()-8,pt4.y()-10)
        <<QPointF(pt4.x()+8,pt4.y()-10);
    path1.addPolygon(plyg);
    path1.addPolygon(plyg1);
    path1.addPolygon(plyg2);
    path1.addPolygon(plyg3);
    painter.fillPath(path1,QBrush(QColor(255,0,0,150)));
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(this->rect(),QBrush(QColor(207,207,214)));
    if(m_pCameraID == 0)
    {
        painter.setFont(QFont(u8"KaiTi",50,QFont::Bold,false));
        painter.drawText(this->rect(),Qt::AlignHCenter | Qt::AlignVCenter,u8"视觉图像");
    }
    else
    {
        painter.setFont(QFont(u8"KaiTi",20,QFont::Bold,false));
//        painter.setPen(QColor(0,255,0,100));
//        QString ss = QCoreApplication::applicationDirPath();
        if(m_pCameraID == 1)
        {
            QPixmap tmpPixmap = QPixmap(m_pImageName);
            painter.drawPixmap(this->rect(),tmpPixmap);
            painter.drawText(this->rect(),Qt::AlignHCenter | Qt::AlignTop,u8"\n上相机1");
            draw_red_cross(m_pPickPtX*this->rect().width()/tmpPixmap.width(),m_pPickPtY*this->rect().height()/tmpPixmap.height());
        }
        else if(m_pCameraID == 2)
        {
//            painter.drawPixmap(this->rect(),QPixmap(m_pImageName));
//            painter.drawText(this->rect(),Qt::AlignHCenter | Qt::AlignTop,u8"下相机1");
            QPixmap tmpPixmap = QPixmap(m_pImageName);
            painter.drawPixmap(this->rect(),tmpPixmap);
            painter.drawText(this->rect(),Qt::AlignHCenter | Qt::AlignTop,u8"\n下相机1");
//            draw_red_cross(m_pPickPtX*this->rect().width()/tmpPixmap.width(),m_pPickPtY*this->rect().height()/tmpPixmap.height());
        }
        else if(m_pCameraID == 3)
        {
            QPixmap tmpPixmap = QPixmap(m_pImageName);
            painter.drawPixmap(this->rect(),QPixmap(m_pImageName));
            painter.drawText(this->rect(),Qt::AlignHCenter | Qt::AlignTop,u8"上相机2");
            draw_red_cross(m_pPickPtX*this->rect().width()/tmpPixmap.width(),m_pPickPtY*this->rect().height()/tmpPixmap.height());
        }
        else
        {
            QPixmap tmpPixmap = QPixmap(m_pImageName);
            painter.drawPixmap(this->rect(),tmpPixmap);
            painter.drawText(this->rect(),Qt::AlignHCenter | Qt::AlignTop,u8"\n下相机2");
//            draw_red_cross(m_pPickPtX*this->rect().width()/tmpPixmap.width(),m_pPickPtY*this->rect().height()/tmpPixmap.height());
//            painter.drawPixmap(this->rect(),QPixmap(m_pImageName));
//            painter.drawText(this->rect(),Qt::AlignHCenter | Qt::AlignTop,u8"下相机2");
         }
    }
}

void ImageWidget::slot_rev_halcon_image(int cameraID,QString imageName)
{
    m_pDrawImageSign = true;
    m_pCameraID = cameraID;
    m_pImageName = imageName;
    this->repaint();
}
