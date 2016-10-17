#ifndef MOVELINEINTERACTORSTYLE_H
#define MOVELINEINTERACTORSTYLE_H

#include "custominteractorstyle.h"

namespace ComDataGui3D
{
    class Graph3D;
}

class vtkPointPicker;
class MoveLineInteractorStyle : public CustomInteractorStyle
{
    Q_OBJECT
public:
    static MoveLineInteractorStyle *New();
    vtkTypeMacro(MoveLineInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnMouseMove();
    virtual void OnLeftButtonUp();

    void setActors(vtkActor* firstBall, vtkActor* secondBall, vtkActor* line);
    void setPoints(double* pot1, double* pot2);

signals:
    void point1Changed(double* pot);
    void point2Changed(double* pot);
    void pointsChanged(double* pot1, double* pot2);

private:
    MoveLineInteractorStyle();
    ~MoveLineInteractorStyle();

    vtkActor* m_firstBall;
    vtkActor* m_secondBall;
    vtkActor* m_line;

    double m_pot1[3];
    double m_pot2[3];
};

#endif // MOVELINEINTERACTORSTYLE_H
