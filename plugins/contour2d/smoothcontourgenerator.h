#ifndef SMOOTHCONTOURGENERATOR_H
#define SMOOTHCONTOURGENERATOR_H

#include "abstractcontourgenerator.h"

class SmoothContourGenerator : public AbstractContourGenerator
{
    Q_OBJECT
public:
    SmoothContourGenerator();

    int type() const;

protected:
    void createContour();
};

#endif // SMOOTHCONTOURGENERATOR_H
