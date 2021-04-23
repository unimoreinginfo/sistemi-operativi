#!/bin/bash

# utilizzo bash perché sì

rm ~/.tmp/count2
touch ~/.tmp/count

PATH=$PATH:$(pwd)
export PATH

y=
dir=

if [[ "$#" -lt 2 ]]
    then
        echo "non ci sono abbastanza parametri"
        exit 1
fi

case $1 in
    0) echo "numero non positivo"
        ;;
    *[!0-9]*) echo "numero non positivo"
        ;;
    *)
        y=$(( $1 ))
        dir=${@:2}
    ;;
esac

for d in $dir
    do
        if [[ ${d:0:1} != "/" ]]
            then echo "tutti i parametri devono essere directory assolute"
            exit 3
        fi
done


for d in $dir
    do 
        FCR.sh $dir $y
done

while read line
    do 
        echo "nome assoluto file: $line"
        cat $line

    done < ~/.tmp/count