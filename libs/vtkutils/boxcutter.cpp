#include "boxcutter.h"

class BoxCutterPrivate
{
public:

};

BoxCutter::BoxCutter(QObject* parent) : Interactor(parent)
{
    d_ptr = new BoxCutterPrivate;
}

BoxCutter::~BoxCutter()
{
    delete d_ptr;
}

void BoxCutter::attach(ComDataGui3D::PostProcessor* pp)
{
    if (!pp)
        return;

    Interactor::attach(pp);

}

void BoxCutter::detach()
{

}

void BoxCutter::update()
{

}

int BoxCutter::type() const
{
    return BOXCUTTER;
}

void BoxCutter::setVisible(bool visible)
{

}

bool BoxCutter::isVisible() const
{
    return Interactor::isVisible();
}

void BoxCutter::setSpecialInteractorStyle()
{

}
