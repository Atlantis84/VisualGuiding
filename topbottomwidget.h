#ifndef TOPBOTTOMWIDGET_H
#define TOPBOTTOMWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QTextEdit>
class TopBottomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TopBottomWidget(QWidget *parent = nullptr);
private:
    void paintEvent(QPaintEvent *event);
    QWidget* createTableWidget();
    QTableWidget* tableWidget;
    QTextEdit* m_pLogEdit;
public:

signals:

public slots:
    void slot_rev_logs(const QString msg);
    void slot_rev_http_data(QByteArray *data);
    void slot_rev_vision_info(int camera_id,QList<double> vision_info);
};

#endif // TOPBOTTOMWIDGET_H
