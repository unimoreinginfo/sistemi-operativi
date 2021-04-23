#!/bin/sh

# FCR.sh <path> <X>

# $1 path assoluto
# $2 <X>

# file tmp per salvarsi i file trovati
FILES_TMP="/tmp/17apr15_files"

if ! test -e $FILES_TMP
then
  touch $FILES_TMP
fi

for f in $1/*
do
  #echo "chiamando: $f"

  if test -d $f
  then
    sh FCR.sh $f $2
  else
    if test -f $f -a -r $f
    then
      OCC=`cat "$f" | grep -E "t$" | wc -l`
      echo "$f ha $OCC t finali"

      if test $OCC -ge $2
      then
        #echo "$f soddisfa"
        echo "$f" >> "$FILES_TMP"
      fi
    fi
  fi
done
