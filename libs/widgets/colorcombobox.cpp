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

#include "colorcombobox.h"

#include <QSettings>
#include <QPixmap>
#include <QPainter>
#include <algorithm>
#include <QCoreApplication>

namespace Widgets
{
/*!
  \class ColorBox
  \brief ColorBox, 是对QComboBox的扩展.

  \ingroup Pictureui
  */
const QColor ColorComboBox::stColors[] = {
    QColor(Qt::black),
    QColor(Qt::red),
    QColor(Qt::green),
    QColor(Qt::blue),
    QColor(Qt::cyan),
    QColor(Qt::magenta),
    QColor(Qt::yellow),
    QColor(Qt::darkYellow),
    QColor(Qt::darkBlue),
    QColor(Qt::darkMagenta),
    QColor(Qt::darkRed),
    QColor(Qt::darkGreen),
    QColor(Qt::darkCyan),
    QColor("#0000A0"),
    QColor("#FF8000"),
    QColor("#8000FF"),
    QColor("#FF0080"),
    QColor(Qt::white),
    QColor(Qt::lightGray),
    QColor(Qt::gray),
    QColor("#FFFF80"),
    QColor("#80FFFF"),
    QColor("#FF80FF"),
    QColor(Qt::darkGray),
};

/*!
 * \brief 构造颜色下拉框类, 初始化颜色下拉框.
 * \param parent, 父窗口.
 */
ColorComboBox::ColorComboBox(QWidget *parent) : QComboBox(parent)
{
    setEditable(false);
    init();
}

/*!
 * \brief 初始化颜色下拉框.
 */
void ColorComboBox::init()
{
    QList<QColor> indexedColors = colorList();
    QStringList color_names = colorNames();

    QPixmap icon = QPixmap(28, 16);
    QRect r = QRect(0, 0, 27, 15);

    QPainter p;
    p.begin(&icon);

    for (int i = 0; i < indexedColors.size(); i++){
        p.setBrush(QBrush(indexedColors[i]));
        p.drawRect(r);
        this->addItem(icon, color_names[i]);
    }
    p.end();
}

/*!
 * \brief 设置当前颜色.
 * \param c 颜色.
 */
void ColorComboBox::setColor(const QColor& c)
{
    setCurrentIndex(colorIndex(c));
}

/*!
 * \brief 获取当前颜色.
 * \return 当前颜色.
 */
QColor ColorComboBox::color() const
{
    return color(this->currentIndex());
}

/*!
 * \brief 获取颜色的索引值.
 * \param c，需要查找索引的颜色.
 * \return 颜色的索引位置值.
 */
int ColorComboBox::colorIndex(const QColor& c)
{
    if (!isValidColor(c))
        return 0;

    return colorList().indexOf(c);
}

/*!
 * \brief 获取给定索引值的颜色.
 * \param colorIndex，颜色的索引值.
 * \return 给定索引值的颜色, 如果给定索引值超出范围，或者在颜色框中没有找到，也返回黑色.
 */
QColor ColorComboBox::color(int colorIndex)
{
    QList<QColor> colorsList = colorList();
    if (colorIndex >= 0 && colorIndex < colorsList.size())
        return colorsList[colorIndex];

    return Qt::black;
}

/*!
 * \brief 获取颜色下拉框的颜色列表.
 * \return 颜色下拉框的颜色列表.
 */
QList<QColor> ColorComboBox::colorList()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "\\config.ini", QSettings::IniFormat);

    settings.beginGroup("/General");

    QList<QColor> indexedColors;
    QStringList lst = settings.value("/IndexedColors").toStringList();
    if (!lst.isEmpty()){
        for (int i = 0; i < lst.size(); i++)
            indexedColors << QColor(lst[i]);
    } else {
        for (int i = 0; i < stColorsCount; i++)
            indexedColors << stColors[i];
    }
    settings.endGroup();

    return indexedColors;
}

/*!
 * \brief 获取颜色下拉框的颜色名称列表.
 * \return 颜色下拉框的颜色名称列表.
 */
QStringList ColorComboBox::colorNames()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "\\config.ini", QSettings::IniFormat);

    settings.beginGroup("/General");
    QStringList color_names = settings.value("/IndexedColorNames", defaultColorNames()).toStringList();
    settings.endGroup();
    return color_names;
}

/*!
 * \brief 获取给定索引值的颜色.
 * \param colorIndex，颜色的索引值.
 * \return 给定索引值的颜色, 如果给定索引值超出范围，或者在颜色框中没有找到，也返回黑色.
 */
QColor ColorComboBox::defaultColor(int colorIndex)
{
    if (colorIndex >= 0 && colorIndex < (int)sizeof(stColors))
        return stColors[colorIndex];

    return Qt::black;
}

/*!
 * \brief 判断是否是一个有效的颜色.
 * \param color 一个颜色.
 * \return 返回true表示是颜色有效, 否则无效.
 */
bool ColorComboBox::isValidColor(const QColor& color)
{
    return colorList().contains(color);
}

/*!
 * \brief 获取颜色下拉框内置颜色数.
 * \return 颜色下拉框内置颜色数.
 */
int ColorComboBox::numPredefinedColors()
{
    return stColorsCount;
}

/*!
 * \brief 获取默认颜色名列表.
 * \return 默认颜色名列表.
 */
QStringList ColorComboBox::defaultColorNames()
{
    QStringList color_names = QStringList() << tr("black");
    color_names << tr("red");
    color_names << tr("green");
    color_names << tr("blue");
    color_names << tr("cyan");
    color_names << tr("magenta");
    color_names << tr("yellow");
    color_names << tr("dark yellow");
    color_names << tr("navy");
    color_names << tr("purple");
    color_names << tr("wine");
    color_names << tr("olive");
    color_names << tr("dark cyan");
    color_names << tr("royal");
    color_names << tr("orange");
    color_names << tr("violet");
    color_names << tr("pink");
    color_names << tr("white");
    color_names << tr("light gray");
    color_names << tr("gray");
    color_names << tr("light yellow");
    color_names << tr("light cyan");
    color_names << tr("light magenta");
    color_names << tr("dark gray");
    return color_names;
}

/*!
 * \brief 获取默认颜色列表.
 * \return 默认颜色列表.
 */
QList<QColor> ColorComboBox::defaultColors()
{
    QList<QColor> lst;
    for (int i = 0; i < stColorsCount; i++)
        lst << stColors[i];

    return lst;
}

}
