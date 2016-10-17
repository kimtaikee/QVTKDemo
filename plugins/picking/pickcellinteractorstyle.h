#ifndef PICKCELLINTERACTORSTYLE_H
#define PICKCELLINTERACTORSTYLE_H

#include <QObject>

#include <vtkInteractorStyleTrackballCamera.h>

class PickCellInteractorStyle : public QObject, public vtkInteractorStyleTrackballCamera
{
    Q_OBJECT
public:
    static PickCellInteractorStyle *New();

    void OnLeftButtonDown();

signals:
    void cellPicked(int cellId);

protected:
    explicit PickCellInteractorStyle(QObject *parent = 0);
};

#endif // PICKCELLINTERACTORSTYLE_H
