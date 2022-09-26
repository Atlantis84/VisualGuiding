#include "topwidget.h"
#include "TitleBar.h"
#include "gdatafactory.h"
#include <qdesktopwidget.h>
#include <QApplication>
#include <QVBoxLayout>
#include "gdatafactory.h"
#include "QsLog.h"
TopWidget::TopWidget(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() | Qt::Widget);
    setWindowIcon(QIcon(":/icon/24x24.png"));
    this->setStyleSheet("QWidget{background-color:#000000}");
    resize(1600,1000);
    QDesktopWidget* desktop = QApplication::desktop();
    int w,h;
    w = desktop->screenGeometry().width();
    h = desktop->screenGeometry().height();
    this->setGeometry(w/2-800,h/2-520,1600,1000);

    TitleBar *pTitleBar = new TitleBar(this);
    pTitleBar->setTitle(u8"TCON自动测_视觉引导");
    pTitleBar->setFixedWidth(1600);
    installEventFilter(pTitleBar);

    QVBoxLayout* vAll = new QVBoxLayout();
    vAll->addSpacing(pTitleBar->height());
    vAll->setSpacing(0);
    vAll->setMargin(0);
    GDataFactory::get_top_main_window()->setParent(this);
    vAll->addWidget(GDataFactory::get_top_main_window());
    this->setLayout(vAll);
}

void TopWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
//    Timer_ID = startTimer(1000);
}

void TopWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    QLOG_INFO()<<"try to connect the central control";
    GDataFactory::get_factory()->exec_http_post_to_get_board_style("TG21B332F1");
}

void TopWidget::kill_query_timer()
{
//    killTimer(Timer_ID);
}
