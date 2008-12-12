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

#include "../BlockingClient.h"
#include <iostream>

static BlockingClient bc;
void test( const char * const domain );

int main( int argc, char* argv[] )
{
    test( "www.last.fm" );
    test( "87.117.229.100" );
    std::cout << "Finish" << std::endl;
    return 0;
}

void test( const char* const domain )
{
    std::string input;
    try
    {
        bc.Connect( domain, 80 );
        std::string httpRequest = "GET / HTTP/1.1\n"
                                  "host: ";
        httpRequest += domain;
        httpRequest += "\r\n\r\n";


        bc.Send( httpRequest );
        bc.Receive( input );
    }
    catch( BlockingClient::NetworkException& e )
    {
        std::cout << "(F) Could not connect to " << domain << ". Exception " << e.what() << " caught." << std::endl;
        return;
    }
    
    std::string httpOkResponse10 = "HTTP/1.0";
    std::string httpOkResponse11 = "HTTP/1.1";
    std::string httpResponse = input.substr( 0, httpOkResponse10.length() );
    if( httpResponse != httpOkResponse10 &&
        httpResponse != httpOkResponse11 )
    {
        std::cout << "(F) Could not connect to " << domain << ". Response = " << input << std::endl;
    }
    else
    {
        std::cout << "(OK) Response from " << domain << " = " << httpResponse << std::endl;
    }

    bc.ShutDown();
}
