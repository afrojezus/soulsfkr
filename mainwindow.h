#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "key_listener.h"
#include "binding_dialog.h"
#include <iostream>
#include <WinSock2.h>
#include <iphlpapi.h>
// QT
#include <QComboBox>
#include <QCoreApplication>
#include <QDebug>
#include <QDialog>
#include <QEventLoop>
#include <QGraphicsView>
#include <QLabel>
#include <QMainWindow>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QThread>
#include <QTime>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void handleBindingButton();
    void handleManualButton();
    void triggerZap();
    void getAdapters();
    void delay(int time)
    {
        QTime dieTime = QTime::currentTime().addSecs(time);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

private:
    Ui::MainWindow *ui;
    key_listener *kl;
    QPushButton *binding_button;
    QPushButton *manual_button;
    QProgressBar *progress;
    QLabel *status_label;
    QLabel *image;
    QComboBox *adapterlist;
    DWORD bound_key;
    QString adapter;
    std::atomic<bool> ONGOING;
    QMediaPlayer *media_player;
    QMediaPlayer *loop_player;
};
#endif // MAINWINDOW_H
