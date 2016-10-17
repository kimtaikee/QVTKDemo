#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMap>

namespace Widgets
{
    class TagWidget;
}

class QTreeWidget;
class QStackedWidget;
class QTreeWidgetItem;
class QLineEdit;
class QMenu;
class QAction;
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    QSize sizeHint() const;
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void init();
    void loadPlugins();

private slots:
    void onTreeItemClicked(QTreeWidgetItem *item, int column);
    void filterExamples(const QString& filter);
    void showTags(bool show);
    void showDemoTree(bool show);

private:
    void listAllExamples();
    void listFilteredExamples(const QString& filter);
    void updateCategoryKeywords(const QString& category);

private:
    QWidget* m_exampleWidget = nullptr;
    QLineEdit* m_filterEdit;
    QTreeWidget* m_demoTree;
    Widgets::TagWidget* m_tagWidget;
    QStackedWidget* m_demoStackedWidget;
    QMap<QString, QWidgetList> m_categoryExamplesMap;
    QMap<QString, QStringList> m_categoryKeywordsMap;
    QString m_currentCategory;

    QMenu* m_contextMenu = nullptr;
};

#endif 
