#include "stdafx.h"
#include "WinampBootstrap.h"
#include "ml.h"

WinampBootStrap::WinampBootStrap()
                :BootStrap( L"wa2" ),
                 m_hWndWinampML( NULL )
{
}

WinampBootStrap::~WinampBootStrap(void)
{
}

bool WinampBootStrap::start()
{    
    log( "WinampBootStrap::start" );

    mlQueryStructW query;
    query.max_results = 0;
    query.query = L"";
    query.results.Alloc = 0;
    query.results.Size = 0;
    query.results.Items = NULL;

    SendMessage( m_hWndWinampML, WM_ML_IPC, (WPARAM)&query, ML_IPC_DB_RUNQUERYW );

    setProgressValue( 0 );
    if( query.results.Size > 0 )
        setProgressRange( query.results.Size - 1 );
    else
        setProgressRange( 0 );

    itemRecordW* curItem = query.results.Items;
    for( int index = 0; index < query.results.Size; index++ )
    {
    
        // Handle the paused state when the cancel button has been pushed
        // and the confirmation dialog has been displayed.
        while( m_paused )
        {
            WaitForSingleObject( m_pauseStateChanged, INFINITE );
            if( m_cancel ) return false;
        }

        curItem = &query.results.Items[ index ];

        std::wstringstream length;
        length << curItem->length;

        std::wstringstream lastPlay;
        lastPlay << curItem->lastplay;

        std::wstringstream playcount;
        playcount << curItem->playcount;
        BootStrapItem bootStrapItem = { curItem->artist ? curItem->artist : L"", 
                                        curItem->title ? curItem->title : L"", 
                                        curItem->album ? curItem->album : L"", 
                                        length.str(), 
                                        playcount.str(), 
                                        curItem->filename ? curItem->filename : L"",
                                        lastPlay.str() };

        appendTrack( &bootStrapItem );
        setProgressValue( index );
      
    }

    completed();
   
    return true;
}

void WinampBootStrap::sethWndWinampML(HWND hWndWinampML)
{
    std::ostringstream os;
    os << "sethWndWinampML: " << hWndWinampML;
    log( os.str() );

    m_hWndWinampML = hWndWinampML;
}