#ifndef SLICEWINDOW_H
#define SLICEWINDOW_H

#include "cutwindow.h"

class SliceWindow : public CutWindow
{
    Q_OBJECT
public:
    explicit SliceWindow(QWidget* parent = nullptr);

    void apply();

};

#endif // SLICEWINDOW_H
