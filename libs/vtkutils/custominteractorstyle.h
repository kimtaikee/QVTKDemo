#ifndef CUSTOMINTERACTORSTYLE_H
#define CUSTOMINTERACTORSTYLE_H

#include <QObject>
#include <vtkInteractorStyleTrackballCamera.h>
#include <QPoint>

namespace ComDataGui3D
{
    class Graph3D;
}

class CustomInteractorStyle : public QObject, public vtkInteractorStyleTrackballCamera
{
    Q_OBJECT
public:
    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();
    virtual void Rotate();
    virtual void Spin();
    virtual void OnMouseMove();
    virtual void OnChar();

    void setGraph(ComDataGui3D::Graph3D* graph);

public slots:
    void setClickPosition(const QPoint& pos);
    void setMovePosition(const QPoint& pos);
    void setBounds(double bounds[6]);

signals:
    void mouseReleased();

protected:
    explicit CustomInteractorStyle();
    virtual ~CustomInteractorStyle();

    vtkActor* pickedActor();
    vtkActor* hoverdActor();

    void setPickingCursor();
    void resetPickingCursor();

    double* computeBoundedPos(double* pos);

    void addWatchActor(vtkActor* actor);
    void notifyHoverActor();
    void highlightActor(vtkActor* actor, bool highlight);

protected:
    ComDataGui3D::Graph3D* m_graph;

    QPoint m_clickPos;
    QPoint m_movePos;

    vtkActor* m_pickedActor = nullptr;
    QList<vtkActor*> m_watchedActorList;

    double m_bounds[6];
    double m_boundedPos[3];
};

#endif // CUSTOMINTERACTORSTYLE_H
