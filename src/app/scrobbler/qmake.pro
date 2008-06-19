TEMPLATE = app
TARGET = AudioScrobbler
CONFIG += unicorn moose 
QT += xml network override

include( $$SRC_DIR/include.pro )

SOURCES += $$system( find . -name \*.cpp )
HEADERS += $$system( find . -name \*.h ) qmake.pro
FORMS += $$system( find -name \*.ui )
RESOURCES += $$system( find -name \*.qrc )
