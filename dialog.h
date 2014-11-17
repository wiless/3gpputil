#ifndef DIALOG_H
#define DIALOG_H
#include <QClipboard>
#include <QDialog>
#include <qmenu.h>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QSettings>
#include <QtNetwork/QFtp>
#include <QDir>
#include <QListWidgetItem>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
public:
    QSystemTrayIcon *trayIcon;
    QMenu *traymenu;
    QClipboard *cboard;
    QFtp *ftp;
    QStringList filelist;
    QSettings setting;
    QString searchfile;
    QString defaultSource;
    QDir dir;
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void LoadSettings();
    void SaveSettings();
private slots:
    void Search();
    bool updateLists();
    void on_actionConvertFTP_triggered();
  void closeEvent(QCloseEvent *event);
   void iconActivated(QSystemTrayIcon::ActivationReason reason);

   void on_actionQuit_triggered();
    void changed(QClipboard::Mode mode);
    void on_toolButton_clicked();
    void connectFTP();
    void on_radFTP_clicked();
    void commandStarted(int);
    void commandFinished(int,bool);
    void listInfo(QUrlInfo);
    void on_radFile_clicked();

    void on_linePrefix_editingFinished();

    void on_pushButton_clicked();

    void on_listFiles_itemDoubleClicked(QListWidgetItem *item);



    void on_toolButton_2_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
