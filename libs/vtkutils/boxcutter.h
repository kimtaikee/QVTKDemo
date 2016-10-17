#ifndef BOXCUTTER_H
#define BOXCUTTER_H

#include "interactor.h"

class BoxCutterPrivate;
class BoxCutter : public Interactor
{
    Q_OBJECT
public:
    explicit BoxCutter(QObject* parent = nullptr);
    ~BoxCutter();

    void attach(ComDataGui3D::PostProcessor* pp);
    void detach();

    void update();
    int type() const;

    void setVisible(bool visible);
    bool isVisible() const;

protected:
    void setSpecialInteractorStyle();

private:
    BoxCutterPrivate* d_ptr;
    Q_DISABLE_COPY(BoxCutter)
};

#endif // BOXCUTTER_H
