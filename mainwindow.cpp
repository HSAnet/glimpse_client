#include "mainwindow.h"
#include "types.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    }
}

Client *MainWindow::client() const
{
    return m_client;
}

void MainWindow::statusChanged()
{
    ui->statusbar->showMessage( enumToString(Client, "Status", m_client->status()) );
}
