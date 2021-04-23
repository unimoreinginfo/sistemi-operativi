#!/bin/bash

# utilizzo bash perché sì

rm ~/.tmp/count2
touch ~/.tmp/count2

PATH=$PATH:$(pwd)
export PATH

dir=
str=

if [[ "$#" -lt 2 ]]
    then
        echo "non ci sono abbastanza parametri"
        exit 1
fi

dir=${@:1:$# -1}
str=${@:$#}

for d in $dir
    do
        if [[ ${d:0:1} != "/" ]]
            then echo "tutti i parametri devono essere directory assolute"
            exit 3
        fi
done

for d in $dir
    do

        FCR.sh $d $str

done

while read line
    do
        filename=${line:2}
        echo $filename
        read -u 1 -p "vuoi riordinare e mostrare il contenuto di $filename a schermo (y/n)?" ans
        
        case $ans in
        y)

            sort -d < $filename

        ;;

        *) echo "ok, salto il file"
        ;;

        esac

done < ~/.tmp/count2