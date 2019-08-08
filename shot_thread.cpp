#include "Shot_thread.h"

Shot_thread::Shot_thread()
{
    isstop = true;
    isexit = false;
}
Shot_thread::~Shot_thread()
{
    if(avi)
        AVI_close(avi);
}

void Shot_thread::setStatus(bool status)
{
    isstop = status;
}
void Shot_thread::setPath(QString& str)
{
    path = str;
}
void Shot_thread::begin(int w, int h)
{
    if(avi!=NULL)
    {
        AVI_close(avi);
        avi=NULL;
    }
    width=w;
    height=h;
    char* ptr;
    QByteArray ba;
    ba = path.toLatin1();
    ptr = ba.data();
    //isexit = false;
    avi = AVI_open_output_file(ptr);
    AVI_set_video(avi,width,height,5,"mjpg");

    isstop = false;
}
void Shot_thread::stopforonce()
{
    isstop = true;
    if(avi)
    {
        AVI_close(avi);
        avi=NULL;
    }
}

void Shot_thread::end()
{
    isexit=true;
    if(avi)
    {
        AVI_close(avi);
        avi=NULL;
    }
    this->quit();
    this->wait();
}

void Shot_thread::run()
{
    while(!isexit)
    {
        if(!isstop)
        {
            if(myframe::mypics.isEmpty())
            {
                msleep(10);
                continue;
            }
            myframe::mutex.lock();
            for(QPixmap map:myframe::mypics)
            {
                QByteArray ba;
                QBuffer bf(&ba);
                if(!map.save(&bf,"jpg",90))
                {
                    myframe::mutex.unlock();
                    continue;
                }
                if(avi==NULL)
                {
                    myframe::mutex.unlock();
                    continue;
                }
                AVI_write_frame(avi,ba.data(),ba.size(),1);
            }
            myframe::mypics.clear();
            myframe::mutex.unlock();
            msleep(50);
        }
        else
        {
           msleep(10);
        }
    }
}
