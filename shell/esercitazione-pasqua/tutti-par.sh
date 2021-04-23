#!/bin/bash

if [ "$#" -ge 1 ]
    then echo $@
else 
    echo "specifica almeno un parametro"
    exit 1
fi