#ifndef DLG_REQUEST_H
#define DLG_REQUEST_H

#include <QDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QPointer>

#include "files.h"
#include "dlg_saveorder.h"
#include "dlg_email.h"

namespace Ui
{
class dlg_order;
}

class dlg_order : public QDialog
{
    Q_OBJECT

    QStandardItemModel *priceModel;

    void setPriceModelColumnsWidth();
    
public:
    explicit dlg_order(QWidget *parent = 0);
    virtual ~dlg_order();

    void setOrderModel(const QList<QStringList> &orderList);
    void deleteRecord(const QModelIndex &index);
    void updateOrder();

public slots:
    void saveOrder(const QList<QStringList> &saveList);
    
private slots:
    void on_btn_close_clicked();
    void on_btn_save_clicked();
    void on_btn_clearOrder_clicked();
    void on_btn_delete_clicked();
    void on_tvw_order_doubleClicked(const QModelIndex &index);
    void on_btn_update_clicked();
    void on_btn_email_clicked();

private:
    Ui::dlg_order *ui;
};

#endif // DLG_REQUEST_H
