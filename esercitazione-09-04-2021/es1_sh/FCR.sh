#!/bin/sh

cd $1

for i in * 
do
	if test -f $i -a -r $i
	then
		NL=`grep '[0-9]' $i | wc -l`
		if test $NL -eq $2
		then
			echo `pwd`/$i
		fi
	fi
done

for i in *
do
	if test -d $i -a -x $i
	then 
		FCR.sh `pwd`/$i $2
	fi
done

