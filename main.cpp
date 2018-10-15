#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QString cfg_file;
    if (argc >= 2)
        cfg_file = argv[1];

    QApplication a(argc, argv);
    MainWindow w(cfg_file);
    w.show();

    return a.exec();
}
