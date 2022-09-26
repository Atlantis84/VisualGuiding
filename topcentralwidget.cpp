#include "topcentralwidget.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include "gdatafactory.h"
TopCentralWidget::TopCentralWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout* hAll = new QHBoxLayout();
    QVBoxLayout* vBoxLeft = new QVBoxLayout();
    QVBoxLayout* vBoxRight = new QVBoxLayout();

    QGroupBox* groupBoxLeft = new QGroupBox(u8"Í¼ÏñA");
    QGroupBox* groupBoxRight = new QGroupBox(u8"Í¼ÏñB");
    groupBoxRight->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");
    groupBoxLeft->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");

    vBoxLeft->addWidget(GDataFactory::get_image_wgtA());
    vBoxRight->addWidget(GDataFactory::get_image_wgtB());

    groupBoxLeft->setLayout(vBoxLeft);
    groupBoxRight->setLayout(vBoxRight);
    hAll->addWidget(groupBoxLeft);
    hAll->addWidget(groupBoxRight);
//    hAll->setSpacing(0);
//    hAll->setMargin(0);
    this->setLayout(hAll);
}

void TopCentralWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(this->rect(),QBrush(QColor(235,236,240)));
    painter.setPen(QPen(QColor(0,0,0,100),2));
    painter.drawRect(this->rect());
}
