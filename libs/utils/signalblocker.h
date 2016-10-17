#ifndef SIGNALBLOCKER_H
#define SIGNALBLOCKER_H

#include <QObject>

#include "utils_global.h"

namespace Utils
{

class UTILS_EXPORT SignalBlocker
{
public:
    explicit SignalBlocker(QObject* object = nullptr);
    ~SignalBlocker();

    void addObject(QObject* object);

private:
    QObjectList m_objectList;
};

} // namespace Utils
#endif // SIGNALBLOCKER_H
