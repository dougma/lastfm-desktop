

#include "UnicornCommon.h"
#include "Settings.h"
#include <QLocale>


QString
Unicorn::Settings::appLanguage() const
{
    QString code = QSettings().value( "AppLanguage" ).toString();
    if ( !code.isEmpty() )
        return code;

    // If none found, use system locale
  #ifdef Q_WS_MAC
    QLocale::Language qtLang = UnicornUtils::osxLanguageCode();
  #else
    QLocale::Language qtLang = QLocale::system().language();
  #endif
    return UnicornUtils::qtLanguageToLfmLangCode( qtLang );
}
