#include "shaderwindow.h"
#include "ui_shaderwindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
//#include <vtkShader.h>
//#include <vtkShaderProgram.h>
#include <vtkSmartPointer.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLProperty.h>

#include <vtkShader2Collection.h>
#include <vtkShader2.h>
#include <vtkShaderProgram2.h>
#include <vtkOpenGLProperty.h>

#include <QThread>

ShaderWindow::ShaderWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ShaderWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Using shader"));

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    vtkSmartPointer<vtkConeSource> cone = vtkConeSource::New();

    vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkPolyDataMapper::New();
    coneMapper->SetInputConnection( cone->GetOutputPort() );

    vtkSmartPointer<vtkActor> coneActor = vtkActor::New();
    coneActor->SetMapper(coneMapper);

    m_vtkWidget->defaultRenderer()->AddActor(coneActor);

    const char* frag = "void propFuncFS(void){ gl_FragColor = vec4(255,0,0,1);}";
    QString fragmentShaderSource(frag);

    //The GLSL binded program
    VTK_CREATE(vtkShaderProgram2, program);

    // Prepare the fragment shader
    VTK_CREATE(vtkShader2, shader);
    shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
    shader->SetSourceCode(fragmentShaderSource.toStdString().c_str());
    program->GetShaders()->AddItem(shader);

    vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
            static_cast<vtkOpenGLProperty*>(coneActor->GetProperty());
    openGLproperty->SetPropProgram(program);
    openGLproperty->ShadingOn();
}

ShaderWindow::~ShaderWindow()
{
    delete m_ui;
}
