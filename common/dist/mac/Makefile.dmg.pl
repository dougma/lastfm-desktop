#!/usr/bin/perl
# Usage: Makefile.dmg.pl $$DESTDIR $$VERSION $$QMAKE_LIBDIR_QT
# You must include this from within an app target qmake pro file for it to work!

@QT_MODULES = ("QtCore", "QtGui", "QtWebKit", "QtXml", "QtNetwork", "phonon");
@DYLIBS = split( ' ', `cd $ARGV[0] && ls *.1.dylib` );
$bundle_frameworks_dir = "\$(CONTENTS)/Frameworks";
$bundle_macos_dir = "\$(CONTENTS)/MacOS";
$plist = "\$(CONTENTS)/Info.plist";
$root = "$ARGV[0]/..";

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
VERSION = $ARGV[1]
BUNDLE_FRAMEWORKS = $bundle_frameworks
BUNDLE_MACOS = $bundle_macos

.PHONY = dmg
.PHONY = dmg-clean

dmg: \$(BUNDLE_FRAMEWORKS) \$(BUNDLE_MACOS) \$(TARGET)
	\$(DIST_TOOLS_DIR)/deposx.sh \$(TARGET)
	perl -pi -e 's/__VERSION/'\$(VERSION)'/g' $plist
	perl -pi -e 's/__SHORT_VERSION/'`echo \$(VERSION) | cut -d'.' -f1,2,3`'/g' $plist
	cd \$(DESTDIR) && \$(DIST_TOOLS_DIR)/gimme_dmg.sh \$(QMAKE_TARGET).app \$(VERSION)
	
dmg-clean:
	rm -rf \$(BUNDLE_FRAMEWORKS)
	rm -f \$(BUNDLE_MACOS)
	rm -f \$(QMAKE_TARGET)-\$(VERSION).dmg

$bundle_frameworks_dir:
	 mkdir -p $bundle_frameworks_dir

\$(CONTENTS)/MacOS/imageformats: $ARGV[2]/../plugins/imageformats
	cp -R $ARGV[2]/../plugins/imageformats $bundle_macos_dir

\$(CONTENTS)/COPYING:
	cp $root/COPYING \$(CONTENTS)/COPYING

END

print $out;

foreach my $x (@QT_MODULES)
{
	print "\$(CONTENTS)/Frameworks/$x.framework: $ARGV[2]/$x.framework $bundle_frameworks_dir\n";
	print "\t" . "cp -R $ARGV[2]/$x.framework $bundle_frameworks_dir\n";
	print "\t" . "rm $bundle_frameworks_dir/$x.framework/Versions/4/${x}_debug\n";
	print "\t" . "rm $bundle_frameworks_dir/$x.framework/${x}_debug*\n";
	print "\t" . "\$(DIST_TOOLS_DIR)/deposx.sh $bundle_frameworks_dir/$x.framework/$x\n";
	my $y = "$x.framework/Versions/4/$x";
	print "\t" . "cd \$(CONTENTS)/Frameworks && install_name_tool -id $ARGV[2]/$y $y\n";
}

foreach my $dylib (@DYLIBS)
{
	print "$bundle_macos_dir/$dylib: \$(DESTDIR)$dylib\n";
	print "\t" . "cp \$(DESTDIR)$dylib $bundle_macos_dir\n";
	print "\t" . "\$(DIST_TOOLS_DIR)/deposx.sh $bundle_macos_dir/$dylib\n";
}
