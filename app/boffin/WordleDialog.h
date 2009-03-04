#ifndef WORDLE_DIALOG_H
#define WORDLE_DIALOG_H

#include <QTextEdit>
#include <QVBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QClipboard>
#include <QDialogButtonBox>

class WordleDialog : public QDialog
{
public:
    WordleDialog( QWidget* parent = 0 ) : QDialog( parent )
    {
        new QVBoxLayout( this );
        
        layout()->addWidget( output = new QTextEdit());
        output->setReadOnly( true );
        
        QLabel* hint;
        layout()->addWidget( hint = new QLabel( tr( "The above wordlist has been copied to your clipboard ready for you to paste into <a href=\"http://www.wordle.net/create\">wordle</a>" )));
        hint->setOpenExternalLinks( true );
        
        QDialogButtonBox* buttons;
        layout()->addWidget( buttons = new QDialogButtonBox( QDialogButtonBox::Ok ));
        connect( buttons, SIGNAL( accepted()), SLOT( accept()));
        
        setModal( true );

        setWindowTitle( tr("Wordlization of your boffin tags") );
    }

    void setText( const QString& t ){ output->setText( t ); QApplication::clipboard()->setText( t ); }

private:
    QTextEdit* output;
};

#endif //WORDLE_DIALOG_H
