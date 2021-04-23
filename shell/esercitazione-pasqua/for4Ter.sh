#!/bin/bash

function is_simple(){
    if [[ ! "$1" =~ \/ ]] # $1 rappresenta il primo parametro passato alla funzione, $2 il secondo ...
        then true # equivalente di scrivere return 0, però meno confusionario, poiché sembra che ritornando falso l'if sia considerato vero a linea 24
    else
        false
    fi
}

if [ "$#" -ne 1 ]
    then
    echo "serve esattamente un singolo parametro"
    exit 1
fi

if ! is_simple $1
    then
    echo "nome file non semplice"
    exit 1
fi


c=0
for i in `cat $1`
    do
    c=$(( c + 1 ))
    if [ $(( c % 2 )) -eq 1 ]
        then
            echo elemento dispari, quindi nome di file $i
                if [ -f $i ]
                    then cat $i
                fi
        else 
            echo ""
            echo elemento pari, quindi lunghezza in linee del file appena visualizzato = $i
    fi
done