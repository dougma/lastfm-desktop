#include "SearchBox.h"
#include <QNetworkReply>
#include <QPushButton>
#include <QListView>
#include <QCompleter>
#include <QStringListModel>
#include <lastfm/XmlQuery>
#include <lastfm/Artist>
#include <lastfm/Tag>

SearchBox::SearchBox(QWidget* parent)
: QLineEdit(parent)
, m_searching(false)
{
    m_completer = new QCompleter(this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(m_completer);

    connect(this, SIGNAL(textEdited(QString)), SLOT(onTextEdited(QString)));
    connect(m_completer, SIGNAL(activated(QString)), SLOT(onCompleterActivated(QString)));
}

void
SearchBox::onTextEdited(const QString& text)
{
    if (!m_searching && text.length()) {
        QNetworkReply* reply = startSearch(text);
        if (reply) {
            m_searching = true;
            connect(reply, SIGNAL(finished()), SLOT(onSearchFinished()));
        }
    }
}

void 
SearchBox::onCompleterActivated(const QString& text)
{
    // click on a completion option, and it's like you typed it in!
    setText(text);
    emit returnPressed();
}

void
SearchBox::onSearchFinished()
{
    sender()->deleteLater();
    QString searchTerm;
    try {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        lastfm::XmlQuery lfm(reply->readAll());
        searchTerm = ((QDomElement)lfm["results"]).attribute("for");
        m_completer->setModel(
            new QStringListModel(
                handleSearchResponse(lfm)));
        m_completer->complete();
    } 
    catch (...) {
    }

    m_searching = false;
    // possibly a search pending:
    if (text() != searchTerm) {
        onTextEdited(text());
    }
}



///////////////////////////////////////////

ArtistSearch::ArtistSearch(QWidget* parent)
: SearchBox(parent)
{
}

QNetworkReply*
ArtistSearch::startSearch(const QString& term)
{
    return lastfm::Artist(term).search();
}

QStringList
ArtistSearch::handleSearchResponse(lastfm::XmlQuery& lfm)
{
    QStringList list;
    foreach(lastfm::XmlQuery i, lfm["results"]["artistmatches"].children("artist")) {
        list << i["name"].text();
    }
    return list;
}

///////////////////////////////////////////

TagSearch::TagSearch(QWidget* parent)
: SearchBox(parent)
{
}

QNetworkReply*
TagSearch::startSearch(const QString& term)
{
    return lastfm::Tag(term).search();
}

QStringList
TagSearch::handleSearchResponse(lastfm::XmlQuery& lfm)
{
    QStringList list;
    foreach(lastfm::XmlQuery i, lfm["results"]["tagmatches"].children("tag")) {
        list << i["name"].text().toLower();
    }
    return list;
}

///////////////////////////////////////////

UserSearch::UserSearch(QWidget* parent)
: SearchBox(parent)
{
}

QNetworkReply*
UserSearch::startSearch(const QString& term)
{
    return lastfm::Tag(term).search();
}

QStringList
UserSearch::handleSearchResponse(lastfm::XmlQuery& lfm)
{
    QStringList list;
    foreach(lastfm::XmlQuery i, lfm["results"]["tagmatches"].children("tag")) {
        list << i["name"].text().toLower();
    }
    return list;
}
