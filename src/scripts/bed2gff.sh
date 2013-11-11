#!/bin/bash

# Combined bash/awk script to convert a set of BED files to GFF.
# Andras Aszodi, 2013-11-07

# -- Functions --

function print_help() {
    cat <<HELP
This script converts one or more BED files to GFF files.
Usage: bed2gff.sh [options] file1.bed [file2.bed ...]
Options:
-c Apply BED->GFF coordinate transformation (default: no change)
-o Overwrite output file(s) if they exist (default: skip)
-h Print this help and exit
Each dir/path/file.bed will be saved to file.gff in the current directory
HELP
}

# == MAIN ==

# Process command-line options
COORDTRANS=0        # -c default
OVERWRITE="no"      # -o default
while getopts coh OPTION; do
    case $OPTION in
        c) echo "BED->GFF coordinate transform will be applied." ; COORDTRANS=1 ;;
        o) OVERWRITE="yes" ;;
        h) print_help ; exit 1 ;;
        *) echo "Unrecognised option -$OPTION"; print_help ; exit 2 ;;
    esac
done
shift `expr $OPTIND - 1`

if [ $# -lt 1 ]; then
    echo "You must specify at least one input BED file."
    print_help
    exit 1
fi

# process all input files
# if `in xxx` is omitted, in $@ is assumed
for BED; do
    # check input
    if [ ! -e $BED ]; then
        echo "Warning: cannot access input file $BED"
        continue
    fi
    
    # automatic output filename generation
    BASENM=`basename $BED`   # get rid of directory path
    GFF=${BASENM%.*}.gff    # remove shortest match from end, append .gff
    
    if [ -e $GFF ]; then
        if [ $OVERWRITE = "no" ]; then
            echo "Warning: $GFF exists, skipped"
            continue
        else
            echo "Warning: $GFF will be overwritten"
        fi
    fi
    
    # GFF header
    echo "##gff_version 2" > $GFF
    echo "##date `date +%Y-%m-%d`" >> $GFF
    
    # records
    # The value of CT is added to the BED start coordinate
    # if it is 1, then the input is assumed to have the BED coordinate system
    # and is to be transformed to the GFF coordinate system
    awk -v CT=$COORDTRANS 'BEGIN { FS = "\t" ; OFS = "\t" }
        /^#/ { print $0 }
        /^[^#]/ { if (NF < 3 ) print "# Too few fields", $0
        else print $1, "bed2gff", $4, $2+CT, $3, $5, $6, ".", "." }' $BED >> $GFF
    echo "$BED --> $GFF done"
done

