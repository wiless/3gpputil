#ifndef DIALOG_H
#define DIALOG_H
#include <QClipboard>
#include <QDialog>
#include <qmenu.h>
#include <QSystemTrayIcon>
#include <QCloseEvent>

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

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_btnToTray_clicked();

    void on_actionConvertFTP_triggered();
  void closeEvent(QCloseEvent *event);
   void iconActivated(QSystemTrayIcon::ActivationReason reason);

   void on_actionQuit_triggered();
    void changed(QClipboard::Mode mode);
    void on_toolButton_clicked();

    void on_radFTP_clicked();

    void on_radFile_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
