#include "colorseries.h"

namespace Utils
{

typedef QList<QColor> ColorList;

static inline QColor hexToColor(QRgb num)
{
    return QColor(qRed(num), qGreen(num), qBlue(num));
}

class ColorSeriesPrivate
{
public:

    static ColorList sSpectrumColorList;
    static ColorList sWarmColorList;
    static ColorList sCoolColorList;
    static ColorList sBluesColorList;
    static ColorList sWildFlowerColorList;
    static ColorList sCitrusColorList;

    ColorSeries::Scheme currentScheme = ColorSeries::Warm;
    int colorCounter = 0;
};

ColorList ColorSeriesPrivate::sSpectrumColorList = ColorList()
        << QColor::fromRgb(0x000000) << QColor::fromRgb(0xE41A1C) << QColor::fromRgb(0x377EB8)
        << QColor::fromRgb(0x4DAF4A) << QColor::fromRgb(0x984EA3) << QColor::fromRgb(0xFF7F00) << QColor::fromRgb(0xA65628);

ColorList ColorSeriesPrivate::sWarmColorList = ColorList()
        << QColor::fromRgb(0x791717) << QColor::fromRgb(0xB50101) << QColor::fromRgb(0xEF4719)
        << QColor::fromRgb(0xF98324) << QColor::fromRgb(0xFFB400) << QColor::fromRgb(0xFFE506);

ColorList ColorSeriesPrivate::sCoolColorList = ColorList()
        << QColor::fromRgb(0x75B101) << QColor::fromRgb(0x588029) << QColor::fromRgb(0x50D7BF)
        << QColor::fromRgb(0x1C95CD) << QColor::fromRgb(0x3B68AB) << QColor::fromRgb(0x9A68FF) << QColor::fromRgb(0x5F3380);

ColorList ColorSeriesPrivate::sBluesColorList = ColorList()
        << QColor::fromRgb(0x3B68AB) << QColor::fromRgb(0x1C95CD) << QColor::fromRgb(0x4ED9EA)
        << QColor::fromRgb(0x739AD5) << QColor::fromRgb(0x423DA9) << QColor::fromRgb(0x505487) << QColor::fromRgb(0x102A52);

ColorList ColorSeriesPrivate::sWildFlowerColorList = ColorList()
        << QColor::fromRgb(0x1C95CD) << QColor::fromRgb(0x3B68AB) << QColor::fromRgb(0x663EB7)
        << QColor::fromRgb(0xA254CF) << QColor::fromRgb(0xDE61CE) << QColor::fromRgb(0xDC6195) << QColor::fromRgb(0x3D1052);

ColorList ColorSeriesPrivate::sCitrusColorList = ColorList()
        << QColor::fromRgb(0x657C37) << QColor::fromRgb(0x75B101) << QColor::fromRgb(0xB2BA30)
        << QColor::fromRgb(0xFFE506) << QColor::fromRgb(0xFFB400) << QColor::fromRgb(0xF98324);

ColorSeries::ColorSeries()
{
    d_ptr = new ColorSeriesPrivate;
}

ColorSeries::~ColorSeries()
{
    delete d_ptr;
}

void ColorSeries::setScheme(ColorSeries::Scheme scheme)
{
    if (d_ptr->currentScheme != scheme) {
        d_ptr->currentScheme = scheme;
        d_ptr->colorCounter = 0;
    }
}

ColorSeries::Scheme ColorSeries::scheme() const
{
    return d_ptr->currentScheme;
}

QColor ColorSeries::color(int index) const
{
    switch (d_ptr->currentScheme) {
    case Spectrum:
        return d_ptr->sSpectrumColorList.at(index % d_ptr->sSpectrumColorList.size());

    case Warm:
        return d_ptr->sWarmColorList.at(index % d_ptr->sWarmColorList.size());

    case Cool:
        return d_ptr->sCoolColorList.at(index % d_ptr->sCoolColorList.size());

    case Blues:
        return d_ptr->sBluesColorList.at(index % d_ptr->sBluesColorList.size());

    case WildFlower:
        return d_ptr->sWildFlowerColorList.at(index % d_ptr->sWildFlowerColorList.size());

    case Citrus:
        return d_ptr->sCitrusColorList.at(index % d_ptr->sCitrusColorList.size());
    }

    return Qt::black;
}

QColor ColorSeries::nextColor() const
{
    return color(d_ptr->colorCounter++);
}

} // namespace Utils
