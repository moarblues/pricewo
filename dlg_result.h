#ifndef DLG_RESULT_H
#define DLG_RESULT_H

#include <QDialog>
#include <QStandardItemModel>
#include <QTime>
#include <QDesktopWidget>
#include <QPointer>

#include "dlg_addtoorder.h"

namespace Ui
{
class dlg_result;
}

class dlg_result : public QDialog
{
    Q_OBJECT

    QStandardItemModel *resModel;

    QList<QStringList> allResList;
    QList<QStringList> s1List;
    QList<QStringList> s2List;
    void createModel(const QList<QStringList> &modList);
    void setModelColumnsWidth(const int &effWidth);
    
public:
    explicit dlg_result(QWidget *parent = 0);
    ~dlg_result();

    void procResVector(const QVector<QStringList> &allVector, const QStringList &searchCrit, const QList<QStringList> &discList);
    
private slots:
    void on_le_search1_textChanged(const QString &arg1);
    void on_le_search2_textChanged(const QString &arg1);
    void on_tvw_Result_doubleClicked(const QModelIndex &index);

public slots:
    void orderLogger(const QStringList &rqdatalist);

private:
    Ui::dlg_result *ui;
};

#endif // DLG_RESULT_H
