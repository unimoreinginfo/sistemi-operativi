#!/bin/sh

cd $1

for i in * 
do
	if test -f $i -a -r $i
	then
		NL=`wc -l`
		if test $NL -eq $2
		then
			echo `pwd`/$i >> $3
		fi
	fi
done

echo `pwd`

for i in *
do
	if test -d $i -a -x $i
	then 
		FCR.sh `pwd`/$i $2 $3
	fi
done
