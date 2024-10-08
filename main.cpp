#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow viewer;  // Changed to MainWindow
    viewer.setWindowTitle("32-bit Hex/Decimal Viewer");
    viewer.resize(800, 250);  // Resize to fit 32 labels
    viewer.show();

    return app.exec();
}