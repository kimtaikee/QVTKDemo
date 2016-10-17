#ifndef VTKARROWWIDGET_H
#define VTKARROWWIDGET_H

#include <vtkAbstractWidget.h>

class vtkHandleWidget;
class vtkArrowWidgetCallback;
class vtkArrowWidget : public vtkAbstractWidget
{
public:
    static vtkArrowWidget *New();
    vtkTypeMacro(vtkArrowWidget,vtkAbstractWidget)

    virtual void SetEnabled(int);

    void CreateDefaultRepresentation();

    void SetProcessEvents(int);

    enum { Start = 0, Define, Manipulate };

    virtual int GetWidgetState() { return this->WidgetState; }

protected:
    vtkArrowWidget();
    ~vtkArrowWidget();

    void StartInteraction();
    void DoInteraction();
    void EndInteraction();

    static void AddPointAction(vtkAbstractWidget*);
    static void MoveAction(vtkAbstractWidget*);
    static void EndSelectAction(vtkAbstractWidget*);

    int WidgetState;
    int CurrentHandle;

    vtkHandleWidget *Point1Widget;
    vtkHandleWidget *Point2Widget;
    vtkArrowWidgetCallback *ArrowWidgetCallback1;
    vtkArrowWidgetCallback *ArrowWidgetCallback2;

    friend class vtkArrowWidgetCallback;

private:
    vtkArrowWidget(const vtkArrowWidget&);
    void operator=(const vtkArrowWidget&);
};

#endif // VTKARROWWIDGET_H
