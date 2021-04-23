#!/bin/bash

if [ "$#" -gt 2 ] || [ "$#" -lt 1 ]
    then
        echo "necessari uno o due parametri"
        exit 1  
fi

# caso unico parametro
if [ "$#" -eq 1 ] 
    then
        echo "modalità singolo parametro (append to file)"
    cat /dev/stdin >> $1

fi

# caso due parametri
if [ "$#" -eq 2 ] 
    then if [[ ! -f $1 ]] 
        then
            echo "il primo file deve necessariamente esistere"
            exit 0
    fi
    
    cat $1 1>> $2

 fi
    