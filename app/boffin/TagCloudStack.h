#ifndef TAG_CLOUD_STACK_H
#define TAG_CLOUD_STACK_H

#include <boost/function.hpp>
#include <QStackedWidget>
#include <QStringList>

class QItemSelection;
class TagCloudView;
class PlaydarTagCloudModel;

class TagCloudStack : public QStackedWidget
{
    Q_OBJECT

public:
    TagCloudStack(boost::function<PlaydarTagCloudModel* (void)> modelFactory, QWidget* parent = 0);

    TagCloudView* getFirstView() const;
    TagCloudView* drillDown(QString tag);
    void pop();
    QString rql() const;

signals:
    void tagsChange(const QStringList& tagstack);

private slots:
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void setupModelView(TagCloudView* view);

    QStringList m_tags;
    boost::function<PlaydarTagCloudModel* (void)> m_modelFactory;
};

#endif
