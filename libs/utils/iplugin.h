#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <QtPlugin>

namespace Utils {

/*!
 * \class IPlugin

 * \brief 类 IPlugin 是所有插件的基类.
 *
 * IPlugin 是一个抽象类, 每一个插件都必须实现它.
 */

class IPlugin
{
public:
    virtual ~IPlugin() {}
    /*!
     * \brief 插件的初始化.
     *
     * 在这里可以利用动作管理器初始化与插件相关的动作, 快捷键, 菜单等.
     * \param arguments 应用启动时带的参数
     * \param parent 一般是主窗口对象
     * \param errorString 输出参数, 插件初始化出错的描述
     * \return 成功初始化返回true, 否则返回false
     */
//    virtual bool initialize(const QStringList &arguments, QObject* parent, QString *errorString) = 0;

    /*!
     * \brief 插件的二次初始化.
     *
     * 当插件的初始化需花费大量时间时, 可使用二次初始化的方法, 在第一次初始化后, 显示应用的主窗口,
     * 然后进行第二次初始化.
     * \return 成功初始化返回true, 否则返回false
     */
//    virtual bool delayedInitialize() { return false; }
};

}

Q_DECLARE_INTERFACE(Utils::IPlugin, "QVTKDemos.IPlugin")

#endif // IPLUGIN_H
