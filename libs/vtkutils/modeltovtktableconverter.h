#ifndef MODELTOVTKTABLECONVERTER_H
#define MODELTOVTKTABLECONVERTER_H

#include <utils/signalledrunable.h>

#include "vtkutils_global.h"

namespace Utils
{
    class TableModel;
}

class vtkTable;
namespace VtkUtils
{

class VTKUTILS_EXPORT ModelToVtkTableConverter : public Utils::SignalledRunnable
{
    Q_OBJECT
public:
    explicit ModelToVtkTableConverter(Utils::TableModel* model);

    void setLabels(const QStringList& labels);
    QStringList labels() const;

    void run();

    vtkTable* table() const;

private:
    Utils::TableModel* m_model = nullptr;
    vtkTable* m_table = nullptr;
    QStringList m_labels;
};

} // namespace VtkUtils
#endif // MODELTOVTKTABLECONVERTER_H
