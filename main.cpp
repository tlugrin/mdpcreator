#include <QApplication>
#include "MDPWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MDPWindow mdpWindow;
    mdpWindow.setFont(QFont("Ubuntu", 13, QFont::DemiBold));

    mdpWindow.show();

    return app.exec();
}
