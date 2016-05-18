#ifndef DLG_ADDTOREQUEST_H
#define DLG_ADDTOREQUEST_H

#include <QDialog>
#include <QPointer>

#include "files.h"

namespace Ui
{
class dlg_addToOrder;
}

class dlg_addToOrder : public QDialog
{
    Q_OBJECT

    QStringList orderList;
    
public:
    explicit dlg_addToOrder(QWidget *parent = 0);
    virtual ~dlg_addToOrder();

    bool setData(const QStringList &dataList);

signals:
    void addOrderData(const QStringList &dataList);
    
private slots:
    void on_btn_add_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::dlg_addToOrder *ui;
};

#endif // DLG_ADDTOREQUEST_H
