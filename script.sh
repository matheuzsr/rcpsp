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

exec_var=$(($1 + 1))

git add "metricas/exec_$exec_var"
git commit -m "Incluindo execucao $exec_var: $start_time -- $duration"
git push origin