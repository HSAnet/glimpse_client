#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include "export.h"

#include <QString>

class CLIENT_API CrashHandler
{
public:
    CrashHandler();
    ~CrashHandler();

    void init(const QString& reportPath);

    void setReportCrashesToSystem(bool report);
    bool reportCrashesToSystem() const;

private:
    class Private;
    Private* d;
};

#endif // CRASHHANDLER_H
