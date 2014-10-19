#!/bin/bash

TESTDATA="
http://snap.stanford.edu/data/as-skitter.txt.gz
http://snap.stanford.edu/data/bigdata/communities/com-lj.ungraph.txt.gz
http://snap.stanford.edu/data/bigdata/communities/com-orkut.ungraph.txt.gz
http://www.cc.gatech.edu/dimacs10/archive/data/coauthor/coPapersCiteseer.graph.bz2
http://www.cc.gatech.edu/dimacs10/archive/data/coauthor/coPapersDBLP.graph.bz2
http://www.cc.gatech.edu/dimacs10/archive/data/kronecker/kron_g500-logn16.graph.bz2
http://www.cc.gatech.edu/dimacs10/archive/data/kronecker/kron_g500-logn17.graph.bz2
http://www.cc.gatech.edu/dimacs10/archive/data/kronecker/kron_g500-logn18.graph.bz2
http://www.cc.gatech.edu/dimacs10/archive/data/kronecker/kron_g500-logn19.graph.bz2
http://www.cc.gatech.edu/dimacs10/archive/data/kronecker/kron_g500-logn20.graph.bz2
http://www.cc.gatech.edu/dimacs10/archive/data/kronecker/kron_g500-logn21.graph.bz2
"

make convert-from-snap-main.e
make convert-from-dimacs-main.e
make create-barabasi-albert-main.e
make create-watts-strogatz-main.e

mkdir -p data
cd data

for url in $TESTDATA
do
  base=`echo $url | grep -o "[^/]\+$" | sed "s/\(\.txt\.gz\|\.graph\.bz2\)$//"`
  if [ -f ${base}.bin ]
  then
    echo Skipping $base because it already exists
    continue
  fi
  wget $url
  echo Unzipping and converting
  if [[ $url == *.txt.gz ]]
  then
    gzip -d ${base}.txt.gz
    ../convert-from-snap-main.e ${base}.txt ${base}.bin
    rm ${base}.txt
  else
    bzip2 -d ${base}.graph.bz2
    ../convert-from-dimacs-main.e ${base}.graph ${base}.bin
    rm ${base}.graph
  fi
done

if [ -f barabasi-albert.bin ]
then
  echo Skipping barabasi-albert because it already exists
else
  ../create-barabasi-albert-main.e 50 200000 barabasi-albert.bin
fi

if [ -f watts-strogatz.bin ]
then
  echo Skipping watts-strogatz because it already exists
else
  ../create-watts-strogatz-main.e 1000000 50 10 watts-strogatz.bin
fi
