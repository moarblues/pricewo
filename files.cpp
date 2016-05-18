#include "files.h"

files::files(QWidget *parent) : QObject(parent)
{
//    settingsFileName = QDir::currentPath().append("/settings.conf");
    settingsFileName = QDir::homePath().append("/.config/pricewo/settings.conf");
    orderFileName = QDir::homePath().append("/.config/pricewo/order");
//    emailSettingsFileName = QDir::currentPath().append("/emailsettings.conf");
    emailSettingsFileName = QDir::homePath().append("/.config/pricewo/emailsettings.conf");
    orderHeaders << "№" << "Наименование" << "Производитель" << "К-во";
}

files::~files()
{
    //    delete this;
}

void files::checkIntegrity()
{
    if (!QFile::exists(settingsFileName))
    {
        emit this->setStatus(0,"Не найден файл настроек, создание");
        this->overwriteSettings();
    }
    if (!QFile::exists(orderFileName))
    {
        emit this->setStatus(0,"Не найден файл заявки, создание");
        this->writeFile("",orderFileName);
    }
    if (!QFile::exists(emailSettingsFileName))
    {
        emit this->setStatus(0,"Не найден файл настроек электронной почты, создание");
        this->writeFile("",emailSettingsFileName);
    }

}

QList<QStringList> files::readSettings()
{
    QFile *chOrder = new QFile(orderFileName,this);
    if (chOrder->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (!chOrder->atEnd())
        {
            emit this->setStatus(0,"Внимание! Заявка не пуста");
        }
    }
    chOrder->close();
    delete chOrder;

    QList<QStringList> outlist;
    QFile *fdata = new QFile(settingsFileName,this);
    if (fdata->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const QString &errWS("Путь каталога или значение не должны содержать символа ");
        QString fline;
        QStringList flist;

        QTextStream in(fdata);
        in.setCodec("UTF-8");
        do
        {
            fline = in.readLine();

            if (fline.isEmpty() || QString::compare(fline.at(0),"#") == 0)
            {continue;}

            if (QString::compare(fline.at(0),"!") == 0)
            {
                flist.clear();
                flist = fline.split("--");

                if (flist.size()!=2)
                {
                    emit this->setStatus(1,QString(errWS).append("--"));
                    continue;
                }

                if (QString::compare(flist.at(0),"!pricedir") == 0)
                {
                    if(QDir(flist.at(1)).exists())
                    {
                        outlist.append(QStringList() << flist.at(1));
                        continue;
                    }
                    else
                    {
                        outlist.append(QStringList() << QDir::currentPath());
                        continue;
                    }
                }

                if (QString::compare(flist.at(0),"!requestdir") == 0)
                {
                    if (QDir(flist.at(1)).exists())
                    {
                        orderDir = flist.at(1);
                        outlist.append(QStringList() << orderDir);

                        if (!QDir(orderDir).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() == 0)
                        {
                            emit this->setStatus(0,"Внимание! Директория заявок не пуста");
                        }
                        continue;
                    }
                    else
                    {
                        outlist.append(QStringList() << QDir::currentPath());
                        continue;
                    }
                }
                continue;
            }

            flist = fline.split(";;");

            if (flist.size() == 1)
            {
                emit this->setStatus(1,"Ошибка в файле настроек, перезапишите файл");
                continue;
            }
            if (flist.size()!=6)
            {
                emit this->setStatus(1,(QString(errWS).append(";;")));
                flist << "" << "" << "" << "0-0-0-0" << "0.00" << "";
            }

            outlist.append(flist);
            continue;

        } while (!fline.isNull());
        fdata->close();
        delete fdata;
    }
    return outlist;
}

QStringList files::readEmailSettings()
{
    QStringList outlist;
    this->readSettings();
    outlist.append(orderDir);
    QFile *fdata = new QFile(emailSettingsFileName,this);
    if (fdata->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const QString &errWS("Путь каталога или значение не должны содержать символа ");
        QString fline;
        QString bodyline;
        QStringList flist;

        QTextStream in(fdata);
        in.setCodec("UTF-8");
        do
        {
            fline = in.readLine();
            if (fline.isEmpty() || QString::compare(fline.at(0),"#") == 0)
            {continue;}

            if (QString::compare(fline.at(0),"!") == 0)
            {
                flist.clear();
                flist = fline.split("--");

                if (flist.size()!=2)
                {
                    emit this->setStatus(1,QString(errWS).append("--"));
                    continue;
                }

                if (QString::compare(fline,"!body--begin")==0)
                {
                    do
                    {
                        fline = in.readLine();
                        if (QString::compare(fline,"!body--end")==0)
                        {break;}
                        bodyline.append(fline).append("\n");
                    } while(!fline.isNull());
                    outlist.append(bodyline);
                    continue;
                }

                if (flist.at(1).isNull() || flist.at(1).isEmpty())
                {
                    outlist.append("");
                    continue;
                }
                else
                {
                    outlist.append(flist.at(1));
                    continue;
                }
            }

        } while (!fline.isNull());
        fdata->close();
        delete fdata;
    }
    return outlist;
}

void files::writeFile(const QString &outstr, const QString &fname)
{
    QFile *outfile = new QFile(this);
    QTextStream out(outfile);

    outfile->setFileName(fname);
    out.setCodec("UTF-8");
    if (outfile->open(QIODevice::WriteOnly | QIODevice::Text))
    {out << outstr;}
    outfile->flush();
    outfile->close();
    delete outfile;
}

void files::writeDirSettings(const QStringList &dataList)
{
    QFile *exFile = new QFile(this);
    QString line, tmpName, outstr;
    QStringList itmList;

    exFile->setFileName(settingsFileName);
    tmpName = settingsFileName;
    tmpName.append(".tmp");

    if (exFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream out(exFile);
        out.setCodec("UTF-8");

        do
        {
            line = out.readLine();
            if (line.isNull() || line.isEmpty()) {continue;}
            if (QString::compare(line.at(0),"!") == 0)
            {
                itmList = line.split("--");

                if (QString::compare(dataList.at(0),itmList.at(0)) == 0)
                {
                    outstr.append(dataList.at(0)).append("--").append(dataList.at(1)).append("\n");
                    continue;
                }
            }
            outstr.append(line).append("\n");
        } while(!line.isNull());
    }

    exFile->close();
    delete exFile;
    this->writeFile(outstr, tmpName);
    QFile::remove(settingsFileName);
    QFile::rename(tmpName,settingsFileName);

    this->readSettings();
}

void files::writeVendorSettings(const QList<QStringList> &ppList)
{
    QFile *exFile = new QFile(this);
    QString line, tmpName, outstr;

    exFile->setFileName(settingsFileName);
    tmpName = settingsFileName;
    tmpName.append(".tmp");

    if (exFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream out(exFile);
        out.setCodec("UTF-8");

        do
        {
            line = out.readLine();
            if (line.isNull() || line.isEmpty()) {continue;}
            if (QString::compare(line.at(0),"#") == 0 || QString::compare(line.at(0),"!") == 0)
            {
                outstr.append(line).append("\n");
                continue;
            }

        } while(!line.isNull());
    }

    for (int irow = 2; irow < ppList.size(); ++irow)
    {
        if (irow > 2) {outstr.append("\n");}
        for (int jcol = 0; jcol < ppList.at(irow).size(); ++jcol)
        {
            QString str = ppList.at(irow).at(jcol);
            if (jcol > 0) {outstr.append(";;");}
            foreach (QChar stritm, str)
            {
                if (stritm == '\\')
                {
                    outstr.append("\\\\");
                }
                else
                {
                    outstr.append(stritm);
                }
            }    
        }
    }

    exFile->close();
    delete exFile;
    this->writeFile(outstr, tmpName);
    QFile::remove(settingsFileName);
    QFile::rename(tmpName,settingsFileName);
}

void files::writeEmailSettings(const QStringList &dataList)
{
    if (dataList.size()!=6)
    {emit this->setStatus(1,QString("Набор данных повреждён"));return;}
    QString outstr;
    outstr.append("#smtp\n").append("!server--").append(dataList.at(0)).append("\n!port--").append(dataList.at(1))
            .append("\n!account--").append(dataList.at(2)).append("\n\n#letter\n").append("!sender--").append(dataList.at(3))
            .append("\n!subject--").append(dataList.at(4))
            .append("\n!body--begin\n").append(dataList.at(5)).append("\n!body--end");

    QFile::remove(emailSettingsFileName);
    this->writeFile(outstr, emailSettingsFileName);
}

void files::overwriteSettings()
{
    this->writeDirSettings(QStringList() << "!pricedir" << QDir::currentPath());
    this->writeDirSettings(QStringList() << "!requestdir" << QDir::currentPath());
    QList<QStringList> lslEmpty;
    lslEmpty.append(QStringList() << "" << "" << "" << "0-0-0-0" << "0.00" << "");
    this->writeVendorSettings(lslEmpty);
}

QList<QStringList> files::readOrder()
{
    QFile *exFile = new QFile(this);
    QString fline;
    QList<QStringList> orderList;

    exFile->setFileName(orderFileName);

    if (exFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream out(exFile);
        out.setCodec("UTF-8");

        do
        {
            fline = out.readLine();
            if (fline.isNull() || fline.isEmpty()) {continue;}
            orderList.append(fline.split(";;"));
        } while(!fline.isNull());

    }
    return orderList;
}

void files::appendOrder(const QStringList &orderList)
{
    QFile *dFile = new QFile(this);
    QString outstr, tmpline;

    dFile->setFileName(orderFileName);

    if (dFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream out(dFile);
        out.setCodec("UTF-8");
        outstr.clear();

        do
        {
            tmpline = out.readLine();
            if (tmpline.isNull() || tmpline.isEmpty()) {continue;}
            outstr.append(tmpline).append("\n");
        } while (!tmpline.isNull());

        if (orderList.size() != 7)
        {
            return;
        }

        for (int iItm = 0; iItm < orderList.size(); ++iItm)
        {
            if (iItm > 0) {outstr.append(";;");}
            outstr.append(orderList.at(iItm));
        }
        outstr.append("\n");

        this->writeFile(outstr, orderFileName);
    }

    dFile->close();
    delete dFile;
}

void files::alterOrder(const QList<QStringList> &outList)
{
    QString outstr;

    for (int irow = 0; irow < outList.size(); ++irow)
    {
        if (irow > 0)
        {outstr.append("\n");}
        for (int jcol = 0; jcol < outList.at(irow).size(); ++jcol)
        {
            if (jcol > 0)
            {outstr.append(";;");}
            QString str = outList.at(irow).at(jcol);
            foreach (QChar stritm, str)
            {
                if (stritm == '\\')
                {outstr.append("\\\\");}
                else
                {outstr.append(stritm);}
            }
        }
    }
    this->writeFile(outstr, orderFileName);
}

QVector<QStringList> files::getXlsData(const QString &fpath, const QString &vendor, const QStringList &dataColumns)
{
    unsigned int worksheet_index;
    unsigned int max_worksheet;
    unsigned int info;
    int ret;
    const void *handle;
    unsigned int rows;
    unsigned short columns;
    unsigned int row;
    unsigned short col;
    unsigned short pos;

    QVector<QStringList> outVector;
    QString cellstr;

    // opening the .XLS file [Workbook]
    ret = freexl_open (fpath.toStdString().c_str(), &handle);
    if (ret != FREEXL_OK)
    {
        emit this->setStatus(1, QString("Ошибка открытия: ").append(QString::number(ret).append(" -").append(vendor)));
        return outVector;
    }

    // checking for Password (obfuscated/encrypted)
    ret = freexl_get_info (handle, FREEXL_BIFF_PASSWORD, &info);
    if (ret != FREEXL_OK && vendor != "Медикус")
    {
        emit this->setStatus(1, QString("Ошибка получения информации (необходим пароль) ")
                .append(QString::number(FREEXL_BIFF_PASSWORD)).append(": ").append(QString::number(ret))
                .append(" -").append(vendor));
        return outVector;
    }
    switch (info)
    {
        case FREEXL_BIFF_PLAIN:
            break;
        case FREEXL_BIFF_OBFUSCATED:
        default:
            emit this->setStatus(1, QString("Защищено паролем, нет доступа").append(" -").append(vendor));
            return outVector;
    };

    // querying BIFF Worksheet entries
    ret = freexl_get_info (handle, FREEXL_BIFF_SHEET_COUNT, &max_worksheet);
    if (ret != FREEXL_OK)
    {
        emit this->setStatus(1, QString("Ошибка получения информации (ошибка чтения листов)")
               .append(FREEXL_BIFF_SHEET_COUNT).append(": ").append(QString::number(ret)).append(" -").append(vendor));
        return outVector;
    }

    for (worksheet_index = 0; worksheet_index < max_worksheet; ++worksheet_index)
    {
        const char *utf8_worsheet_name;
        ret = freexl_get_worksheet_name (handle, worksheet_index, &utf8_worsheet_name);
        if (ret != FREEXL_OK)
        {
            emit this->setStatus(1, QString("Ошибка чтения листов: ")
                                 .append(QString::number(ret)).append(" -").append(vendor));
            return outVector;
         }
    // selecting the active Worksheet
        ret = freexl_select_active_worksheet (handle, worksheet_index);
        if (ret != FREEXL_OK)
        {
            emit this->setStatus(1, QString("Ошибка выбора текущего листа: ").append(QString::number(ret))
                    .append(" -").append(vendor));
            return outVector;
        }
    // dimensions
        ret = freexl_worksheet_dimensions (handle, &rows, &columns);
        if (ret != FREEXL_OK)
        {
            emit this->setStatus(1, QString("Ошибка структуры листов: ")
                                 .append(QString::number(ret)).append(" - ").append(vendor));
            return outVector;
        }

        for (row = 0; row < rows; ++row)
        {
//            entry = 1;
            QStringList recordRow;
            recordRow << "" << "" << "" << "";
            FreeXL_CellValue cell;
            for (col = 0; col < columns; ++col)
            {
                if (!dataColumns.contains(QString::number(col+1))) {continue;}
                else
                {
                    pos = dataColumns.indexOf(QString::number(col+1));
//                    if (pos == -1) {entry++;}
                }

                ret = freexl_get_cell_value (handle, row, col, &cell);
                if (ret != FREEXL_OK)
                {
                    emit this->setStatus(1, QString("Ошибка значения ячейки (строка=")
                                         .append(row).append(" столбец=").append(col).append("): ")
                                         .append(QString::number(ret)).append(" - ").append(vendor));
                    return outVector;
                }
                switch (cell.type)
                {
                case FREEXL_CELL_INT:
                    cellstr = QString::number(cell.value.int_value);
                    break;
                case FREEXL_CELL_DOUBLE:
                    cellstr = QString::number(cell.value.double_value);
                    break;
                case FREEXL_CELL_TEXT:
                case FREEXL_CELL_SST_TEXT:
                    cellstr = cell.value.text_value;
                    break;
                case FREEXL_CELL_DATE:
                case FREEXL_CELL_DATETIME:
                case FREEXL_CELL_TIME:
                    cellstr = cell.value.text_value;
                    break;
                case FREEXL_CELL_NULL:
                    cellstr = "";
                default:
                    break;
                };
                recordRow[pos] = cellstr.simplified();
            }
            if (recordRow.at(0).isEmpty()) {continue;}
            recordRow.append(vendor);
            outVector.append(recordRow);
        }
    }
    ret = freexl_close(handle);
    if (ret != FREEXL_OK)
    {
        emit this->setStatus(1, QString("Ошибка закрытия: ").append(QString::number(ret)).append(" -").append(vendor));
        return outVector;
    }
    return outVector;
}

QVector<QStringList> files::getXlsxData(const QString &fpath, const QString &vendor, const QStringList &dataColumns)
{
    QXlsx::Document xlsxentry(fpath);
    QVector<QStringList> outVector;

    for (int irow=0; irow<xlsxentry.dimension().rowCount(); ++irow)
    {
        QStringList recordRow;
        recordRow << "" << "" << "" << "";

        recordRow[0] = xlsxentry.read(irow,dataColumns.at(0).toInt()).toString().simplified();
        recordRow[1] = xlsxentry.read(irow,dataColumns.at(1).toInt()).toString().simplified();
        recordRow[2] = xlsxentry.read(irow,dataColumns.at(2).toInt()).toString().simplified();
        recordRow[3] = xlsxentry.read(irow,dataColumns.at(3).toInt()).toString().simplified();

        if (recordRow.at(0).isEmpty()) {continue;}
        recordRow.append(vendor);
        outVector.append(recordRow);
        continue;
    }
    return outVector;
}

void files::writeXls(const QList<QStringList> &dataList, const QString &vendor)
{
    if (dataList.isEmpty()) {return;}

//    unsigned32_t modelColumns = 5;
    QString tmpstr;

   // Get the workbook handler
   xlslib_core::workbook wb;
   xlslib_core::worksheet* sh = wb.sheet("Заявка");
//   xlslib_core::xf_t* xformat = wb.xformat();

    sh->colwidth(0,700);
    sh->colwidth(1,13000);
    sh->colwidth(2,4300);
    sh->colwidth(3,1500);

    // Title
   sh->label(0,0,"Заявка от \"ИП Прохорова\"");
   sh->merge(0,0,0,1);
   sh->merge(1,0,1,1);
   sh->label(0,2,QDate::currentDate().toString("dd.MM.yyyy").toStdString().c_str());
   sh->rowheight(1,400);
   sh->rowheight(2,400);

   sh->label(1,0,vendor.toStdString().c_str());

   for (int tcol = 0; tcol <= 3; ++tcol)
   {
       xlslib_core::cell_t *curcell = sh->label(3,tcol,orderHeaders.at(tcol).toStdString().c_str());
       curcell->fontbold((xlslib_core::boldness_option_t)0);
       curcell->borderstyle((xlslib_core::border_side_t)0, (xlslib_core::border_style_t)1);
       curcell->borderstyle((xlslib_core::border_side_t)1, (xlslib_core::border_style_t)1);
       curcell->borderstyle((xlslib_core::border_side_t)2, (xlslib_core::border_style_t)1);
       curcell->borderstyle((xlslib_core::border_side_t)3, (xlslib_core::border_style_t)1);
   }

   for (int irow = 0; irow < dataList.size(); ++irow)
   {
       for (int jcol = 0; jcol < dataList.at(irow).size(); ++jcol)
       {
           tmpstr.clear();
           xlslib_core::cell_t *numcell =sh->label(irow+4, 0, QString::number(irow+1).toStdString().c_str());
           numcell->borderstyle((xlslib_core::border_side_t)0, (xlslib_core::border_style_t)1);
           numcell->borderstyle((xlslib_core::border_side_t)1, (xlslib_core::border_style_t)1);
           numcell->borderstyle((xlslib_core::border_side_t)2, (xlslib_core::border_style_t)1);
           numcell->borderstyle((xlslib_core::border_side_t)3, (xlslib_core::border_style_t)1);

           xlslib_core::cell_t *curcell = sh->label(irow+4,jcol+1,this->customWordWrap(dataList.at(irow).at(jcol),53).toStdString().c_str());
           curcell->borderstyle((xlslib_core::border_side_t)0, (xlslib_core::border_style_t)1);
           curcell->borderstyle((xlslib_core::border_side_t)1, (xlslib_core::border_style_t)1);
           curcell->borderstyle((xlslib_core::border_side_t)2, (xlslib_core::border_style_t)1);
           curcell->borderstyle((xlslib_core::border_side_t)3, (xlslib_core::border_style_t)1);
       }
   }


//   WARNING: column and row numbers are zero based in xlslib, but Excel starts numbering the buggers at '1' instead!
//   std::string s("This tab should should show three numbers: two in row 131/133 + one in row 132 at columns 129(DY)/173(FQ) respectively");
//   sh->label(1,1,s);

//   qDebug() << requestDir.append(vendor).append(".xls").toStdString().c_str();
//   int err = wb.Dump(requestDir.append(vendor).toStdString().c_str());

   QString fName = this->processFileName("xls",vendor);
   int err = wb.Dump(fName.toStdString().c_str());
//   int err = wb.Dump("/1.xls");

   if (err != NO_ERRORS)
   {
       emit this->setStatus(1,QString("Не удалось записать файл заявки поставщика ").append(vendor));
       return;
   }

//   char *checkP = file_err;
//   if (err != NO_ERRORS)
//   {
//       std::cerr << "StandardTest failed: I/O failure: " << err << std::endl;
//       return checkP;
//   }
//   if ((checkP = check_file("./testCPP.xls", md5_checksum)))
//   {
//       std::cerr << "StandardTest failed: MD5 of generated XLS mismatch or I/O failure." << std::endl;
//   }
//   return checkP;
}

void files::writeXlsx(const QList<QStringList> &dataList, const QString &vendor)
{
    if (dataList.isEmpty()) {return;}

    QXlsx::Document xlsx;
    QXlsx::Format fFontOnly;
    QXlsx::Format fList;
    QXlsx::Format fListCaps;
//    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
//    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    fFontOnly.setFont(QFont("Verdana", 10));
    fList.setFont(QFont("Verdana", 10));
    fList.setBorderStyle(QXlsx::Format::BorderThin);
    fListCaps.setFont(QFont("Verdana", 10, QFont::Bold));
    fListCaps.setBorderStyle(QXlsx::Format::BorderThin);

    xlsx.renameSheet("Sheet1", "Заявка");
    xlsx.setColumnWidth(1,1,3);
    xlsx.setColumnWidth(2,2,48);
    xlsx.setColumnWidth(3,3,15);
    xlsx.setColumnWidth(4,4,5);
    xlsx.setRowHeight(2,20);
    xlsx.setRowHeight(3,20);
    xlsx.write(1,1, "Заявка от \"ИП Прохорова\"");
    xlsx.mergeCells("A1:B1",fFontOnly);
    xlsx.write(2,1,vendor);
    xlsx.write(1,3,QDate::currentDate().toString("dd.MM.yyyy"),fFontOnly);
    xlsx.mergeCells("A2:B2",fFontOnly);
    xlsx.write(3,1,"");

    for (int iitm=0;iitm<orderHeaders.size();++iitm)
    {
        xlsx.write(4,iitm+1,orderHeaders.at(iitm),fListCaps);
    }

    for (int irow=0; irow<dataList.size(); ++irow)
    {
        for (int jcol=0; jcol<dataList.at(irow).size(); ++jcol)
        {
            xlsx.write(irow+5,1,QString::number(irow+1),fList);
            xlsx.write(irow+5,jcol+2,this->customWordWrap(dataList.at(irow).at(jcol),53),fList);
        }
    }

    QString fName = this->processFileName("xlsx",vendor);
    xlsx.saveAs(fName);
}

QString files::customWordWrap(const QString &instr, const int wraplen)
{
    QStringList seplist;
    seplist << "-" << " " << "/" << "." << ",";

    QString outstr;

    int seppos = 0;

    for (int curpos = 0; curpos < instr.size(); ++curpos)
    {
        if (seplist.contains(instr.at(curpos)))
        {seppos = curpos;}

        outstr.append(instr.at(curpos));

        if (outstr.size() == wraplen)
        {
            if (seppos == 0)
            {outstr.append("\n");}
            else
            {outstr.insert(seppos+1, "\n");}
        }
    }

    return outstr;
}

inline QString files::processFileName(const QString &mode, const QString &vendor)
{
    QString fName;
    QString tmpstr;

    this->readSettings();
    tmpstr = orderDir.append("/").append(vendor).append("-").append(QDate::currentDate().toString("dd-MM-yyyy"));
    fName = tmpstr;
    fName.append(".").append(mode);

    int iNum = 0;
    while(QDir().exists(fName))
    {
        fName = tmpstr;
        fName.append("-").append(QString::number(iNum)).append(".").append(mode);
        iNum++;
    }
    return fName;
}

void files::printOrder(const QList<QStringList> &dataList, const QString &vendor, QWidget *pp)
{
    QPrinter printer;
    QPainter prPainter;

//    printer.setOutputFileName("/home/zen/Desktop/p.pdf");

    QPrintDialog *dlg_print = new QPrintDialog(&printer, pp);
    dlg_print->setWindowTitle(tr("Печать заявки ").append(vendor));
    dlg_print->setModal(true);

    if (dlg_print->exec() != QDialog::Accepted) {return;}

    prPainter.begin(&printer);
    QPen prPen(Qt::black);
    prPen.setWidth(1);
    QFont prFont("Verdana", 10);
    prFont.setFixedPitch(true);
    prPainter.setPen(prPen);
    prPainter.setFont(prFont);

    double xscale = printer.pageRect().width();
    double yscale = printer.pageRect().height();
    double indentx = xscale * 0.03;
    double indenty = yscale * 0.05;
    double widthheader = xscale * 0.3;
    double widthnum = xscale * 0.03;
    double widthn = xscale * 0.68;
    double widthv = xscale * 0.18;
    double widthq = xscale * 0.085;
    double hspacer = xscale * 0.005;
    double rheight = yscale * 0.017;
    double actRheight = rheight;

    QRect rHeader(indentx, indenty, widthheader, rheight);
    QRect rDate(indentx + widthheader + hspacer, indenty, widthheader, rheight);
    indenty += rheight;
    QRect rVendor(indentx, indenty, widthheader, rheight);
    indenty += rheight*2;

    prPainter.drawText(rHeader,Qt::AlignLeft|Qt::AlignTop, "Заявка от \"ИП Прохорова\"");
    prPainter.drawText(rDate,Qt::AlignLeft|Qt::AlignTop, QDate::currentDate().toString("dd-MM-yyyy"));
    prPainter.drawText(rVendor,Qt::AlignLeft|Qt::AlignTop, vendor);

    for (int irow=0; irow<dataList.size(); ++irow)
    {
        if (indenty>=yscale*0.95)
        {
            printer.newPage();
            indenty = yscale * 0.05;
        }

        QStringList tmplist;
        for (int jcol=0; jcol<dataList.at(irow).size(); ++jcol)
        {
            tmplist.append(this->customWordWrap(dataList.at(irow).at(jcol),74));
        }

        for (int iitm = 0; iitm<tmplist.size(); ++iitm)
        {
            if (tmplist.at(iitm).contains("\n"))
            {actRheight = rheight*1.9;}
        }

        QRect rNum(indentx, indenty, widthnum, actRheight);
        QRect rName(indentx + widthnum + hspacer, indenty, widthn, actRheight);
        QRect rVendor(widthn + widthnum + indentx + hspacer, indenty, widthv, actRheight);
        QRect rQuantity(widthn + widthnum + widthv + indentx + hspacer, indenty, widthq, actRheight);

        prPainter.drawText(rNum, Qt::AlignLeft|Qt::AlignTop, QString::number(irow+1));       
        prPainter.drawText(rName,Qt::AlignLeft|Qt::AlignTop, tmplist.at(0));
        prPainter.drawText(rVendor,Qt::AlignLeft|Qt::AlignTop, tmplist.at(1));
        prPainter.drawText(rQuantity,Qt::AlignLeft|Qt::AlignTop, tmplist.at(2));

        prPainter.drawLine(indentx + rNum.width() + hspacer/2,indenty,indentx + rNum.width() + hspacer/2,indenty+rName.height());
        prPainter.drawLine(indentx + rNum.width() + rName.width() + hspacer/2,indenty,
                           indentx + rNum.width() + rName.width() + hspacer/2,indenty+rName.height());
        prPainter.drawLine(indentx + rNum.width() + rName.width() + rVendor.width() + hspacer/2,indenty,
                           indentx + rNum.width() + rName.width() + rVendor.width() + hspacer/2,indenty+rName.height());

        prPainter.drawLine(indentx,indenty+actRheight,widthn + widthv + widthq + indentx + hspacer*3,indenty+actRheight);

        indenty+=actRheight;
        actRheight = rheight;
    }
    prPainter.end();
}
