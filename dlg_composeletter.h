#ifndef DLG_COMPOSELETTER_H
#define DLG_COMPOSELETTER_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>

namespace Ui {
class dlg_composeLetter;
}

class dlg_composeLetter : public QDialog
{
    Q_OBJECT

    QStandardItemModel *attModel;
    QList<QStringList> vList;
    QStringList sList;
    QStringList cList;
    QStringList attList;

    void createModel();
    
public:
    explicit dlg_composeLetter(const QList<QStringList> &vendorsList, const QStringList &settingsList,
                               const QStringList &currentData, QWidget *parent = 0);
    ~dlg_composeLetter();

signals:
    void sendAtts(const QStringList &attList);
    
private slots:
    void on_cb_vendor_currentIndexChanged(const QString &arg1);
    void on_btn_ok_clicked();
    void on_tvw_attachments_doubleClicked(const QModelIndex &index);
    void on_btn_add_clicked();
    void on_btn_delete_clicked();
    void on_btn_cancel_clicked();

private:
    Ui::dlg_composeLetter *ui;
};

#endif // DLG_COMPOSELETTER_H
