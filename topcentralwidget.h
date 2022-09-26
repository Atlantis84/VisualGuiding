#ifndef TOPCENTRALWIDGET_H
#define TOPCENTRALWIDGET_H

#include <QWidget>

class TopCentralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TopCentralWidget(QWidget *parent = nullptr);
private:
    void paintEvent(QPaintEvent *event);
public:

signals:

public slots:
};

#endif // TOPCENTRALWIDGET_H
