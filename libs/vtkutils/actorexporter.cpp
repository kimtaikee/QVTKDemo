#include "actorexporter.h"
#include "vtkutils.h"

#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkVRMLExporter.h>

namespace VtkUtils
{

ActorExporter::ActorExporter(vtkActor* actor, const QString& file) : m_actor(actor), m_exportFile(file)
{
    setAutoDelete(true);
}

void ActorExporter::run()
{
    if (!m_actor || m_exportFile.isEmpty())
        return;

    VTK_CREATE(vtkRenderWindow, renderWindow);
    VTK_CREATE(vtkRenderer, renderer);
    renderer->AddActor(m_actor);
    renderWindow->AddRenderer(renderer);

    VTK_CREATE(vtkVRMLExporter, exporter);
    exporter->SetFileName(m_exportFile.toUtf8().data());
    exporter->SetRenderWindow(renderWindow);
    exporter->Write();
}

} // namespace VtkUtils
