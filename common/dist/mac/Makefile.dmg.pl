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
$bundle_frameworks_dir = "\$(CONTENTS)/Frameworks";
$bundle_macos_dir = "\$(CONTENTS)/MacOS";
$plist = "\$(CONTENTS)/Info.plist";
$root = abs_path( dirname( $0 ) . "/../../../" );

foreach my $x (@QT_MODULES)
{
	$bundle_frameworks .= "$bundle_frameworks_dir/$x.framework ";
}
foreach my $x (@DYLIBS)
{
	$bundle_macos .= "$bundle_macos_dir/$x ";
}

$out = <<END;
DIST_TOOLS_DIR = $root/common/dist/mac
BUNDLE = \$(DESTDIR)\$(QMAKE_TARGET).app
CONTENTS = \$(BUNDLE)/Contents
VERSION = $VERSION
BUNDLE_FRAMEWORKS = $bundle_frameworks
BUNDLE_MACOS = $bundle_macos

.PHONY = dmg
.PHONY = dmg-clean

everything: all

bundle: all \$(BUNDLE_FRAMEWORKS) \$(BUNDLE_MACOS)
	perl -pi -e 's/@VERSION@/'\$(VERSION)'/g' $plist
	perl -pi -e 's/@SHORT_VERSION@/'`echo \$(VERSION) | cut -d'.' -f1,2,3`'/g' $plist
	\$(DIST_TOOLS_DIR)/deposx.sh \$(TARGET) $QT_FRAMEWORKS_DIR

bundle-clean:
	rm -rf \$(BUNDLE_FRAMEWORKS)
	rm -f \$(BUNDLE_MACOS)

bundle-install: bundle
	rm -rf /Applications/\$(QMAKE_TARGET).app
	cp -R \$(DESTDIR)\$(QMAKE_TARGET).app /Applications

\$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION).dmg:
	cd \$(DESTDIR) && \$(DIST_TOOLS_DIR)/gimme_dmg.sh \$(QMAKE_TARGET).app \$(VERSION)

dmg: bundle \$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION).dmg
	
dmg-clean: bundle-clean
	rm -f \$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION).dmg

\$(CONTENTS)/MacOS/imageformats: $QT_FRAMEWORKS_DIR/../plugins/imageformats
	cp -R $QT_FRAMEWORKS_DIR/../plugins/imageformats $bundle_macos_dir

\$(CONTENTS)/COPYING:
	cp $root/COPYING \$(CONTENTS)/COPYING

END

print $out;

foreach my $x (@QT_MODULES)
{
	print "\$(CONTENTS)/Frameworks/$x.framework: $QT_FRAMEWORKS_DIR/$x.framework\n";
	print "\t" . "rm -rf $bundle_frameworks_dir/$x.framework\n";
	print "\t" . "mkdir -p $bundle_frameworks_dir\n";
	print "\t" . "cp -R $QT_FRAMEWORKS_DIR/$x.framework $bundle_frameworks_dir\n";
	print "\t" . "rm $bundle_frameworks_dir/$x.framework/Versions/4/${x}_debug\n";
	print "\t" . "rm $bundle_frameworks_dir/$x.framework/${x}_debug*\n";
	print "\t" . "\$(DIST_TOOLS_DIR)/deposx.sh $bundle_frameworks_dir/$x.framework/$x $QT_FRAMEWORKS_DIR\n";
	my $y = "$x.framework/Versions/4/$x";
	print "\t" . "install_name_tool -id $y \$(CONTENTS)/Frameworks/$y\n";
}

foreach my $dylib (@DYLIBS)
{
	print "$bundle_macos_dir/$dylib: \$(DESTDIR)$dylib\n";
	print "\t" . "cp \$(DESTDIR)$dylib $bundle_macos_dir\n";
	print "\t" . "\$(DIST_TOOLS_DIR)/deposx.sh $bundle_macos_dir/$dylib $QT_FRAMEWORKS_DIR\n";
}
