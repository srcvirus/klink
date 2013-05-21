#!/bin/bash
for(( i=101; i<=110; i++  ))
do
	scp agent pweb.sqlite pweb@cn$i.cs.uwaterloo.ca:~/pweb_demo/
done
