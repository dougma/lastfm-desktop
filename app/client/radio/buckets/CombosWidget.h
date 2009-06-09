/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef COMBOS_WIDGET_H
#define COMBOS_WIDGET_H

#include <QDebug>
#include <QWidget>
#include <QListView>
#include <QAbstractListModel>
#include <QStackedLayout>
#include <lastfm/User>
#include "widgets/UnicornWidget.h"
#include "RqlStationTemplate.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>


class TemplateInput : public QLineEdit
{
Q_OBJECT
public:
    TemplateInput( FieldNode* fieldNode, QWidget* parent = 0 ): QLineEdit( parent ), m_fieldNode( fieldNode )
    {
        connect( this, SIGNAL( textChanged( const QString&)), SLOT( updateQuery()));
    }
    
    FieldNode* fieldNode() const{ return m_fieldNode; }
    
    private slots:
    void updateQuery()
    {
        m_fieldNode->setValue( text());
    }
    
private:
    FieldNode* m_fieldNode;
};

class TemplateInputWidget : public QWidget
{
Q_OBJECT
public:
    
    TemplateInputWidget( RqlStationTemplate* t ): m_template( t )
    {
        QHBoxLayout* h;
        QVBoxLayout* v;
        setLayout(  h = new QHBoxLayout );
        h->addLayout( v = new QVBoxLayout );
        RqlStationTemplate::Iterator i = m_template->iterator();
        
        while( FieldNode* f = i.nextField() )
        {
            if( !f->value().isEmpty())
                continue;
            
            QLineEdit* lineEdit;
            v->addWidget( lineEdit = new TemplateInput( f ));            
            
            connect( lineEdit, SIGNAL( textChanged( const QString& )), SLOT( updateQuery()));
            
            UnicornWidget::paintItBlack( lineEdit );
            
        }
        v->addWidget( ui.query = new QLabel() );
        v->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ));
        UnicornWidget::paintItBlack( this );
        updateQuery();
    }
    
protected slots:
    void updateQuery()
    {
        ui.query->setText( m_template->rql());
    }
    
protected:
    RqlStationTemplate* m_template;
    
    void focusInEvent( QFocusEvent* e )
    {
        qDebug() << "FocusInEvent: " << e;
    }
    
    struct 
    {
        QLabel* query;
    } ui;
            
};

class ComboListModel : public QAbstractListModel
{
public:
    ComboListModel( QObject* parent = 0 ) : QAbstractListModel( parent )
    { 
    };
    
    ~ComboListModel()
    {
        RqlStationTemplate* t;
        while( !m_templates.isEmpty() )
        {
            t = m_templates.takeFirst();
            delete t;
        }
    }

    /** Ownership of the \a t is handed to this ComboListModel instance
      * and will deleted when the ComboListModel is destructed */
    void addRqlStationTemplate( RqlStationTemplate* t )
    {
        beginInsertRows( QModelIndex(), m_templates.count(), m_templates.count() );
        m_templates << t;
        endInsertRows();
    }
    
    virtual int rowCount( const QModelIndex& parent = QModelIndex()) const
    {
        if( parent.isValid() )
            return 0;
        
        return m_templates.count();
    }
    
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const
    {
        switch ( role )
        {
            case Qt::DisplayRole: return QVariant::fromValue<QString>( m_templates[ index.row() ]->title() );
            case Qt::DecorationRole: return QVariant::fromValue<QIcon>( QIcon( "" ));
            default: return QVariant();
        }
    }
    
    RqlStationTemplate* templateFromIndex( const QModelIndex& index ){ return m_templates[ index.row() ]; }
    
private:
    QList< RqlStationTemplate* > m_templates;
};

class CombosWidget : public QWidget
{
    Q_OBJECT
public:
    CombosWidget( QWidget* parent = 0 )
    :QWidget( parent )
    {
        RqlStationTemplate* t = new RqlStationTemplate( new FieldNode("simart:", "Artist" ));
        t->and( new FieldNode( "simart:", "Artist" ));
        t->setTitle( "Multi Artist Radio" );
        t->setDescription( "Play music similar to both artists:" );
        m_model.addRqlStationTemplate( t );
        
        t = new RqlStationTemplate( new FieldNode("user:", "User" ));
        t->and( new FieldNode( "user:", "User" ));
        t->setTitle( "Multi Friend Radio" );
        t->setDescription( "Play music similar to both friends:" );
        m_model.addRqlStationTemplate( t );
        
        t = new RqlStationTemplate( new FieldNode("recs:", "User", AuthenticatedUser() ));
        t->and( new FieldNode( "tag:", "Tag" ));
        t->setTitle( "Recommended by Tag" );
        m_model.addRqlStationTemplate( t );
        
        setLayout( new QStackedLayout() );
        layout()->addWidget( ui.comboList = new QListView );
        ui.comboList->setModel( &m_model );
        UnicornWidget::paintItBlack( ui.comboList );
        ui.comboList->setAttribute( Qt::WA_MacShowFocusRect, false );
        ui.comboList->setViewMode( QListView::IconMode );
        
        connect( ui.comboList, SIGNAL( doubleClicked( const QModelIndex& )), SLOT( onDoubleClicked( const QModelIndex& )));
    }
    
protected slots:
    void onDoubleClicked( const QModelIndex& i )
    {
        TemplateInputWidget* w = new TemplateInputWidget( m_model.templateFromIndex( i ) );
        QStackedLayout* l = qobject_cast<QStackedLayout*>(layout());
        Q_ASSERT( l );
        l->addWidget( w );
        l->setCurrentWidget( w );
        connect( parentWidget(), SIGNAL( currentChanged(int)), w, SLOT( deleteLater()));
    }
    
private:
    struct {
        QListView* comboList;
    } ui;
  
    ComboListModel m_model;
};

#endif //COMBOS_WIDGET_H