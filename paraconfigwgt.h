#ifndef PARACONFIGWGT_H
#define PARACONFIGWGT_H

#include <QWidget>
#include <QLineEdit>
class ParaConfigWgt : public QWidget
{
    Q_OBJECT
public:
    explicit ParaConfigWgt(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent* event);
    void set_ui_para();

signals:

public slots:
    void slot_SetParaValue();
    void slot_CloseWgt();
private:
    QLineEdit* lineEditPLCAddr;
    QLineEdit* lineEditPLCPort;
    QLineEdit* lineEditVisualAddr;
    QLineEdit* lineEditVisualPort;
    QLineEdit* lineEditCCAddr;
    QLineEdit* lineEditCCPort;
};

#endif // PARACONFIGWGT_H
