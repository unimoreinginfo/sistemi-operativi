#!/bin/sh
cd $1
if [ -d $2 ]
    then echo la directory $2 si trova in `pwd`
fi
for i in *
    do
        if [ -d $i -a -x $i ]
            then
                echo Stiamo per esplorare la directory `pwd`/$i
                cercafile.sh `pwd`/$i $2
        fi
done