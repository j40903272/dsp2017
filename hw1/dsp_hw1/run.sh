#!/bin/bash
make || exit 1
./train.sh $1
./test.sh
