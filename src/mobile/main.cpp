#ifdef HAVE_WNCK
#include <gdk/gdk.h>
#endif // HAVE_WNCK

#include "client.h"
#include "storage/storagepaths.h"
#include "log/logger.h"
#include "log/logmodel.h"
#include "log/filelogger.h"
#include "qmlmodule.h"
#include "settings.h"

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

    if (dir.exists())
    {
        QStringList nameFilters;
        nameFilters.append("*.ttf");

        int ok = 0;

        foreach(const QString& name, dir.entryList(nameFilters, QDir::NoDotAndDotDot|QDir::Files))
        {
            int id = QFontDatabase::addApplicationFont(dir.absoluteFilePath(name));
            if (id == -1)
            {
                LOG_ERROR(QString("Failed to load font: %1").arg(dir.absoluteFilePath(name)));
            }
            else
            {
                ++ok;
            }
        }

        if (ok)
        {
            LOG_INFO(QString("Loaded %1 fonts").arg(ok));
        }
        else
        {
            LOG_INFO("All fonts failed to load.");
        }
    }
    else
    {
        LOG_ERROR(QString("Font directory does not exist: %1").arg(path));
    }
}

class SuspendHelper : public QObject
{
    Q_OBJECT

public slots:
    void applicationStateChanged(Qt::ApplicationState state)
    {
#ifdef Q_OS_ANDROID
        if (state != Qt::ApplicationSuspended)
        {
            return;
        }

        Settings* settings = Client::instance()->settings();
        settings->sync();

        LOG_DEBUG("Application suspended, settings saved.");
#else // Q_OS_ANDROID
        Q_UNUSED(state);
#endif
    }
};

int main(int argc, char* argv[])
{
#ifdef HAVE_WNCK
    gdk_init (&argc, &argv);
#endif // HAVE_WNCK

    QCoreApplication::setOrganizationDomain("de.hsaugsburg.informatik");
    QCoreApplication::setOrganizationName("HS-Augsburg");
    QCoreApplication::setApplicationName("mPlaneClient");
    QCoreApplication::setApplicationVersion(Client::version());

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    QGuiApplication app(argc, argv);
    SuspendHelper suspend;
    QObject::connect(&app, SIGNAL(applicationStateChanged(Qt::ApplicationState)), &suspend, SLOT(applicationStateChanged(Qt::ApplicationState)));
#else
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);
#endif

    LogModel loggerModel;
    Logger::addAppender(new FileLogger);

    LOG_INFO(QString("Glimpse version %1").arg(Client::version()));

#ifdef HAVE_BREAKPAD
    QDir crashdumpDir = StoragePaths().crashDumpDirectory();
    if (!crashdumpDir.exists())
    {
        if (!QDir::root().mkpath(crashdumpDir.absolutePath()))
        {
            LOG_ERROR(QString("Failed to create crashdump directory: %1").arg(crashdumpDir.absolutePath()));
        }
        else
        {
            LOG_INFO("Crashdump directory created");
        }
    }

    CrashHandler crashHandler;
    crashHandler.init(crashdumpDir.absolutePath());
#endif // HAVE_BREAKPAD

    QmlModule::registerTypes();

    QtQuick2ApplicationViewer* view = new QtQuick2ApplicationViewer;
    QObject::connect(view, SIGNAL(closing(QQuickCloseEvent*)), &app, SLOT(quit()));

    QQmlEngine* engine = view->engine();
    QmlModule::initializeEngine(engine);

    // Allow QFileSelector to be automatically applied on qml scripting
    QQmlFileSelector* selector = new QQmlFileSelector(engine);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    QFileSelector desktopSelector;
    desktopSelector.setExtraSelectors( QStringList() << "desktop" );
    selector->setSelector(&desktopSelector);
#endif

    Client* client = Client::instance();

    QQmlContext* rootContext = view->rootContext();
    rootContext->setContextProperty("client", client);
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

    LOG_INFO("Syncing settings before shutdown");
    client->settings()->sync();

    LOG_INFO("Cleaning up before shutdown");

    // Cleanly shutdown
    delete selector;
    delete view;
    Client::instance()->deleteLater();
    QTimer::singleShot(1, &app, SLOT(quit()));
    app.exec();

    LOG_INFO("Shutdown complete");

    return returnCode;
}

#include "main.moc"
