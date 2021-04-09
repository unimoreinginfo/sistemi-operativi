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

FCR.sh $1 $2

