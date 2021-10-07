#include <QApplication>
#include "MDPWindow.h"
#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MDPWindow mdpWindow;

    mdpWindow.show();

    return app.exec();
    /*QChar a;
    for (int i=33; i<128; i++) {
        a = QChar(i);
        //if (a.category() == QChar::Other_Control || a.category() == QChar::Symbol_Modifier) {
            std::cout << a.toLatin1() << " " << a.category() << ", ";
        //}
    }
    std::cout << std::endl;*/
}
