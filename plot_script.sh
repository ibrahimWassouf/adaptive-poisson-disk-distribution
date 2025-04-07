#! /bin/bash

for d in data/*
do
   python3 -m plot $d 
done

