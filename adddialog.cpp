#include "adddialog.h"
#include "ui_adddialog.h"

addDialog::addDialog(const int &sw, QWidget *parent) : QDialog(parent), ui(new Ui::addDialog)
{
    // sw 0-normal, 1-masked
    ui->setupUi(this);

    if (sw==1)
    {ui->le_data->setEchoMode(QLineEdit::Password);}
}

addDialog::~addDialog()
{
    delete ui;
}

void addDialog::on_btn_save_clicked()
{
    emit this->sendLine(ui->le_data->text());
    this->close();
}

void addDialog::on_btn_Cancel_clicked()
{
    this->close();
}
