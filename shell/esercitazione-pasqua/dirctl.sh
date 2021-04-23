#!/bin/bash

if [ "$#" -ne 1 ]
    then
        echo "necessario esattamente un parametro"
        exit 1        
fi

echo sono dir.sh
echo '$0 ==>' $0
echo '$1 ==>' $1

ls -l $1