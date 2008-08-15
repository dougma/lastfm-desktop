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

$DEPOSX = "\$(DIST_TOOLS_DIR)/deposx.sh";

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

\$(DESTDIR)mxcl-is-super: \$(TARGET) $plist
	perl -pi -e 's/@VERSION@/'\$(VERSION)'/g' $plist
	perl -pi -e 's/@SHORT_VERSION@/'`echo \$(VERSION) | cut -d'.' -f1,2,3`'/g' $plist
	$DEPOSX \$(TARGET) $QT_FRAMEWORKS_DIR
	touch \$\@

bundle-clean:
	rm -rf \$(BUNDLE_FRAMEWORKS)
	rm -f \$(BUNDLE_MACOS)
	rm -rf \$(CONTENTS)/MacOS/imageformats
	rm -f \$(CONTENTS)/COPYING
	rm -f \$(CONTENTS)/Resources/qt.conf

dmg: \$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION).dmg

dmg-clean: bundle-clean
	rm -f \$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION).dmg

\$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION).dmg: bundle
	hdiutil create -srcfolder '\$(DESTDIR)\$(QMAKE_TARGET).app' -format UDZO -imagekey zlib-level=9 -scrub '\$\@'

\$(CONTENTS)/Resources/qt.conf:
	echo '[Paths]' > /tmp/qt.conf
	echo 'plugins = MacOS' >> /tmp/qt.conf
	mv /tmp/qt.conf \$(CONTENTS)/Resources

\$(CONTENTS)/COPYING:
	cp $root/COPYING \$(CONTENTS)/COPYING

END


QtFrameworks();
imagePlugins();
dylibs();


sub imagePlugins 
{
	my $from = abs_path( "$QT_FRAMEWORKS_DIR/../plugins/imageformats" );
	my $to = "\$(CONTENTS)/MacOS/imageformats";
	my $install = "\$(INSTALLDIR)/Contents/MacOS/imageformats";

print <<END;
$to:
	mkdir -p \$\@

$install:
	mkdir -p \$\@

END

	opendir( DIR, $from );
	foreach my $name (grep( /\.dylib$/, readdir( DIR ) ))
	{
		print <<END;
$to/$name: $from/$name |$to
	cp $from/$name \$\@
	$DEPOSX \$\@ $QT_FRAMEWORKS_DIR

$install/$name: $to/$name |$install
	cp $to/$name \$\@

END
		$bundle_deps .= " $to/$name";
		$install_deps .= " $install/$name";
	}
	closedir( DIR );
}


sub QtFrameworks
{
	my $to = "\$(CONTENTS)/Frameworks";
	my $install = "\$(INSTALLDIR)/Contents/Frameworks";
	
	print <<END;
$to:
	mkdir -p \$\@

$install:
	mkdir -p \$\@
END
	foreach my $module (@QT_MODULES)
	{
		print <<END;

$to/$module.framework: $QT_FRAMEWORKS_DIR/$module.framework |$to
	cp -Rf $QT_FRAMEWORKS_DIR/$module.framework \$\@
	rm \$\@/Versions/4/${module}_debug \$\@/${module}_debug*
	$DEPOSX \$\@/$module $QT_FRAMEWORKS_DIR
	install_name_tool -id \$\@/Versions/4/$module \$\@/Versions/4/$module

$install/$module.framework: $to/$module.framework
	cp -Rf $to/$module.framework $install
END
		$install_deps .= " $install/$module.framework";
	}
}

sub dylibs
{
	my $to = "\$(CONTENTS)/MacOS";
	my $install = "\$(INSTALLDIR)/Contents/MacOS";
	
	foreach my $dylib (@DYLIBS)
	{
		print <<END;

$to/$dylib: \$(DESTDIR)$dylib
	cp \$(DESTDIR)$dylib \$\@
	$DEPOSX \$\@ $QT_FRAMEWORKS_DIR

$install/$dylib: $to/$dylib |$install
	cp $to/$dylib \$\@
END
		$install_deps .= " $install/$dylib";
	}	
}

print <<END;

\$(INSTALLDIR)/Contents:
	mkdir -p \$\@

\$(INSTALLDIR)/Contents/MacOS:
	mkdir -p \$\@
	
\$(INSTALLDIR)/Contents/MacOS/\$(QMAKE_TARGET): |\$(INSTALLDIR)/Contents/MacOS
	cp \$(TARGET) \$(INSTALLDIR)/Contents/MacOS

\$(INSTALLDIR)/Contents/Info.plist: |\$(INSTALLDIR)/Contents
	cp \$(CONTENTS)/Info.plist \$(INSTALLDIR)/Contents

\$(INSTALLDIR)/Contents/Resources/qt.conf: \$(INSTALLDIR)/Contents/Resources
	cp \$(CONTENTS)/Resources/qt.conf \$(INSTALLDIR)/Contents/Resources

\$(INSTALLDIR)/Contents/Resources: |\$(INSTALLDIR)/Contents
	cp -r \$(CONTENTS)/Resources \$(INSTALLDIR)/Contents

bundle-install: bundle \$(INSTALLDIR)/Contents/MacOS/\$(QMAKE_TARGET) \$(INSTALLDIR)/Contents/Info.plist $install_deps \$(INSTALLDIR)/Contents/Resources/qt.conf

bundle: all \$(BUNDLE_FRAMEWORKS) \$(BUNDLE_MACOS) \$(CONTENTS)/COPYING \$(DESTDIR)mxcl-is-super \$(CONTENTS)/Resources/qt.conf $bundle_deps

END
