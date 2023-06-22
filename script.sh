#!/bin/bash

make clean && make

# Hora de início
start_time=$(date +%s)
echo "Hora de início: $(date +%H:%M:%S)"

./rcpsp $1

# Hora de fim
end_time=$(date +%s)
echo "Hora de fim: $(date +%H:%M:%S)"

# Cálculo do tempo gasto
duration=$((end_time - start_time))
echo "Tempo gasto: $duration s"

git add "metricas/exec_$1"
git commit -m "Incluindo execucao $1 - $start_time"
git push origin