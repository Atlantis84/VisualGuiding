#ifndef WELCOMEDLG_H
#define WELCOMEDLG_H

#include <QDialog>
class WelcomeDlg : public QDialog
{
public:
    WelcomeDlg();
    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // WELCOMEDLG_H
