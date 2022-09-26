#ifndef TOPRIGHTWIDGET_H
#define TOPRIGHTWIDGET_H

#include <QWidget>
class QLineEdit;
class TopRightWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TopRightWidget(QWidget *parent = nullptr);
private:
    void paintEvent(QPaintEvent *event);
    QLineEdit* lt_project_id;
    QLineEdit* lt_half_material_number;
    QLineEdit* lt_machine_style_name;
    QLineEdit* lt_board_type;
    QLineEdit* lt_board_size;
    QLineEdit* lt_board_sn;
public:

signals:

public slots:
    void slot_test_rev_from_central_control(QList<QString> lstinfo);//just for test,delete later
};

#endif // TOPRIGHTWIDGET_H
