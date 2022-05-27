#!/bin/sh

MAX_THREAD=20
NB_ESSAI=149

#On note les entêtes
echo -e 'TYPE;NBTHREAD;INDEXESSAI;TIME' > ./ex1.result

#On essaye plusieurs fois pour avoir une moyenne
for i in `seq 0 $NB_ESSAI`
do
    #Avec un nombre de threads croissant
    for j in `seq 1 $MAX_THREAD`
    do
        ti=`/tmp/tp2_1_matmat.o 1 |head -n 2|tail -n 1|cut -d" " -f5`
        echo -e "0;$j;$i;$ti" >> ./ex1.result

        env OMP_SCHEDULE="static"
        ti=`/tmp/tp2_1_matmat.o $j |head -n 2|tail -n 1|cut -d" " -f5`
        echo -e "1;$j;$i;$ti" >> ./ex1.result

        env OMP_SCHEDULE="dynamic"
        ti=`/tmp/tp2_1_matmat.o $j |head -n 2|tail -n 1|cut -d" " -f5`
        echo -e "2;$j;$i;$ti" >> ./ex1.result
    
    done

done