#! /bin/bash

magnify_source="$(dirname $(readlink -f $BASH_SOURCE))"

# no pramater specified, or given an "-h" option
if [ "$#" -lt 1 ] || [ "$1" == "-h" ]; then
    echo ""
    echo "NAME"
    echo "      channelscan.sh - wrapper for channel looper"
    echo ""
    echo "SYNOPSIS"
    echo "      ./channelscan.sh magnify.root"
    echo ""
    echo "DESCRIPTION"
    echo "      The user-defined script for operation per channel is in test/userdef.h"
    echo "      The channel list can be defined in"
    echo "        1) default  - a predefined bad channel tree in magnify file (e.g., T_bad)"
    echo "        2) optional - a text file"
    echo "      To specify them, try these examples:"
    echo "          ./channelscan.sh magnify.root tree:T_bad"
    echo "          ./channelscan.sh magnify.root /path/to/channels.txt"
    echo ""
    exit 0
fi

rootfile="$1"
rootfile=$(readlink -f "$rootfile") # complete the fullpath
channels="tree:T_bad"

if [ "$#" -eq 2 ]; then
	channels=$(readlink -f "$2") # complete the fullpath
fi


# cd $magnify_source/test_feature

root -l -b -q channelscan.C+'("'"$rootfile"'", "'"$channels"'")'
