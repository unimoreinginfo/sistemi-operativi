#!/bin/sh

# ./FCP.sh <X> <N>

# $1 <X> numero intero strettamente positivo
# $* <N> nomi assoluti di directory

FILES_TMP="/tmp/17apr15_files"
echo "" > $FILES_TMP

if test $# -ge 3
then
  echo "N deve essere maggiore o uguale a 2."
  exit 1
fi

X=$1

case $X in
    ''|*[!0-9]*)
      echo "L'argomento non e' un numero."
      exit 2
      ;;
    *)
      ;;
esac

shift

for f in $*
do
  case $f in
  /*)
   ;;
  *)
    echo "\`$f\` non e' un nome di file assoluto."
    exit 3
    ;;
  esac
done

./FCR.sh "$f" "$X"

FILES=`cat $FILES_TMP`
echo "contenuto: $FILES"

for f in `cat $FILES_TMP`
do
  echo "=> file trovato: $f"

  while test 1
  do
    echo "inserisci un numero K (> 0 e < $X):"
    read K

    if test $K -gt 0 -a $K -lt $X
    then
      echo "OK!"
      break
    fi

    echo "numero non valido"
  done

  echo -n "$K riga: "
  cat $f | head -$K | tail -1
done


# cancello il file temporaneo solamente alla fine
# rm $FILES_TMP

