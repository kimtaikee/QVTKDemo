#ifndef DISABLEROTATIONINTERACTORSTYLE_H
#define DISABLEROTATIONINTERACTORSTYLE_H

#include "custominteractorstyle.h"
#include "../cutter.h"

namespace ComDataGui3D
{
    class Graph3D;
}

class vtkActor;
class DisableRotationInteractorStyle : public CustomInteractorStyle
{
    Q_OBJECT
public:
    static DisableRotationInteractorStyle *New();
    vtkTypeMacro(DisableRotationInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonUp();
    virtual void OnMouseMove();

    void setCutType(ComDataGui3D::Cutter::CutType ct);
    void setSphereActor(vtkActor* sphere);
    void setPlaneActor(vtkActor* plane);
    void setFrameActor(vtkActor* frame);
    void setOriginActor(vtkActor* origin);
    void setTopBallActor(vtkActor* topBall);
    void setBottomBallActor(vtkActor* bottomBall);
    void setStickActor(vtkActor* stick);

public slots:
    void setOrigin(double* origin);
    void setNormal(double* normal);
    void onStickPointsChanged(double* topPot, double* bottomPot);

signals:
    void graphMoved();
    void originChanged(double* origin);
    void planeChanged(double* pos);
    void normalChanged(bool positive, double* normal);
    void update();

private:
    void resetActors();
    void highlightStick(bool highlight);
    void highlightOrigin(bool highlight);
    void highlightActor();
    void notifySpherePosChanged();

private:
    DisableRotationInteractorStyle();
    ~DisableRotationInteractorStyle();

    ComDataGui3D::Cutter::CutType m_cutType;

    bool m_isDirty = false;

    vtkActor* m_sphereActor = nullptr;
    vtkActor* m_planeActor = nullptr;
    vtkActor* m_frameActor = nullptr;
    vtkActor* m_originActor = nullptr;
    vtkActor* m_stickActor = nullptr;
    vtkActor* m_topBallActor = nullptr;
    vtkActor* m_bottomBallActor = nullptr;

    double m_origin[3];
    double m_normal[3];
    double m_topPoint[3];
    double m_bottomPoint[3];
};

#endif // DISABLEROTATIONINTERACTORSTYLE_H
