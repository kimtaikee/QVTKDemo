#include "signalblocker.h"

namespace Utils
{

SignalBlocker::SignalBlocker(QObject* object)
{
    addObject(object);
}

void SignalBlocker::addObject(QObject* object)
{
    if (object) {
        object->blockSignals(true);
        m_objectList.append(object);
    }
}

SignalBlocker::~SignalBlocker()
{
    foreach (QObject* obj, m_objectList)
        obj->blockSignals(false);
}

} // namespace Utils
