#include "myframe.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    myframe w;
    w.show();

    return a.exec();
}
