/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "playercommandparser.h"

#include "exceptions.h"
#include "MooseCommon.h"
#include "logger.h"

#include <QString>

#include <sstream>
#include <algorithm> // transform

using namespace std;

/******************************************************************************
    CPlayerCommandParser
******************************************************************************/
CPlayerCommandParser::CPlayerCommandParser()
{
    // Initialise the command map
    mCmdMap["START"]     = PCMD_START;
    mCmdMap["STOP"]      = PCMD_STOP;
    mCmdMap["PAUSE"]     = PCMD_PAUSE;
    mCmdMap["RESUME"]    = PCMD_RESUME;
    mCmdMap["EXIT"]      = PCMD_EXIT;
    mCmdMap["BOOTSTRAP"] = PCMD_BOOTSTRAP;
}

/******************************************************************************
    Parse
******************************************************************************/
void
CPlayerCommandParser::Parse(
    const std::string& sCmd,
    CPlayerCommand&    cmdOut)
{
    string sNonConstCmd = sCmd;

    cmdOut.mCmd = ParseCommand(sNonConstCmd);

    map<char, string> argsMap;
    ParseArgs(sNonConstCmd, argsMap);

    // Will throw if validation fails
    Validate(cmdOut.mCmd, argsMap);

    cmdOut.mPluginId = QString::fromStdString(argsMap['c']);

    switch( cmdOut.mCmd )
    {
        case PCMD_START:
            ParseTrack(argsMap, cmdOut.mTrack);
            break;

        case PCMD_BOOTSTRAP:
            cmdOut.mUserName = QString::fromStdString( argsMap[ 'u' ] );
            break;

        default:
            break;
    }

}

/******************************************************************************
    ParseCommand
******************************************************************************/
EPlayerCommand
CPlayerCommandParser::ParseCommand(
    std::string& sCmd)
{
    string sFirstWord;

    // Trim any preceding whitespace
    string::size_type nStartPos = sCmd.find_first_not_of(" \t");
    if (nStartPos == string::npos)
    {
        // No chars
        throw ParseException(QT_TR_NOOP("Command string seems to be empty"));
    }

    // Find end of command keyword
    string::size_type nEndPos = sCmd.find_first_of(" ", nStartPos);
    if (nEndPos == string::npos)
    {
        throw ParseException(QT_TR_NOOP("Command doesn't seem to be followed by a space followed by arguments"));
    }

    sFirstWord = sCmd.substr(nStartPos, nEndPos - nStartPos);

    // Upper-case conversion uses C toupper function but that's OK as the
    // protocol is strictly ASCII English.
    transform(sFirstWord.begin(), sFirstWord.end(), sFirstWord.begin(), (int(*)(int))toupper);

    // Check if command exists (not using operator[] as that will create an
    // entry if not found)
    if (mCmdMap.find(sFirstWord) == mCmdMap.end())
    {
        QString err = QString(QT_TR_NOOP("Command '%1' isn't a valid command")).arg( QString::fromStdString(sFirstWord) );
        throw ParseException( err );
    }
    else
    {
        // Trim off command from passed in string
        sCmd.erase(0, nEndPos);
        return mCmdMap[sFirstWord];
    }
}


/******************************************************************************
    ParseArgs
******************************************************************************/
void
CPlayerCommandParser::ParseArgs(
    string& sArgs,
    map<char, string>& argsMap)
{
    // Trim any preceding whitespace
    string::size_type nStartPos = sArgs.find_first_not_of(" \t");
    if (nStartPos == string::npos)
    {
        // No args
        throw ParseException(QT_TR_NOOP("No arguments found in command. All commands need at least the plugin ID."));
    }

    string::iterator iPos = sArgs.begin() + nStartPos;
    char cField;
    string sValue;
    
    bool bMoreArgs = true;
    Q_UNUSED( bMoreArgs )
    
    while (iPos != sArgs.end())
    {
        // Read one field/value pair
        cField = *iPos;

        ++iPos;

        // Verify that next char is equals and skip it
        if (iPos == sArgs.end() || *iPos != '=')
        {
            QString err = QT_TR_NOOP("Missing equals (=) after field identifier: ") +
                cField;
            throw ParseException((err));
        }

        ++iPos;

        // Here comes the data
        // (this stuff is UTF-8 so fine to use std::string)
        while (iPos != sArgs.end())
        {
            if (*iPos == '&')
            {
                ++iPos;
                if (*iPos == '&')
                {
                    // Escaped & found, add one & to result and carry on
                    sValue += '&';
                }
                else
                {
                    // Found a single & delimiter, stop
                    break;
                }
            }
            else
            {
                // Un-escaped character, copy it
                sValue += *iPos;
            }

            ++iPos;

        } // end while reading one arg

        // Verify that a field of this name doesn't already exist
        if (argsMap.find(cField) != argsMap.end())
        {
            QString err = QString(QT_TR_NOOP("Field identifier '%1' occurred twice in request")).arg( cField );
            throw ParseException((err));
        }

        argsMap[cField] = sValue;
        sValue.clear();

        // iPos is now pointing to the first character of the next field.

    } // end while more args

}

/******************************************************************************
    Validate
******************************************************************************/
void
CPlayerCommandParser::Validate(
    EPlayerCommand cmd,
    const map<char, string>& argsMap)
{
    string sRequiredCmds;
    switch (cmd)
    {   
        case PCMD_START:
        {
            sRequiredCmds = "catblp";
        }
        break;

        case PCMD_STOP:
        case PCMD_PAUSE:
        case PCMD_RESUME:
        case PCMD_EXIT:
        {
            sRequiredCmds = "c";
        }
        break;
        case PCMD_BOOTSTRAP:
        {
            sRequiredCmds = "cu";
        }
        break;

        default:
        {
            Q_ASSERT( !"wtf" );
        }
    }

    // Check for required commands
    map<char, string>::const_iterator pos;
    for (pos = argsMap.begin(); pos != argsMap.end(); ++pos)
    {
        string::size_type idx = sRequiredCmds.find(pos->first);
        if (idx != string::npos)
        {
            sRequiredCmds.erase(idx, 1);
        }
    }

    // Did we tick them all off?
    if (sRequiredCmds.size() != 0)
    {
        // All not found, throw
        LOG(2, "Missing parameters in player cmd: " << sRequiredCmds);
        QString err = "Missing arguments: " + QString::fromStdString( sRequiredCmds );
        throw BadCommandException((err));
    }

}

/******************************************************************************
    ParseTrack
******************************************************************************/
void
CPlayerCommandParser::ParseTrack(
    map<char, string>& argsMap,
    TrackInfo& track)
{
    // Read arguments into strings
    QString sArtist = QString::fromUtf8(argsMap['a'].c_str()).trimmed();
    QString sTrack  = QString::fromUtf8(argsMap['t'].c_str()).trimmed();
    QString sAlbum  = QString::fromUtf8(argsMap['b'].c_str()).trimmed();
    QString sMbId   = QString::fromUtf8(argsMap['m'].c_str()).trimmed();
    QString sLength = QString::fromUtf8(argsMap['l'].c_str()).trimmed();
    QString sPath   = QString::fromUtf8(argsMap['p'].c_str()).trimmed();

    track = TrackInfo();
    track.setArtist( sArtist );
    track.setTrack( sTrack );
    track.setAlbum( sAlbum );
    track.setMbId( sMbId );
    track.setDuration( sLength );
    track.setPath( QUrl::fromPercentEncoding( sPath.toUtf8() ) );
    track.setSource( TrackInfo::Player );
}
