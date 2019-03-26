#! /bin/bash

magnify_source="$(dirname $(readlink -f $BASH_SOURCE))"

# no pramater specified, or given an "-h" option
if [ "$#" -lt 1 ] || [ "$1" == "-h" ]; then
    echo ""
    echo "NAME"
    echo "      preprocess.sh - wrapper for merging magnify histograms from all APAs"
    echo ""
    echo "SYNOPSIS"
    echo "      ./preprocess.sh /path/and/input.root [/output/directory] [extension label]"
    echo ""
    echo "DESCRIPTION"
    echo "      The output directory and the extension label are optional."
    echo "      For example, if they were NOT specified explictly, the default"
    echo "      output path is data/, and extension label would be v2."
    echo "      To specify them, try this example:"
    echo "          ./preprocess.sh /data1/protodune.root /home/user/data/ v3"
    echo ""
    exit 0
fi

rootfile="$1"
rootfile=$(readlink -f "$rootfile") # complete the fullpath
outPath="$magnify_source/data"
ext="v2"

if [ "$#" -eq 2 ]; then
	outPath=$(readlink -f "$2") # complete the fullpath
elif [ "$#" -eq 3 ]; then
	outPath=$(readlink -f "$2")
	ext="$3"
fi

# echo $rootfile
# echo $outPath
# echo $ext

cd $magnify_source/scripts

root -l -b -q preprocess.C+'("'"$rootfile"'", "'"$outPath"'", "orig", "orig", "'"$ext"'", false, "recreate")'
root -l -b -q preprocess.C+'("'"$rootfile"'", "'"$outPath"'", "raw", "raw", "'"$ext"'", true)'
root -l -b -q preprocess.C+'("'"$rootfile"'", "'"$outPath"'", "gauss", "decon", "'"$ext"'")'
root -l -b -q preprocess.C+'("'"$rootfile"'", "'"$outPath"'", "threshold", "threshold", "'"$ext"'")'
root -l -b -q preprocess.C+'("'"$rootfile"'", "'"$outPath"'", "tree:T_ldg", "T_bad", "'"$ext"'")'
#root -l -b -q preprocess.C+'("'"$rootfile"'", "'"$outPath"'", "tree:T_stky", "", "'"$ext"'")'
