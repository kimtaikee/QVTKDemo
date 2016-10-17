#ifndef CONTOUR2DINTERACTORSTYLE_H
#define CONTOUR2DINTERACTORSTYLE_H

#include <QObject>

#include <vtkInteractorStyleTrackballCamera.h>

class Contour2DInteractorStyle : public QObject, public vtkInteractorStyleTrackballCamera
{
    Q_OBJECT
public:
    Contour2DInteractorStyle(QObject* parent = nullptr);

    void OnLeftButtonDown();

signals:
    void mousePressed(double* pos);
};

#endif // CONTOUR2DINTERACTORSTYLE_H
