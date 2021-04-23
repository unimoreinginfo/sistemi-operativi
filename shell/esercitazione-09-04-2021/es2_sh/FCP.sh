#!/bin/sh

case $# in 
	0|1) echo Errore pochi paramentri.
	exit 1;;
	2) echo Ok;;
	*) echo Errore troppi parametri.
	exit 2;;
esac

case $2 in 
	0|*[!1-9]*) echo $2 non numerico o non strettamente positivo
	exit 3;;
*) ;;
esac

case $1 in 
	/*) if test ! -d $1 -o ! -x $1
	then
		echo $1 non directory o non traversabile 
		exit 4
	fi;;
	*) echo $1 nome non assoluto; exit 5;;
esac

PATH=`pwd`:$PATH
export PATH
> /tmp/temp$$

FCR.sh $1 $2 /tmp/temp$$

for i in `cat /tmp/temp$$`
do
	echo Trovato il file $i

	echo -n "Dammi il numero X (strettamente positivo e minore o uguale a K): "
	read X
	case $X in
		0|*[!1-9]*|) echo $X non numerico o negativo
			rm /tmp/temp$$
			exit 5;;
		*) if test $X -gt $2
			echo $X maggiore di K
			rm /tmp/temp
			exit 6;;
		fi
	esac
	echo "Invocando la parte C con parametri i nomi assoluti dei file intervallati dal numero corrispondente chiesto all'utente"
done

rm /tmp/temp$$





