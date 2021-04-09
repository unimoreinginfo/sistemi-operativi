#!/bin/bash

# ./FCP.sh /path/to/folder/dogles 8

PATH=$PATH:$(pwd)
export PATH

if [[ ! "$#" -eq 2 ]]
    then
        echo "richiesti due parametri"
        exit 1
fi

path=$1
lines=$2

if [[ ! $lines =~ ^[0-9]+$ ]]
    then 
    echo "il secondo parametro non è numerico"
    exit 2
fi

if [[ ${path:0:1} != "/" && $lines -ge 0 ]]
    then
        echo "i parametri non rispettano i vincoli"
        exit 3
fi

FCR.sh $path $lines

for line in $(cat ~/.config/duglerio/togle)
    do
        echo soddiego ti spiego
        v=
        read v
        while [[ $v -gt $2 ]]
            do
                read v
        done
        
        echo -n "$line $v " >> ~/.config/duglerio/togetis

done

echo "dugle.c" $(cat ~/.config/duglerio/togetis)