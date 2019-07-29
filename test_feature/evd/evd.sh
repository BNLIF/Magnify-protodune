#! /bin/bash

magnify_source="$(dirname $(readlink -f $BASH_SOURCE))"

# no pramater specified, or given an "-h" option
if [ "$#" -lt 1 ] || [ "$1" == "-h" ]; then
    echo ""
    echo "NAME"
    echo "      evd.sh - wrapper for event display with python/matplotlib"
    echo ""
    echo "SYNOPSIS"
    echo "      ./evd.sh magnify.root x1 x2 y1 y2 nx ny"
    echo ""
    echo "DESCRIPTION"
    echo "      Please specify the subregion for chn [x1,x2], tick [y1,y2], rebin in chn & tick [nx, ny]"
    echo ""
    exit 0
fi

rootfile="$1"
rootfile=$(readlink -f "$rootfile") # complete the fullpath
outfile="output.root"
x1="${2:-0}"
x2="${3:-1000}"
y1="${4:-0}"
y2="${5:-1000}"
nx="${6:-1}"
ny="${7:-4}"

echo "chn: " $x1 $x2 
echo "tick: " $y1 $y2
echo "rebin x: " $nx
echo "rebin y: " $ny

cd $magnify_source/test_feature/evd

# input, output, x1, x2, y1, y2, x_rebin, y_rebin
root -l -q -b preselect.C'("'"$rootfile"'","'"$outfile"'",'$x1','$x2','$y1','$y2','$nx','$ny')'

# file, histogram, chn_offset, tick_offset, tick_rebin
python evd-subregion.py output.root orig  $x1 $y1 $ny
python evd-subregion.py output.root raw $x1 $y1 $ny
python evd-subregion.py output.root decon $x1 $y1 $ny
