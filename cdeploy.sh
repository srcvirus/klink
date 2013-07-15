#!/bin/bash
for(( i=101; i<=110; i++  ))
do
	scp agent config chostlist imonitorlist pweb.sqlite3 pweb@cn$i.cs.uwaterloo.ca:~/pweb_demo/
done
