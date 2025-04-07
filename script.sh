#!/bin/bash

for image in preprocessed/*
do
    for M in 3000 9000 30000 60000 120000 180000 300000
    do
      ./adaptive.o $image $M $((M/3)) >> result.txt
    done
done

