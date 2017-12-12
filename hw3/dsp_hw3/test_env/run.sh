#!/bin/bash

if [ ! -z $1 ]; then
	echo "Add $1 into PATH"
	export PATH=$PATH:$1
fi
map=ZhuYin-Big5.map
LM=bigram.lm
order=2

#init
rm -f $map $LM lm.cnt result1/* result2/*
make clean

#Segment corpus and all test data into characters
./separator_big5.pl corpus.txt > corpus_seg.txt
for i in $(seq 1 10) ;
do
	./separator_big5.pl testdata/$i.txt > testdata/seg_$i.txt
done

#Train character-based bigram LM
ngram-count -text corpus_seg.txt -write lm.cnt -order 2
ngram-count -read lm.cnt -lm bigram.lm -unk -order 2

#Generate ZhuYin-Big5.map from Big5-ZhuYin.map
if [ ! -e ZhuYin-Big5.map ]; then
	echo "create ZhuYin-Big5.map"
	python3 z2b.py
fi

#disambig decode testdata
for i in $(seq 1 10) ;
do
	disambig -text testdata/seg_$i.txt -map $map -lm $LM -order $order > result1/$i.txt
done

#mydisambig decode testdata
make
for i in $(seq 1 10) ;
do
	./mydisambig testdata/seg_$i.txt $LM $order > result2/$i.txt
done

#diff
for i in $(seq 1 10) ;
do
	diff result1/$i.txt result2/$i.txt
done



