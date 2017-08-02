#!/bin/bash

set -e

# The VID, PID and DID are defined in the Makefile without the preceeding '0x'
VID=0x$1
PID=0x$2
DID=0x$3

git fetch --unshallow && git fetch --tags
if [ z"$TRAVIS_BRANCH" != z ]; then
	echo "Fixing detached head"
	git branch -v
	git branch -D $TRAVIS_BRANCH || true
	git checkout $TRAVIS_COMMIT -b $TRAVIS_BRANCH
	git branch -v
fi

# These must be outside the heredoc below otherwise the script won't error.
COMMIT="$(git log --abbrev-commit --format="%h" -n 1)"
BRANCH="$(git symbolic-ref --short HEAD)"

TMPFILE_H=$(tempfile -s .h | mktemp --suffix=.h)
TMPFILE_C=$(tempfile -s .c | mktemp --suffix=.c)

cat > $TMPFILE_H <<EOF
// This is an auto-generated file. Please modify common/version_data.sh instead
#ifndef __VERSION_DATA_H
#define __VERSION_DATA_H

extern const long git_commit;
extern const char* git_branch;
extern const char* git_status;

#define VID $VID
#define PID $PID
#define DID $DID

#endif  // __VERSION_DATA_H
EOF

cat > $TMPFILE_C <<EOF
// This is an auto-generated file. Please modify common/version_data.sh instead
#include "version_data.h"
const long git_commit = 0x$COMMIT;
const char* git_branch = "$BRANCH";
const char* git_status =
    "    --\r\n"
$(git status --short | sed -e's-^-   "    -' -e's-$-\\r\\n"-')
    "    --\r\n";
EOF

if ! cmp -s $TMPFILE_H version_data.h; then
	echo "Updating version_data.h"
	mv $TMPFILE_H version_data.h
fi

if ! cmp -s $TMPFILE_C version_data.c; then
	echo "Updating version_data.c"
	mv $TMPFILE_C version_data.c
fi
