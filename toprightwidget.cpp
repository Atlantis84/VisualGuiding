#include "toprightwidget.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include "QsLog.h"
TopRightWidget::TopRightWidget(QWidget *parent) : QWidget(parent)
{
    lt_project_id = new QLineEdit();
    lt_project_id->setEnabled(false);
    lt_project_id->setAlignment(Qt::AlignCenter);
    lt_project_id->setStyleSheet("min-height:30px;");

    lt_half_material_number = new QLineEdit();
    lt_half_material_number->setEnabled(false);
    lt_half_material_number->setAlignment(Qt::AlignCenter);
    lt_half_material_number->setStyleSheet("min-height:30px;");

    lt_machine_style_name = new QLineEdit();
    lt_machine_style_name->setEnabled(false);
    lt_machine_style_name->setAlignment(Qt::AlignCenter);
    lt_machine_style_name->setStyleSheet("min-height:30px;");

    lt_board_type = new QLineEdit();
    lt_board_type->setEnabled(false);
    lt_board_type->setAlignment(Qt::AlignCenter);
    lt_board_type->setStyleSheet("min-height:30px;");

    lt_board_size = new QLineEdit();
    lt_board_size->setEnabled(false);
    lt_board_size->setAlignment(Qt::AlignCenter);
    lt_board_size->setStyleSheet("min-height:30px;");

    lt_board_sn = new QLineEdit();
    lt_board_sn->setEnabled(false);
    lt_board_sn->setAlignment(Qt::AlignCenter);
    lt_board_sn->setStyleSheet("min-height:30px;");

    QGroupBox* grpSN = new QGroupBox(u8"视觉扫码信息");
    grpSN->setStyleSheet("QGroupBox{color:rgb(0,0,0);border:1px solid rgba(0,0,0,100);}");
    QGroupBox* grpMES = new QGroupBox(u8"MES信息");
    grpMES->setStyleSheet("QGroupBox{color:rgb(0,0,0);border:1px solid rgba(0,0,0,100);}");

    QVBoxLayout* vAll = new QVBoxLayout();
    QVBoxLayout* vTop = new QVBoxLayout();
    QHBoxLayout* hBox_1 = new QHBoxLayout();
    hBox_1->addWidget(new QLabel(u8"  工单号:"));
    hBox_1->addWidget(lt_project_id);
    QHBoxLayout* hBox_2 = new QHBoxLayout();
    hBox_2->addWidget(new QLabel(u8"半品料号:"));
    hBox_2->addWidget(lt_half_material_number);
    QHBoxLayout* hBox_3 = new QHBoxLayout();
    hBox_3->addWidget(new QLabel(u8"机型名称:"));
    hBox_3->addWidget(lt_machine_style_name);
    QHBoxLayout* hBox_4 = new QHBoxLayout();
    hBox_4->addWidget(new QLabel(u8"    板型:"));
    hBox_4->addWidget(lt_board_type);
    QHBoxLayout* hBox_5 = new QHBoxLayout();
    hBox_5->addWidget(new QLabel(u8"板型尺寸:"));
    hBox_5->addWidget(lt_board_size);
    vTop->addLayout(hBox_1);
    vTop->addLayout(hBox_2);
    vTop->addLayout(hBox_3);
    vTop->addLayout(hBox_4);
    vTop->addLayout(hBox_5);
    grpMES->setLayout(vTop);

    QHBoxLayout* hBox_6 = new QHBoxLayout();
    hBox_6->addWidget(new QLabel(u8" 产品SN:"));
    hBox_6->addWidget(lt_board_sn);
    grpSN->setLayout(hBox_6);

    vAll->addWidget(grpSN);
    vAll->addWidget(grpMES);
    vAll->setStretch(0,2);
    vAll->setStretch(1,8);
    this->setLayout(vAll);
}

void TopRightWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(this->rect(),QBrush(QColor(235,236,240)));
    painter.setPen(QPen(QColor(0,0,0,100),2));
    painter.drawRect(this->rect());
}

void TopRightWidget::slot_test_rev_from_central_control(QList<QString> lstinfo)
{
//    if(lstinfo.length() != 6)
//        return;
    if(lstinfo.length() == 1)
        lt_board_sn->setText(lstinfo[0]);
    else if(lstinfo.length() == 2)
    {
        lt_board_sn->setText(lstinfo[0]);
        this->lt_project_id->setText(lstinfo[1]);
    }
    else if(lstinfo.length() == 3)
    {
        lt_board_sn->setText(lstinfo[0]);
        this->lt_project_id->setText(lstinfo[1]);
        lt_half_material_number->setText(lstinfo[2]);
    }
    else if(lstinfo.length() == 4)
    {
        lt_board_sn->setText(lstinfo[0]);
        this->lt_project_id->setText(lstinfo[1]);
        lt_half_material_number->setText(lstinfo[2]);
        lt_machine_style_name->setText(lstinfo[3]);
    }
    else if(lstinfo.length() == 5)
    {
        lt_board_sn->setText(lstinfo[0]);
        this->lt_project_id->setText(lstinfo[1]);
        lt_half_material_number->setText(lstinfo[2]);
        lt_machine_style_name->setText(lstinfo[3]);
        lt_board_type->setText(lstinfo[4]);
    }
    else if(lstinfo.length() == 6)
    {
        lt_board_sn->setText(lstinfo[0]);
        this->lt_project_id->setText(lstinfo[1]);
        lt_half_material_number->setText(lstinfo[2]);
        lt_machine_style_name->setText(lstinfo[3]);
        lt_board_type->setText(lstinfo[4]);
        lt_board_size->setText(lstinfo[5]);
    }

}
