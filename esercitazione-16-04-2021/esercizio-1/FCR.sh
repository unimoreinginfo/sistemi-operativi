#!/bin/bash

dir=$1
y=$2

cd $dir

for f in *
    do
        if [[ -d $f && -x $f ]]
            then FCR.sh $f $y
        elif [[ -f $f && -r $f ]]
            then if [[ $(wc -l < $f) == $y ]]
                then if [[ $y -lt 5 ]]
                    then
                    touch "$f.NOquinta"
                    echo $(pwd)/"$f.NOquinta" >> ~/.tmp/count
                else
                    line=$(echo $f | head -5 | tail -1)
                    echo $line > "$f.quinta"
                    echo $(pwd)/"$f.quinta" >> ~/.tmp/count
                fi
            fi
        
        fi
done