#ifndef Shot_thread_H
#define Shot_thread_H

#include <QThread>

#include "avilib.h"
#include "myframe.h"

class myframe;
class Shot_thread : public QThread
{
public:
    Shot_thread();
    ~Shot_thread();
    void run();
    void setPath(QString& str);
    void setStatus(bool status);
    void begin(int w,int h);
    void end();
    void stopforonce();


private:
    avi_t *avi=NULL;
    bool isexit = false;
    bool isstop = true;
    QString path;
    int width,height;

};

#endif // Shot_thread_H
