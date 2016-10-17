#include "dialog.h"
#include "ui_dialog.h"
#include "widget.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    m_vtkWidget = new VTKWidget(this);
    ui->verticalLayout->addWidget(m_vtkWidget);

    setWindowTitle(tr("Display Chinese"));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_lineEdit_textChanged(const QString &arg1)
{
    m_vtkWidget->setText(arg1);
}
