#ifndef THREADS_H
#define THREADS_H

#include <QObject>
#include <QPointer>
#include <smtpclient/SmtpMime>

#include "files.h"

class threads : public QObject
{
    Q_OBJECT

//    QVector<QStringList> allVector;

public:
    explicit threads(QObject *parent = 0);
    virtual ~threads();
    
signals:
//    void setStatus(const unsigned short &stType, const QString &stLine);
//    void sendPBData(const int &pbData);
//    void sendExcelVector(const QVector<QStringList> &exVector);
//    void finished();

public slots:
//    void processExcel(const QList<QStringList> &xlData);
//    void sendEmail();
//    void logger(const unsigned short &stType,const QString &errLine);
};

class xlThreads : public QObject
{
    Q_OBJECT

    QList<QStringList> xlData;

public:
    explicit xlThreads(const QList<QStringList> &xlDataList);
    virtual ~xlThreads();

signals:
    void setStatus(const unsigned short &stType, const QString &stLine);
    void sendPBData(const int &pbData);
    void sendExcelVector(const QVector<QStringList> &exVector);
    void finished();

public slots:
    void processExcel();
};

class sendEmails : public QObject
{
    Q_OBJECT

    QStringList settingsList;
    QList<QStringList> sendDataList;

public:
    explicit sendEmails(const QStringList &emSettingsList, const QList<QStringList> &emDataList);
    virtual ~sendEmails();

signals:
    void setStatus(const unsigned short &stType, const QString &stLine);
    void sendInfo(const int &rowNum, const int &sendCode);
    void finished();

public slots:
    void sendEmail();
};

#endif // THREADS_H
