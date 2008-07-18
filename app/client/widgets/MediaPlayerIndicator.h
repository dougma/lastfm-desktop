#ifndef MEDIAPLAYERINDICATOR_H
#define MEDIAPLAYERINDICATOR_H

#include <QWidget>

class MediaPlayerIndicator : public QWidget
{
    Q_OBJECT

public:
    MediaPlayerIndicator(QWidget *parent = 0);
    ~MediaPlayerIndicator();

private slots:
    void onAppEvent( int e, const QVariant& v );
    void mediaPlayerConnected( const QString& id );
    void mediaPlayerDisconnected( const QString& id );
};

#endif // MEDIAPLAYERINDICATOR_H
