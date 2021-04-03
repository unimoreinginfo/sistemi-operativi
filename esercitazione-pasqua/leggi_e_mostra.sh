#!/bin/bash

function print_and_show(){

    echo "vuoi visualizzare il file $1 (si/no)?" > /dev/tty 
    read var1
    if [ $var1 = si ]
        then ls -la $1; cat $1
    else echo Niente stampa di $1
    fi

}

if [ -z $1 ]
    then 
        echo Errore: almeno voglio un parametro
        exit 1
fi

for i in $@
    do
        if [[ ! -f $i || ! -r $i ]]
        then
            echo Errore: $i non è un file oppure non è leggibile
            continue
        fi
        print_and_show $i
done