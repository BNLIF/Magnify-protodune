#! /bin/bash

magnify_source="$(dirname $(readlink -f $BASH_SOURCE))"

# no pramater specified, or given an "-h" option
if [ "$#" -lt 1 ] || [ "$1" == "-h" ]; then
    echo ""
    echo "NAME"
    echo "      display.sh - wrapper for raw/denoised/deconvolved waveform display"
    echo ""
    echo "SYNOPSIS"
    echo "      ./display.sh magnify.root channel_number"
    echo ""
    # echo "DESCRIPTION"
    # echo "      Please specify the subregion for chn [x1,x2], tick [y1,y2], rebin in chn & tick [nx, ny]"
    echo ""
    exit 0
fi

rootfile="$1"
rootfile=$(readlink -f "$rootfile") # complete the fullpath
ch="${2:-0}"

echo "chn: " $ch

# input, output, x1, x2, y1, y2, x_rebin, y_rebin
# root -l display.C'("'"$rootfile"'",'$ch')'
root -l buttonTest.C'("'"$rootfile"'",'$ch')'

