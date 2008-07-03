#!/bin/sh
#
# Puts Last.fm.app into a pretty DMG file
# =======================================

# Vars to change
# --------------
#
# Name of our volume
VOL_NAME="Fingerprinter"
# Where to grab the .app bundle?
APP_PATH="bin/Fingerprinter.app"
# Which template to use?
ORIGINAL_DMG="tools/Fingerprinter.Original.dmg"
# How would you like to name the output file?
FINAL_DMG="bin/Last.fm-Fingerprinter.dmg"
# Where to find the Japanese background image?
JAPANESE_BACKGROUND="res/dmg_background_jp.png"

# Other vars - consider before change
# -----------------------------------
VOL_PATH="/Volumes/$VOL_NAME"
DST_PATH="$VOL_PATH/Last.fm Fingerprinter.app"
TMP_ORIGINAL_DMG="tools/tmp.dmg"

# And finally the code...
# -----------------------
rm -rf "$TMP_ORIGINAL_DMG";

echo "Converting template image to r/w image..."
if hdiutil convert "$ORIGINAL_DMG" -format UDRW -o "$TMP_ORIGINAL_DMG" -quiet; then
	echo "Made template writable"
else
	echo "Couldn't convert template image $ORIGINAL_DMG"
	exit 1;
fi

echo "Attaching disk image..."
if hdiutil attach "$TMP_ORIGINAL_DMG" -noautoopen -quiet; then
	echo "Disk image attached";
else
	echo "Couldn't attach disk image at $TMP_ORIGINAL_DMG"
	exit 1;
fi

if rm -rf "$DST_PATH"; then
        rm -Rf "$DST_PATH/*"
        cp -Rp "$APP_PATH" "$DST_PATH"
        echo "Application replaced";
else
	echo "Error deleting old application"
	exit 1;
fi

if [ "$1" = "-j" ]; then
    if rm "$VOL_PATH/.background/background.png"; then
        if cp "$JAPANESE_BACKGROUND" "$VOL_PATH/.background/background.png"; then
            echo "Japanese background copied.";
        else
            echo "Error copying Japanese background image."
            exit 1;
        fi
    else
        echo "Error deleting international background image."
        exit 1;
    fi
fi

if rm -f "$FINAL_DMG"; then
	echo "Copying DMG...";
else
	echo "Couldn't remove old DMG"
	exit 1;
fi;

#if hdiutil convert "$TMP_ORIGINAL_DMG" -format UDZO -o "$FINAL_DMG" -imagekey zlib-level=6 -quiet; then
if hdiutil create -srcfolder "$VOL_PATH" -format UDZO -imagekey zlib-level=9 -scrub "$FINAL_DMG" -quiet; then
	echo "DMG copied";
else
	echo "Error copying DMG"
	exit 1;
fi

IMAGE_DEV=`hdiutil info | grep "$VOL_PATH" | awk '{print $1}'`

if hdiutil detach $IMAGE_DEV -quiet -force; then
	echo "Disk image detached";
else
	echo "Error detaching disk image"
	exit 1;
fi;

echo 'Final DMG size:' `du -hs "$FINAL_DMG" | awk '{print $1}'`

rm -f "$TMP_ORIGINAL_DMG"
