#include "dlg_result.h"
#include "ui_dlg_result.h"

dlg_result::dlg_result(QWidget *parent) : QDialog(parent), ui(new Ui::dlg_result)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->resize(parent->width(),parent->height());
    this->move(parent->width()/2,parent->height()/2);
    this->setFixedWidth(parent->width());

    resModel = 0;
}

dlg_result::~dlg_result()
{
    delete ui;
}

void dlg_result::procResVector(const QVector<QStringList> &allVector, const QStringList &searchCrit, const QList<QStringList> &discList)
{
    allResList.clear();
    QString nameStr;
    QStringList subNameList;

    double infoPrice = 0;
    int discInfoPrice = 0;
    bool resHit = false;

    for (int irow = 0; irow < allVector.size(); ++irow)
    {
        resHit=false;
        nameStr = allVector.at(irow).at(0).trimmed();
        if (searchCrit.at(0).isEmpty() && searchCrit.at(1).isEmpty() && searchCrit.at(2).isEmpty())
        {resHit=true;}
        else
        {
            if (nameStr.contains(searchCrit.at(0),Qt::CaseInsensitive) && !searchCrit.at(0).isEmpty())
            {
                subNameList = nameStr.split(" ");
                if (searchCrit.at(1).isEmpty() && searchCrit.at(2).isEmpty())
                {resHit=true;}
                for (int iItm = 0; iItm < subNameList.size(); ++iItm)
                {
                    if (subNameList.at(iItm).isEmpty()) {continue;}
                    if (subNameList.at(iItm).at(0).isDigit()
                            && subNameList.at(iItm).contains(searchCrit.at(1),Qt::CaseInsensitive)
                            && !searchCrit.at(1).trimmed().isEmpty())
                    {if(searchCrit.at(2).isEmpty()) {resHit=true;}}
                    if (QString::compare(subNameList.at(iItm).at(0), "№") == 0 && !searchCrit.at(2).isEmpty())
                    {
                        if(searchCrit.at(1).isEmpty()) {resHit=true;}
                        if (subNameList.at(iItm).size() == 1)
                        {
                            subNameList = nameStr.split("№ ");
                            if(QString::compare(subNameList.at(1).split(" ").at(0),searchCrit.at(2)) == 0
                                    || QString::compare(subNameList.at(1).split(", ").at(0),searchCrit.at(2)) == 0)
                            {if (resHit==true) {resHit = true;}}
                        }
                        else if (QString::compare(subNameList.at(iItm).section("№",1,1),searchCrit.at(2)) == 0)
                        {if (resHit==true) {resHit = true;}}
                    }
                }
            }
        }

        if (resHit==true)
        {
            infoPrice = allVector.at(irow).at(1).toDouble();
            for (int infoRow = 0; infoRow < discList.size(); ++infoRow)
            {
                if (QString::compare(discList.at(infoRow).at(0),allVector.at(irow).at(4)) == 0)
                {
                    infoPrice = infoPrice - (infoPrice/100*discList.at(infoRow).at(1).toDouble());
                    discInfoPrice = infoPrice+0.5;
                }
            }
            if (allVector.at(irow).at(0).isEmpty() || allVector.at(irow).at(1).toDouble() == 0.00) {continue;}
            allResList.append(QStringList() << allVector.at(irow).at(0) << allVector.at(irow).at(1)
                                 << QString::number(discInfoPrice) << allVector.at(irow).at(2) << allVector.at(irow).at(3)
                                 << allVector.at(irow).at(4));
        }
    }

    this->createModel(allResList);
}

void dlg_result::on_le_search1_textChanged(const QString &arg1)
{
    s1List.clear();
    for (int irow = 0; irow < allResList.size(); ++irow)
    {
        if (allResList.at(irow).at(0).contains(arg1.trimmed(),Qt::CaseInsensitive))
        {   
            s1List.append(allResList.at(irow));
        }
    }
    this->createModel(s1List+s2List);
}

void dlg_result::on_le_search2_textChanged(const QString &arg1)
{
    s2List.clear();
    for (int irow = 0; irow < allResList.size(); ++irow)
    {
        if (allResList.at(irow).at(0).contains(arg1.trimmed(),Qt::CaseInsensitive))
        {
            s2List.append(allResList.at(irow));
        }
    }
    this->createModel(s2List+s1List);
}

void dlg_result::setModelColumnsWidth(const int &effWidth)
{
    ui->tvw_Result->setColumnWidth(0,effWidth * 0.36);
    ui->tvw_Result->setColumnWidth(1,effWidth * 0.08);
    ui->tvw_Result->setColumnWidth(2,effWidth * 0.08);
    ui->tvw_Result->setColumnWidth(3,effWidth * 0.1);
    ui->tvw_Result->setColumnWidth(4,effWidth * 0.25);
    ui->tvw_Result->setColumnWidth(5,effWidth * 0.08);
}

void dlg_result::on_tvw_Result_doubleClicked(const QModelIndex &index)
{
    QStringList dataList;

    for (int jcol=0; jcol<resModel->columnCount(); ++jcol)
    {
        dataList.append(resModel->item(index.row(),jcol)->text().simplified());
    }
    QPointer<dlg_addToOrder> aOrd = new dlg_addToOrder(this);
    connect(aOrd, SIGNAL(addOrderData(QStringList)), this, SLOT(orderLogger(QStringList)));
    aOrd->setModal(true);
    aOrd->show();
    aOrd->setData(dataList);
}

void dlg_result::createModel(const QList<QStringList> &modList)
{
    if (resModel!=0) {delete resModel; resModel=0;}
    resModel = new QStandardItemModel(this);
    double dprice;
    int iprice;

    for (int irow = 0; irow < modList.size(); ++irow)
    {
        QStandardItem *name = new QStandardItem(modList.at(irow).at(0));
        QStandardItem *price = new QStandardItem();
        dprice = modList.at(irow).at(1).toDouble();
        price->setData(dprice,Qt::DisplayRole);
        QStandardItem *discPrice = new QStandardItem();
        iprice = modList.at(irow).at(2).toInt();
        discPrice->setData(iprice,Qt::DisplayRole);
        QStandardItem *expDate = new QStandardItem(modList.at(irow).at(3));
        QStandardItem *manufacturer = new QStandardItem(modList.at(irow).at(4));
        QStandardItem *vendor = new QStandardItem(modList.at(irow).at(5));

        QList<QStandardItem *> modelrow;
        modelrow.append(name);
        modelrow.append(price);
        modelrow.append(discPrice);
        modelrow.append(expDate);
        modelrow.append(manufacturer);
        modelrow.append(vendor);

        resModel->appendRow(modelrow);
    }

    resModel->setHorizontalHeaderLabels(QStringList() << "Наименование" << "Цена" << "Цена со скидкой"
                                        << "Срок годности" << "Производитель"<< "Поставщик");

    resModel->sort(2);
    ui->tvw_Result->setModel(resModel);
    this->setModelColumnsWidth(this->width() * 0.964);
}

void dlg_result::orderLogger(const QStringList &orderdatalist)
{
    QString outstr;
    outstr.append(QTime::currentTime().toString());
    for (int iitm = 0; iitm < orderdatalist.size(); ++iitm)
    {
        outstr.append(" ").append(orderdatalist.at(iitm));
    }

    ui->lw_logger->addItem(outstr);
}
