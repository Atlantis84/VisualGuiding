#ifndef DATABASEWGT_H
#define DATABASEWGT_H

#include <QWidget>
#include <QTableWidget>

class DataBaseWgt : public QWidget
{
    Q_OBJECT
public:
    explicit DataBaseWgt(QWidget *parent = nullptr);
private:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    QWidget* createTableWidget();
    QWidget* createTableWidgetDetail();
    QTableWidget* tableWidget;
    QTableWidget* tableWidgetDetail;
    void load_history_entry();
    void load_history_details(int id);

public:

signals:

public slots:
    void slot_OnClickEntryTable(int row,int col);
};

#endif // DATABASEWGT_H
