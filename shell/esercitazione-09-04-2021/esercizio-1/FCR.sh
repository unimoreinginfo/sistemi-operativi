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
                grepped=$(cat $f | grep -c [0-9])
                if [[ $grepped -eq $lines ]]
                    then
                    echo "nella directory: " $(pwd)
                    echo "trovato" $f
                    found="$found $f"
                    
                fi

            fi

        fi
done 

v=
echo "vuoi avviare la parte in C?"
read v

case $v in 
    y ) echo "file.c" $found
    ;;
    * ) echo "ok, ciao" 
    ;;
esac

echo 
echo
echo
exit 0