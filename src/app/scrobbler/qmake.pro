TEMPLATE = app
TARGET = AudioScrobbler
CONFIG += unicorn moose
QT += xml network

include( $$SRC_DIR/include.pro )

SOURCES = $$system( ls *.cpp )
HEADERS = $$system( ls *.h )
FORMS = $$system( ls *.ui )
