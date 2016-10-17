#include "widget.h"
#include "dialog.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Dialog dlg;
    dlg.show();
    return app.exec();
}
