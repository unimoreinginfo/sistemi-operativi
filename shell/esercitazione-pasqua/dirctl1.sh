#!/bin/bash

if [ "$#" -ne 1 ]
    then
        echo "necessario esattamente un parametro"
        exit 1        
fi

echo sono dir.sh
echo '$0 ==>' $0
echo '$1 ==>' $1

if [ -d "$1" ]
    then 
        ls -ld $1
    else
        ls -l $1
fi

exit 0