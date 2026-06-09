#include "GravitySimulation.h"
#include <QtWidgets/QApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    GravitySimulation window;
    window.show();
    window.showMaximized();
    return app.exec();
}
