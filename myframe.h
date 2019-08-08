#ifndef MYFRAME_H
#define MYFRAME_H

#include <QWidget>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QPixmap>
#include <QBuffer>
#include <QMutex>
#include "avilib.h"
#include "shot_thread.h"

class Shot_thread;
class myframe : public QWidget
{
    Q_OBJECT

public:
    myframe(QWidget *parent = 0);
    ~myframe();
    static QList<QPixmap> mypics;
    static QMutex mutex;

private:
    QPushButton *startBtn,*stopBtn,*saveBtn;
    QHBoxLayout *mainlayout;

    QTimer *mytimer;
    QString mypath;

    Shot_thread *mythread;


public slots:
    void slot_savepath();
    void slot_start();
    void slot_stop();
    void slot_timer();

};

#endif // MYFRAME_H
