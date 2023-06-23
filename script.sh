#!/bin/bash

make clean && make

# Hora de início
start_time=$(date +%s)
formated_start_time=$(date +%H:%M:%S)

echo "Hora de início: $(date +%H:%M:%S)"

./rcpsp $1

# Hora de fim
end_time=$(date +%s)
formated_end_time=$(date +%H:%M:%S)

echo "Hora de fim: $(date +%H:%M:%S)"

# Cálculo do tempo gasto
duration=$((end_time - start_time))
echo "Tempo gasto: $duration s"

exec_var=$(($1 + 1))

git add "metricas/exec_$exec_var"
git commit -m "Incluindo execucao $exec_var: $formated_start_time -- $formated_end_time"
git push origin