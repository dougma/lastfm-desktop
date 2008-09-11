
#ifndef GROWL_H
#define GROWL_H

#include <QString>


class Growl
{
    QString const m_name;
    QString m_title;
    QString m_description;

public:
    /** name is the notification name */
    Growl( const QString& name );
    
    void setTitle( const QString& s ) { m_title = s; }
    void setDescription( const QString& s ) { m_description = s; }
    
    /** shows the notification */
    void notify();
    
    static bool isGrowlAvailable();
};

#endif
