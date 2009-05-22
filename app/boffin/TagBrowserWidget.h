#ifndef TAG_BROWSER_WIDGET_H
#define TAG_BROWSER_WIDGET_H

#include <boost/function.hpp>
#include <QStringList>
#include <QWidget>

class QItemSelection;
class TagCloudView;
class PlaydarTagCloudModel;
class SideBySideLayout;
class HistoryWidget;
class PlaydarConnection;
class PlaylistModel;
class PlaylistWidget;

class TagBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    TagBrowserWidget(boost::function<PlaydarTagCloudModel* (void)> modelFactory,
        const QString& firstButton,
        PlaydarConnection*,
        QWidget* parent = 0);

    QString rql() const;

signals:
    void selectionChanged();

private slots:
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onHistoryClicked(int position, const QString& text);

private:
    void setupModelView(TagCloudView* view);
    
    HistoryWidget* m_history;
    TagCloudView* m_view;
    PlaylistWidget* m_playlistWidget;
    PlaylistModel* m_playlistModel;

    QStringList m_tags;
    boost::function<PlaydarTagCloudModel* (void)> m_modelFactory;
    PlaydarConnection* m_playdar;
};

#endif
