#include "MainFrame.h"
#include "ui_MainFrame.h"

MainFrame::MainFrame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainFrame)
{
    ui->setupUi(this);

    m_gamePanel = new GamePanel(this);

    setFixedSize(1600,900);
    setWindowTitle(tr("Lord Card Game"));
}

MainFrame::~MainFrame()
{
    delete ui;
}

void MainFrame::resizeEvent(QResizeEvent* event)
{
    m_gamePanel->setGeometry(rect());

    QDialog::resizeEvent(event);
}
