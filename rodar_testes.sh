#!/bin/bash

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 250 5 &> saida_5_250_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 2500 5 &> saida_5_2500_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 25000 5 &> saida_5_25000_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 250 30 &> saida_0_250_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 2500 30 &> saida_0_2500_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 25000 30 &> saida_0_25000_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 250 60 &> saida_0_250_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 2500 60 &> saida_0_2500_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 25000 60 &> saida_0_25000_$i
done

