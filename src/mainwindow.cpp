#include "mainwindow.h"
//#include "ui_mainwindow.h"

#include <utils/ivtkplugin.h>

#include <widgets/tagwidget.h>

#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QDebug>
#include <QTreeWidget>
#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QApplication>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QContextMenuEvent>

static const QString AppTitle = QObject::tr("QVTK Demos");

template <typename Interface>
QMap<Interface*, QVariantMap> loadPlugins(const QString& strPluginDir)
{
    QMap<Interface*, QVariantMap> loadedPlugins;

    QDir pluginDir(strPluginDir);
    for (const QString& fileName : pluginDir.entryList(QDir::Files)) {
        if (!QLibrary::isLibrary(fileName))
            continue;

        QString file = pluginDir.absoluteFilePath(fileName);
        QPluginLoader loader(file);
        Interface* interface = qobject_cast<Interface*>(loader.instance());
        if (interface) {
            QVariantMap temp = loader.metaData().toVariantMap();
            temp[QString::fromLatin1("fileName")] = QFileInfo(fileName).baseName();
            loadedPlugins[interface] = temp;
        } else {
            qDebug() << loader.errorString();
            loader.unload();
        }
    }
    return loadedPlugins;
}

template <typename Interface>
QPair<Interface*, QVariantMap> loadPlugin(const QString& pluginFile, QString* error = nullptr)
{
    if (!QFile::exists(pluginFile)) {
        Q_ASSERT(0);
        return qMakePair(nullptr, QVariantMap());
    }
    QPluginLoader loader(pluginFile);
    Interface* interface = qobject_cast<Interface*>(loader.instance());
    if (error)
        *error = loader.errorString();
    if (interface) {
        return qMakePair(interface, loader.metaData().toVariantMap());
    } else {
        loader.unload();
        return qMakePair(nullptr, QVariantMap());
    }
}


MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    init();
    loadPlugins();
}

MainWindow::~MainWindow()
{
}

QSize MainWindow::sizeHint() const
{
    return QSize(1500, 600);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if (!m_contextMenu) {
        m_contextMenu = new QMenu(this);

        QAction* act = new QAction(tr("Show Tags"), this);
        act->setCheckable(true);
        act->setChecked(true); // default to checked
        connect(act, SIGNAL(toggled(bool)), this, SLOT(showTags(bool)));
        m_contextMenu->addAction(act);

        act = new QAction(tr("Show Demos"), this);
        act->setCheckable(true);
        act->setChecked(true);
        connect(act, SIGNAL(toggled(bool)), this, SLOT(showDemoTree(bool)));
        m_contextMenu->addAction(act);
    }

    m_contextMenu->exec(event->globalPos());
}

void MainWindow::init()
{
    setWindowTitle(AppTitle);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(0);
    setLayout(layout);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    layout->addWidget(splitter);

    m_exampleWidget = new QWidget(this);
    m_exampleWidget->setMaximumWidth(350);
    m_exampleWidget->setMinimumWidth(250);

    QVBoxLayout* exampleWidgetLayout = new QVBoxLayout;
    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText(tr("Filter"));
    m_filterEdit->setClearButtonEnabled(true);
    exampleWidgetLayout->addWidget(m_filterEdit);

    m_demoTree = new QTreeWidget(this);
    m_demoTree->header()->setVisible(false);
    m_demoTree->setAnimated(true);
    m_demoTree->setAlternatingRowColors(true);
    exampleWidgetLayout->addWidget(m_demoTree);

    m_tagWidget = new Widgets::TagWidget(this);
    exampleWidgetLayout->addWidget(m_tagWidget);

    m_exampleWidget->setLayout(exampleWidgetLayout);

    splitter->addWidget(m_exampleWidget);

    m_demoStackedWidget = new QStackedWidget(this);
    splitter->addWidget(m_demoStackedWidget);

    connect(m_demoTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*,int)));
    connect(m_filterEdit, SIGNAL(textChanged(QString)), this, SLOT(filterExamples(QString)));
}

void MainWindow::loadPlugins()
{
    QString pluginDir = QString("%1/plugins").arg(qApp->applicationDirPath());
    QMap<Utils::IVtkPlugin*, QVariantMap> plugins = ::loadPlugins<Utils::IVtkPlugin>(pluginDir);

    QString category;
    int exampleCounter = 0;
    for (Utils::IVtkPlugin* interface : plugins.keys()) {
          // skip empty example
//        if (interface->examples().isEmpty())
//            continue;

        category = interface->category();
        QTreeWidgetItem* categoryItem = new QTreeWidgetItem;
        categoryItem->setText(0, interface->category());

        QString title;
        QWidgetList examples;
        foreach (QWidget* widget, interface->examples()) {
            title = widget->windowTitle();
            QTreeWidgetItem* subItem = new QTreeWidgetItem;
            subItem->setText(0, title);
            categoryItem->addChild(subItem);

            examples.append(widget);
            m_demoStackedWidget->addWidget(widget);

            ++exampleCounter;
        }

        m_categoryExamplesMap.insert(category, examples);
        m_categoryKeywordsMap.insert(category, interface->keywords());

        m_demoTree->addTopLevelItem(categoryItem);
    }

    m_demoTree->expandAll();

    // show first category's first demo
    m_filterEdit->setPlaceholderText(tr("%1 categorys %2 examples").arg(m_categoryExamplesMap.size()).arg(exampleCounter));
}

static inline int widgetIndex(const QWidgetList& widgetList, const QString& title)
{
    int index = -1;
    for (int i = 0; i < widgetList.size(); ++i) {
        if (widgetList.at(i)->windowTitle().compare(title, Qt::CaseInsensitive) == 0) {
            index = i;
            break;
        }
    }
    return index;
}

void MainWindow::onTreeItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)

    // if it's category item then returns
    if (item->parent() == nullptr) {
        updateCategoryKeywords(item->text(0));
        return;
    }

    QString category = item->parent()->text(0);
    QWidgetList examples = m_categoryExamplesMap.value(category);

    QString title = item->text(0);
    int index = widgetIndex(examples, title);

    // load category keywords
    updateCategoryKeywords(category);

    if (index >= 0) {
        QWidget* widget = examples.at(index);
        if (widget)
            m_demoStackedWidget->setCurrentWidget(widget);

        setWindowTitle(tr("%1 - [%2 -> %3]").arg(AppTitle).arg(category).arg(widget->windowTitle()));
    } else {
        qDebug() << "oops, not find: " << title;
    }
}

void MainWindow::filterExamples(const QString &filter)
{
    if (filter.isEmpty())
        listAllExamples();
    else
        listFilteredExamples(filter);
}

void MainWindow::showTags(bool show)
{
    m_tagWidget->setVisible(show);
}

void MainWindow::showDemoTree(bool show)
{
    m_exampleWidget->setVisible(show);
}

void MainWindow::listAllExamples()
{
    m_demoTree->clear();
    auto iter = m_categoryExamplesMap.begin();
    while (iter != m_categoryExamplesMap.end()) {

        QTreeWidgetItem* categoryItem = new QTreeWidgetItem;
        categoryItem->setText(0, iter.key());

        QList<QWidget*> examples = m_categoryExamplesMap.value(iter.key());

        QString title;
        foreach (QWidget* widget, examples) {
            title = widget->windowTitle();
            QTreeWidgetItem* subItem = new QTreeWidgetItem;
            subItem->setText(0, title);
            categoryItem->addChild(subItem);
        }

        m_demoTree->addTopLevelItem(categoryItem);
        ++iter;
    }
}

void MainWindow::listFilteredExamples(const QString& filter)
{
    m_demoTree->clear();

    auto iter = m_categoryExamplesMap.begin();
    while (iter != m_categoryExamplesMap.end()) {
        QTreeWidgetItem* categoryItem = nullptr;
        QList<QWidget*> examples = m_categoryExamplesMap.value(iter.key());

        QString title;
        foreach (QWidget* widget, examples) {
            title = widget->windowTitle();

            if (title.contains(filter, Qt::CaseInsensitive)) {
                QTreeWidgetItem* subItem = new QTreeWidgetItem;
                subItem->setText(0, title);

                if (!categoryItem) {
                    categoryItem = new QTreeWidgetItem;
                    categoryItem->setText(0, iter.key());
                }

                categoryItem->addChild(subItem);
            }
        }

        m_demoTree->addTopLevelItem(categoryItem);
        m_demoTree->expandItem(categoryItem);
        ++iter;
    }
}

void MainWindow::updateCategoryKeywords(const QString& category)
{
    if (m_currentCategory != category) {
        m_tagWidget->clear();
        m_tagWidget->addTags(m_categoryKeywordsMap.value(category));
        m_currentCategory = category;
    }
}



