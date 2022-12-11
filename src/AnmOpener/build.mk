#!/bin/bash

baseDir=$(pwd)
if [ $# -gt 0 ]; then
	baseDir=$1
fi

currentDir=.
if [ $# -gt 1 ]; then
    currentDir=$2
fi

files=$currentDir/*.cpp

for file in ${files}
do
    g++ -c $file -o $baseDir/Linux/obj/$(basename $file).o -I$baseDir -ggdb -g3 &
	#echo $file
	#echo $baseDir/Linux/obj/$(basename $file).o
done

dirs=$(find $currentDir -maxdepth 1 -mindepth 1 -type d)

for d in ${dirs}; do 
    buldFile=$d/build.mk
    if [ -f "$buldFile" ]; then
        #echo $d
        #chmod +x $buldFile
        sh $buldFile $baseDir $d &
    fi
done

wait
