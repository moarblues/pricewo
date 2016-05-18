#include "threads.h"

threads::threads(QObject *parent) : QObject(parent)
{
}

threads::~threads()
{
}

xlThreads::xlThreads(const QList<QStringList> &xlDataList)
{
    xlData = xlDataList;
}

xlThreads::~xlThreads()
{
}

void xlThreads::processExcel()
{
    QVector<QStringList> allVector;
    QVector<QStringList> currVector;

    QPointer<files> fw = new files();
    connect(fw, SIGNAL(setStatus(unsigned short,QString)), this, SIGNAL(setStatus(unsigned short,QString)));

    for (int irow = 0; irow < xlData.size(); ++irow)
    {
        if (QString::compare(xlData.at(irow).at(0).split(".").last(),"xls") == 0)
        {
            currVector = fw->getXlsData(xlData.at(irow).at(0), xlData.at(irow).at(1),
                                         xlData.at(irow).at(2).split("-"));
            allVector += currVector;
        }
        if (QString::compare(xlData.at(irow).at(0).split(".").last(),"xlsx") == 0)
        {
            currVector = fw->getXlsxData(xlData.at(irow).at(0), xlData.at(irow).at(1),
                                         xlData.at(irow).at(2).split("-"));
            allVector += currVector;
        }
        emit this->sendPBData(irow+1);
    }

    delete fw;
    emit this->sendExcelVector(allVector);
    emit this->finished();
}

sendEmails::sendEmails(const QStringList &emSettingsList, const QList<QStringList> &emDataList)
{
    settingsList = emSettingsList;
    sendDataList = emDataList;
}

sendEmails::~sendEmails()
{
}

void sendEmails::sendEmail()
{
    if (settingsList.size()!=7)
    {
        emit this->setStatus(1,"Настройки почтового клиента повреждены");
        return;
    }

    SmtpClient smtp(settingsList.at(0), settingsList.at(1).toInt(), SmtpClient::SslConnection);
    smtp.setUser(settingsList.at(2));
    smtp.setPassword(settingsList.at(3));

    if (!smtp.connectToHost())
    {
        emit this->setStatus(1,"Не удалось установить соединение");
        return;
    }

    if (!smtp.login())
    {
        emit this->setStatus(1,"Не удалось авторизоваться\nПроверьте данные");
        return;
    }

    for(int irow=0; irow<sendDataList.size(); ++irow)
    {
        MimeMessage msg;

        msg.setSender(new EmailAddress(settingsList.at(2), settingsList.at(4)));
        msg.addRecipient(new EmailAddress(sendDataList.at(irow).at(2), sendDataList.at(irow).at(1)));
        msg.setSubject(settingsList.at(5));

        MimeText body;
        body.setText(settingsList.at(6));
        msg.addPart(&body);

        for (int iitm=3; iitm<sendDataList.at(irow).size();++iitm)
        {msg.addPart(new MimeAttachment(new QFile(sendDataList.at(irow).at(iitm))));}

        if (!smtp.sendMail(msg))
        {emit this->sendInfo(irow,1);}
        else
        {emit this->sendInfo(irow,0);}
    }
    smtp.quit();
    emit this->finished();
}
