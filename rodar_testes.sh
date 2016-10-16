#!/bin/bash

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 250 5 &> saidas/saida_5_250_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 1000 5 &> saidas/saida_5_1000_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 5000 5 &> saidas/saida_5_5000_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 250 30 &> saidas/saida_0_250_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 1000 30 &> saidas/saida_0_1000_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 5000 30 &> saidas/saida_0_5000_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 250 60 &> saidas/saida_0_250_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 1000 60 &> saidas/saida_0_1000_$i
done

for i in $(seq 1 30)
do
  /usr/bin/time -v ./ep2 5000 60 &> saidas/saida_0_5000_$i
done

