#include "abstractwidgetobserver.h"

#include <vtkInteractorObserver.h>

namespace VtkUtils
{

AbstractWidgetObserver::AbstractWidgetObserver(QObject *parent) : QObject(parent)
{

}

AbstractWidgetObserver::~AbstractWidgetObserver()
{

}

void AbstractWidgetObserver::attach(vtkInteractorObserver *widget)
{
    if (widget && widget != m_widget) {
        m_widget = widget;

        // we only care about this specific event
        m_widget->AddObserver(vtkCommand::EndInteractionEvent, this);
    }
}

} // namespace VtkUtils
