#include "databasewgt.h"
#include "TitleBar.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "QsLog.h"
#include "gdatafactory.h"
#include <QSqlQuery>
DataBaseWgt::DataBaseWgt(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags() | Qt::Widget);
    setWindowIcon(QIcon(":/icon/24x24.png"));
    resize(1400,600);
    TitleBar *pTitleBar = new TitleBar(this);
    pTitleBar->setTitle(u8"历史标定");
    pTitleBar->setFixedWidth(1400);
    installEventFilter(pTitleBar);

    QGroupBox* grpHistoryEntry = new QGroupBox(u8"历史条目");
    grpHistoryEntry->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");
    QGroupBox* grpHistoryEntryDetail = new QGroupBox(u8"当前历史条目明细");
    grpHistoryEntryDetail->setStyleSheet("QGroupBox{border:1px solid rgba(0,0,0,100);color:rgb(0,0,0);background-color:rgba(0,0,0,0);}");
    QVBoxLayout* vAll = new QVBoxLayout();
    QHBoxLayout* hAll = new QHBoxLayout();
    QVBoxLayout* vBox1 = new QVBoxLayout();
    QVBoxLayout* vBox2 = new QVBoxLayout();

    vBox1->addWidget(createTableWidget());
    grpHistoryEntry->setLayout(vBox1);
    vBox2->addWidget(createTableWidgetDetail());
    grpHistoryEntryDetail->setLayout(vBox2);

    hAll->addWidget(grpHistoryEntry);
    hAll->addWidget(grpHistoryEntryDetail);
    hAll->setStretch(0,4);
    hAll->setStretch(1,6);
    vAll->addSpacing(pTitleBar->height());
    vAll->addLayout(hAll);
    this->setLayout(vAll);
    connect(tableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(slot_OnClickEntryTable(int,int)));
}

QWidget *DataBaseWgt::createTableWidget()
{
    QFont* font = new QFont(u8"LiSu", 15, QFont::StyleNormal);
    tableWidget = new QTableWidget(0,3);
    tableWidget->setFont(*font);
    tableWidget->setWindowTitle("QTableWidget & Item");
    QStringList header;
    header<<u8"相机ID"<<u8"标定时间"<<u8"备注";
    tableWidget->setHorizontalHeaderLabels(header);
    tableWidget->horizontalHeader()->setFont(*font);
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{border-color:rgb(0,0,0);color:rgb(0,0,0);background-color:rgba(216,217,222,255);}");
    tableWidget->setStyleSheet("QTableWidget{color:rgb(0,0,0);"
                               "background-color:rgba(0,0,0,0);}"
                               "QTableWidget::item::selected{color:rgb(0,0,0);"
                               "background-color:rgb(207,207,217);}");
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{border-color:rgb(0,0,0);color:rgb(0,0,0);background-color:rgba(216,217,222,255);}");
    tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);

    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    return tableWidget;
}

QWidget *DataBaseWgt::createTableWidgetDetail()
{
    QFont* font = new QFont(u8"LiSu", 15, QFont::StyleNormal);
    tableWidgetDetail = new QTableWidget(12,6);
    tableWidgetDetail->setFont(*font);
    tableWidgetDetail->setWindowTitle("QTableWidget & Item");
    QStringList header;
    header<<u8"机械X"<<u8"机械Y"<<u8"机械角度α"<<u8"像素X"<<u8"像素Y"<<u8"相机角度θ";
    tableWidgetDetail->setHorizontalHeaderLabels(header);
    tableWidgetDetail->horizontalHeader()->setFont(*font);
    tableWidgetDetail->horizontalHeader()->setStyleSheet("QHeaderView::section{border-color:rgb(0,0,0);color:rgb(0,0,0);background-color:rgba(216,217,222,255);}");
    tableWidgetDetail->setStyleSheet("QTableWidget{color:rgb(0,0,0);"
                                     "background-color:rgba(0,0,0,0);}"
                                     "QTableWidget::item::selected{color:rgb(0,0,0);"
                                     "background-color:rgb(207,207,217);}");
    tableWidgetDetail->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableWidgetDetail->verticalHeader()->setStyleSheet("QHeaderView::section{border-color:rgb(0,0,0);color:rgb(0,0,0);background-color:rgba(216,217,222,255);}");
    tableWidgetDetail->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidgetDetail->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    return tableWidgetDetail;
}

void DataBaseWgt::load_history_entry()
{
    QString strSql = QString("select * from public.%1").arg(constCalibrationEntryTable);
    QSqlQuery queryResult;
    if(GDataFactory::get_pgsql()->GetQueryResult(strSql,queryResult))
    {
        QLOG_TRACE()<<u8"query database success!";
        int idRow=0;
        while(queryResult.next())
        {
            tableWidget->insertRow(idRow);
            tableWidget->setItem(idRow,0,new QTableWidgetItem(queryResult.value(1).toString()));
            tableWidget->item(idRow,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            tableWidget->setItem(idRow,1,new QTableWidgetItem(queryResult.value(2).toString()));
            tableWidget->item(idRow,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            tableWidget->setItem(idRow,2,new QTableWidgetItem(queryResult.value(0).toString()));
            tableWidget->item(idRow,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            idRow++;
        }
    }
}

void DataBaseWgt::load_history_details(int id)
{
    tableWidgetDetail->clearContents();
    QString strSql = QString("select * from public.%1 where \"ID\"=%2").arg(constCalibrationDetailsTable).
            arg(id);
    QSqlQuery queryResult;
    if(GDataFactory::get_pgsql()->GetQueryResult(strSql,queryResult))
    {
        QLOG_TRACE()<<u8"query database success!";
        int idRow=0;
        while(queryResult.next())
        {
            tableWidgetDetail->setItem(idRow,0,new QTableWidgetItem(queryResult.value(1).toString()));
            tableWidgetDetail->item(idRow,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            tableWidgetDetail->setItem(idRow,1,new QTableWidgetItem(queryResult.value(2).toString()));
            tableWidgetDetail->item(idRow,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            tableWidgetDetail->setItem(idRow,2,new QTableWidgetItem(queryResult.value(3).toString()));
            tableWidgetDetail->item(idRow,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            tableWidgetDetail->setItem(idRow,3,new QTableWidgetItem(queryResult.value(4).toString()));
            tableWidgetDetail->item(idRow,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            tableWidgetDetail->setItem(idRow,4,new QTableWidgetItem(queryResult.value(5).toString()));
            tableWidgetDetail->item(idRow,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            tableWidgetDetail->setItem(idRow,5,new QTableWidgetItem(queryResult.value(6).toString()));
            tableWidgetDetail->item(idRow,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            idRow++;
        }

    }
}

void DataBaseWgt::slot_OnClickEntryTable(int row,int col)
{
    qDebug()<<"click on entry table";
    QList<QTableWidgetItem*> items = tableWidget->selectedItems();
    int id = items.at(2)->text().toInt();
    load_history_details(id);
}

void DataBaseWgt::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(this->rect(),QBrush(QColor(235,236,240)));
    painter.setPen(QPen(QColor(0,0,0,100),2));
    painter.drawRect(this->rect());
}

void DataBaseWgt::showEvent(QShowEvent *event)
{
    load_history_entry();
}

void DataBaseWgt::closeEvent(QCloseEvent *event)
{
    int j = tableWidget->rowCount();
    for (int i=0;i<j;i++)
    {
        tableWidget->removeRow(0);
    }
    this->tableWidgetDetail->clearContents();
}
