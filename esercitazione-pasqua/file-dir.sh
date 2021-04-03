#!/bin/bash

for item in $1*
    do 
    if [[ -f $item ]] 
        then echo "[F] $item"
    elif [[ -d $item ]]
        then echo "[D] $item"
    fi
done