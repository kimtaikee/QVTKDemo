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

#include "colorpushbutton.h"
#include "colorcombobox.h"

#include <QColorDialog>

namespace Widgets
{

/*!
 * \class ColorPushButton
 * \brief ColorPushButton, 是对QtColorPicker的扩展.
 * \ingroup Pictureui
 */

/*!
 * \brief 构造拾色器按钮类, 初始化拾色器按钮.
 * \param parent, 父窗口
 */
ColorPushButton::ColorPushButton(QWidget *parent) : QtColorPicker(parent)
{
    QStringList color_names = ColorComboBox::defaultColorNames();
    QList<QColor> defaultColors = ColorComboBox::defaultColors();
    for (int i = 0; i < ColorComboBox::numPredefinedColors(); i++)
        insertColor(defaultColors[i], color_names[i]);

    QList<QColor> colors = ColorComboBox::colorList();
    color_names = ColorComboBox::colorNames();
    for (int i = 0; i < colors.count(); i++){
        QColor c = colors[i];
        if (!defaultColors.contains(c))
            insertColor(c, color_names[i]);
    }

    connect(this, SIGNAL(colorChanged(const QColor &)), this, SIGNAL(colorChanged()));
}

ColorPushButton::~ColorPushButton()
{

}

}
