#!/usr/bin/env bash
set -u
umask 0022


#
# args
#
src="$1"



#
# Validation
#
if [ -z "$src" ]; then
    echo "error: specify icon file" 1>&2
    exit 1
fi

if [ -f "$src" ]; then
    : # OK
else
    echo "error: file not found: $src" 1>&2
    exit 1
fi



#
# Make icon file
#
readonly tmp=$(mktemp -d "./tmp-XXXXXX")
convert $src -resize  16x16     $tmp/icon-16.png
convert $src -resize  32x32     $tmp/icon-32.png
convert $src -resize  64x64     $tmp/icon-64.png
convert $src -resize 128x128    $tmp/icon-128.png
convert $src -resize 256x256    $tmp/icon-256.png
convert $tmp/*.png icon.ico
rm -r $tmp

