#include "dlg_saveorder.h"
#include "ui_dlg_saveorder.h"

dlg_saveOrder::dlg_saveOrder(const QStringList &vendorList, QWidget *parent) : QDialog(parent), ui(new Ui::dlg_saveOrder)
{
    ui->setupUi(this);
    saveModel = 0;
    this->fillModel(vendorList);
}

dlg_saveOrder::~dlg_saveOrder()
{
    delete ui;
}

void dlg_saveOrder::fillModel(const QStringList &vendorList)
{
    saveModel = new QStandardItemModel(this);

    for (int iitm=0; iitm<vendorList.size(); ++iitm)
    {
        QStandardItem *vendor = new QStandardItem(vendorList.at(iitm));
        QStandardItem *xlsCB = new QStandardItem();
        xlsCB->setCheckable(true);
        xlsCB->setCheckState(Qt::Checked);
        QStandardItem *xlsxCB = new QStandardItem();
        xlsxCB->setCheckable(true);
        xlsxCB->setCheckState(Qt::Unchecked);
        QStandardItem *printCB = new QStandardItem();
        printCB->setCheckable(true);
        printCB->setCheckState(Qt::Unchecked);

        QList<QStandardItem *> modelRow;
        modelRow.append(vendor);
        modelRow.append(xlsCB);
        modelRow.append(xlsxCB);
        modelRow.append(printCB);

        saveModel->appendRow(modelRow);
    }

    saveModel->setHorizontalHeaderLabels(QStringList() << "Поставщик" << "xls" << "xlsx" << "печать");

    ui->tvw_vendors->setModel(saveModel);

    unsigned int effWidth = this->width() * 0.964;
    ui->tvw_vendors->setColumnWidth(0,effWidth * 0.71);
    ui->tvw_vendors->setColumnWidth(1,effWidth * 0.06);
    ui->tvw_vendors->setColumnWidth(2,effWidth * 0.06);
    ui->tvw_vendors->setColumnWidth(3,effWidth * 0.09);
}

void dlg_saveOrder::on_btn_process_clicked()
{
    QList<QStringList> saveList;
    QStringList rowList;

    for(int irow=0; irow<saveModel->rowCount(); ++irow)
    {
        rowList.clear();
        rowList.append(saveModel->item(irow,0)->text());
        if (saveModel->item(irow,1)->checkState() == Qt::Checked)
        {rowList.append("true");}
        else
        {rowList.append("false");}

        if (saveModel->item(irow,2)->checkState() == Qt::Checked)
        {rowList.append("true");}
        else
        {rowList.append("false");}

        if (saveModel->item(irow,3)->checkState() == Qt::Checked)
        {rowList.append("true");}
        else
        {rowList.append("false");}

        saveList.append(rowList);
    }
    emit this->sendSaveList(saveList);
    this->close();
}

void dlg_saveOrder::on_btn_close_clicked()
{
    this->close();
}
