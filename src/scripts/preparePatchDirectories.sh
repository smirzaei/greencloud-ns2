#!/bin/sh

echo setting working variables...
rootDir=$(cd $(dirname "$0"); pwd)
workingDir=$rootDir/../ns2-patching
tempDir=$workingDir.tmp
unPatchedDir=$workingDir.original
upstream=$rootDir/../../upstream

echo creating a temp working dir...
rm -rf $tempDir
mkdir -p $tempDir

echo unzipping the raw content...
cd $tempDir
tar -zxf $upstream/ns-*gz

echo copying the original version into $unPatchedDir...
unzipTarget=$tempDir/ns-*
rsync -a $unzipTarget/ $unPatchedDir/

echo patching...
cd $unzipTarget
patch -p0 < $upstream/NS2-GreenCloud.diff

echo copying the patched version into $workingDir...
cd ..
rsync -a $unzipTarget/ $workingDir/

echo cleaning up...
rm -rf $tempDir

echo done.
