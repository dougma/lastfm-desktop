#ifndef MEDIAPLAYERINDICATOR_H
#define MEDIAPLAYERINDICATOR_H

#include <QWidget>

class QLabel;

class MediaPlayerIndicator : public QWidget
{
    Q_OBJECT

public:
    MediaPlayerIndicator();

private slots:
    void onAppEvent( int e, const QVariant& v );
    void mediaPlayerConnected( const QString& id );
    void mediaPlayerDisconnected( const QString& id );
	
private:
	QLabel* m_playerDescription;
	QLabel* m_nowPlayingIndicator;
	void formatRadioStationString();
	QString m_currentContext;
};

#endif // MEDIAPLAYERINDICATOR_H
