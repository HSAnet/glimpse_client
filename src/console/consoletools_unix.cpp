#include "consoletools.h"

#include <QTextStream>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

class ConsoleTools::Private
{
public:
    termios oflags;
};

ConsoleTools::ConsoleTools()
: d(new Private)
{
}

ConsoleTools::~ConsoleTools()
{
    delete d;
}

bool ConsoleTools::enableEcho()
{
    if (tcsetattr(fileno(stdin), TCSANOW, &d->oflags) != 0)
    {
        //perror("tcsetattr");
        return false;
    }

    return true;
}

bool ConsoleTools::disableEcho()
{
    tcgetattr(fileno(stdin), &d->oflags);

    termios nflags = d->oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        //perror("tcsetattr");
        return false;
    }

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
