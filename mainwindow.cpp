#include "mainwindow.h"
#include "ui_mainwindow.h"

mainWindow::mainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType <QVector<QStringList> > ("QVector<QStringList>");

    sb_lbl_technical = new QLabel(this);
    sb_lbl_info = new QLabel(this);
    sb_pb_status = new QProgressBar(this);

    ui->statusBar->addPermanentWidget(sb_lbl_info,6);
    ui->statusBar->addPermanentWidget(sb_pb_status,2);
    ui->statusBar->addPermanentWidget(sb_lbl_technical,1);

    ui->le_searchP2->setValidator(new QDoubleValidator(0, 999999999, 3, this));
    ui->le_searchP3->setValidator(new QDoubleValidator(0, 999999999, 3, this));

    QDesktopWidget *desktop = QApplication::desktop();
    this->move((desktop->width()-this->width())/2,((desktop->height()-this->height()))/2);
    this->resize(desktop->availableGeometry().width()-20,desktop->availableGeometry().height());
//    QDesktopWidget *desktop = QApplication::desktop();
//    this->resize(desktop->width(),desktop->height()-50);
//    this->setWindowState(Qt::WindowMaximized);
//    this->showMaximized();

    settingsModel = 0;
    resModel = 0;

    this->refreshSettings();
    int tvwSettingsWidth = this->width() * 0.964;
    ui->tvw_settings->setFixedWidth(tvwSettingsWidth);
    ui->tvw_settings->setColumnWidth(0,tvwSettingsWidth * 0.13);
    ui->tvw_settings->setColumnWidth(1,tvwSettingsWidth * 0.35);
    ui->tvw_settings->setColumnWidth(2,tvwSettingsWidth * 0.13);
    ui->tvw_settings->setColumnWidth(3,tvwSettingsWidth * 0.08);
    ui->tvw_settings->setColumnWidth(4,tvwSettingsWidth * 0.05);
    ui->tvw_settings->setColumnWidth(5,tvwSettingsWidth * 0.19);
    ui->tvw_settings->setColumnWidth(6,tvwSettingsWidth * 0.05);

    connect(ui->le_searchP1, SIGNAL(returnPressed()), this, SLOT(on_btn_search_clicked()));
    connect(ui->le_searchP2, SIGNAL(returnPressed()), this, SLOT(on_btn_search_clicked()));
    connect(ui->le_searchP3, SIGNAL(returnPressed()), this, SLOT(on_btn_search_clicked()));

//    for (int i=0; i<10000; ++i) {
//        this->on_btn_loadPrices_clicked();
//    }
}

mainWindow::~mainWindow()
{
    delete ui;
}

void mainWindow::refreshSettings()
{
    QString tmpline;
    QStringList tmplist;

    QPointer<files> fw = new files(this);
    connect(fw, SIGNAL(setStatus(unsigned short,QString)), this, SLOT(logger(unsigned short,QString)));
    fw->checkIntegrity();

    this->createSettingsModelTemplate();

    settingsList = fw->readSettings();
    delete fw;

    if (settingsList.size() == 2)
    {
        settingsList.append(QStringList() << "" << "" << "" << "" << "" << "");
    }
    for (int iitm = 0; iitm < settingsList.size(); ++iitm)
    {
        if (iitm == 0) {ui->le_pricesDir->setText(settingsList.at(iitm).at(0));continue;}
        if (iitm == 1) {ui->le_requestDir->setText(settingsList.at(iitm).at(0));continue;}

        if (settingsList.at(iitm).size()!=6)
        {
            this->logger(0, QString("Ошибка в файле настроек, запись ").append(QString::number(iitm-2)));
            continue;
        }
        QStandardItem *vendor = new QStandardItem(settingsList.at(iitm).at(0));
        QStandardItem *fpath = new QStandardItem(settingsList.at(iitm).at(1));
        QStandardItem *fdate = new QStandardItem(settingsList.at(iitm).at(2));
        QStandardItem *datacols = new QStandardItem(settingsList.at(iitm).at(3));
        QStandardItem *discount = new QStandardItem();
        discount->setData(settingsList.at(iitm).at(4).toDouble(), Qt::DisplayRole);
        QStandardItem *email = new QStandardItem(settingsList.at(iitm).at(5));
        QStandardItem *process = new QStandardItem();
        process->setCheckable(true);
        process->setCheckState(Qt::Checked);

        tmplist = settingsList.at(iitm).at(1).split(".");
        tmpline = tmplist.at(tmplist.size()-1);

        if ((QString::compare(tmpline,"xls")!=0 && QString::compare(tmpline,"xlsx")!=0)
                || !QFile::exists(fpath->text()))
        {
            fpath->setText("Файл не найден");
            vendor->setBackground(Qt::gray);
            fpath->setBackground(Qt::gray);
            fdate->setBackground(Qt::gray);
            datacols->setBackground(Qt::gray);
            discount->setBackground(Qt::gray);
            email->setBackground(Qt::gray);
            process->setBackground(Qt::gray);
            process->setCheckState(Qt::Unchecked);
            process->setEnabled(false);
        }

        QList<QStandardItem *> modelRow;
        modelRow.append(vendor);
        modelRow.append(fpath);
        modelRow.append(fdate);
        modelRow.append(datacols);
        modelRow.append(discount);
        modelRow.append(email);
        modelRow.append(process);
        settingsModel->appendRow(modelRow);
    }

    if(settingsModel->rowCount() != 0)
    {ui->tvw_settings->setModel(settingsModel);}
    else
    {
        ui->btn_loadPrices->setEnabled(false);
        ui->btn_clearMemory->setEnabled(false);
        ui->btn_search->setEnabled(false);
        ui->btn_clearNames->setEnabled(false);
    }
}

inline bool mainWindow::createSettingsModelTemplate()
{
    if (settingsModel!=0) {delete settingsModel; settingsModel=0;}
    settingsModel = new QStandardItemModel(this);
    QStringList settingsLabels;
    settingsLabels << "Поставщик" << "Путь к файлу прайса" << "Дата обновления"
                   << "Столбцы данных" << "Скидка" << "E-mail" << "Обрабатывать";
    settingsModel->setHorizontalHeaderLabels(settingsLabels);

    return true;
}

void mainWindow::editVendors(const QModelIndex &index)
{
    QPointer<dlg_settings> ds = new dlg_settings(settingsList.at(0).at(0), this);
    ds->setAttribute(Qt::WA_DeleteOnClose);
    connect(ds,SIGNAL(sendSettings(QStringList,int)),this,SLOT(updateVendor(QStringList,int)));
    ds->alterVendor(settingsList.at(index.row()+2), index.row()+2);
    ds->setModal(true);
    ds->show();
}

void mainWindow::loadPrices(const QVector<QStringList> &xlData)
{
    allVector = xlData;
}

void mainWindow::setPBValue(const int &val)
{
    sb_pb_status->setValue(val);
}

void mainWindow::on_btn_search_clicked()
{
    if (allVector.size() == 0)
    {
        this->logger(1, "Прайсы не загружены");
        return;
    }

    QList<QStringList> discList;
    for (int irow=2; irow < settingsList.size(); ++irow)
    {
        discList.append(QStringList() << settingsList.at(irow).at(0) << settingsList.at(irow).at(4));
    }

    QPointer<dlg_result> dlgRes = new dlg_result(this);
    dlgRes->setAttribute(Qt::WA_DeleteOnClose);
    dlgRes->procResVector(allVector, (QStringList() << ui->le_searchP1->text().trimmed() << ui->le_searchP2->text().trimmed()
                                     << ui->le_searchP3->text().trimmed()), discList);
    dlgRes->setModal(true);
    dlgRes->show();
}

void mainWindow::on_btn_clearNames_clicked()
{
    ui->le_searchP1->clear();
    ui->le_searchP2->clear();
    ui->le_searchP3->clear();
    ui->le_searchP1->setFocus();
}

void mainWindow::on_btn_loadPrices_clicked()
{
    allVector.clear();
    sb_pb_status->setMaximum(settingsList.size()-2);

    QList<QStringList> xlData;

    for (int irow=2; irow<settingsList.size(); ++irow)
    {
        if (settingsModel->item(irow-2,6)->checkState() != Qt::Checked) {sb_pb_status->setMaximum(sb_pb_status->maximum()-1);continue;}
        xlData.append(QStringList() << settingsList.at(irow).at(1) << settingsList.at(irow).at(0) << settingsList.at(irow).at(3)); //file path, vendor, data columns
    }

    if (xlData.isEmpty())
    {
        this->logger(1,"Не выбраны прайсы для загрузки");
        return;
    }

    QThread *xlThread = new QThread;
    xlThreads *xtw = new xlThreads(xlData);
    xtw->moveToThread(xlThread);
    connect(xtw,SIGNAL(setStatus(unsigned short,QString)),this,SLOT(logger(unsigned short,QString)));
    connect(xtw,SIGNAL(sendPBData(int)),this,SLOT(setPBValue(int)));
    connect(xtw,SIGNAL(sendExcelVector(QVector<QStringList>)),this,SLOT(loadPrices(QVector<QStringList>)));
    connect(xlThread,SIGNAL(started()),xtw,SLOT(processExcel()));
    connect(xtw,SIGNAL(finished()),xlThread,SLOT(quit()));
    connect(xlThread,SIGNAL(finished()),xtw,SLOT(deleteLater()));
    connect(xtw,SIGNAL(destroyed()),xlThread,SLOT(deleteLater()));
    xlThread->start();
}

void mainWindow::on_btn_clearMemory_clicked()
{
    allVector.clear();
    sb_pb_status->setValue(0);
}

void mainWindow::on_btn_order_clicked()
{
    QPointer<dlg_order> dlgOrder = new dlg_order(this);
    dlgOrder->setAttribute(Qt::WA_DeleteOnClose);
    QPointer<files> fw = new files(dlgOrder);
    dlgOrder->setOrderModel(fw->readOrder());
    dlgOrder->setModal(true);
    dlgOrder->show();
}

void mainWindow::updateVendor(const QStringList &vendorData, const int &shRow)
{
    if (shRow==-1) {settingsList.append(vendorData);}
    else
    {
        for (int iitm = 0; iitm < vendorData.size(); ++iitm)
        {
            settingsList[shRow][iitm] = vendorData.at(iitm);
        }
    }

    QPointer<files> fw = new files(this);
    fw->writeVendorSettings(settingsList);
    delete fw;
    this->refreshSettings();
}

void mainWindow::logger(const unsigned short &stType, const QString &errLine)
{
    ui->lW_logger->addItem(QTime::currentTime().toString().append("  ").append(errLine));
    ui->lW_logger->scrollToBottom();

    if (stType==1)
    {QMessageBox::critical(this,"Ошибка",errLine,QMessageBox::Ok);}
}

void mainWindow::on_btn_editPricesDir_clicked()
{
    QStringList dirList;
    dirList.append("!pricedir");
    dirList.append(QFileDialog::getExistingDirectory(this, "Укажите директорию с прайс-листами",
                        settingsList.at(0).at(0), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));

    if(dirList.at(1).contains("--"))
    {
        this->logger(1, "Путь к каталогу не должен содержать символа --");
        return;
    }

    QPointer<files> fw = new files(this);
    fw->writeDirSettings(dirList);
    delete fw;
    this->refreshSettings();
}

void mainWindow::on_btn_editRequestDir_clicked()
{
    QStringList dirList;
    dirList.append("!requestdir");
    dirList.append(QFileDialog::getExistingDirectory(this, "Укажите директорию с файлами заявок",
                        settingsList.at(1).at(0), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));

    if(dirList.at(1).contains("--"))
    {
        this->logger(1, "Путь к каталогу не должен содержать символа --");
        return;
    }

    QPointer<files> fw = new files(this);
    fw->writeDirSettings(dirList);
    delete fw;
    this->refreshSettings();
}

void mainWindow::on_btn_addVendor_clicked()
{
    QPointer<dlg_settings> ds = new dlg_settings(settingsList.at(0).at(0), this);
    ds->setAttribute(Qt::WA_DeleteOnClose);
    connect(ds,SIGNAL(sendSettings(QStringList,int)),this,SLOT(updateVendor(QStringList,int)));
    connect(ds, SIGNAL(setStatus(unsigned short,QString)), this, SLOT(logger(unsigned short,QString)));
    ds->alterVendor(QStringList() << "" << "" << "" << "" << "" << "", -1);
    ds->setModal(true);
    ds->show();
}

void mainWindow::on_btn_removeVendor_clicked()
{
    if (ui->tvw_settings->currentIndex().isValid())
    {
        QString vendorStr;
        vendorStr.append("Удалить ").append(settingsList.at(ui->tvw_settings->currentIndex().row()+2).at(0)).append("?");
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Удаление", vendorStr,
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
//            settingsModel->removeRow(ui->tvw_settings->currentIndex().row());
            settingsList.removeAt(ui->tvw_settings->currentIndex().row()+2);

            if (settingsList.size() == 2)
            {
                this->updateVendor(QStringList() << "" << "" << "" << "0-0-0-0" << "0.00" << "",-1);
            }

            QPointer<files> fw = new files(this);
            fw->writeVendorSettings(settingsList);
            this->refreshSettings();
        }
    }
}

void mainWindow::on_btn_refresh_clicked()
{
    this->refreshSettings();
}

void mainWindow::on_btn_setUp_clicked()
{
    if (ui->tvw_settings->currentIndex().isValid()) {this->editVendors(ui->tvw_settings->currentIndex());}
}

void mainWindow::on_btn_overwriteSettings_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Удаление", "Перезапись удалит ВСЕ текущие настройки\n Вы хотите продолжить?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QPointer<files> fw = new files(this);
        fw->overwriteSettings();
        delete fw;
        this->refreshSettings();
    }
}


void mainWindow::on_tvw_settings_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {this->editVendors(index);}
}

void mainWindow::on_btn_openPricesDir_clicked()
{
    QDesktopServices::openUrl(QUrl("file://" + settingsList.at(0).at(0)));
}

void mainWindow::on_btn_openRequestDir_clicked()
{
    QDesktopServices::openUrl(QUrl("file://" + settingsList.at(1).at(0)));
}
