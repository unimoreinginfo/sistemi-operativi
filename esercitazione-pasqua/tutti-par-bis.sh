#!/bin/bash

if [ "$#" -ge 1 ]
    then
    a=0
    for i in $@
        do
            echo "$i - " $(( a += 1))
    done
    
else 
    echo "specifica almeno un parametro"
    exit 1
fi