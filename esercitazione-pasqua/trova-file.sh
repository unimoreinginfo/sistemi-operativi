#!/bin/bash

if [ "$#" -ne 1 ]
    then
        echo "necessario esattamente un parametro"
        exit 1  
fi

if [[ "$1" =~ \/ ]] # uso [[ ]] per usare il testing esteso 
        then
            echo "il path indicato non è relativo semplice"
            exit 1
    fi

if [[ -f ./$1 ]]
    then echo file \"$1\" trovato
else
    echo file \"$1\" non trovato
fi