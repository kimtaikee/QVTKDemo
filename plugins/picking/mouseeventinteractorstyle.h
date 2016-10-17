#ifndef MOUSEEVENTINTERACTORSTYLE_H
#define MOUSEEVENTINTERACTORSTYLE_H

#include <QObject>

#include <vtkInteractorStyleTrackballCamera.h>

class MouseEventInteractorStyle : public QObject, public vtkInteractorStyleTrackballCamera
{
    Q_OBJECT
public:
    static MouseEventInteractorStyle *New();
    vtkTypeMacro(MouseEventInteractorStyle,vtkInteractorStyleTrackballCamera);

    void OnRightButtonDown();
    void OnRightButtonUp();
    void OnLeftButtonDown();
    void OnLeftButtonUp();
    void OnMouseMove();
    void OnKeyDown();
    void OnKeyUp();

    void setMoveActor(bool move);
    bool moveActor() const;

signals:
    void ctrlPressed(bool pressed);
    void mousePressed(double* pos);
    void mouseMoved(double* pos);
    void mouseReleased(double* pos);

protected:
    explicit MouseEventInteractorStyle(QObject *parent = 0);

    void updateCurrentPos();
    void getPressedActor();

    vtkActor* m_pickedActor = nullptr;
    double m_pos[3];
    bool m_moveActor = false;
};

#endif // MOUSEEVENTINTERACTORSTYLE_H
