/*! \file
*  \brief Picture基本界面
*  \author 杨永盛 谭立方
*  \date 2013
*  \version 1.0
*  \copyright 2013 PERAGlobal Ltd. All rights reserved.
*
*  供应用程序中所有其它模块使用的基本通用界面PenStyleBox
*/
/****************************************************************************
**
** Copyright (c) 2013 PERAGlobal Ltd. All rights reserved.
** All rights reserved.
**
****************************************************************************/

#ifndef COLORCOMBOBOX_H
#define COLORCOMBOBOX_H

#include <QComboBox>

#include "widgets_global.h"

namespace Widgets
{

class WIDGETS_EXPORT ColorComboBox : public QComboBox
{
    Q_OBJECT

public:
    ColorComboBox(QWidget *parent = 0);
    void setColor(const QColor& c);
    QColor color() const;
    static QList<QColor> colorList();
    static QStringList colorNames();
    static int colorIndex(const QColor& c);
    static QColor color(int colorIndex);
    static QColor defaultColor(int colorIndex);
    static bool isValidColor(const QColor& color);
    static int numPredefinedColors();
    static QStringList defaultColorNames();
    static QList<QColor> defaultColors();

protected:
    void init();
    static const int stColorsCount = 24;
    static const QColor stColors[];

private:
    Q_DISABLE_COPY(ColorComboBox)
};

}
#endif // COLORCOMBOBOX_H
