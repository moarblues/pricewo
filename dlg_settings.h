#ifndef DLG_SETTINGS_H
#define DLG_SETTINGS_H

#include <QDialog>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QDateTime>
#include <QPointer>
#include <QMessageBox>

#include "adddialog.h"

namespace Ui
{
    class dlg_settings;
}

class dlg_settings : public QDialog
{
    Q_OBJECT

    QString filename;
    QString pricesDirPath;
    int ppRow;

public:
    explicit dlg_settings(const QString &pricesDir, QWidget *parent = 0);
    ~dlg_settings();

    void alterVendor(const QStringList &dataList, const int &updRow);
    void appendVendor(const QModelIndex &shIndex);
    
signals:
    void sendSettings(const QStringList &dataList, const int &row);
    void setStatus(const unsigned short &stType, const QString &stLine);

private slots:
    void on_btn_save_clicked();
    void on_btn_setFilePath_clicked();
    void on_btn_cancel_clicked();
    void on_btn_addEmail_clicked();
    void on_btn_removeEmail_clicked();

public slots:
    void addEmailList(const QString &emailLine);

private:
    Ui::dlg_settings *ui;
};

#endif // DLG_SETTINGS_H
