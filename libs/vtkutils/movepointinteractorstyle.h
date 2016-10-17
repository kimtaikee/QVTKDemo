#ifndef MOVEPOINTINTERACTORSTYLE_H
#define MOVEPOINTINTERACTORSTYLE_H

#include <QObject>

#include "vtkutils_global.h"

namespace VtkUtils
{

class VTKUTILS_EXPORT MovePointInteractorStyle : public QObject
{
    Q_OBJECT
public:
    explicit MovePointInteractorStyle(QObject *parent = 0);

signals:

public slots:
};

} // namespace VtkUtils
#endif // MOVEPOINTINTERACTORSTYLE_H
