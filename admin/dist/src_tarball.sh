#!/bin/bash

# OSX copies special file attributes to ._files when you tar!
export COPY_EXTENDED_ATTRIBUTES_DISABLE=true
export COPYFILE_DISABLE=true

d=boffin-$1
mkdir -p $d/app/client

cp -R common admin lib $d
cp app/* $d/app
cp -R app/boffin app/clientplugins $d/app
cp app/client/Resolver.cpp $d/app/client
cp app/client/XspfTrackSource.cpp $d/app/client
cp app/client/ResolvingTrackSource.cpp $d/app/client
cp app/client/XspfResolvingTrackSource.cpp $d/app/client
cp app/client/LocalRql.cpp $d/app/client
cp app/client/LocalRadioTrackSource.cpp $d/app/client
cp app/client/Resolver.h $d/app/client
cp app/client/XspfTrackSource.h $d/app/client
cp app/client/ResolvingTrackSource.h $d/app/client
cp app/client/XspfResolvingTrackSource.h $d/app/client
cp app/client/LocalRql.h $d/app/client
cp app/client/LocalRadioTrackSource.h $d/app/client


dirs=`find $d -name .svn`
for x in $dirs; do ../../tools/svn-clean `dirname "$x"`; done
echo $dirs | xargs rm -rf
rm -r $d/lib/listener $d/lib/lastfm/fingerprint $d/lib/3rdparty
cp COPYING configure .qmake.cache $d

cat <<END>$d/Last.fm.pro
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = lib/lastfm/core/libcore.pro \
          lib/lastfm/ws/libws.pro \
          lib/lastfm/types/libtypes.pro \
          lib/lastfm/radio/libradio.pro \
          lib/unicorn/libunicorn.pro \
          app/clientplugins/localresolver/libresolver.pro \
          app/boffin
END


cat <<END>$d/README
Dependencies
============
taglib 1.5
Qt 4.4
phonon [1]
sqlite3
boost

[1] phonon may or may not come with Qt

Build Dependencies
==================
ruby

Compile Errors
==============
Please, I'm not a support dude! I'd appreciate it if you don't email me until 
you're sure it's our fault it isn't compiling! Thanks. <max@last.fm>
END

tar pcjf $d.tar.bz2 $d
rm -rf $d
