#ifndef LINEPROBER_H
#define LINEPROBER_H

#include "interactor.h"

class vtkPointData;
class LineProberPrivate;
class LineProber : public Interactor
{
    Q_OBJECT
public:
    explicit LineProber(QObject* parent = nullptr);

    int type() const;

public slots:
    void setPoint1(double* pot);
    double* point1() const;

    void setPoint2(double* pot);
    double* point2() const;

    void setPoints(double* pot1, double* pot2);

    void update();

    void attach(ComDataGui3D::PostProcessor* pp);
    void detach();

    void setData(vtkPointData* ds);

    void setVisible(bool visible);
    bool isVisible() const;

    void setPlotVisible(bool visible);
    bool isPlotVisible() const;

signals:
    void point1Changed(double* pot);
    void point2Changed(double* pot);
    void pointsChanged(double* pot1, double* pot2);

private slots:
    void onMouseReleased();

private:
    void createPlot();
    void destroyPlot();

protected:
    void setSpecialInteractorStyle();

private:
    Q_DISABLE_COPY(LineProber)
    LineProberPrivate* d_ptr;
};

#endif // LINEPROBER_H
