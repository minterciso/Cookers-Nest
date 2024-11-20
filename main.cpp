#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSettings>
#include <QMainWindow>
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Cookers-Nest_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    // Connect the signal "about to close", and close the database
    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        DatabaseManager::instance().closeDatabase();
    });
    // Set application information for ease of usage on the Settings
    QCoreApplication::setOrganizationName("JITProgramer");
    QCoreApplication::setOrganizationDomain("jitprogramer.com");
    QCoreApplication::setApplicationName("Cookers Nest");
    // Check if we have a product opened (this means that there was some issue when exiting the application), and clear it
    QSettings settings;
    settings.beginGroup("product");
    if(settings.value("open").toBool()==true){
        settings.setValue("open", false);
        settings.setValue("id", -1);
    }
    settings.endGroup();
    // Open the database
    qDebug() << "[Main] Trying to open the default database";
    DatabaseManager::instance().openDatabase();
    // Open the main window
    MainWindow w;
    w.show();

    return a.exec();
}
