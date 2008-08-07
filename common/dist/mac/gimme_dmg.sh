#!/bin/sh
# author: max@last.fm
# brief:  Produces a compressed DMG from a bundle directory
# usage:  Pass the bundle directory as the only parameter
################################################################################

if [[ $# < 2 ]]
then
	echo "Usage: $0 [path] [version]"
	exit 1
fi

BUNDLE="$1"
VERSION="$2"

################################################################################


NAME=$(basename "$BUNDLE" | perl -pe 's/(.*).app/\1/')
IN="$BUNDLE"
TMP="/tmp/$NAME"
OUT="$NAME-$VERSION.dmg"
################################################################################


# clean up
rm -rf "$TMP"
rm -f "$OUT"

# create DMG contents and copy files
mkdir -p "$TMP/.background"
cp common/dist/mac/dmg_background.png "$TMP/.background/background.png"
cp common/dist/mac/DS_Store.in "$TMP/.DS_Store"
chmod go-rwx "$TMP/.DS_Store"
ln -s /Applications "$TMP/Applications"
# copies the prepared bundle into the dir that will become the DMG 
cp -R "$IN" "$TMP"

# create
hdiutil create -srcfolder "$TMP" \
               -format UDZO -imagekey zlib-level=9 \
               -scrub \
               "$OUT" \
               || die "Error creating DMG :("

# done !
echo 'DMG size:' `du -hs "$OUT" | awk '{print $1}'`
