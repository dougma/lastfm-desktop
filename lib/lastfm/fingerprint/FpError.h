/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#ifndef LASTFM_FP_ERROR_H
#define LASTFM_FP_ERROR_H


namespace Fp
{
    enum Error
    {
        /** File does not exist or cannot be read */
        ReadError = 0,

        /** GetInfo failed to extract samplerate, bitrate, channels, duration etc */
        GetInfoError,
        
        /** Track is shorter than minimum track duration for fingerprinting */
        TrackTooShortError,
        
        /** Could not initialize the fingerprintExtractor (probably ran out of RAM) */
        ExtractorInitError,
        
        /** The fingerprintExtractor has not been initialized before process() is called */
        ExtractorProcessError,
        
        /** The fingerprintExtractor has been starved of data to generate a fingerprint */
        ExtractorNotEnoughDataError,
        
        /** FingerprintExtractor::getFingerprint() has been called prematurely */
        ExtractorNotReadyError
    };
}

#endif
