#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <iostream>
#include <fstream>

#include "globaleventfilter.h"
#include "rules/rules.h"
#include "data/dictionary.h"
#include "ui/models/skilltreemodel.h"

static std::ofstream logFile;

/**
 * @brief Logs the message into the log file
 * @param p_type    The type of message.
 * @param p_context The context of the message.
 * @param p_msg     The message itself.
 */
void logMessage(QtMsgType p_type, const QMessageLogContext& p_context, const QString& p_msg)
{
    QString finalMessage;

    // Severity
    switch (p_type)
    {
    case QtDebugMsg:
        finalMessage.append("Debug:-----------------------------------\n");
        break;
    case QtWarningMsg:
        finalMessage.append("Warning:-----------------------------------\n");
        break;
    case QtCriticalMsg:
        finalMessage.append("CRITICAL ERROR:-----------------------------------\n");
        break;
    case QtFatalMsg:
        finalMessage.append("FATAL ERROR:-----------------------------------\n");
        break;
    }

    // Position
    finalMessage.append(QString("%1 : %2 : %3\n").arg(p_context.file, p_context.function).arg(p_context.line));

    // Message
    finalMessage.append(p_msg);
    finalMessage.append("\n");

    // Output and log
    std::cout << finalMessage.toStdString();
    logFile << finalMessage.toStdString();
}

/**
 * @brief Main function, initializes the application and the main window.
 * @return
 */
int main(int argc, char *argv[])
{
    // Init logging
    logFile.open("SR5ShamanLog.log", std::ios::out | std::ios::trunc);
    qInstallMessageHandler(logMessage);

    // Init application
    QApplication a(argc, argv);

    qRegisterMetaType<SkillDefinition>();

    QCoreApplication::setOrganizationName("SR5 Shaman Team");
    QCoreApplication::setOrganizationDomain("http://www.github.com");
    QCoreApplication::setApplicationName("SR5 Shaman");

    // Initialize dictionary
    Dictionary::initialize("../../language/dictionary.json");

    // Initialize rules
    Rules::getSingletonPtr()->initialize();

    // Install global event filter
    GlobalEventFilter eventFilter(NULL);
    a.installEventFilter(&eventFilter);

    // Init & show main window
    MainWindow w;
    w.initialize(&eventFilter);
    w.show();

    int ret = a.exec();
    logFile.close();
    return ret;
}
