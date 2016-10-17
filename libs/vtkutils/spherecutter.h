#ifndef SPHERECUTTER_H
#define SPHERECUTTER_H

#include "interactor.h"

class SphereCutterPrivate;
class SphereCutter : public Interactor
{
    Q_OBJECT
public:
    explicit SphereCutter(QObject* parent = nullptr);
    ~SphereCutter();

    int type() const;

public slots:
    void setCenter(double* pot, bool update = true);
    double *center() const;

    void setRadius(double radius, bool update = true);
    double radius() const;

    void update();

    void attach(ComDataGui3D::PostProcessor* pp);
    void detach();

    void setVisible(bool visible);
    bool isVisible() const;

signals:
    void centerChanged(double* pot);

private slots:
    void onMouseReleased();

protected:
    void setSpecialInteractorStyle();

private:
    SphereCutterPrivate* d_ptr;
    Q_DISABLE_COPY(SphereCutter)
};

#endif // SPHERECUTTER_H
