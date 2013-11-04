#import "crashhandler.h"
#import "client/ios/Breakpad.h"

class CrashHandler::Private
{
public:
    BreakpadRef breakpad;
};

CrashHandler::CrashHandler()
: d(new Private)
{
}

CrashHandler::~CrashHandler()
{
    BreakpadRelease(d->breakpad);
    delete d;
}

void CrashHandler::setReportCrashesToSystem(bool report)
{
    Q_UNUSED(report);
}

bool CrashHandler::reportCrashesToSystem() const
{
    return false;
}

void CrashHandler::init(const QString& reportPath)
{
    d->breakpad = BreakpadCreate(nil);
}
