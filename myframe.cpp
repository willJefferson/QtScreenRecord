#include "myframe.h"
//#include <QDebug>

QList<QPixmap> myframe::mypics;//
QMutex myframe::mutex;

myframe::myframe(QWidget *parent)
    : QWidget(parent)
{
    startBtn = new QPushButton(this);
    startBtn->setText(tr("开始"));
    stopBtn = new QPushButton(this);
    stopBtn->setText((tr("停止")));
    saveBtn = new QPushButton(this);
    saveBtn->setText(tr("选择"));
    mainlayout = new QHBoxLayout();
    mainlayout->addWidget(saveBtn);
    mainlayout->addWidget(startBtn);
    mainlayout->addWidget(stopBtn);
    setLayout(mainlayout);
    resize(400,200);

    mythread = new Shot_thread();
    mythread->start();
    mytimer = new QTimer(this);


    connect(saveBtn,SIGNAL(clicked()),this,SLOT(slot_savepath()));
    connect(startBtn,SIGNAL(clicked()),this,SLOT(slot_start()));
    connect(stopBtn,SIGNAL(clicked()),this,SLOT(slot_stop()));
    connect(mytimer,SIGNAL(timeout()),this,SLOT(slot_timer()));

}

myframe::~myframe()
{
    if(!mypics.isEmpty())
        mypics.clear();
    mythread->end();
    delete mythread;
}

void myframe::slot_start()
{

    if(mypath.isEmpty())
    {
        QMessageBox::information(this,"err","please select savepath!");
        saveBtn->setFocus();
        return;
    }
    if(!mypics.isEmpty())
        mypics.clear();
    QDesktopWidget *desktop = QApplication::desktop();
    mythread->setPath(mypath);
    mythread->begin(desktop->width(),desktop->height());
    mytimer->start(50);

}

void myframe::slot_stop()
{
   mytimer->stop();
   mythread->stopforonce();
   mutex.lock();
   if(!mypics.isEmpty())
       mypics.clear();
   mutex.unlock();

}

void myframe::slot_savepath()
{
    QString s = QFileDialog::getSaveFileName(this,"select outputfile","D:/","avi files(*.avi)");
        if(s.isEmpty())
            return;
        mypath = s;
}
void myframe::slot_timer()
{
    QScreen *myscreen = QApplication::primaryScreen();
    QPixmap map = myscreen->grabWindow(QApplication::desktop()->winId());
    mutex.lock();
    mypics.push_back(map);
    mutex.unlock();
}
