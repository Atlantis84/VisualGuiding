#include "topbottomwidget.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QGroupBox>

#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include "QsLog.h"
TopBottomWidget::TopBottomWidget(QWidget *parent) : QWidget(parent)
{
    m_pLogEdit = new QTextEdit();
    m_pLogEdit->setReadOnly(true);
//    m_pLogEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(216,217,222);font: 10pt 'LiSu'");
    m_pLogEdit->setStyleSheet("color:rgb(0,0,0);background-color:rgb(0,0,0);font: 10pt 'LiSu'");
    QHBoxLayout* hAll = new QHBoxLayout();
    QGroupBox* grpBox1 = new QGroupBox(u8"引导信息");
    grpBox1->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");
    QGroupBox* grpBox2 = new QGroupBox(u8"日志信息");
    grpBox2->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");

    QVBoxLayout* vBox1 = new QVBoxLayout();
    vBox1->addWidget(createTableWidget());
    QVBoxLayout* vBox2 = new QVBoxLayout();
    vBox2->addWidget(m_pLogEdit);

    grpBox1->setLayout(vBox1);
    grpBox2->setLayout(vBox2);

    hAll->addWidget(grpBox1);
    hAll->addWidget(grpBox2);
    this->setLayout(hAll);
}

void TopBottomWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(this->rect(),QBrush(QColor(235,236,240)));
    painter.setPen(QPen(QColor(0,0,0,100),2));
    painter.drawRect(this->rect());
}

QWidget *TopBottomWidget::createTableWidget()
{
    QFont* font = new QFont(u8"LiSu", 15, QFont::StyleNormal);
    tableWidget = new QTableWidget(4,4);

    for(int i = 0; i < 4; i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        QTableWidgetItem *item3 = new QTableWidgetItem();
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        item1->setFlags(item->flags() & (~Qt::ItemIsEditable));
        item2->setFlags(item->flags() & (~Qt::ItemIsEditable));
        item3->setFlags(item->flags() & (~Qt::ItemIsEditable));
        tableWidget->setItem(i, 0, item);
        tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        tableWidget->setItem(i, 1, item1);
        tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        tableWidget->setItem(i, 2, item2);
        tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        tableWidget->setItem(i, 3, item3);
        tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
     }

    tableWidget->setFont(*font);
    tableWidget->setWindowTitle("QTableWidget & Item");
    QStringList header;
    header<<u8"相机ID"<<u8"像素X"<<u8"像素Y"<<u8"角度θ";
    tableWidget->setHorizontalHeaderLabels(header);
//    tableWidget->horizontalHeader()->setFont(*font);
//    tableWidget->setAlternatingRowColors(true);
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{border-color:rgb(0,0,0);color:rgb(0,0,0);background-color:rgba(216,217,222,255);}");
    tableWidget->setStyleSheet("QTableWidget{color:rgb(0,0,0);"
                               "background-color:rgba(0,0,0,0);}"
                               "QTableWidget::item::selected{color:rgb(0,0,0);"
                               "background-color:rgb(207,207,217);}");
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    tableWidget->verticalHeader()->setFont(*font);
    tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{border-color:rgb(0,0,0);color:rgb(0,0,0);background-color:rgba(216,217,222,255);}");
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    slot_InsertDataToTable();//test
    return tableWidget;
}

void TopBottomWidget::slot_rev_logs(const QString msg)
{
    this->m_pLogEdit->append(msg);
}

void TopBottomWidget::slot_rev_http_data(QByteArray *data)
{
    //update board-style info
}

void TopBottomWidget::slot_rev_vision_info(int camera_id, QList<double> vision_info)
{
//    if(camera_id == 3)
//    {
//        tableWidget->item(camera_id - 2,0)->setText(QString::number(camera_id));
//        tableWidget->item(camera_id - 2,1)->setText(QString::number(vision_info[0]));
//        tableWidget->item(camera_id - 2,2)->setText(QString::number(vision_info[1]));
//        tableWidget->item(camera_id - 2,3)->setText(QString::number(vision_info[2]));
//    }
//    else {
        tableWidget->item(camera_id - 1,0)->setText(QString::number(camera_id));
        tableWidget->item(camera_id - 1,1)->setText(QString::number(vision_info[0]));
        tableWidget->item(camera_id - 1,2)->setText(QString::number(vision_info[1]));
        tableWidget->item(camera_id - 1,3)->setText(QString::number(vision_info[2]));
//    }

//    QLOG_INFO()<<"vision_info[0]:"<<vision_info[0];
//    QLOG_INFO()<<"vision_info[1]:"<<vision_info[1];
//    QLOG_INFO()<<"vision_info[2]:"<<vision_info[2];
}
