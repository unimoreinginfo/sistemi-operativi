#!/bin/sh
d=
f=

case $# in
    0) echo "POCHI PARAMETRI - Usage is: $0 [directory] nomedirectory"
        exit 1;;
    1) d = `pwd`; f=$1;;
    2) d=$1; f=$2;;
    *) echo "TROPPI PARAMETRI - Usage is: $0 [directory] nomedirectory"
        exit 2;;
esac

case $d in
    /*) ;;
    *) echo Errore: $d non in forma assoluta
        exit 3;;
esac

case $f in
    /*) ;;
    */*) echo Errore: $d non in relativa semplice
        exit 4;;
esac

if [ ! -d $d -o ! -x $d ]
    then echo Errore: $d non directory o non attraversabile; exit 5
fi

PATH=`pwd`:$PATH
export PATH
echo beginc.sh: Stiamo per esplorare la directory $d
cercafile.sh $d $f
echo HO FINITO TUTTO