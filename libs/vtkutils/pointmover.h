#ifndef POINTMOVER_H
#define POINTMOVER_H

#include "interactor.h"

class PointMoverPrivate;
class PointMover : public Interactor
{
    Q_OBJECT
public:
    explicit PointMover(QObject* parent = nullptr);
    ~PointMover();

    int type() const;

public slots:
    void update();

    void attach(ComDataGui3D::PostProcessor* pp);
    void detach();

    void setPosition(double* pos);
    double* position() const;

    void setPointVisible(bool visible);
    bool isPointVisible() const;

    void setVisible(bool visible);
    bool isVisible() const;

signals:
    void pointChanged(double* pos);

private slots:
    void onCenterChanged(double* center);
    void onMouseReleased();

protected:
    void setSpecialInteractorStyle();

private:
    PointMoverPrivate* d_ptr;
    Q_DISABLE_COPY(PointMover)
};

#endif // POINTMOVER_H
