#!/bin/bash

set -e

git fetch --unshallow && git fetch --tags
if [ z"$TRAVIS_BRANCH" != z ]; then
	TRAVIS_COMMIT_ACTUAL=$(git log --pretty=format:'%H' -n 1)
	echo "Fixing detached head (current $TRAVIS_COMMIT_ACTUAL -> $TRAVIS_COMMIT)"
	echo "---------------------------------------------"
	git log -n 5 --graph
	echo "---------------------------------------------"
	git fetch origin $TRAVIS_COMMIT
	git branch -v
	echo "---------------------------------------------"
	git log -n 5 --graph
	echo "---------------------------------------------"
	git branch -D $TRAVIS_BRANCH || true
	git checkout $TRAVIS_COMMIT -b $TRAVIS_BRANCH
	git branch -v
fi
