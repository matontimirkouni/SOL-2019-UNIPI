#!/bin/bash

IFS=@
while read -r t_parser ok_parser ko_parser ; do
   # echo $t_parser;
    if [ $t_parser = 1 ]; then
        ((output_1[0]++));
        ((output_1[1]+=$ok_parser + $ko_parser));
        ((output_1[2]+=$ok_parser));
        ((output_1[3]+=$ko_parser));
    
    elif [ $t_parser = 2 ]; then
        ((output_1[4]++));
        ((output_1[5]+=$ok_parser + $ko_parser));
        ((output_1[6]+=$ok_parser));
        ((output_1[7]+=$ko_parser));
    
    elif [ $t_parser = 3 ]; then
        ((output_1[8]++));
        ((output_1[9]+=$ok_parser + $ko_parser));
        ((output_1[10]+=$ok_parser));
        ((output_1[11]+=$ko_parser));
    else
        echo $t_parser;
    fi
  
done < testout.log
echo "Output risultati test"
for i in {0..11..4}
  do 
    echo "#--------------------#"
    echo "Client lanciati:" "${output_1[i]}" 
    echo "Operazioni eseguite: ""${output_1[i+1]}" 
    echo "Terminate con successo: ""${output_1[i+2]}" 
    echo "Terminate con erorre: ""${output_1[i+3]}" 
 done
 

killall -s USR1 server.o