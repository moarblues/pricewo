#include "dlg_composeletter.h"
#include "ui_dlg_composeletter.h"

dlg_composeLetter::dlg_composeLetter(const QList<QStringList> &vendorsList, const QStringList &settingsList,
                                     const QStringList &currentData, QWidget *parent) :
    QDialog(parent), ui(new Ui::dlg_composeLetter)
{
    ui->setupUi(this);
    ui->cb_email->setEnabled(false);

    vList = vendorsList;
    sList = settingsList;
    cList = currentData;

    if (!vendorsList.isEmpty())
    {
        for(int iitm = 0; iitm < vendorsList.size(); ++iitm)
        {ui->cb_vendor->addItem(vendorsList.at(iitm).at(0));}
    }
    else
    {ui->cb_vendor->setEnabled(false);}

    if (currentData.size()>1)
    {
        for(int iitm = 0; iitm < vList.size(); ++iitm)
        {
            if (QString::compare(vList.at(iitm).at(0),cList.at(1)) == 0)
            {
                ui->cb_vendor->setCurrentIndex(iitm);
                for(int il = 1; il < vList.at(iitm).size(); ++il)
                {
                    if (QString::compare(vList.at(iitm).at(il),cList.at(2)) == 0)
                    {ui->cb_email->setCurrentIndex(il-1);}
                }
                break;
            }
        }
    }
    else
    {cList << ui->cb_vendor->currentText() << ui->cb_email->currentText();}

    ui->le_subject->setText(settingsList.at(4));
    ui->te_body->setText(settingsList.at(5));

    attModel=0;
    this->createModel();

    ui->tvw_attachments->setTextElideMode(Qt::ElideLeft);

    ui->gb_attachments->setFixedWidth(this->width() * 0.964);
    int tvwSettingsWidth = ui->gb_attachments->width() * 0.964;
    ui->tvw_attachments->setFixedWidth(tvwSettingsWidth);
    ui->tvw_attachments->setColumnWidth(0,tvwSettingsWidth * 0.95);
}

dlg_composeLetter::~dlg_composeLetter()
{
    delete ui;
}

void dlg_composeLetter::createModel()
{
    if (attModel!=0) {delete attModel; attModel=0;}
    attModel = new QStandardItemModel(this);

    attModel->setHorizontalHeaderLabels(QStringList() << "Приложение");

    for (int irow=0; irow<cList.size(); ++irow)
    {
        if (irow<3) {continue;}
        QStandardItem *att = new QStandardItem(cList.at(irow));
        QList<QStandardItem *> modelrow;
        modelrow.append(att);

        attModel->appendRow(modelrow);
    }

    attModel->appendRow(QList<QStandardItem *>() << new QStandardItem("--Добавить--"));
    ui->tvw_attachments->setModel(attModel);
}

void dlg_composeLetter::on_cb_vendor_currentIndexChanged(const QString &arg1)
{
    ui->cb_email->clear();
    ui->cb_email->setEnabled(true);

    for(int iitm = 0; iitm < vList.size(); ++iitm)
    {
        if (QString::compare(vList.at(iitm).at(0),arg1) == 0)
        {
            if (vList.at(iitm).at(1).isEmpty())
            {ui->cb_email->setEnabled(false); break;}
            for(int il = 1; il < vList.at(iitm).size(); ++il)
            {ui->cb_email->addItem(vList.at(iitm).at(il));}
            break;
        }
    }
}

void dlg_composeLetter::on_btn_ok_clicked()
{
    if (ui->cb_email->currentText().isEmpty())
    {
        QMessageBox::critical(this,"Ошибка","Пустой e-mail адрес",QMessageBox::Ok);
        return;
    }
    cList[1] = ui->cb_vendor->currentText();
    cList[2] = ui->cb_email->currentText();
    emit this->sendAtts(cList);
    this->close();
}

void dlg_composeLetter::on_tvw_attachments_doubleClicked(const QModelIndex &index)
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", sList.at(0));

    if (index.row()+3==cList.size() || !index.isValid())
    {cList.append(filename);}
    else {cList[index.row()+3] = filename;}

    this->createModel();
}

void dlg_composeLetter::on_btn_add_clicked()
{
    this->on_tvw_attachments_doubleClicked(QModelIndex());
}

void dlg_composeLetter::on_btn_delete_clicked()
{
    cList.removeAt(ui->tvw_attachments->currentIndex().row()+3);
    this->createModel();
}

void dlg_composeLetter::on_btn_cancel_clicked()
{
    this->close();
}
