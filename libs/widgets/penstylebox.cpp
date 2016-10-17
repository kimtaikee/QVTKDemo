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

#include "penstylebox.h"

#include <algorithm>

namespace Widgets
{

const Qt::PenStyle PenStyleBox::patterns[] = {
    Qt::SolidLine,
    Qt::DashLine,
    Qt::DotLine,
    Qt::DashDotLine,
    Qt::DashDotDotLine
};

/*!
 * \brief 构造PenStyleBox.
 * \param parent，父窗口.
 */
PenStyleBox::PenStyleBox(QWidget *parent) : QComboBox(parent)
{
    setEditable(false);
    addItem("_____");
    addItem("_ _ _");
    addItem(".....");
    addItem("_._._");
    addItem("_.._..");
}

/*!
 * \brief 设置样式.
 * \param style，样式.
 */
void PenStyleBox::setStyle(const Qt::PenStyle& style)
{
    const Qt::PenStyle*ite = std::find(patterns, patterns + sizeof(patterns), style);
    if (ite == patterns + sizeof(patterns))
        this->setCurrentIndex(0);
    else
        this->setCurrentIndex(ite - patterns);
}

/*!
 * \brief 获取样式.
 * \param index，样式的索引值.
 * \return 样式，默认为SolidLine.
 */
Qt::PenStyle PenStyleBox::penStyle(int index)
{
    if (index < (int)sizeof(patterns))
        return patterns[index];
    else
        return Qt::SolidLine;
}

/*!
 * \brief 获取当前选中的样式.
 * \return 样式，默认为SolidLine.
 */
Qt::PenStyle PenStyleBox::style() const
{
    size_t i = this->currentIndex();
    if (i < sizeof(patterns))
        return patterns[i];
    else
        return Qt::SolidLine;
}

/*!
 * \brief 获取样式的索引值.
 * \param style，样式.
 * \return 样式的索引值.
 */
int PenStyleBox::styleIndex(const Qt::PenStyle& style)
{
    const Qt::PenStyle*ite = std::find(patterns, patterns + sizeof(patterns), style);
    if (ite == patterns + sizeof(patterns))
        return 0;
    else
        return (ite - patterns);
}

}
