#include "dialog.h"
#include <QApplication>


int main(int argc, char *argv[])
{

   QApplication a(argc, argv);
   QCoreApplication::setApplicationName("3gpptools");
   QCoreApplication::setOrganizationName("wiless");
   QCoreApplication::setOrganizationDomain("wiless.github.com");
//     QtSingleApplication a(argc, argv);
    Dialog w;
//    w.show();

    return a.exec();
}
