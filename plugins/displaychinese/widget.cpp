#include "widget.h"

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextActor.h>
#include <vtkUnicodeString.h>
#include <vtkTextProperty.h>

#include <QDebug>

VTKWidget::VTKWidget(QWidget *parent) : QVTKWidget(parent)
{
    vtkObject::GlobalWarningDisplayOff();

    GetRenderWindow()->SetMultiSamples(0);

    vtkRenderer* renderer = vtkRenderer::New();

    QString text = "你好 VTK";
    m_textActor = vtkTextActor::New();
    m_textActor->GetTextProperty()->SetFontFamily(VTK_FONT_FILE);
    m_textActor->GetTextProperty()->SetFontSize(20);
    m_textActor->GetTextProperty()->SetFontFile("E:/minisong.ttf");

    vtkUnicodeString unistr = vtkUnicodeString::from_utf8(text.toStdString());
    m_textActor->SetInput(unistr.utf8_str());
    renderer->AddActor(m_textActor);

    GetRenderWindow()->AddRenderer(renderer);

    setWindowTitle(tr("QVTK Tutorial: Display Chinese characters"));
}

void VTKWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    layoutTextActor();
}

void VTKWidget::layoutTextActor()
{
    if (m_textActor) {
        QString text(m_textActor->GetInput());
        qreal textLenght = fontMetrics().width(text);
        m_textActor->SetPosition(width() / 2 - textLenght / 2, height() / 2);
    }
}

void VTKWidget::setText(const QString& text)
{
    vtkUnicodeString unistr = vtkUnicodeString::from_utf8(text.toStdString());
    m_textActor->SetInput(unistr.utf8_str());
    layoutTextActor();
    update();
}
