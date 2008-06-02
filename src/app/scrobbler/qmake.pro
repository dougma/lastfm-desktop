TEMPLATE = vcapp
TARGET = AudioScrobbler
#CONFIG += unicorn moose

include( $$SRC_DIR/include.pro )

SOURCES = $$system( ls *.cpp )
HEADERS = $$system( ls *.h )
