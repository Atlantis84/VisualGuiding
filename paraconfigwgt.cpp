#include "paraconfigwgt.h"
#include "TitleBar.h"
#include <QPainter>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "gdatafactory.h"
#include <QDebug>
#include <QCheckBox>
#include <QDir>
ParaConfigWgt::ParaConfigWgt(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() | Qt::Widget);
    setWindowIcon(QIcon(":/icon/24x24.png"));
    resize(700,400);
    TitleBar *pTitleBar = new TitleBar(this);
    pTitleBar->setTitle(u8"参数配置");
    pTitleBar->setFixedWidth(700);
    installEventFilter(pTitleBar);

    QVBoxLayout* vAll = new QVBoxLayout();
    QLabel* labelPLCAddr = new QLabel(u8"通信地址:");
    lineEditPLCAddr = new QLineEdit();
    QLabel* labelPLCPort = new QLabel(u8"通信端口:");
    lineEditPLCPort = new QLineEdit();
    QHBoxLayout* hBox1 = new QHBoxLayout();
    hBox1->addWidget(labelPLCAddr);
    hBox1->addWidget(lineEditPLCAddr);
    hBox1->addWidget(labelPLCPort);
    hBox1->addWidget(lineEditPLCPort);

    QLabel* labelVisualAddr = new QLabel(u8"通信地址:");
    lineEditVisualAddr = new QLineEdit();
    QLabel* labelVisualPort = new QLabel(u8"通信端口:");
    lineEditVisualPort = new QLineEdit();
    QHBoxLayout* hBox2 = new QHBoxLayout();
//    hBox2->addWidget(labelVisualAddr);
//    hBox2->addWidget(lineEditVisualAddr);
//    hBox2->addWidget(labelVisualPort);
//    hBox2->addWidget(lineEditVisualPort);
    QVBoxLayout* vBox_1 = new QVBoxLayout();
    QCheckBox* ckBox = new QCheckBox(u8"二维码");
    ckBox->setStyleSheet("QCheckBox{color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");
    vBox_1->addWidget(ckBox);
    QVBoxLayout* vBox_2 = new QVBoxLayout();
    QHBoxLayout* hBox_2_1 = new QHBoxLayout();
    QLabel* labelX1 = new QLabel("X1:");
    QLineEdit* lineEditX1 = new QLineEdit();
    QLabel* labelY1 = new QLabel("Y1:");
    QLineEdit* lineEditY1 = new QLineEdit();
    hBox_2_1->addWidget(labelX1);
    hBox_2_1->addWidget(lineEditX1);
    hBox_2_1->addSpacing(30);
    hBox_2_1->addWidget(labelY1);
    hBox_2_1->addWidget(lineEditY1);
    QHBoxLayout* hBox_2_2 = new QHBoxLayout();
    QLabel* labelX2 = new QLabel("X2:");
    QLineEdit* lineEditX2 = new QLineEdit();
    QLabel* labelY2 = new QLabel("Y2:");
    QLineEdit* lineEditY2 = new QLineEdit();
    hBox_2_2->addWidget(labelX2);
    hBox_2_2->addWidget(lineEditX2);
    hBox_2_2->addSpacing(30);
    hBox_2_2->addWidget(labelY2);
    hBox_2_2->addWidget(lineEditY2);


    vBox_2->addLayout(hBox_2_1);
    vBox_2->addLayout(hBox_2_2);

    hBox2->addSpacing(30);
    hBox2->addLayout(vBox_1);
    hBox2->addSpacing(80);
    hBox2->addLayout(vBox_2);
    hBox2->addSpacing(30);

    QLabel* labelCCAddr = new QLabel(u8"通信地址:");
    lineEditCCAddr = new QLineEdit();
    QLabel* labelCCPort = new QLabel(u8"通信端口:");
    lineEditCCPort = new QLineEdit();
    QHBoxLayout* hBox3 = new QHBoxLayout();
    hBox3->addWidget(labelCCAddr);
    hBox3->addWidget(lineEditCCAddr);
    hBox3->addWidget(labelCCPort);
    hBox3->addWidget(lineEditCCPort);

    QPushButton* btnOK = new QPushButton(u8" 确  定 ");
    connect(btnOK,SIGNAL(clicked()), this, SLOT(slot_SetParaValue()));
    QPushButton* btnCancel = new QPushButton(u8" 取  消 ");
    connect(btnCancel,SIGNAL(clicked()), this, SLOT(slot_CloseWgt()));
    QHBoxLayout* hBox4 = new QHBoxLayout();
    hBox4->addStretch(1);
    hBox4->addWidget(btnOK);
    hBox4->addStretch(1);
    hBox4->addWidget(btnCancel);
    hBox4->addStretch(1);

    QGroupBox* grpPLC = new QGroupBox(u8"PLC配置");
    grpPLC->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");
    grpPLC->setLayout(hBox1);
    QGroupBox* grpVisual = new QGroupBox(u8"视觉配置");
    grpVisual->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");
    grpVisual->setLayout(hBox2);
    QGroupBox* grpCC = new QGroupBox(u8"中控系统配置");
    grpCC->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");
    grpCC->setLayout(hBox3);

    vAll->addSpacing(pTitleBar->height());
    vAll->addWidget(grpPLC);
    vAll->addWidget(grpCC);
    vAll->addWidget(grpVisual);
    vAll->addLayout(hBox4);
    this->setLayout(vAll);
}

void ParaConfigWgt::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(this->rect(),QBrush(QColor(235,236,240)));
    painter.setPen(QPen(QColor(0,0,0,100),2));
    painter.drawRect(this->rect());
}

void ParaConfigWgt::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    GDataFactory::get_factory()->load_json_config(QByteArray(QString(QDir::currentPath()+"/release/config.json").toLatin1()).data());
    this->set_ui_para();
}

void ParaConfigWgt::set_ui_para()
{
    this->lineEditPLCAddr->setText(GDataFactory::get_factory()->get_config_para("PLC_IP"));
    this->lineEditPLCPort->setText(GDataFactory::get_factory()->get_config_para("PLC_PORT"));
    this->lineEditVisualAddr->setText(GDataFactory::get_factory()->get_config_para("VISUAL_IP"));
    this->lineEditVisualPort->setText(GDataFactory::get_factory()->get_config_para("VISUAL_PORT"));
    this->lineEditCCAddr->setText(GDataFactory::get_factory()->get_config_para("CENTRAL_CONTROL_IP"));
    this->lineEditCCPort->setText(GDataFactory::get_factory()->get_config_para("CENTRAL_CONTROL_PORT"));
}

void ParaConfigWgt::slot_SetParaValue()
{
    GDataFactory::get_factory()->set_config_para("PLC_IP",this->lineEditPLCAddr->text());
    GDataFactory::get_factory()->set_config_para("PLC_PORT",this->lineEditPLCPort->text());
    this->close();
}

void ParaConfigWgt::slot_CloseWgt()
{
    this->close();
}
