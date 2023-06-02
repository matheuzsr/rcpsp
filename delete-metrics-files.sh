#!/bin/bash

# Função recursiva para excluir arquivos de um diretório e seus subdiretórios
delete_files() {
  local directory="$1"

  # Verifica se o diretório existe
  if [ -d "$directory" ]; then
    # Loop pelos arquivos e subdiretórios no diretório
    for file in "$directory"/*; do
      if [ -f "$file" ]; then
        # Exclui o arquivo
        rm -f "$file"
      elif [ -d "$file" ]; then
        # Chama recursivamente a função para excluir os arquivos do subdiretório
        delete_files "$file"
      fi
    done
  fi
}

# Diretório a ser excluído
directory="./metricas"

# Chama a função para excluir os arquivos
delete_files "$directory"