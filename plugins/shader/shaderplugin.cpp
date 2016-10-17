#include "shaderplugin.h"
#include "shaderwindow.h"
#include "editshaderwindow.h"

#include <QMap>

ShaderPlugin::ShaderPlugin()
{

}

ShaderPlugin::~ShaderPlugin()
{

}

QString ShaderPlugin::category() const
{
    return QObject::tr("Shader");
}

QList<QWidget*> ShaderPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new ShaderWindow);
    widgetList.append(new EditShaderWindow);
    return widgetList;
}

QStringList ShaderPlugin::keywords() const
{
    QStringList words;
    words << "shader";
    return words;
}
