#include "dialog.h"
#include "ui_dialog.h"
#include<QMessageBox>
#include <QRegExp>
#include <QDebug>
#include <qicon.h>
#include <QClipboard>
#include <QDesktopServices>
#include <qurl.h>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
//    this->setWindowFlags();
    setWindowFlags(Qt::WindowStaysOnTopHint);
    ui->setupUi(this);

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
        on_btnToTray_clicked();
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
            on_btnToTray_clicked();
            show();
        }
    }


}

Dialog::~Dialog()
{
    delete ui;
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


void Dialog::on_btnToTray_clicked()
{
    QString str;

    QString cname=cboard->text(QClipboard::Clipboard).trimmed();
    if (!validate(cname)) return;
    QString prefix;
    if (ui->radFTP->isChecked())
    prefix=ui->linePrefix->text().trimmed();
    else
    prefix=ui->linePrefixFile->text().trimmed();

    QString RPtype=ui->lineRPType->text().trimmed();
    QString RPprefix=ui->lineRP->text().trimmed();

    if(cname.isEmpty() || prefix.isEmpty() || cname.contains(prefix)) return;

    ui->lineClipboard->setText(cname);

    if (!cname.contains(".zip",Qt::CaseInsensitive))
        cname=cname+".zip";

    /// contribution is from current PREFIX

    if (cname.startsWith(RPtype))
    {
        str=RPprefix+"./"+cname;

    }else if(((prefix.contains("RAN1") || prefix.startsWith("/") )  && cname.startsWith("R1")) )
    {

        str=prefix+"./"+cname;

    }else
    {
        str="";
    }


if(!str.isEmpty())
{
//    str.remove("//");
    ui->lineOutput->setText(str);


    if(ui->chkOpen->isChecked())
    {
        QDesktopServices::openUrl(QUrl(str));
    }else
        cboard->setText(str);
}

}

void Dialog::on_actionConvertFTP_triggered()
{

    on_btnToTray_clicked();


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

void Dialog::on_radFTP_clicked()
{
    ui->linePrefix->setEnabled(true);
    ui->linePrefixFile->setEnabled(false);
    on_btnToTray_clicked();
}

void Dialog::on_radFile_clicked()
{
    ui->linePrefix->setEnabled(false);
    ui->linePrefixFile->setEnabled(true);
    on_btnToTray_clicked();
}
