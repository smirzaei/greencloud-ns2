#!/bin/sh

echo setting working variables...
rootDir=$(cd $(dirname "$0"); pwd)
workingDir=$rootDir/../ns2-patching
unPatchedDir=$workingDir.original
upstream=$rootDir/../../upstream

echo generating diff...
cd $unPatchedDir
diff -ruN ./ $workingDir/ > $upstream/NS2-GreenCloud.diff

echo cleaning up...
cd $rootDir
rm -rf $workingDir
rm -rf $unPatchedDir
