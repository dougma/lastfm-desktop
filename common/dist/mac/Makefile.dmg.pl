#!/usr/bin/perl
# Author: max@last.fm
# Usage: Makefile.dmg.pl $$DESTDIR $$VERSION $$QMAKE_LIBDIR_QT $$LIBS
# You must include this from within an app target qmake pro file for it to work!
# the above usage refers to the QMAKE variables

use Cwd 'abs_path';
use File::Basename;

$DESTDIR = shift;
$VERSION = shift;
$QT_FRAMEWORKS_DIR = shift;

while( my $v = shift )
{
	if ($v =~ m/^-l(.*)/)
	{
		push( @DYLIBS, "lib$1.1.dylib" );
	}
}

@QT_MODULES = ("QtCore", "QtGui", "QtWebKit", "QtXml", "QtNetwork", "phonon");
$plist = "\$(CONTENTS)/Info.plist";
$root = abs_path( dirname( $0 ) . "/../../../" );

foreach my $x (@QT_MODULES)
{
	$bundle_frameworks .= "\$(CONTENTS)/Frameworks/$x.framework ";
}
foreach my $x (@DYLIBS)
{
	$bundle_macos .= "\$(CONTENTS)/MacOS/$x ";
}

print <<END;
DIST_TOOLS_DIR = $root/common/dist/mac
BUNDLE = \$(DESTDIR)\$(QMAKE_TARGET).app
CONTENTS = \$(BUNDLE)/Contents
VERSION = $VERSION
BUNDLE_FRAMEWORKS = $bundle_frameworks
BUNDLE_MACOS = $bundle_macos
INSTALLDIR = /Applications/\$(QMAKE_TARGET).app

.PHONY = bundle bundle-clean bundle-install dmg dmg-clean help everything

YOUR_MUM: all

help:
	echo "make bundle"
	echo "make bundle-clean"
	echo "make bundle-install"
	echo "make dmg"
	echo "make dmg-clean"

bundle: all \$(BUNDLE_FRAMEWORKS) \$(BUNDLE_MACOS) \$(CONTENTS)/MacOS/imageformats \$(CONTENTS)/COPYING \$(DESTDIR)mxcl-rules

\$(DESTDIR)mxcl-rules:
	perl -pi -e 's/@VERSION@/'\$(VERSION)'/g' $plist
	perl -pi -e 's/@SHORT_VERSION@/'`echo \$(VERSION) | cut -d'.' -f1,2,3`'/g' $plist
	\$(DIST_TOOLS_DIR)/deposx.sh \$(TARGET) $QT_FRAMEWORKS_DIR
	touch \$(DESTDIR)mxcl-rules

bundle-clean:
	rm -rf \$(BUNDLE_FRAMEWORKS)
	rm -f \$(BUNDLE_MACOS)
	rm -rf \$(CONTENTS)/MacOS/imageformats
	rm -f \$(CONTENTS)/COPYING

dmg: bundle \$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION).dmg
	
dmg-clean: bundle-clean
	rm -f \$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION).dmg

\$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION).dmg:
	cd \$(DESTDIR) && \$(DIST_TOOLS_DIR)/gimme_dmg.sh \$(QMAKE_TARGET).app \$(VERSION)

\$(CONTENTS)/MacOS/imageformats: $QT_FRAMEWORKS_DIR/../plugins/imageformats
	cp -R $QT_FRAMEWORKS_DIR/../plugins/imageformats \$(CONTENTS)/MacOS

\$(CONTENTS)/COPYING:
	cp $root/COPYING \$(CONTENTS)/COPYING

END

foreach my $x (@QT_MODULES)
{
	my $d = "\$(CONTENTS)/Frameworks";
	my $target = "$d/$x.framework";

	my $install_dir = "\$(INSTALLDIR)/Contents/Frameworks";
	my $install_target = "$install_dir/$x.framework";
	my $dep = $target;
	
	print <<END;

$target: $QT_FRAMEWORKS_DIR/$x.framework
	mkdir -p $d
	rm -rf $target
	cp -R $QT_FRAMEWORKS_DIR/$x.framework $d
	rm $target/Versions/4/${x}_debug $target/${x}_debug*
	\$(DIST_TOOLS_DIR)/deposx.sh $target/$x $QT_FRAMEWORKS_DIR
	install_name_tool -id $x.framework/Versions/4/$x $target/Versions/4/$x
	
$install_target: $dep
	mkdir -p $install_dir
	cp -R $dep $install_dir
END
	
	$install_deps .= " $install_target";
}

foreach my $dylib (@DYLIBS)
{
	my $target = "\$(CONTENTS)/MacOS/$dylib";
	my $install_dir = "\$(INSTALLDIR)/Contents/MacOS";
	my $install_target = "$install_dir/$dylib";
	my $dep = $target;

	print <<END;
	
$target: \$(DESTDIR)$dylib
	cp \$(DESTDIR)$dylib \$(CONTENTS)/MacOS
	\$(DIST_TOOLS_DIR)/deposx.sh $target $QT_FRAMEWORKS_DIR

$install_target: $dep
	mkdir -p $install_dir
	cp $dep $install_dir
END

	$install_deps .= " $install_target";
}

print <<END;

\$(INSTALLDIR)/Contents/MacOS/\$(QMAKE_TARGET):
	mkdir -p \$(INSTALLDIR)/Contents/MacOS
	cp \$(TARGET) \$(INSTALLDIR)/Contents/MacOS

\$(INSTALLDIR)/Contents/Info.plist:
	mkdir -p \$(INSTALLDIR)/Contents
	cp \$(CONTENTS)/Info.plist \$(INSTALLDIR)/Contents

\$(INSTALLDIR)/Contents/MacOS/imageformats:
	mkdir -p \$(INSTALLDIR)/Contents/MacOS
	cp -R \$(CONTENTS)/MacOS/imageformats \$(INSTALLDIR)/Contents/MacOS/

\$(INSTALLDIR)/Resources:
	mkdir -p \$(INSTALLDIR)/Contents
	cp -r \$(CONTENTS)/Resources \$(INSTALLDIR)/Contents

bundle-install: bundle \$(INSTALLDIR)/Contents/MacOS/\$(QMAKE_TARGET) \$(INSTALLDIR)/Contents/Info.plist \$(INSTALLDIR)/Resources $install_deps

END
