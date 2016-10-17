#ifndef IVTKPLUGIN_H
#define IVTKPLUGIN_H

#include "iplugin.h"
#include "utils_global.h"

namespace Utils
{

class UTILS_EXPORT IVtkPlugin : public IPlugin
{
public:
    IVtkPlugin();
    virtual ~IVtkPlugin();

    virtual QString category() const = 0;
    virtual QList<QWidget*> examples() const = 0;
    virtual QStringList keywords() const = 0;
};

} // namespace Utils

Q_DECLARE_INTERFACE(Utils::IVtkPlugin, "QVTKDemos.IVtkPlugin")

#endif
