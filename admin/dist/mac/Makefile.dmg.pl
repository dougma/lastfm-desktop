#!/usr/bin/perl
# Author: max@last.fm
# Usage: export QT from your QMake project
# Usage: export QMAKE_LIBDIR_QT from your QMake project
# Usage: Makefile.dmg.pl $$DESTDIR $$VERSION $$LIBS
# You must include this from within an app target qmake pro file for it to work!
# the above usage refers to the QMAKE variables

use Cwd 'abs_path';
use File::Basename;

##############################################################################
open FILE, '_build_parameters.pl.h' or die $!;
while ($line = <FILE>) {
    $str .= $line;
}
eval $str;
close FILE;
##############################################################################


while( my $v = shift )
{
	if ($v =~ m/^-l(.*)/)
	{
		push( @DYLIBS, "lib$1.1.dylib" );
	}
}

$plist = "\$(CONTENTS)/Info.plist";
$root = abs_path( dirname( $0 ) . "/../../../" );

sub getQtModules()
{
	# these 4 lines de-dupe $QT
	my %saw;
	my @in = split( ' ', $QT );
	@saw{@in} = ();
	my @out = keys %saw;

	foreach my $x (@out)
	{
	    $x = 'Qt'.ucfirst( $x ) unless $x eq "phonon";
		$bundle_frameworks .= "\$(CONTENTS)/Frameworks/$x.framework ";
	    push( @QT_MODULES, $x );
	}
}

getQtModules();

foreach my $x (@DYLIBS)
{
	$bundle_macos .= "\$(CONTENTS)/MacOS/$x ";
}

$DEPOSX = "\$(DIST_TOOLS_DIR)/deposx.sh";
$dmg = "\$(DESTDIR)\$(QMAKE_TARGET)-\$(VERSION)-\$(REVISION).dmg";

print <<END;

DIST_TOOLS_DIR = $root/admin/dist/mac
BUNDLE = \$(DESTDIR)\$(QMAKE_TARGET).app
CONTENTS = \$(BUNDLE)/Contents
VERSION = $VERSION
REVISION = $REVISION
BUNDLE_FRAMEWORKS = $bundle_frameworks
BUNDLE_MACOS = $bundle_macos
BUNDLE_RESOURCES = \$(CONTENTS)/Resources
BUNDLE_PLUGINS = \$(CONTENTS)/MacOS/plugins
.PHONY = bundle bundle-extra dmg

thirst: first

\$(OBJECTS_DIR)Makefile.dmg.dummy: \$(TARGET) $plist
	perl -pi -e 's/@VERSION@/'\$(VERSION)'/g' $plist
	perl -pi -e 's/@SHORT_VERSION@/'`echo \$(VERSION) | cut -d'.' -f1,2,3`'/g' $plist
	$DEPOSX \$(TARGET) $QT_FRAMEWORKS_DIR
	touch \$\@

dmg: $dmg

$dmg: bundle
	rm -f '\$\@'
	hdiutil create -srcfolder '\$(DESTDIR)\$(QMAKE_TARGET).app' -format UDZO -imagekey zlib-level=9 -scrub '\$\@'

\$(CONTENTS)/Resources/qt.conf:
	echo '[Paths]' > /tmp/qt.conf
	echo 'plugins = MacOS' >> /tmp/qt.conf
	mv /tmp/qt.conf \$(CONTENTS)/Resources

\$(CONTENTS)/COPYING:
	cp $root/COPYING \$(CONTENTS)/COPYING

END


QtFrameworks();
plugins( "imageformats" );
plugins( "phonon_backend" );
plugins( "sqldrivers" );
dylibs();


sub plugins
{
	my $d = shift;
	
	my $from = abs_path( "$QT_FRAMEWORKS_DIR/../plugins/$d" );
	my $to = "\$(CONTENTS)/MacOS/$d";

print <<END;
$to:
	mkdir -p \$\@

END

	opendir( DIR, $from );
	foreach my $name (grep( /\.dylib$/, readdir( DIR ) ))
	{
		next if ($name =~ /_debug\.dylib$/);

		print <<END;
$to/$name: $from/$name |$to
	cp $from/$name \$\@
	$DEPOSX \$\@ $QT_FRAMEWORKS_DIR
END
		$bundle_deps .= " $to/$name";
	}
	closedir( DIR );
}


sub QtFrameworks
{
	my $to = "\$(CONTENTS)/Frameworks";
	
	print <<END;
$to:
	mkdir -p \$\@
END
	foreach my $module (@QT_MODULES)
	{
		print <<END;

$to/$module.framework: $QT_FRAMEWORKS_DIR/$module.framework |$to
	cp -Rf $QT_FRAMEWORKS_DIR/$module.framework \$\@
	rm -f \$\@/Versions/4/${module}_debug \$\@/${module}_debug*
	$DEPOSX \$\@/$module $QT_FRAMEWORKS_DIR
	install_name_tool -id \$\@/Versions/4/$module \$\@/Versions/4/$module

END
	}
}

sub dylibs
{
	foreach my $dylib (@DYLIBS)
	{
		print <<END;

\$(CONTENTS)/MacOS/$dylib: \$(DESTDIR)$dylib
	cp \$(DESTDIR)$dylib \$\@
	$DEPOSX \$\@ $QT_FRAMEWORKS_DIR
END
	}	
}

print <<END

bundle: all \$(BUNDLE_FRAMEWORKS) \$(BUNDLE_MACOS) \$(CONTENTS)/COPYING \$(CONTENTS)/Resources/qt.conf \$(OBJECTS_DIR)Makefile.dmg.dummy $bundle_deps
END
