#ifndef CONSOLETOOLS_H
#define CONSOLETOOLS_H

#include <QString>

class ConsoleTools
{
public:
    ConsoleTools();
    ~ConsoleTools();

    bool enableEcho();
    bool disableEcho();

    QString readLine();
    QString readPassword();

protected:
    class Private;
    Private *d;
};

#endif // CONSOLETOOLS_H
