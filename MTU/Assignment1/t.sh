#!/bin/sh

MAXCOUNT=10
count=1

while [ "$count" -le $MAXCOUNT ]; do
 number[$count]=$RANDOM
 let "count += 1"
done

a.out "${number[@]}"

