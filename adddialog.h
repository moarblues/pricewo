#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>

namespace Ui
{
class addDialog;
}

class addDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit addDialog(const int &sw, QWidget *parent = 0);
    ~addDialog();

signals:
    void sendLine(const QString &line);
    
private slots:
    void on_btn_save_clicked();

    void on_btn_Cancel_clicked();

private:
    Ui::addDialog *ui;
};

#endif // ADDDIALOG_H
