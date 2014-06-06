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

private:
    Q_DISABLE_COPY(ConsoleTools)
};

#endif // CONSOLETOOLS_H
