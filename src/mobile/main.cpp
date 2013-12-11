#ifdef HAVE_WNCK
#include <gdk/gdk.h>
#endif // HAVE_WNCK

#include "client.h"
#include "storage/storagepaths.h"
#include "log/logger.h"
#include "log/logmodel.h"
#include "qmlmodule.h"

#include "qtquick2applicationviewer.h"

#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlFileSelector>
#include <QFileSelector>
#include <QTimer>
#include <QDir>
#include <QFontDatabase>

#ifdef Q_OS_ANDROID
#include "statusbarhelper.h"
#elif !defined(Q_OS_IOS)
#include <QApplication>
#include "desktopstatusbarhelper.h"
#endif // Q_OS_ANDROID

#define HAVE_BREAKPAD // We always have breakpad at the moment
#ifdef HAVE_BREAKPAD
#include "crashhandler.h"
#endif

#ifdef Q_OS_IOS
#include <QtPlugin>
Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQuickControlsPlugin)
#endif

LOGGER(main)

void loadFonts(const QString& path)
{
    QDir dir(path);

    if (dir.exists()) {
        QStringList nameFilters;
        nameFilters.append("*.ttf");

        int ok = 0;

        foreach(const QString& name, dir.entryList(nameFilters, QDir::NoDotAndDotDot|QDir::Files)) {
            int id = QFontDatabase::addApplicationFont(dir.absoluteFilePath(name));
            if (id == -1) {
                LOG_ERROR(QString("Failed to load font: %1").arg(dir.absoluteFilePath(name)));
            } else {
                ++ok;
            }
        }

        if (ok) {
            LOG_INFO(QString("Loaded %1 fonts").arg(ok));
        } else {
            LOG_INFO("All fonts failed to load.");
        }
    } else {
        LOG_ERROR(QString("Font directory does not exist: %1").arg(path));
    }
}

int main(int argc, char* argv[])
{
#ifdef HAVE_WNCK
    gdk_init (&argc, &argv);
#endif // HAVE_WNCK

    QCoreApplication::setOrganizationDomain("de.hsaugsburg.informatik");
    QCoreApplication::setOrganizationName("HS-Augsburg");
    QCoreApplication::setApplicationName("mPlaneClient");

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    QGuiApplication app(argc, argv);
#else
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);
#endif

    LogModel loggerModel;

#ifdef HAVE_BREAKPAD
    QDir crashdumpDir = StoragePaths().crashDumpDirectory();
    if (!crashdumpDir.exists()) {
        if (!QDir::root().mkpath(crashdumpDir.absolutePath())) {
            LOG_ERROR(QString("Failed to create crashdump directory: %1").arg(crashdumpDir.absolutePath()));
        } else {
            LOG_INFO("Crashdump directory created");
        }
    }

    CrashHandler crashHandler;
    crashHandler.init(crashdumpDir.absolutePath());
#endif // HAVE_BREAKPAD

    QmlModule::registerTypes();

    QtQuick2ApplicationViewer* view = new QtQuick2ApplicationViewer;

    QQmlEngine* engine = view->engine();
    QmlModule::initializeEngine(engine);

    // Allow QFileSelector to be automatically applied on qml scripting
    QQmlFileSelector selector(engine);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    QFileSelector desktopSelector;
    desktopSelector.setExtraSelectors( QStringList() << "desktop" );
    selector.setSelector(&desktopSelector);
#endif

    QQmlContext* rootContext = view->rootContext();
    rootContext->setContextProperty("client", Client::instance());
    rootContext->setContextProperty("logModel", &loggerModel);
#ifdef Q_OS_ANDROID
    rootContext->setContextProperty("statusBar", new StatusBarHelper(view));
#elif defined(Q_OS_IOS)
    // TODO: iOS Code
#else
    rootContext->setContextProperty("statusBar", new DesktopStatusBarHelper(view));
#endif // Q_OS_ANDROID

#ifdef Q_OS_IOS
    view.addImportPath(QStringLiteral("imports/qml"));
#endif

    loadFonts(view->adjustPath(QStringLiteral("qml/fonts")));

    view->setMainQmlFile(QStringLiteral("qml/main.qml"));
    view->showExpanded();

    int returnCode = app.exec();

    // Cleanly shutdown
    delete view;
    Client::instance()->deleteLater();
    QTimer::singleShot(1, &app, SLOT(quit()));
    app.exec();

    return returnCode;
}
