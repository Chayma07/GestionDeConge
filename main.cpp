#include "FenConnexion.h"
#include <QApplication>
#include "WindowCenter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FenConnexion w;
    setCenterOfApplication(&w);
    w.show();

    return a.exec();
}


