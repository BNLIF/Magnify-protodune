#!/bin/bash

magnify_source="$(dirname $(readlink -f $BASH_SOURCE))"

rootfile="$1" ; shift
if [[ "$rootfile" =~ :// ]] ; then
    echo "Loading URL $rootfile"
else
    rootfile=$(readlink -f "$rootfile")
fi
threshold="${1:-600}"
frame="${2:-decon}"
startdir=$(pwd)
rebin="${3:-4}"

#echo "Loading frame \"$frame\" rebin \"$rebin\""

cd $magnify_source/scripts

#echo $rootfile
#echo $frame
#echo $rebin

suffix="-v2.root"
root -l -b -q MergeAPA.C'("'"$rootfile"'", "'"$suffix"'")'
newrootfile="${rootfile/.root/$suffix}"
if [ -f $newrootfile ]
then
    rootfile=$newrootfile
fi
echo $rootfile

root -l loadClasses.C Magnify.C'("'"$rootfile"'", '$threshold', "'"$frame"'", '$rebin')'
