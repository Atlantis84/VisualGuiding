#ifndef TOPWIDGET_H
#define TOPWIDGET_H

#include <QWidget>

class TopWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TopWidget(QWidget *parent = nullptr);
private:
    void showEvent(QShowEvent *event);
    void timerEvent(QTimerEvent *event);
    int Timer_ID;
signals:

public slots:
    void kill_query_timer();
};

#endif // TOPWIDGET_H
