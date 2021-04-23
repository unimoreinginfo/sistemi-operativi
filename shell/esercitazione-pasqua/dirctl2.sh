#!/bin/bash
echo sono dir.sh

if [ "$#" -gt 1 ]
    then
        echo "necessario esattamente un parametro"
        exit 1
    elif [ "$#" -eq 0 ]
        then
            ls -la | less    
fi


echo '$0 ==>' $0
echo '$1 ==>' $1

if [ -d "$1" ]
    then 
        echo $(ls -ld $1)
    else
        echo $(ls -l $1)
fi

exit 0