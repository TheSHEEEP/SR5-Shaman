#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

/**
 * @brief Main function, initializes the application and the main window.
 * @return
 */
int main(int argc, char *argv[])
{
    // Init application
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("SR5 Shaman Team");
    QCoreApplication::setOrganizationDomain("http://www.github.com");
    QCoreApplication::setApplicationName("SR5 Shaman");

    // Init & show main window
    MainWindow w;
    w.initialize();
    w.show();

    return a.exec();
}
