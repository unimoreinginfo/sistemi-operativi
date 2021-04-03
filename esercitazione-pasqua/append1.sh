#!/bin/bash

function is_simple(){
    if [[ ! "$1" =~ \/ ]] # $1 rappresenta il primo parametro passato alla funzione, $2 il secondo ...
        then true # equivalente di scrivere return 0, però meno confusionario, poiché sembra che ritornando falso l'if sia considerato vero a linea 24
    else
        false
    fi
}


if [ "$#" -ne 2 ]
    then
        echo "necessari esattamente due parametri"
        exit 1  
fi

if [ "$#" -eq 2 ] 
    then if [[ ! -f $1 || ! -r $1 ]] 
        then
            echo "il primo file deve necessariamente esistere ed essere leggibile!"
            exit 1
    fi
    
    if is_simple $1 && is_simple $2;
        then if [[ -f $2 ]]
            then if [[ -w $2 ]]
                then cat $1 1>> $2
            else 
                echo "il secondo file non è leggibile!"
                exit 1
            fi

            else
                echo "il secondo file non esiste, creazione e append di \"$2\""
                if [[ -r ./ ]]
                    then 
                    touch $2
                    cat $1 1>> $2 
                fi
            fi

        else 
            echo "entrambi i file non sono assoluti semplici"
            exit 1
    fi

fi
