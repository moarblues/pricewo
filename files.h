#ifndef FILES_H
#define FILES_H

#include <QFileDialog>
#include <QTextStream>
#include <QDate>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QRect>

#include "lib/include/freexl/freexl.h"
#include "lib/include/xlslib/xlslib.h"
//#include "freexl.h"
//#include "xlslib/xlslib.h"
#include "xlsxdocument.h"
#include "xlsxcellrange.h"
#include "xlsxformat.h"

class files : public QObject
{
    Q_OBJECT

    QString orderDir;
    QString settingsFileName;
    QString orderFileName;
    QString emailSettingsFileName;
    QStringList orderHeaders;

    QString customWordWrap(const QString &instr, const int wraplen);
    inline QString processFileName(const QString &mode, const QString &vendor);

public:
    explicit files(QWidget *parent = 0);
    virtual ~files();

    void checkIntegrity();
    QList<QStringList> readSettings();
    QStringList readEmailSettings();
    QList<QStringList> readOrder();
    void overwriteSettings();
    void writeFile(const QString &outstr, const QString &fname);
    void writeVendorSettings(const QList<QStringList> &ppList);
    void writeDirSettings(const QStringList &dataList);
    void writeEmailSettings(const QStringList &dataList);
    void appendOrder(const QStringList &orderList);
    void alterOrder(const QList<QStringList> &outList);
    QVector<QStringList> getXlsData(const QString &fpath, const QString &vendor, const QStringList &dataColumns);
    QVector<QStringList> getXlsxData(const QString &fpath, const QString &vendor, const QStringList &dataColumns);
    void writeXls(const QList<QStringList> &dataList, const QString &vendor);
    void writeXlsx(const QList<QStringList> &dataList, const QString &vendor);
    void printOrder(const QList<QStringList> &dataList, const QString &vendor, QWidget *pp);

signals:
    void setStatus(const unsigned short &stType, const QString &stLine);
};

#endif // FILES_H
