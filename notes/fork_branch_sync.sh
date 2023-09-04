#!/bin/bash
#fork sync

export BRANCH=$1
echo "Branch - $BRANCH"
echo "User - $USER"
export USER=vijayr

git clone git@github.com:$USER-arrcus/arrcus_sw.git
cd arrcus_sw/
git remote add Arrcus/arrcus_sw git@github.com:Arrcus/arrcus_sw.git
git fetch Arrcus/arrcus_sw
git checkout -b $BRANCH Arrcus/arrcus_sw/$BRANCH
git push origin $BRANCH
cd ../
rm -rf arrcus_sw
git clone git@github.com:$USER-arrcus/arrcus_sw.git -b $BRANCH
