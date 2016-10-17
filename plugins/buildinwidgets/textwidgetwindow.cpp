#include "textwidgetwindow.h"
#include "ui_textwidgetconfig.h"
#include "ui_basewidgetwindow.h"

#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/distancewidgetobserver.h>

#include <vtkTextWidget.h>
#include <vtkTextRepresentation.h>
#include <vtkRenderer.h>

TextWidgetWindow::TextWidgetWindow(QWidget* parent) : BaseWidgetWindow(parent)
{
    createUi();
    createWidget();
}

void TextWidgetWindow::createWidget()
{
    VtkUtils::vtkInitOnce(m_rep);
    VtkUtils::vtkInitOnce(m_text);
    m_text->SetRepresentation(m_rep);
    m_rep->SetText("Hello Text Widget");
    m_text->SetInteractor(m_vtkWidget->GetInteractor());
    m_text->On();
    m_vtkWidget->defaultRenderer()->ResetCamera();
}

void TextWidgetWindow::createUi()
{
    m_configUi = new Ui::TextWidgetConfig;
    setupConfigWidget(m_configUi);

    BaseWidgetWindow::createUi();

    setWindowTitle(tr("Text Widget"));
}

void TextWidgetWindow::on_textEdit_textChanged(const QString &arg1)
{
    m_rep->SetText(arg1.toUtf8().data());
    m_vtkWidget->update();
}

void TextWidgetWindow::on_positionCombo_currentIndexChanged(int index)
{
    m_rep->SetWindowLocation(index);
    m_vtkWidget->update();
}


void TextWidgetWindow::on_resizableCheckBox_toggled(bool checked)
{
    m_text->SetResizable(checked);
}

void TextWidgetWindow::on_selectableCheckBox_toggled(bool checked)
{
    m_text->SetSelectable(checked);
}
