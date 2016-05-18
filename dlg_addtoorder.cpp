#include "dlg_addtoorder.h"
#include "ui_dlg_addtoorder.h"

dlg_addToOrder::dlg_addToOrder(QWidget *parent) : QDialog(parent), ui(new Ui::dlg_addToOrder)
{
    ui->setupUi(this);
    ui->le_quantity->setValidator(new QIntValidator(0, 999999999, this));
    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->le_quantity, SIGNAL(returnPressed()), this, SLOT(on_btn_add_clicked()));
}

dlg_addToOrder::~dlg_addToOrder()
{
    delete ui;
}

void dlg_addToOrder::on_btn_add_clicked()
{
    orderList.append(ui->le_quantity->text());

    QPointer<files> fw = new files(this);
    fw->appendOrder(orderList);
    delete fw;
    emit this->addOrderData(orderList);
    this->close();
}

void dlg_addToOrder::on_btn_cancel_clicked()
{
    this->close();
}

bool dlg_addToOrder::setData(const QStringList &dataList)
{
    if (dataList.size() == 6)
    {
        orderList = dataList;
        ui->lbl_name->setText(dataList.at(0));
        ui->lbl_name->setWordWrap(true);
        ui->lbl_realPrice->setText(dataList.at(1));
        ui->lbl_discPrice->setText(dataList.at(2));
        ui->lbl_expDate->setText(dataList.at(3));
        ui->lbl_manufacturer->setText(dataList.at(4));
        ui->lbl_vendor->setText(dataList.at(5));
    }
    return true;
}
