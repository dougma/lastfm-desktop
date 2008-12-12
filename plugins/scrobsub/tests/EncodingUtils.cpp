/* Copyright 205-2009, Last.fm Ltd. <client@last.fm>                       
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "../EncodingUtils.h"
#include <iostream>
#include <wchar.h>

void UnicodeToUtf8();
void AnsiToUtf8();
void Utf8ToAnsi();

int main( int argc, char* argv[] )
{
    UnicodeToUtf8();
    AnsiToUtf8();
    Utf8ToAnsi();
    char c;
    std::cin >> c;
    return 0;
}

void Utf8ToAnsi()
{
    char utf8[] = { 'a', 0xC2, 0xA1, 0x00 };
    char expectedAnsi[] = { 'a', 0xA1, 0x00 };
    
    std::string Ansi = EncodingUtils::Utf8ToAnsi( utf8 );

    if( Ansi.compare( expectedAnsi ) == 0 )
    {
        std::cout << "(OK) UTF8 -> Ansi conversion" << std::endl;
    }
    else
    {
        std::cout << "(E) UTF8 -> Ansi conversion failed:" << std::endl
                  << "UTF8 = " << utf8 << std::endl
                  << "Converted Ansi = " << Ansi << std::endl;
    }

}

void AnsiToUtf8()
{
    char ansi[] = { 0xA1, 0x00 };
    char expectedUtf8[] = { 0xC2, 0xA1, 0x00 };
    char utf8[ 8 ];
    EncodingUtils::AnsiToUtf8( ansi, 
                               utf8, 
                               ( strlen( ansi ) + 1 ) * 4 );
    
    if( strcmp( utf8, expectedUtf8 ) == 0 )
    {
        std::cout << "(OK) Ansi -> UTF8 conversion" << std::endl;
    }
    else
    {
        std::cout << "(E) Ansi -> UTF8 conversion failed:" << std::endl
                  << "Ansi = " << ansi << std::endl
                  << "Converted UTF8 = " << utf8 << std::endl;
    }


}

void UnicodeToUtf8()
{
    wchar_t unicode[] = { 0xA1, 0x00 };
    char expectedUtf8[] = { 0xC2, 0xA1, 0x00 };
    char utf8[ 8 ];
    EncodingUtils::UnicodeToUtf8( unicode, 
                                  wcslen( unicode ) + 1, 
                                  utf8, 
                                  ( wcslen( unicode ) + 1 ) * 4 );
    
    if( strcmp( utf8, expectedUtf8 ) == 0 )
    {
        std::cout << "(OK) Unicode -> UTF8 conversion" << std::endl;
    }
    else
    {
        std::cout << "(E) Unicode -> UTF8 conversion failed:" << std::endl
                  << "Unicode = ";
        std::wcout << unicode << std::endl;
        std::cout << "Converted UTF8 = " << utf8 << std::endl;
    }

}
