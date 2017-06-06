#!/bin/bash

FILES=data/*.gph
OUTFILE=testrun_output.txt

echo "##### New testrun: #####" >> $OUTFILE

for f in $FILES
do
    echo "Processing file $f..."
    echo $f >> $OUTFILE 
    echo "standalone"
    (time python3 ex5_standalone.py $f) &>> $OUTFILE
    echo "graph_tool"
    (time python3 ex5.py $f) &>> $OUTFILE
done
