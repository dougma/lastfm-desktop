#ifndef YOU_LIST_WIDGET_H_
#define YOU_LIST_WIDGET_H_

#include <QTreeWidget>

class YouListWidget : public QTreeWidget
{
    Q_OBJECT
public:
    YouListWidget( QWidget* parent )
    :QTreeWidget( parent ) {
        setAlternatingRowColors( true );
        setHeaderHidden( true );
        setExpandsOnDoubleClick( false );
        setItemsExpandable( false );
        setIndentation( 10 );
        setRootIsDecorated( false );
        QList<QTreeWidgetItem*> headerItems;
        QTreeWidgetItem* h;
        headerItems << (h = new QTreeWidgetItem( QStringList() << tr( "Your Stations" )));
        h->addChild( new QTreeWidgetItem( QStringList() << "Your Library"));
        h->addChild( new QTreeWidgetItem( QStringList() << "Your Loved Tracks"));
        h->addChild( new QTreeWidgetItem( QStringList() << "Your Recommendations"));

        headerItems << new QTreeWidgetItem( QStringList() << tr( "Your Artists" ));
        headerItems << new QTreeWidgetItem( QStringList() << tr( "Your Tags" ));
    
        addTopLevelItems( headerItems );
        h->setExpanded( true );
        h->setFlags( h->flags() & ~Qt::ItemIsSelectable );
    }
};

#endif //YOU_LIST_WIDGET_H_
