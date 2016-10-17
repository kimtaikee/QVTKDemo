#ifndef SIGNALLEDRUNNABLE_H
#define SIGNALLEDRUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "utils_global.h"

namespace Utils
{

class UTILS_EXPORT SignalledRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    SignalledRunnable();

signals:
    void finished();
};

} // namespace Utils
#endif // SIGNALLEDRUNABLE_H
