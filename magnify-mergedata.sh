#! /bin/bash

magnify_source="$(dirname $(readlink -f $BASH_SOURCE))"

if [ "$1" == "-h" ]; then
    echo ""
    echo "Usage: ./magnify-mergedata.sh /absolute/path/to/file/directory/ filename.root"
    echo "Please specify path & filename separately."
    echo "The merged file will be available at data/filename-v2.root"
    echo ""
    exit 0
fi

inpath="$1"
rootfile="$2"

cd $magnify_source

root -l -b -q scripts/MergeData.C+'("'"$rootfile"'", "'"$inpath"'", "data/", "recreate", "orig", "orig", "-v2.root")'
root -l -b -q scripts/MergeData.C+'("'"$rootfile"'", "'"$inpath"'", "data/", "update", "orig", "raw", "-v2.root")'
root -l -b -q scripts/MergeData.C+'("'"$rootfile"'", "'"$inpath"'", "data/", "update", "gauss", "decon", "-v2.root")'
root -l -b -q scripts/MergeData.C+'("'"$rootfile"'", "'"$inpath"'", "data/", "update", "threshold", "threshold", "-v2.root")'
# root -l -b -q scripts/MergeSim.C+'("'"$rootfile"'", "'"$inpath"'", "data/", "update", "threshold", "baseline", "-v2.root")'
