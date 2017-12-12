#!/bin/bash

./test modellist.txt testing_data1.txt result1.txt
./test modellist.txt testing_data2.txt result2.txt
python3 acc.py result1.txt testing_answer.txt acc.txt
