#ifndef TAG_CLOUD_WIDGET_H
#define TAG_CLOUD_WIDGET_H

#include <boost/function.hpp>
#include <QStringList>
#include <QWidget>

class QItemSelection;
class TagCloudView;
class PlaydarTagCloudModel;
class SideBySideLayout;
class HistoryWidget;

class TagCloudWidget : public QWidget
{
    Q_OBJECT

public:
    TagCloudWidget(boost::function<PlaydarTagCloudModel* (void)> modelFactory, 
        const QString& firstButton,
        QWidget* parent = 0);

    TagCloudView* getFirstView() const;
    void pop();
    QString rql() const;

private slots:
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onHistoryClicked(int position, const QString& text);
    void onAnimationFinished();

private:
    void setupModelView(TagCloudView* view);

    SideBySideLayout* m_layout;
    HistoryWidget* m_history;
    QStringList m_tags;
    boost::function<PlaydarTagCloudModel* (void)> m_modelFactory;
};

#endif
