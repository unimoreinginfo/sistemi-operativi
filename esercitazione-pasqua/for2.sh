#!/bin/bash

for i in *
    do 
        first=${i:0:1}
        if [ $first == "p" ] 
        then if [[ -f $i ]] 
                then echo Contenuto di $i:
                cat $i
                echo "---"
            fi
        fi
done