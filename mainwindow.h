#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QProgressBar>
#include <QStandardItemModel>
#include <QStylePainter>
#include <QFileDialog>
#include <QDesktopServices>
#include <QPointer>
#include <QUrl>
#include <QThread>

#include "files.h"
#include "threads.h"
#include "dlg_result.h"
#include "dlg_settings.h"
#include "dlg_order.h"

namespace Ui
{
    class mainWindow;
}

class mainWindow : public QMainWindow
{
    Q_OBJECT

    QLabel *sb_lbl_technical;
    QLabel *sb_lbl_info;
    QProgressBar *sb_pb_status;

    QString filename;
    QList<QStringList> settingsList;
    QStandardItemModel *settingsModel;
    QVector<QStringList> allVector; //main resulting search vector
    QStandardItemModel *resModel;
    void refreshSettings();
    inline bool createSettingsModelTemplate();
    void editVendors(const QModelIndex &index);
    
public:
    explicit mainWindow(QWidget *parent = 0);
    ~mainWindow();

signals:
    void sendXlData(const QList<QStringList> &xlData);
    
private slots:
    void on_btn_search_clicked();
    void on_btn_clearNames_clicked();
    void on_btn_loadPrices_clicked();
    void on_btn_clearMemory_clicked();
    void on_btn_order_clicked();

    void on_btn_editPricesDir_clicked();
    void on_btn_editRequestDir_clicked();
    void on_btn_addVendor_clicked();
    void on_btn_removeVendor_clicked();
    void on_btn_refresh_clicked();
    void on_btn_setUp_clicked();
    void on_tvw_settings_doubleClicked(const QModelIndex &index);
    void on_btn_openPricesDir_clicked();
    void on_btn_openRequestDir_clicked();
    void on_btn_overwriteSettings_clicked();

public slots:
    void loadPrices(const QVector<QStringList> &vendorData);
    void updateVendor(const QStringList &vendorData, const int &shRow);
    void setPBValue(const int &val);
    void logger(const unsigned short &stType,const QString &errLine);

private:
    Ui::mainWindow *ui;
};

#endif // MAINWINDOW_H
