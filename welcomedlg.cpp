#include "welcomedlg.h"
#include <QPainter>
#include "gdatafactory.h"
WelcomeDlg::WelcomeDlg()
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    setAttribute(Qt::WA_TranslucentBackground,true);
    this->resize(1920,1080);
    this->startTimer(100);
}

int count1 = 100;
int rotateValue = 0;
void WelcomeDlg::timerEvent(QTimerEvent *event)
{
    if(count1 == 0)
        this->close();
    this->setWindowOpacity((float)count1/100.0);
    this->repaint();
    count1-=2;
    rotateValue += 5;
}

void WelcomeDlg::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
//    painter.drawPixmap(this->rect(),GDataFactory::get_vision_obj()->get_halcon_to_qpixmap());
//    return;
    int CenterX,CenterY;
    CenterX = this->width()/2;
    CenterY = this->height()/2;
    painter.translate(CenterX,CenterY);
    painter.rotate(rotateValue);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPixmap *img = new QPixmap(":/icon/hisense2.png");
    img->scaled(960,520,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    QPainterPath path;
    path.addEllipse(-480,-260,960,520);
    QBrush br(*img);
    painter.setBrush(br);
    painter.setBrushOrigin(480,260);//设置画刷规则，不设置，画刷默认为平铺绘制
    painter.drawPath(path);

//    QRadialGradient radialGradient(this->width()/2,this->height()/2,this->width()/2,this->width()/2,this->height()/2);
    QRadialGradient radialGradient(0,
                                   0,//前两个参数是圆心
                                   1080,//该参数为圆的半径
                                   0,//焦点的x,y值
                                   0);
    radialGradient.setColorAt(0,QColor(0,255,0,100));
    radialGradient.setColorAt(1.0,QColor(255,0,0,100));
    painter.fillPath(path,QBrush(radialGradient));

    int l,t,r;
    r = 100;
    l = -100;
    t = -100;
    QPainter painter1(this);
    painter1.setRenderHint(QPainter::Antialiasing,true);
    painter1.translate(CenterX,CenterY);
    painter1.rotate(rotateValue);
    QPainterPath path1,path2;
    path1.moveTo(0,0);
    path2.moveTo(0,0);
    path1.arcTo(l,t,2*r,2*r,90,180);
    path2.arcTo(l,t,2*r,2*r,-90,180);
    painter1.fillPath(path1,QBrush(QColor(255,255,255)));
    painter1.fillPath(path2,QBrush(QColor(0,0,0)));

    QPainterPath path3,path4;
    path3.addEllipse(-r/2,-r,r,r);
    path4.addEllipse(-r/2,0,r,r);
    painter1.fillPath(path3,QBrush(QColor(255,255,255)));
    painter1.fillPath(path4,QBrush(QColor(0,0,0)));

    QPainterPath path5,path6;
    path5.addEllipse(-r/4,-3*r/4,r/2,r/2);
    path6.addEllipse(-r/4,r/4,r/2,r/2);
    painter1.fillPath(path5,QBrush(QColor(0,0,0)));
    painter1.fillPath(path6,QBrush(QColor(255,255,255)));

}

