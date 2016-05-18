#include "dlg_email.h"
#include "ui_dlg_email.h"

dlg_email::dlg_email(QWidget *parent) : QDialog(parent), ui(new Ui::dlg_email)
{
    ui->setupUi(this);

    ui->le_port->setValidator(new QIntValidator(0, 99999, this));

    venModel = 0;
    this->createModel();
    this->readVendorsList();
    this->readSettingsList();

    int tvwSettingsWidth = ui->tvw_vendors->width() * 0.964;
    ui->tvw_vendors->setFixedWidth(tvwSettingsWidth);
    ui->tvw_vendors->setColumnWidth(0,tvwSettingsWidth * 0.37);
    ui->tvw_vendors->setColumnWidth(1,tvwSettingsWidth * 0.45);
    ui->tvw_vendors->setColumnWidth(2,tvwSettingsWidth * 0.10);
}

dlg_email::~dlg_email()
{
    delete ui;
}

void dlg_email::createModel()
{
    if (venModel!=0) {delete venModel; venModel=0;}
    venModel = new QStandardItemModel(this);

    venModel->setHorizontalHeaderLabels(QStringList() << "Поставщик" << "Адрес" << "К-во");

    for (int irow=0; irow<currentData.size(); ++irow)
    {
        QStandardItem *vendor = new QStandardItem(currentData.at(irow).at(1));
        QStandardItem *email = new QStandardItem(currentData.at(irow).at(2));
        QStandardItem *atts = new QStandardItem(QString::number(currentData.at(irow).size()-3));

        QList<QStandardItem *> modelrow;
        modelrow.append(vendor);
        modelrow.append(email);
        modelrow.append(atts);

        venModel->appendRow(modelrow);
    }

    venModel->appendRow(QList<QStandardItem *>() << new QStandardItem("--Добавить--") << new QStandardItem("")
                        << new QStandardItem(""));
    ui->tvw_vendors->setModel(venModel);
}

void dlg_email::readVendorsList()
{
    QPointer<files> fw = new files(this);
    QList<QStringList> sList = fw->readSettings();
    delete fw;

    vendorList.clear();

    for (int iitm = 0; iitm < sList.size(); ++iitm)
    {
        if (iitm<3) {continue;}
        vendorList.append(QStringList() << sList.at(iitm).at(0) << sList.at(iitm).at(5).split("|"));
    }
}

void dlg_email::readSettingsList()
{
    QPointer<files> fw = new files(this);
    settingsList = fw->readEmailSettings();
    delete fw;

    if (settingsList.size()!=7)
    {
        settingsList.clear();
        this->logger(1,"Настройки e-mail повреждены");
        settingsList << "" << "" << "" << "" << "" << "" << "";
    }

    ui->le_server->setText(settingsList.at(1));
    ui->le_port->setText(settingsList.at(2));
    ui->le_account->setText(settingsList.at(3));
    ui->le_sender->setText(settingsList.at(4));
    ui->le_subject->setText(settingsList.at(5));
    ui->te_body->setText(QString(settingsList.at(6)));
}

void dlg_email::addVendorRecord(const QStringList &vendorData)
{
    if (currentData.size()==vendorData.at(0).toInt())
    {currentData.append(vendorData);}
    else
    {currentData[vendorData.at(0).toInt()] = vendorData;}
    this->createModel();
}

void dlg_email::logger(const unsigned short &stType, const QString &errLine)
{
    ui->lw_logger->addItem(QTime::currentTime().toString().append("  ").append(errLine));
    ui->lw_logger->scrollToBottom();
    if (stType==1) {QMessageBox::critical(this,"Ошибка",errLine,QMessageBox::Ok);}
}

void dlg_email::sendEmail(const QString &pwd)
{
    working = true;
    QStringList settingList;
    settingList << ui->le_server->text() << ui->le_port->text() << ui->le_account->text() << pwd << ui->le_sender->text()
                << ui->le_subject->text() << ui->te_body->toPlainText();

    QThread *seThread = new QThread;
    sendEmails *semw = new sendEmails(settingList,currentData);
    semw->moveToThread(seThread);
    connect(semw,SIGNAL(setStatus(unsigned short,QString)),this,SLOT(logger(unsigned short,QString)));
    connect(semw,SIGNAL(sendInfo(int,int)),this,SLOT(sendInfo(int,int)));
    connect(seThread,SIGNAL(started()),semw,SLOT(sendEmail()));
    connect(semw,SIGNAL(finished()),seThread,SLOT(quit()));
    connect(seThread,SIGNAL(finished()),semw,SLOT(deleteLater()));
    connect(semw,SIGNAL(destroyed()),seThread,SLOT(deleteLater()));
    seThread->start();
}

void dlg_email::sendInfo(const int &rowNum, const int &sendCode)
{
    if (sendCode == 1)
    {
        this->logger(0,QString("Не удалось отправить - ").append(QString::number(rowNum+1)).append("-")
                     .append(currentData.at(rowNum).at(1)).append(" ").append(currentData.at(rowNum).at(2)));
        for(int jcol=0;jcol<venModel->rowCount();++jcol)
        {venModel->item(rowNum,jcol)->setBackground(Qt::red);}
    }

    if (sendCode == 0)
    {
        this->logger(0,QString("Успешно отправлено - ").append(QString::number(rowNum+1)).append("-")
                     .append(currentData.at(rowNum).at(1)));
        for(int jcol=0;jcol<venModel->columnCount();++jcol)
        {venModel->item(rowNum,jcol)->setBackground(Qt::green);}
    }
    if (rowNum == currentData.size()-1) {working = false;}
}

void dlg_email::on_btn_save_clicked()
{
    QStringList sList;
    sList << ui->le_server->text() << ui->le_port->text() << ui->le_account->text() << ui->le_sender->text()
                 << ui->le_subject->text() << ui->te_body->toPlainText();

    QPointer<files> fw = new files(this);
    connect(fw,SIGNAL(setStatus(unsigned short,QString)),this,SLOT(logger(unsigned short,QString)),Qt::QueuedConnection);
    fw->writeEmailSettings(sList);
    delete fw;
    this->readSettingsList();
}

void dlg_email::on_btn_reset_clicked()
{
    this->readSettingsList();
}

void dlg_email::on_tvw_vendors_doubleClicked(const QModelIndex &index)
{
    QStringList sList;
    sList << settingsList.at(0) << ui->le_server->text() << ui->le_port->text() << ui->le_account->text()
          << ui->le_subject->text() << ui->te_body->toPlainText();

    QPointer<dlg_composeLetter> cm;
    if (currentData.size()==index.row())
    {cm = new dlg_composeLetter(vendorList, sList, QStringList() << QString::number(index.row()), this);}
    else
    {
        currentData[index.row()][0] = QString::number(index.row());
        cm = new dlg_composeLetter(vendorList, sList, currentData.at(index.row()), this);
    }

    connect(cm,SIGNAL(sendAtts(QStringList)),this,SLOT(addVendorRecord(QStringList)));
    cm->setModal(true);
    cm->show();
}

void dlg_email::on_tn_send_clicked()
{
    if (currentData.size() == 0)
    {
        this->logger(1,"Список отправки пуст");
        return;
    }

    QPointer<addDialog> addDlg = new addDialog(1,this);
    connect(addDlg, SIGNAL(sendLine(QString)), this, SLOT(sendEmail(QString)));
    addDlg->setModal(true);
    addDlg->show();
}

void dlg_email::on_btn_delete_clicked()
{
    currentData.removeAt(ui->tvw_vendors->currentIndex().row());
    this->createModel();
}

void dlg_email::on_btn_close_clicked()
{
    this->close();
}

void dlg_email::closeEvent(QCloseEvent *ce)
{
    if (working == true)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Внимание", "Отправка писем в процессе\nПрервать?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {ce->accept();}
        else
        {ce->ignore();}
    }

    if (currentData.size()>0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Внимание", "Текущая рассылка не будет сохранена\nВсё равно выйти?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {ce->accept();}
        else
        {ce->ignore();}
    }
}
