#include <QApplication>
#include "MDPWindow.h"
#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MDPWindow mdpWindow;

    mdpWindow.show();

    return app.exec();
}
