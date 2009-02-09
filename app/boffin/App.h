#ifndef APP_H
#define APP_H

#include "lib/unicorn/UnicornApplication.h"

class App : public Unicorn::Application
{
public:
    App( int argc, char* argv[] );
};

#endif //APP_H
