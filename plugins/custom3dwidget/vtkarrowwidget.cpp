#include "vtkarrowwidget.h"
#include "vtkarrowrepresentation.h"

#include <vtkObjectFactory.h>
#include <vtkCommand.h>
#include <vtkHandleWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkRenderer.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>

#include <QDebug>

class vtkArrowWidgetCallback : public vtkCommand
{
public:
    static vtkArrowWidgetCallback *New() { return new vtkArrowWidgetCallback; }
    virtual void Execute(vtkObject*, unsigned long eventId, void*)
    {
        switch (eventId) {
        case vtkCommand::StartInteractionEvent:
            this->ArrowWidget->StartInteraction();
            break;

        case vtkCommand::InteractionEvent:
            this->ArrowWidget->DoInteraction();
            break;

        case vtkCommand::EndInteractionEvent:
            this->ArrowWidget->EndInteraction();
            break;
        }
    }
    vtkArrowWidget *ArrowWidget;
};

vtkStandardNewMacro(vtkArrowWidget)
vtkArrowWidget::vtkArrowWidget()
{
    this->Point1Widget = vtkHandleWidget::New();
    this->Point1Widget->SetParent(this);
    this->Point2Widget = vtkHandleWidget::New();
    this->Point2Widget->SetParent(this);
    this->WidgetState = vtkArrowWidget::Start;

    this->ArrowWidgetCallback1 = vtkArrowWidgetCallback::New();
    this->ArrowWidgetCallback1->ArrowWidget = this;
    this->Point1Widget->AddObserver(vtkCommand::StartInteractionEvent, this->ArrowWidgetCallback1,
                                    this->Priority);
    this->Point1Widget->AddObserver(vtkCommand::InteractionEvent, this->ArrowWidgetCallback1,
                                    this->Priority);
    this->Point1Widget->AddObserver(vtkCommand::EndInteractionEvent, this->ArrowWidgetCallback1,
                                    this->Priority);

    this->ArrowWidgetCallback2 = vtkArrowWidgetCallback::New();
    this->ArrowWidgetCallback2->ArrowWidget = this;
    this->Point2Widget->AddObserver(vtkCommand::StartInteractionEvent, this->ArrowWidgetCallback2,
                                    this->Priority);
    this->Point2Widget->AddObserver(vtkCommand::InteractionEvent, this->ArrowWidgetCallback2,
                                    this->Priority);
    this->Point2Widget->AddObserver(vtkCommand::EndInteractionEvent, this->ArrowWidgetCallback2,
                                    this->Priority);

    // These are the event callbacks supported by this widget
    this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
                                            vtkWidgetEvent::AddPoint,
                                            this, vtkArrowWidget::AddPointAction);
    this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
                                            vtkWidgetEvent::Move,
                                            this, vtkArrowWidget::MoveAction);
    this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
                                            vtkWidgetEvent::EndSelect,
                                            this, vtkArrowWidget::EndSelectAction);
}

vtkArrowWidget::~vtkArrowWidget()
{
    this->Point1Widget->RemoveObserver(this->ArrowWidgetCallback1);
    this->Point1Widget->Delete();
    this->ArrowWidgetCallback1->Delete();

    this->Point2Widget->RemoveObserver(this->ArrowWidgetCallback2);
    this->Point2Widget->Delete();
    this->ArrowWidgetCallback2->Delete();
}

void vtkArrowWidget::StartInteraction()
{
    this->Superclass::StartInteraction();
    this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
}

void vtkArrowWidget::DoInteraction()
{
    this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
}

void vtkArrowWidget::EndInteraction()
{
    this->Superclass::EndInteraction();
    this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
}

void vtkArrowWidget::AddPointAction(vtkAbstractWidget *w)
{
    vtkArrowWidget *self = reinterpret_cast<vtkArrowWidget*>(w);
    int x = self->Interactor->GetEventPosition()[0];
    int y = self->Interactor->GetEventPosition()[1];

    // Freshly enabled and placing the first point
    if (self->WidgetState == vtkArrowWidget::Start ) {
        self->GrabFocus(self->EventCallbackCommand);
        self->WidgetState = vtkArrowWidget::Define;
        self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
        reinterpret_cast<vtkArrowRepresentation*>(self->WidgetRep)->VisibilityOn();
        double e[2];
        e[0] = static_cast<double>(x);
        e[1] = static_cast<double>(y);
        reinterpret_cast<vtkArrowRepresentation*>(self->WidgetRep)->StartWidgetInteraction(e);
        self->CurrentHandle = 0;
        self->InvokeEvent(vtkCommand::PlacePointEvent,&(self->CurrentHandle));
    } else if (self->WidgetState == vtkArrowWidget::Define) {
        // Placing the second point
        self->CurrentHandle = 1;
        self->InvokeEvent(vtkCommand::PlacePointEvent,&(self->CurrentHandle));
        self->WidgetState = vtkArrowWidget::Manipulate;
        self->Point1Widget->SetEnabled(1);
        self->Point2Widget->SetEnabled(1);
        self->CurrentHandle = -1;
        self->ReleaseFocus();
        self->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
    } else if (self->WidgetState == vtkArrowWidget::Manipulate) {
        // Maybe we are trying to manipulate the widget handles
        int state = self->WidgetRep->ComputeInteractionState(x,y);
        if (state == vtkArrowRepresentation::Outside) {
            self->CurrentHandle = -1;
            return;
        }

        self->GrabFocus(self->EventCallbackCommand);
        if (state == vtkArrowRepresentation::NearP1)
            self->CurrentHandle = 0;
        else if (state == vtkArrowRepresentation::NearP2)
            self->CurrentHandle = 1;

        self->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL);
    }

    // Clean up
    self->EventCallbackCommand->SetAbortFlag(1);
    self->Render();
}

//-------------------------------------------------------------------------
void vtkArrowWidget::MoveAction(vtkAbstractWidget *w)
{
    vtkArrowWidget *self = reinterpret_cast<vtkArrowWidget*>(w);

    // Do nothing if in start mode or valid handle not selected
    if (self->WidgetState == vtkArrowWidget::Start)
        return;

    // Delegate the event consistent with the state
    if (self->WidgetState == vtkArrowWidget::Define) {
        int x = self->Interactor->GetEventPosition()[0];
        int y = self->Interactor->GetEventPosition()[1];
        double e[2];
        e[0] = static_cast<double>(x);
        e[1] = static_cast<double>(y);
        reinterpret_cast<vtkArrowRepresentation*>(self->WidgetRep)->WidgetInteraction(e);
        self->InvokeEvent(vtkCommand::InteractionEvent,NULL);
        self->EventCallbackCommand->SetAbortFlag(1);
    } else {
        // must be moving a handle, invoke a event for the handle widgets
        self->InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
    }

    self->WidgetRep->BuildRepresentation();
    self->Render();
}

//-------------------------------------------------------------------------
void vtkArrowWidget::EndSelectAction(vtkAbstractWidget *w)
{
    vtkArrowWidget *self = reinterpret_cast<vtkArrowWidget*>(w);

    // Do nothing if outside
    if (self->WidgetState == vtkArrowWidget::Start ||
        self->WidgetState == vtkArrowWidget::Define ||
        self->CurrentHandle < 0)
        return;

    self->ReleaseFocus();
    self->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, NULL);
    self->CurrentHandle = -1;
    self->WidgetRep->BuildRepresentation();
    self->EventCallbackCommand->SetAbortFlag(1);
    self->Render();
}

void vtkArrowWidget::SetEnabled(int enabling)
{
    if (enabling) {
        if (this->WidgetState == vtkArrowWidget::Start) {
            reinterpret_cast<vtkArrowRepresentation*>(this->WidgetRep)->VisibilityOff();
        } else {
            // The interactor must be set prior to enabling the widget.
            if (this->Interactor) {
                this->Point1Widget->SetInteractor(this->Interactor);
                this->Point2Widget->SetInteractor(this->Interactor);
            }

            this->Point1Widget->SetEnabled(1);
            this->Point2Widget->SetEnabled(1);
        }
    }

    if (enabling) {
        if (this->Enabled) //already enabled, just return
            return;

        if (!this->Interactor) {
            vtkErrorMacro(<<"The interactor must be set prior to enabling the widget");
            return;
        }

        int x = this->Interactor->GetEventPosition()[0];
        int y = this->Interactor->GetEventPosition()[1];

        if (!this->CurrentRenderer) {
            this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(x, y));
            if (this->CurrentRenderer == NULL)
                return;
        }

        // We're ready to enable
        this->Enabled = 1;
        this->CreateDefaultRepresentation();
        this->WidgetRep->SetRenderer(this->CurrentRenderer);

        // Set the renderer, interactor and representation on the two handle
        // widgets.
        this->Point1Widget->SetRepresentation(reinterpret_cast<vtkArrowRepresentation*>(this->WidgetRep)->
                                              GetPoint1Representation());
        this->Point1Widget->SetInteractor(this->Interactor);
        this->Point1Widget->GetRepresentation()->SetRenderer(this->CurrentRenderer);

        this->Point2Widget->SetRepresentation(reinterpret_cast<vtkArrowRepresentation*>(this->WidgetRep)->
                                              GetPoint2Representation());
        this->Point2Widget->SetInteractor(this->Interactor);
        this->Point2Widget->GetRepresentation()->SetRenderer(this->CurrentRenderer);

        // listen for the events found in the EventTranslator
        if (!this->Parent)
            this->EventTranslator->AddEventsToInteractor(this->Interactor, this->EventCallbackCommand,this->Priority);
        else
            this->EventTranslator->AddEventsToParent(this->Parent, this->EventCallbackCommand,this->Priority);

        if (this->ManagesCursor) {
            this->WidgetRep->ComputeInteractionState(x, y);
            this->SetCursor(this->WidgetRep->GetInteractionState());
        }

        this->WidgetRep->BuildRepresentation();
        this->CurrentRenderer->AddViewProp(this->WidgetRep);

        if (this->WidgetState == vtkArrowWidget::Start) {
            reinterpret_cast<vtkArrowRepresentation*>(this->WidgetRep)->VisibilityOff();
        } else {
            this->Point1Widget->SetEnabled(1);
            this->Point2Widget->SetEnabled(1);
        }

        this->InvokeEvent(vtkCommand::EnableEvent,NULL);
    } else {
        // Disable
        if (!this->Enabled) //already disabled, just return
            return;

        this->Enabled = 0;

        // don't listen for events any more
        if (!this->Parent)
            this->Interactor->RemoveObserver(this->EventCallbackCommand);
        else
            this->Parent->RemoveObserver(this->EventCallbackCommand);

        this->CurrentRenderer->RemoveViewProp(this->WidgetRep);

        this->Point1Widget->SetEnabled(0);
        this->Point2Widget->SetEnabled(0);

        this->InvokeEvent(vtkCommand::DisableEvent, NULL);
        this->SetCurrentRenderer(NULL);
    }

    // Should only render if there is no parent
    if (this->Interactor && !this->Parent)
        this->Interactor->Render();
}

void vtkArrowWidget::CreateDefaultRepresentation()
{
    if (!this->WidgetRep)
        this->WidgetRep = vtkArrowRepresentation::New();
    reinterpret_cast<vtkArrowRepresentation*>(this->WidgetRep)->Initialize();
}

void vtkArrowWidget::SetProcessEvents(int pe)
{
    this->Superclass::SetProcessEvents(pe);
    this->Point1Widget->SetProcessEvents(pe);
    this->Point2Widget->SetProcessEvents(pe);
}

