#include "crashhandler.h"
#include "log/logger.h"

#include <QDir>
#include <QProcess>
#include <QCoreApplication>
#include <QString>

#if defined(Q_OS_OSX)
#include "client/mac/handler/exception_handler.h"
#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN32)
#include "client/windows/handler/exception_handler.h"
#endif

#define P_LOG_INFO(message) \
    printf("INFO: %s : %s\n", Q_FUNC_INFO, message);

#define P_LOG_ERROR(message) \
    printf("ERROR: %s : %s\n", Q_FUNC_INFO, message);

LOGGER(CrashHandler);

class CrashHandler::Private
{
public:
    Private()
    {
    }

    ~Private()
    {
        delete exceptionHandler;
    }

    void init(const QString& dumpPath);

    static google_breakpad::ExceptionHandler* exceptionHandler;
    static bool reportCrashesToSystem;

#if defined(Q_OS_WIN32)
    static bool DumpCallback(const wchar_t* _dump_dir,const wchar_t* _minidump_id,void* context,EXCEPTION_POINTERS* exinfo,MDRawAssertionInfo* assertion,bool success);
#elif defined(Q_OS_LINUX)
    static bool DumpCallback(const google_breakpad::MinidumpDescriptor &md,void *context, bool success);
#elif defined(Q_OS_OSX)
    static bool DumpCallback(const char* _dump_dir,const char* _minidump_id,void *context, bool success);
#endif
};

google_breakpad::ExceptionHandler* CrashHandler::Private::exceptionHandler = NULL;
bool CrashHandler::Private::reportCrashesToSystem = false;

#if defined(Q_OS_WIN32)
bool CrashHandler::Private::DumpCallback(const wchar_t* _dump_dir,const wchar_t* _minidump_id,void* context,EXCEPTION_POINTERS* exinfo,MDRawAssertionInfo* assertion,bool success)
#elif defined(Q_OS_LINUX)
bool CrashHandler::Private::DumpCallback(const google_breakpad::MinidumpDescriptor &md,void *context, bool success)
#elif defined(Q_OS_OSX)
bool CrashHandler::Private::DumpCallback(const char* _dump_dir,const char* _minidump_id,void *context, bool success)
#endif
{
    Q_UNUSED(context);
#if defined(Q_OS_WIN32)
    Q_UNUSED(_dump_dir);
    Q_UNUSED(_minidump_id);
    Q_UNUSED(assertion);
    Q_UNUSED(exinfo);
#elif defined(Q_OS_LINUX)
    Q_UNUSED(md);
#endif

    P_LOG_INFO("Crash detected.");

    /*
    NO STACK USE, NO HEAP USE THERE !!!
    Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
    */
    return CrashHandler::Private::reportCrashesToSystem ? success : true;
}

void CrashHandler::Private::init(const QString& dumpPath)
{
    if ( exceptionHandler != NULL )
    {
        return;
    }

#if defined(Q_OS_WIN32)
    std::wstring pathAsStr = (const wchar_t*)dumpPath.utf16();
    exceptionHandler = new google_breakpad::ExceptionHandler(
        pathAsStr,
        /*FilterCallback*/ 0,
        CrashHandler::Private::DumpCallback,
        /*context*/
        0,
        true
    );
#elif defined(Q_OS_LINUX)
    std::string pathAsStr = dumpPath.toStdString();
    google_breakpad::MinidumpDescriptor md(pathAsStr);
    exceptionHandler = new google_breakpad::ExceptionHandler(
        md,
        /*FilterCallback*/ 0,
        CrashHandler::Private::DumpCallback,
        /*context*/ 0,
        true,
        -1
    );
#elif defined(Q_OS_OSX)
    std::string pathAsStr = dumpPath.toStdString();
    exceptionHandler = new google_breakpad::ExceptionHandler(
        pathAsStr,
        /*FilterCallback*/ 0,
        CrashHandler::Private::DumpCallback,
        /*context*/
        0,
        true,
        NULL
    );
#endif

    LOG_INFO("Crash handler initialized.");
}

CrashHandler::CrashHandler()
: d(new Private)
{
}

CrashHandler::~CrashHandler()
{
    delete d;
}

void CrashHandler::setReportCrashesToSystem(bool report)
{
    d->reportCrashesToSystem = report;
}

bool CrashHandler::reportCrashesToSystem() const
{
    return d->reportCrashesToSystem;
}

void CrashHandler::init(const QString& reportPath)
{
    d->init(reportPath);
}
