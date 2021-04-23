#!/bin/bash

dir=$1
str=$2

echo $dir

cd $dir

for f in *
    do
        if [[ -d $f && -x $f ]]
            then FCR.sh $f $str

        elif [[ -f $f && -r $f && -w $f ]]
            then if [[ $f == "$str.txt" ]]
                then 
                
                obj="$(wc -m < $f) $(pwd)/$f"
                echo $obj >> ~/.tmp/count2          

            fi

        fi

done