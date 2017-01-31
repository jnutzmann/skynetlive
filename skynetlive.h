#ifndef SKYNETLIVE_H
#define SKYNETLIVE_H

#include <QMainWindow>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;


class SkynetLive : public QMainWindow
{
    Q_OBJECT

public:
    SkynetLive();
    ~SkynetLive();

private slots:
    void openPort();

private:
    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QAction *openPortAct;

    QLabel *infoLabel;
};

#endif // SKYNETLIVE_H
