#ifndef SEARCH_BOX_H
#define SEARCH_BOX_H

#include <QLineEdit>

class QNetworkReply;
class QCompleter;

namespace lastfm {
    class XmlQuery;
}


class SearchBox : public QLineEdit
{
    Q_OBJECT;

public:
    SearchBox(QWidget *parent = 0);

signals:
    void selected(const QString& item);

private slots:
    void onTextEdited(const QString& text);
    void onSearchFinished();
    void onCompleterActivated(const QString& text);

protected:
    virtual QNetworkReply* startSearch(const QString& term) = 0;
    virtual QStringList handleSearchResponse(lastfm::XmlQuery& lfm) = 0;

    QCompleter* m_completer;
    bool m_searching;
};


class ArtistSearch : public SearchBox
{
public:
    ArtistSearch(QWidget *parent = 0);
    virtual QNetworkReply* startSearch(const QString& term);
    virtual QStringList handleSearchResponse(lastfm::XmlQuery& lfm);
};


class TagSearch : public SearchBox
{
public:
    TagSearch(QWidget *parent = 0);
    virtual QNetworkReply* startSearch(const QString& term);
    virtual QStringList handleSearchResponse(lastfm::XmlQuery& lfm);

};


class UserSearch : public SearchBox
{
public:
    UserSearch(QWidget *parent = 0);
    virtual QNetworkReply* startSearch(const QString& term);
    virtual QStringList handleSearchResponse(lastfm::XmlQuery& lfm);
};

#endif

