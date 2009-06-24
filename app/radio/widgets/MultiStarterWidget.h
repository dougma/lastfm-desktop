#ifndef MULTI_STARTER_WIDGET_H
#define MULTI_STARTER_WIDGET_H

#include <QWidget>

class SourceListWidget;
class SourceSelectorWidget;

class MultiStarterWidget : public QWidget
{
    Q_OBJECT;

public:
    MultiStarterWidget(int maxSources, QWidget *parent = 0);

private slots:
    void onAdd(const QString& item);
    void onUserGotTopTags();
    void onUserGotTopArtists();
    void onUserGotFriends();

private:
    SourceListWidget* m_sourceList;
    SourceSelectorWidget* m_tags;
    SourceSelectorWidget* m_artists;
    SourceSelectorWidget* m_users;
    const int m_minTagCount;
    const int m_minArtistCount;
};

#endif
