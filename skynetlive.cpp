#include <QtWidgets>

#include "skynetlive.h"

SkynetLive::SkynetLive()
{
    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Expanding);

    infoLabel = new QLabel("Test");
    infoLabel->setFrameStyle(QFrame::StyledPanel
                             | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignHCenter);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(5);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

    createActions();
    createMenus();

    statusBar()->showMessage("Welcome. Open a port to"
                             " get started!");

    setWindowTitle("SkynetLive");
    setMinimumSize(160, 160);
    resize(480, 320);
}

void SkynetLive::createActions()
{
    openPortAct = new QAction("Open Port", this);
    openPortAct->setShortcut(QKeySequence::Open);
    connect(openPortAct, &QAction::triggered, this,
            &SkynetLive::openPort);
}

void SkynetLive::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openPortAct);
}

void SkynetLive::openPort()
{
    infoLabel->setText("Opening Port.");
}

SkynetLive::~SkynetLive()
{
}
