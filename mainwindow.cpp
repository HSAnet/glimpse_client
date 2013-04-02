#include "mainwindow.h"
#include "types.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startClicked()));

    m_tray.setIcon( QIcon(":/tray.png") );
    m_tray.setContextMenu(ui->menu_File);
    m_tray.setToolTip(tr("mPlane client"));
    m_tray.show();
}

MainWindow::~MainWindow()
{
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
    Q_ASSERT(m_client);
    RemoteInfoList remotes = m_client->remoteInfo();
    Q_ASSERT(!remotes.isEmpty());
    RemoteInfo remote = remotes.first();

    m_negotiator.sendRequest(remote.peerAddress, remote.peerPort);
}

void MainWindow::statusChanged()
{
    ui->statusbar->showMessage( enumToString(Client, "Status", m_client->status()) );
}
