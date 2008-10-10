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
