#ifndef _WMP_BOOTSTRAP_H
#define _WMP_BOOTSTRAP_H

#include "Bootstrap.h"
#include "wmp.h"

class wmpBootStrap :
    public BootStrap
{
public:
    wmpBootStrap();
    ~wmpBootStrap(void);

    void setCore( IWMPCore *pCore );

private:
    IWMPCore *m_core;
    void readAttributes( IWMPMedia* media );
    bool start();
    std::map<_bstr_t, enum BootStrapDetails > attributes;
    void getDateTimeFormat();

    std::stringstream m_dateFormat;
    std::stringstream m_timeFormat;    
};

#endif // _WMP_BOOTSTRAP_H