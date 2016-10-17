#ifndef SELECTAREAINTERACTORSTYLE_H
#define SELECTAREAINTERACTORSTYLE_H

#include <QObject>

#include <vtkInteractorStyleRubberBandPick.h>

class vtkPlanes;
class SelectAreaInteractorStyle : public QObject, public vtkInteractorStyleRubberBandPick
{
    Q_OBJECT
public:
    static SelectAreaInteractorStyle *New();
    vtkTypeMacro(SelectAreaInteractorStyle,vtkInteractorStyleRubberBandPick);

    virtual void OnLeftButtonUp();

signals:
    void areaSelected(vtkPlanes* plane);

protected:
    explicit SelectAreaInteractorStyle(QObject *parent = 0);
};

#endif // SELECTAREAINTERACTORSTYLE_H
