#ifndef DLG_SAVEORDER_H
#define DLG_SAVEORDER_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class dlg_saveOrder;
}

class dlg_saveOrder : public QDialog
{
    Q_OBJECT

    QStandardItemModel *saveModel;
    
    void fillModel(const QStringList &vendorList);

public:
    explicit dlg_saveOrder(const QStringList &vendorList, QWidget *parent = 0);
    virtual ~dlg_saveOrder();
    
private slots:
    void on_btn_process_clicked();
    void on_btn_close_clicked();

signals:
    void sendSaveList(const QList<QStringList> &saveList);

private:
    Ui::dlg_saveOrder *ui;
};

#endif // DLG_SAVEORDER_H
