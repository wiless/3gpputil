#include "dialog.h"
#include "ui_dialog.h"
#include<QMessageBox>
#include <QRegExp>
#include <QDebug>
#include <qicon.h>
#include <QClipboard>
#include <QDesktopServices>
#include <qurl.h>
#include <qdir.h>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    //    this->setWindowFlags();
    setWindowFlags(Qt::WindowStaysOnTopHint);
    ui->setupUi(this);


    LoadSettings();

    cboard =  QApplication::clipboard();
    connect(cboard,SIGNAL(changed(QClipboard::Mode)),this,SLOT(changed(QClipboard::Mode)));
    /// tray icon
    trayIcon=new QSystemTrayIcon(this);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->setIcon(QIcon("3gpp.png"));
    trayIcon->show();

    /// menu
    traymenu=new QMenu();
    traymenu->addAction(ui->actionConvertFTP);
    traymenu->addAction(ui->actionQuit);

    trayIcon->setContextMenu(traymenu);

    hide();
    ftp = new QFtp(this);

}

void Dialog::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}


void Dialog::changed(QClipboard::Mode mode)
{
    if (isVisible() && mode==QClipboard::Clipboard)
    {
        Search();
        //        ui->lineClipboard->setText(cboard->text(mode));

    }

}
void Dialog::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    //    ui->lineClipboard->setText(cboard->text());
    if(reason==QSystemTrayIcon::Trigger)
    {
        if (this->isVisible())
            hide();
        else
        {
            //            ui->lineClipboard->setText(cboard->text(QClipboard::Clipboard));
            Search();
            show();
        }
    }


}

Dialog::~Dialog()
{
    delete ui;
}
void Dialog::SaveSettings()
{

    QString linerp=ui->lineRP->text();
    QString localftp=ui->linePrefix->text();
    //    QString local=ui->linePrefixFile->text();
    bool isauto= ui->chkOpen->isChecked();


    QStringList pathlists; //=setting.value("SearchPaths","/home/ssk/3gppdocs/LAA/docs/").toStringList();

    for (int var = 0; var < ui->cmbPathLists->count(); ++var) {
        pathlists.append(ui->cmbPathLists->itemText(var));
    }
    setting.setValue("SearchPaths",pathlists);



    setting.setValue("Onlinelocation",linerp);
    setting.setValue("LocalFtp",localftp);
    //    setting.setValue("Local",local);
    setting.setValue("AutoOpen",isauto);
    setting.setValue("DefaultSource",defaultSource);



}
void Dialog::LoadSettings()
{
    QString linerp=setting.value("Onlinelocation","ftp://ftp.3gpp.org/tsg_ran/TSG_RAN/TSGR_65/Docs/").toString();
    QString localftp=setting.value("LocalFtp","ftp://10.10.10.10/RAN/RAN1/Docs/").toString();
    //    QString local=setting.value("Local","/home/ssk/3gppdocs/docs/FDMIMO").toString();
    bool isauto = setting.value("AutoOpen",false).toBool();
    defaultSource=setting.value("DefaultSource","FTP").toString();

    ui->lineRP->setText(linerp);
    ui->linePrefix->setText(localftp);
    //    ui->linePrefixFile->setText(local);
    ui->chkOpen->setChecked(isauto);


    ui->radFTP->setChecked((defaultSource=="FTP"));
    ui->radFile->setChecked((defaultSource=="FILE"));

    QStringList  pathlists=setting.value("SearchPaths","/home/ssk/3gppdocs/LAA/docs/").toStringList();
    ui->cmbPathLists->addItems(pathlists);


}

bool validate(QString str)
{
    if(str.isEmpty()) return false;

    if(str.endsWith(".zip",Qt::CaseInsensitive))
        str=str.left(9);

    if(str.length()!=9)  return false;

    QRegExp exp;
    exp.setPattern("[Rr][pP1234]-[0-9]{6}");
    return exp.exactMatch(str);
    //    return true;

    //            return false;
}


void Dialog::Search()
{
    QString str;
    QString RPtype=ui->lineRPType->text().trimmed();
    QString RPprefix=ui->lineRP->text().trimmed();
    if (!RPprefix.endsWith('/'))
        RPprefix.append('/');

    QString cname=cboard->text(QClipboard::Clipboard).trimmed();
    ui->textBrowser->append("\n Searching Clipboard : "+cname);
    QString contribNumber;
    if (!validate(cname) || cname.isEmpty()) return;

    contribNumber=cname;
    if (!cname.contains(".zip",Qt::CaseInsensitive))
    {
        contribNumber=cname;
        cname=cname+".zip";
    }


    QString prefix;
    if (ui->radFTP->isChecked())
    {

        prefix=ui->linePrefix->text().trimmed();
        if (!prefix.endsWith('/'))
            prefix.append('/');

        if (prefix.isEmpty() || cname.contains(prefix)) return;

        if (cname.startsWith(RPtype))
        {
            str=RPprefix+cname;

        }else if(((prefix.contains("RAN1") || prefix.startsWith("/") )  && cname.startsWith("R1")) )
        {

            str=prefix+cname;

        }else
        {
            str="";
        }
        ui->lineOutput->setText(str);

        searchfile=cname;
        ftp->list();
        return;

    }
    else
    {
        if(ui->cmbPathLists->count()==0) return;
        /// List all possible files in the box
        for (int var = 0; var < ui->cmbPathLists->count(); ++var) {
            //    dir.setPath(ui->linePrefixFile->text());
            dir.setPath(ui->cmbPathLists->itemText(var));

            QStringList filters;
            filters << "*"+contribNumber+"*" ;
            dir.setNameFilters(filters);
            dir.setSorting(QDir::Type);

            if(updateLists())
                break;
        }
    }

    ui->lineClipboard->setText(contribNumber);

}

bool Dialog::updateLists()
{

    ui->listFiles->clear();

    QStringList result=dir.entryList();
    QFileInfoList infolist=  dir.entryInfoList();
    //    ui->textBrowser->append("Found these matching contents");
    for(int k=0;k<infolist.length();k++)
    {
        ui->listFiles->addItem(infolist[k].fileName());

    }



    QString zipfile;

    for(int k=0;k<infolist.length();k++)
    {

        if (infolist[k].completeSuffix()!="zip") {
            if(ui->chkOpen->isChecked())
            {
                QDesktopServices::openUrl(QUrl(infolist[k].filePath()));
            }
//            else
//                cboard->setText(infolist[k].filePath());
    break;
        }else
            zipfile=infolist[k].filePath();
    }
    if(ui->chkOpen->isChecked())
    {
        QDesktopServices::openUrl(QUrl(zipfile));
    }
//    else
//        cboard->setText(zipfile);

    if (infolist.length()>0)
        return true;
    else
        return false;

}

void Dialog::on_actionConvertFTP_triggered()
{

    Search();

    //    QMessageBox::information(this,"Convert to ftp","test");
}

void Dialog::on_actionQuit_triggered()
{
    accept();
}

void Dialog::on_toolButton_clicked()
{
    if(!ui->lineOutput->text().isEmpty())
        QDesktopServices::openUrl(QUrl(ui->lineOutput->text()));
}



void Dialog::connectFTP()
{
    //    if (ftp==NULL)
    {
        ftp->abort();

        QString addr=ui->linePrefix->text();
        QUrl url(addr);
        //            if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp")) {
        ftp->connectToHost(url.host(), 21);
        ftp->login();
        //            } else {
        //                ftp->connectToHost(url.host(), url.port(21));

        //                if (!url.userName().isEmpty())
        //                    ftp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
        //                else
        ftp->login();
        if (!url.path().isEmpty())
            ftp->cd(url.path());
        //            }

        connect(ftp,SIGNAL(commandStarted(int)),this,SLOT(commandStarted(int)));
        connect(ftp,SIGNAL(commandFinished(int,bool)),this,SLOT(commandFinished(int,bool)));
        connect(ftp,SIGNAL(listInfo(QUrlInfo)),this,SLOT(listInfo(QUrlInfo)));
    }
}

void Dialog::on_radFTP_clicked()
{

    defaultSource="FTP";
    ui->linePrefix->setEnabled(true);
    //    ui->linePrefixFile->setEnabled(false);
    ui->cmbPathLists->setEnabled(false);
    connectFTP();

    Search();

}

void Dialog::commandStarted(int cmd)
{
    ui->lblStatus->setText("Started "+QString::number(cmd));
    ui->textBrowser->clear();
    ui->textBrowser->append("Starting "+QString::number(cmd));
    ui->textBrowser->setHtml("Current command "+QString::number(ftp->currentCommand()));
    if(ftp->currentCommand()==QFtp::List)
        filelist.clear();
}

void Dialog::commandFinished(int cmd, bool err)
{
    if (ftp->currentCommand()==QFtp::List)
    {
        ui->textBrowser->append("FTP:: List");

        ui->lblStatus->setText("Finished " + QString::number(cmd));
        if (!searchfile.isEmpty())
        {
            //        ui->textBrowser->append("Looking in "+filelist.join("\n"));

            if (filelist.contains(searchfile,Qt::CaseInsensitive))
                ui->textBrowser->append("Found file ");
            else
                ui->textBrowser->append("Not Found");
            filelist.clear();

        } }else
    {
        ui->textBrowser->append("Running some other command");
    }
}

void Dialog::listInfo(QUrlInfo uinfo)
{
    if(uinfo.isFile())
        filelist.append(uinfo.name());
    // ui->textBrowser->append(uinfo.name());
}

void Dialog::on_radFile_clicked()
{
    defaultSource="FILE";
    ui->linePrefix->setEnabled(false);
    //    ui->linePrefixFile->setEnabled(true);
    ui->cmbPathLists->setEnabled(true);
    Search();
}

void Dialog::on_linePrefix_editingFinished()
{

    QString addr=ui->linePrefix->text();
    QUrl url(addr);
    ftp->cd(url.path());
}

void Dialog::on_pushButton_clicked()
{
    SaveSettings();

}

void Dialog::on_listFiles_itemDoubleClicked(QListWidgetItem *item)
{
    //    ui->textBrowser->append("Selected Path : "+dir.filePath(item->text()));
    QDesktopServices::openUrl(QUrl(dir.filePath(item->text())));

}

void Dialog::on_toolButton_2_clicked()
{
    ui->cmbPathLists->removeItem(ui->cmbPathLists->currentIndex());

}
