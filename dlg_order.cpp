#include "dlg_order.h"
#include "ui_dlg_order.h"

dlg_order::dlg_order(QWidget *parent) : QDialog(parent), ui(new Ui::dlg_order)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->resize(parent->width(),parent->height());
    this->move(parent->width()/2,parent->height()/2);
    this->setFixedWidth(parent->width());
    priceModel=0;
}

dlg_order::~dlg_order()
{
    delete ui;
}

void dlg_order::setOrderModel(const QList<QStringList> &orderList)
{
    if (priceModel!=0) {delete priceModel; priceModel=0;}
    priceModel = new QStandardItemModel(this);
    if (orderList.isEmpty())
    {
        ui->btn_email->setEnabled(false);
        ui->btn_save->setEnabled(false);
        ui->btn_clearOrder->setEnabled(false);
        ui->btn_delete->setEnabled(false);
        return;
    }

    for (int irow=0; irow<orderList.size(); ++irow)
    {
        QStandardItem *name = new QStandardItem(orderList.at(irow).at(0));
        QStandardItem *price = new QStandardItem();
        price->setData(orderList.at(irow).at(1).toDouble(), Qt::DisplayRole);
        QStandardItem *discprice = new QStandardItem();
        discprice->setData(orderList.at(irow).at(2).toInt(), Qt::DisplayRole);
        QStandardItem *expDate = new QStandardItem(orderList.at(irow).at(3));
        QStandardItem *manufactorer = new QStandardItem(orderList.at(irow).at(4));
        QStandardItem *vendor = new QStandardItem(orderList.at(irow).at(5));
        QStandardItem *quantity = new QStandardItem();
        quantity->setData(orderList.at(irow).at(6).toInt(),Qt::DisplayRole);
        name->setEditable(false);
        price->setEditable(false);
        discprice->setEditable(false);
        expDate->setEditable(false);
        manufactorer->setEditable(false);
        vendor->setEditable(false);

        QList<QStandardItem *> modelrow;
        modelrow.append(name);
        modelrow.append(price);
        modelrow.append(discprice);
        modelrow.append(expDate);
        modelrow.append(manufactorer);
        modelrow.append(vendor);
        modelrow.append(quantity);

        priceModel->appendRow(modelrow);
    }

    priceModel->setHorizontalHeaderLabels(QStringList() << "Наименование" << "Цена" << "Цена со скидкой"
                                          << "Срок годности"<< "Производитель"<< "Поставщик"
                                          << "Количество");

    ui->tvw_order->setModel(priceModel);
    this->setPriceModelColumnsWidth();
}

void dlg_order::deleteRecord(const QModelIndex &index)
{
    QMessageBox::StandardButton reply;
    QString pos;
    pos.append("Удалить ").append(priceModel->item(index.row(),0)->text()).append("?");
    reply = QMessageBox::question(this, "Удаление", pos,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        priceModel->removeRow(index.row());
        this->updateOrder();
    }
}

void dlg_order::updateOrder()
{
    QList<QStringList> outList;
    QStringList rowList;

    for (int irow=0; irow<priceModel->rowCount(); ++irow)
    {
        rowList.clear();
        for (int jcol=0; jcol<priceModel->columnCount(); ++jcol)
        {
            rowList.append(priceModel->item(irow,jcol)->text());
        }
        outList.append(rowList);
    }

    QPointer<files> fw = new files(this);
    fw->alterOrder(outList);
    this->setOrderModel(fw->readOrder());
    delete fw;
}

void dlg_order::saveOrder(const QList<QStringList> &saveList)
{
    QList<QStringList> outList;
    QStringList rowList;

    QPointer<files> fw = new files(this);
    for (int iitm = 0; iitm < saveList.size(); ++iitm)
    {
        outList.clear();
        for (int irow = 0; irow < priceModel->rowCount(); ++irow)
        {
            rowList.clear();
            if (QString::compare(saveList.at(iitm).at(0),priceModel->item(irow,5)->text()) == 0)
            {
                rowList.append(priceModel->item(irow,0)->text());
                rowList.append(priceModel->item(irow,4)->text());
                rowList.append(priceModel->item(irow,6)->text());

                outList.append(rowList);
            }
        }
        if (QString::compare(saveList.at(iitm).at(1),"true") == 0)
        {fw->writeXls(outList, saveList.at(iitm).at(0));}
        if (QString::compare(saveList.at(iitm).at(2),"true") == 0)
        {fw->writeXlsx(outList, saveList.at(iitm).at(0));}
        if (QString::compare(saveList.at(iitm).at(3),"true") == 0)
        {fw->printOrder(outList, saveList.at(iitm).at(0),this);}
    }
    delete fw;
}

void dlg_order::setPriceModelColumnsWidth()
{
    unsigned int effWidth = this->width() * 0.964;
    ui->tvw_order->setColumnWidth(0,effWidth * 0.345);
    ui->tvw_order->setColumnWidth(1,effWidth * 0.07);
    ui->tvw_order->setColumnWidth(2,effWidth * 0.07);
    ui->tvw_order->setColumnWidth(3,effWidth * 0.1);
    ui->tvw_order->setColumnWidth(4,effWidth * 0.23);
    ui->tvw_order->setColumnWidth(5,effWidth * 0.11);
    ui->tvw_order->setColumnWidth(6,effWidth * 0.06);
    ui->btn_delete->setEnabled(true);
}

void dlg_order::on_btn_close_clicked()
{
    this->close();
}

void dlg_order::on_btn_save_clicked()
{
    QStringList vendorList;

    for (int irow = 0; irow < priceModel->rowCount(); ++irow)
    {
        if (vendorList.contains(priceModel->item(irow,5)->text()))
        {continue;}
        else
        {vendorList.append(priceModel->item(irow,5)->text());}
    }

    QPointer<dlg_saveOrder> so = new dlg_saveOrder(vendorList, this);
    connect(so,SIGNAL(sendSaveList(QList<QStringList>)),this,SLOT(saveOrder(QList<QStringList>)));
    so->setModal(true);
    so->show();
}

void dlg_order::on_btn_clearOrder_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Удаление", "Удалить заявку?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        priceModel->clear();
        this->updateOrder();
    }
}

void dlg_order::on_btn_delete_clicked()
{
    if (ui->tvw_order->currentIndex().isValid()) {this->deleteRecord(ui->tvw_order->currentIndex());}
}

void dlg_order::on_tvw_order_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {this->deleteRecord(index);}
}

void dlg_order::on_btn_update_clicked()
{
    this->updateOrder();
}

void dlg_order::on_btn_email_clicked()
{
    QPointer<dlg_email> email = new dlg_email(this);
    email->setModal(true);
    email->show();
}
