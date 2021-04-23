#!/bin/bash

if [ "$#" -ne 1 ]
    then
        echo "necessario esattamente un parametro"
        exit 1  
fi

first=${1:0:1} # prendo il primo carattere del parametro 1 ${nome_variabile:posizione:lunghezza_slice}

if [ $first = "/" ]
    then echo "path assoluto"
elif [ $first = "." ] || [ $first != "/" ]
    then if [[ "$1" =~ \/ ]] # uso [[ ]] per usare il testing esteso 
        then echo "path relativo"
    else
        echo "path relativo semplice"
    fi
fi

# ho assunto che il path relativo semplice sia un file indicato relativamente (quindi una stringa senza nessuno slash al suo interno)