#include <QApplication>
#include "TestKeyboard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Face f;
    f.show();

    return a.exec();
}