#include "dlg_settings.h"
#include "ui_dlg_settings.h"

dlg_settings::dlg_settings(const QString &pricesDir, QWidget *parent) : QDialog(parent), ui(new Ui::dlg_settings)
{
    ui->setupUi(this);
    pricesDirPath = pricesDir;
}

dlg_settings::~dlg_settings()
{
    delete ui;
}

void dlg_settings::alterVendor(const QStringList &dataList, const int &updRow)
{
    ppRow = updRow;
    QStringList emailList;
    QStringList columns(QStringList() << "0" << "0" << "0" << "0");
    if (!dataList.at(3).isEmpty()) {columns.clear(); columns = dataList.at(3).split("-");}
    filename = dataList.at(1);
    QString refreshed = "Обновлено: ";
    ui->le_vendor->setText(dataList.at(0));
    ui->btn_setFilePath->setText(dataList.at(1));
    ui->btn_setFilePath->setStyleSheet("Text-align:right");
    ui->spb_nameCol->setValue(columns.at(0).toInt());
    ui->spb_priceCol->setValue(columns.at(1).toInt());
    ui->spb_expDateCol->setValue(columns.at(2).toInt());
    ui->spb_manufactorerCol->setValue(columns.at(3).toInt());
    ui->spb_discount->setValue(dataList.at(4).toDouble());
    ui->lbl_refreshed->setText(refreshed.append(dataList.at(2)));

    if (!dataList.at(5).isEmpty())
    {
        emailList = dataList.at(5).split("|");
        for (int itm = 0; itm < emailList.size(); ++itm)
        {
            ui->cb_emailList->addItem(emailList[itm]);
        }
    }
}

void dlg_settings::on_btn_save_clicked()
{
    QStringList dataList;
    QString columns, emailLine;
    columns.append(QString::number(ui->spb_nameCol->value())).append("-").append(QString::number(ui->spb_priceCol->value())).append("-")
            .append(QString::number(ui->spb_expDateCol->value())).append("-").append(QString::number(ui->spb_manufactorerCol->value()));

    dataList.append(ui->le_vendor->text());
    dataList.append(filename);
    dataList.append(QDateTime::currentDateTime().toString("dd.MM.yyyy-hh:mm"));
    dataList.append(columns);
    dataList.append(QString::number(ui->spb_discount->value()));

    if (ui->cb_emailList->count() == 0)
    {
        emailLine = "";
    }
    else
    {
        for (int irow = 0; irow < ui->cb_emailList->count(); ++irow)
        {
            if (irow > 0) {emailLine.append("|");}
            emailLine.append(ui->cb_emailList->itemText(irow));
        }
    }
    dataList.append(emailLine);
    emit this->sendSettings(dataList, ppRow);

    this->close();
}

void dlg_settings::on_btn_setFilePath_clicked()
{
    filename = QFileDialog::getOpenFileName(this, ui->le_vendor->text(), pricesDirPath, QObject::tr("Файлы Excel (*.xls *.xlsx)"));

    ui->btn_setFilePath->setText(filename);
}

void dlg_settings::on_btn_cancel_clicked()
{
    this->close();
}

void dlg_settings::on_btn_addEmail_clicked()
{
    QPointer<addDialog> addDlg = new addDialog(0,this);
    connect(addDlg, SIGNAL(sendLine(QString)), this, SLOT(addEmailList(QString)));
    addDlg->setModal(true);
    addDlg->show();
}

void dlg_settings::on_btn_removeEmail_clicked()
{
    ui->cb_emailList->removeItem(ui->cb_emailList->currentIndex());
}

void dlg_settings::addEmailList(const QString &emailLine)
{
    QRegExp mailREX("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    mailREX.setCaseSensitivity(Qt::CaseInsensitive);
    mailREX.setPatternSyntax(QRegExp::RegExp);

    if (!mailREX.exactMatch(emailLine))
    {
        QMessageBox::critical(this,"Ошибка","Неверный адрес",QMessageBox::Ok);
        return;
    }
    ui->cb_emailList->addItem(emailLine);
}
