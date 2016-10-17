#ifndef BANDEDCONTUORGENERATOR_H
#define BANDEDCONTUORGENERATOR_H

#include "abstractcontourgenerator.h"

class BandedContourGenerator : public AbstractContourGenerator
{
    Q_OBJECT
public:
    explicit BandedContourGenerator();

    int type() const;

protected:
    void createContour();
};

#endif // BANDEDCONTUORGENERATOR_H
