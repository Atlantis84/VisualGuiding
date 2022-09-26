#ifndef MESPROCESS_H
#define MESPROCESS_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
class MesProcess : public QObject
{
    Q_OBJECT
public:
    explicit MesProcess(QObject *parent = nullptr);

    void exec_http_post();
private:
    QNetworkAccessManager* accessManager;

signals:
    void signal_to_ui(QByteArray* data);
    void signal_test_central_control(QList<QString> lstinfo);//just for test delete later
public slots:
    void slot_urlReply(QNetworkReply* reply);
};

#endif // MESPROCESS_H
