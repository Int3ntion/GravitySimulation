#include "GravitySimulation.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    GravitySimulation window;
    window.show();
    window.showMaximized();
    return app.exec();
}
