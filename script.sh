#!/bin/bash

make clean && make

# Hora de início
start_time=$(date +%H:%M:%S)
echo "Hora de início: $start_time"

./rcpsp

# Hora de fim
end_time=$(date +%H:%M:%S)
echo "Hora de fim: $end_time"
