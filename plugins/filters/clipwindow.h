#ifndef CLIPWINDOW_H
#define CLIPWINDOW_H

#include "cutwindow.h"

namespace Ui
{
    class ClipConfig;
}

class ClipWindow : public CutWindow
{
    Q_OBJECT

public:
    explicit ClipWindow(QWidget *parent = 0);
    ~ClipWindow();

    void apply();

};

#endif // CLIPWINDOW_H
