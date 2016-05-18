#ifndef DLG_EMAIL_H
#define DLG_EMAIL_H

#include <QDialog>
#include <QMessageBox>
#include <QPointer>
#include <QThread>
#include <QCloseEvent>

#include "adddialog.h"
#include "files.h"
#include "threads.h"
#include "dlg_composeletter.h"

namespace Ui {
class dlg_email;
}

class dlg_email : public QDialog
{
    Q_OBJECT

    QStandardItemModel *venModel;
    QList<QStringList> currentData;
    QList<QStringList> vendorList;
    QStringList settingsList;
    bool working;

    void createModel();
    void readVendorsList();
    void readSettingsList();

protected:
    void closeEvent(QCloseEvent * ce);
    
public:
    explicit dlg_email(QWidget *parent = 0);
    virtual ~dlg_email();

signals:
    void setStatus(const unsigned short &stType, const QString &stLine);

public slots:
    void addVendorRecord(const QStringList &vendorData);
    void logger(const unsigned short &stType,const QString &errLine);
    void sendEmail(const QString &pwd);
    void sendInfo(const int &rowNum, const int &sendCode);
    
private slots:
    void on_btn_save_clicked();
    void on_btn_reset_clicked();
    void on_tvw_vendors_doubleClicked(const QModelIndex &index);
    void on_tn_send_clicked();
    void on_btn_delete_clicked();
    void on_btn_close_clicked();

private:
    Ui::dlg_email *ui;
};

#endif // DLG_EMAIL_H
