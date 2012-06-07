#include "mainwindow.h"

#include <QtGui/QApplication>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QCoreApplication::setOrganizationName("Wadobo");
    QCoreApplication::setOrganizationDomain("wadobo.com");
    QCoreApplication::setApplicationName("text2screen");

    MainWindow mainWindow;
    mainWindow.show();

    return app->exec();
}
