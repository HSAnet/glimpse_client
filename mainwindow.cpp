#include "mainwindow.h"
#include "types.h"
#include "connectiontester.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startClicked()));
    connect(ui->actionConnection_does_not_work, SIGNAL(triggered()), this, SLOT(noConnectionClicked()));
    connect(ui->actionConnection_is_slow, SIGNAL(triggered()), this, SLOT(connectionSlowClicked()));

#ifndef Q_OS_MAC // Mac applications don't use icons
    setWindowIcon(QIcon(":/tray.png"));
#endif // Q_OS_MAC

    // No reason to continue when there is no system tray supported
    if (!QSystemTrayIcon::isSystemTrayAvailable())
        return;

    QIcon trayIcon;
#ifdef Q_OS_MAC
    trayIcon.addFile(":/tray_mac.png");
    trayIcon.addFile(":/tray_mac_active.png", QSize(), QIcon::Selected);
#else // Q_OS_MAC
    trayIcon.addFile(":/tray.png");
#endif // Q_OS_MAC
    m_tray.setIcon(trayIcon);
    m_tray.setContextMenu(ui->menu_File);
    m_tray.setToolTip(tr("mPlane client"));
    m_tray.show();
}

MainWindow::~MainWindow()
{
    m_tray.hide();
    delete ui;
}

void MainWindow::setClient(Client *client)
{
    if (client == m_client)
        return;

    if (m_client) {
        disconnect(m_client.data(), SIGNAL(statusChanged()), this, SLOT(statusChanged()));
    }

    m_client = client;

    if (m_client) {
        connect(m_client.data(), SIGNAL(statusChanged()), this, SLOT(statusChanged()));

        // Update methods
        statusChanged();

        // Update variables
        m_negotiator.setManagerSocket(m_client->managerSocket());
    }
}

Client *MainWindow::client() const
{
    return m_client;
}

void MainWindow::startClicked()
{
    // Don't continue when client is no registered
    if (m_client->status() != Client::Registered) {
        return;
    }

    Q_ASSERT(m_client);
    RemoteInfoList remotes = m_client->remoteInfo();
    Q_ASSERT(!remotes.isEmpty());
    RemoteInfo remote = remotes.first();

    m_negotiator.sendRequest(remote.peerAddress, remote.peerPort);
}

void MainWindow::noConnectionClicked()
{
    ConnectionTester tester;
    connect(&tester, SIGNAL(message(QString,MessageType)), ui->textEdit, SLOT(append(QString)));
    tester.start();
}

void MainWindow::connectionSlowClicked()
{
}

void MainWindow::statusChanged()
{
    ui->startButton->setEnabled( m_client->status() == Client::Registered );
    ui->statusbar->showMessage( enumToString(Client, "Status", m_client->status()) );
}
