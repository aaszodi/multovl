#!/usr/bin/awk -f

# Simple script to extract an ancestor track from MULTOVL's GFF output.
# 2010-10-28 Andras Aszodi
BEGIN {
    if (ARGC < 2 || trackid !~ /^[1-9][0-9]*$/ ) {
        print "Usage: anctrack.awk -v trackid=X multovl.gff"
        print "where 'X' is the ancestor track index number 1,2,...,"
        print "and 'multovl.gff' is a MULTOVL output file in GFF format"
        exit
    } else {
        idrex = "^(" trackid "|[1-9][0-9]*\\*" trackid "):"
        print "# Ancestor track " trackid " of " ARGV[1]
    }
}

# non-comment lines
/^[^#]/ {
    chrom = $1
    split($NF, ancs, "|")
    for (idx in ancs) {
        anc = ancs[idx]
        if (anc ~ idrex) {
            split(anc, ancparts, ":")
            regname = ancparts[2]
            strand = ancparts[3]
            split(ancparts[4], coords, "-")
            printf "%s\t%s\t%s\t%s\t0\t%s\n", 
                chrom, coords[1], coords[2], regname, strand
            break
        }
    }
}
