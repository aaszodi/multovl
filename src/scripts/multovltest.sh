#!/bin/bash

# Simple stand-alone script for testing the MULTOVL executable.
# Takes a single command-line argument, the path to the MULTOVL executable.
# 2022-04-26 Andras Aszodi

if [ $# -lt 1 ]; then
	echo "Very simple test to check whether MULTOVL is working"
	echo "Usage: multovltest.sh <MULTOVL_executable_path>"
	exit 99
fi
MULTOVL=$1

# Create a temporary directory (with a name template, "XXXXXX" will be random)
TMPDIR=$(mktemp -d /tmp/multovl.XXXXXX)

# Write input files into the temporary directory
# (these are the same as `src/test/data/reg[ab].bed)
# The column separators are actual TAB characters!
cat > ${TMPDIR}/rega.bed << EOF
# Same regions as in multioverlaptest.cc	pairwise testing
chr1	100	200	REGa
chr1	210	220	REGa
chr1	222	222	REGa
chr1	224	230	REGa
chr1	250	300	REGa
chr1	333	333	REGa
chr1	340	340	REGa
chr1	350	450	REGa
EOF

cat > ${TMPDIR}/regb.bed << EOF
# Same regions as in multioverlaptest.cc	pairwise testing
chr1	50	150	REGb
chr1	223	223	REGb
chr1	250	300	REGb
chr1	340	340	REGb
chr1	400	410	REGb
chr1	415	415	REGb
chr1	420	430	REGb
chr1	440	500	REGb
# plus an intra-region overlap
chr1	490	510	REGb
EOF

# Run MULTOVL, keep only last 6 lines of the output, convert tabs to spaces
$MULTOVL ${TMPDIR}/rega.bed ${TMPDIR}/regb.bed | tail -6 | sed -e 's/\t/ /g' > ${TMPDIR}/regab.gff
RESULT=$(cat ${TMPDIR}/regab.gff)

# Clean up
rm -rf ${TMPDIR}

# Expected result
EXPECTED=$(cat << EOF
chr1 multovl overlap 340 340 2 . . ANCESTORS 1:REGa:.:340-340|2:REGb:.:340-340
chr1 multovl overlap 400 410 2 . . ANCESTORS 1:REGa:.:350-450|2:REGb:.:400-410
chr1 multovl overlap 415 415 2 . . ANCESTORS 1:REGa:.:350-450|2:REGb:.:415-415
chr1 multovl overlap 420 430 2 . . ANCESTORS 1:REGa:.:350-450|2:REGb:.:420-430
chr1 multovl overlap 440 450 2 . . ANCESTORS 1:REGa:.:350-450|2:REGb:.:440-500
chr1 multovl overlap 490 500 2 . . ANCESTORS 2:REGb:.:440-500|2:REGb:.:490-510
EOF
)
if [[ $EXPECTED = $RESULT ]]; then
	echo "Test passed"
	exit 0
else
	echo "Test failed"
	exit 1
fi

