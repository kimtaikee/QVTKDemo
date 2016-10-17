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

#ifndef PEN_STYLE_BOX_H
#define PEN_STYLE_BOX_H


#include <QComboBox>

#include "widgets_global.h"

namespace Widgets
{

class WIDGETS_EXPORT PenStyleBox : public QComboBox
{
    Q_OBJECT

public:
    PenStyleBox(QWidget *parent = 0);
    void setStyle(const Qt::PenStyle& style);
    Qt::PenStyle style() const;

    static int styleIndex(const Qt::PenStyle& style);
    static Qt::PenStyle penStyle(int index);

private:
    static const Qt::PenStyle patterns[];
};

}
#endif
