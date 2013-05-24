#ifndef DESKTOPSTATUSBARHELPER_H
#define DESKTOPSTATUSBARHELPER_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>

#include "client.h"

class DesktopStatusBarHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)

public:
    DesktopStatusBarHelper(QObject* parent = 0)
    : QObject(parent)
    {
        m_menu.addAction(tr("Speedtest"), Client::instance(), SLOT(speedTest()));

#ifndef Q_OS_MAC
        m_menu.addSeparator();
        m_menu.addAction(tr("Quit"), qApp, SLOT(quit()));
#endif
        m_icon.setContextMenu(&m_menu);
        m_icon.setIcon( QIcon(":/tray.png") );
        m_icon.show();
    }

    void setVisible(bool visible) {
        if ( m_visible != visible ) {
            m_visible = visible;
            emit visibleChanged(visible);
        }
    }

    bool isVisible() const {
        return m_visible;
    }

signals:
    void visibleChanged(bool visible);

protected:
    bool m_visible;
    QSystemTrayIcon m_icon;
    QMenu m_menu;
};

#endif // DESKTOPSTATUSBARHELPER_H
