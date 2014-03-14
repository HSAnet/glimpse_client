#include "consoletools.h"
#include "log/logger.h"

#include <QTextStream>
#include <Windows.h>

LOGGER(ConsoleTools);

class ConsoleTools::Private
{
public:
    Private()
    {
        hConsole = ::GetStdHandle(STD_INPUT_HANDLE);

        if (hConsole == INVALID_HANDLE_VALUE)
        {
            LOG_ERROR("Unable to get console handle");
        }
    }

    HANDLE hConsole;
};

ConsoleTools::ConsoleTools()
: d(new Private)
{
}

ConsoleTools::~ConsoleTools()
{
    enableEcho();
    delete d;
}

bool ConsoleTools::enableEcho()
{
    DWORD value;
    ::GetConsoleMode(d->hConsole, &value);

    value |= ENABLE_ECHO_INPUT;
    ::SetConsoleMode(d->hConsole, value);
    return true;
}

bool ConsoleTools::disableEcho()
{
    DWORD value;
    ::GetConsoleMode(d->hConsole, &value);

    value &= ~ENABLE_ECHO_INPUT;
    ::SetConsoleMode(d->hConsole, value);
    return true;
}

QString ConsoleTools::readLine()
{
    QTextStream stream(stdin);
    return stream.readLine();
}

QString ConsoleTools::readPassword()
{
    disableEcho();
    QTextStream stream(stdin);
    QString pw = stream.readLine();
    enableEcho();

    return pw;
}
