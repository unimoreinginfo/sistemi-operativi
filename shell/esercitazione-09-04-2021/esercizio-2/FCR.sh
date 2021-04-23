#!/bin/bash
cd $1

echo "== cartella corrente: $1"

found=

for f in *
    do
        if [[ -d $f && -x $f ]]
            then 
            echo "> trovata cartella attraversabile $f"
            FCR.sh $(pwd)/$f $2
        
        elif [[ -f $f && -r $f ]]
            then 
            lines=$(wc -l < $f)
            if [[ $lines -eq $2 ]]
                then 
                echo "il file $f ha le linee richieste"
                echo "nella directory: " $(pwd)
                echo "$found $(pwd)/$f" >> ~/.config/duglerio/togle
                
            fi

        fi
done 

exit 0