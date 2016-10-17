#include "datafilter.h"

namespace VtkUtils
{

QString VTKUTILS_EXPORT filterName(FilterType type)
{
    QString strType;

    switch (type) {
    case FT_Clip:
        strType = QObject::tr("Clip");
        break;

    case FT_Cut:
        strType = QObject::tr("Cut");
        break;

    case FT_Slice:
        strType = QObject::tr("Slice");
        break;

    case FT_Isosurface:
        strType = QObject::tr("Isosurface");
        break;

    case FT_Threshold:
        strType = QObject::tr("Threshold");
        break;

    case FT_Streamline:
        strType = QObject::tr("Streamline");
        break;

    case FT_Smooth:
        strType = QObject::tr("Smooth");
        break;

    case FT_Decimate:
        strType = QObject::tr("Decimate");
        break;

    default:
        strType = "none";
    }

    return strType;
}

} // namespace VtkUtils
