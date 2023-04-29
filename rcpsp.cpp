#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <limits.h>
#include "rcpsp.hpp"
#include <iostream>
#include <fstream>

#include <string.h>

#define MAX(X, Y) ((X > Y) ? X : Y)
using namespace std;

void handleOrdenarTarefasTempo()
{
  bool flag = true;
  int indexAux;
  int qtdSucessorAux;
  while (flag)
  {
    flag = false;
    for (int i = 0; i < qtdTarefas - 1; i++)
    {
      if (tarefaQtdSucessores[1][i] > tarefaQtdSucessores[1][i + 1])
      {
        flag = true;
        indexAux = tarefaQtdSucessores[0][i + 1];
        qtdSucessorAux = tarefaQtdSucessores[1][i + 1];

        tarefaQtdSucessores[0][i + 1] = tarefaQtdSucessores[0][i];
        tarefaQtdSucessores[1][i + 1] = tarefaQtdSucessores[1][i];

        tarefaQtdSucessores[0][i] = indexAux;
        tarefaQtdSucessores[1][i] = qtdSucessorAux;
      }
    }
  }
}

int matriz_tarefas_escalonamento[2][MAX_QTD_TAREFAS];

bool todosPredecessoresJaEntraram(int idTarefa)
{
  tPrececessores predecessores = getPredecessores(idTarefa);
  // zerar_vetor(predecessores.list, qtdTarefas, 0);
  for (int j = 0; j < predecessores.qtdPrecedessores; j++)
  {
    int predecessor = predecessores.list[j];

    // A segunda parte da condicional é para
    if (!includes_array(predecessor, matriz_solucao_com_tempos[0], qtdTarefas) && predecessor != 0)
    {
      return false;
    }
  }

  return true;
}

bool algumPredecessoresJaEntrou(int idTarefa)
{
  tPrececessores predecessores = getPredecessores(idTarefa);

  for (int j = 0; j < predecessores.qtdPrecedessores; j++)
  {
    int predecessor = predecessores.list[j];
    if (includes_array(predecessor, matriz_solucao_com_tempos[0], qtdTarefas))
    {
      return true;
    }
  }

  return false;
}

int getMaiorEndTimeMatrizSolucao()
{
  int maiorTempo = 0;
  int i = 0;
  while (matriz_solucao_com_tempos[0][i] != -1)
  {
    if (matriz_solucao_com_tempos[2][i] > maiorTempo)
    {
      maiorTempo = matriz_solucao_com_tempos[2][i];
    }

    i++;
  }

  return maiorTempo;
}

int getStartTimeTarefa(int idTarefa)
{
  // verifica se os predecessores entraram caso não entraram retorna o maior
  // 1- caso entraram retorna o maior end-time dentre eles
  if (!algumPredecessoresJaEntrou(idTarefa))
  {
    return getMaiorEndTimeMatrizSolucao();
  }

  tPrececessores predecessores = getPredecessores(idTarefa);

  int maiorEndTime = 0;
  for (int j = 0; j < predecessores.qtdPrecedessores; j++)
  {
    int predecessor = predecessores.list[j];

    int idMatriz = findIndexByValue(predecessor, qtdTarefas, matriz_solucao_com_tempos[0]);

    if (idMatriz != -1)
    {
      maiorEndTime = MAX(matriz_solucao_com_tempos[2][idMatriz], maiorEndTime);
    }
  }

  return maiorEndTime;
}

// caso não entraram retorna 0

void inserirTarefaNaSolucao(int idTarefa)
{
  int startTime = getStartTimeTarefa(idTarefa);

  push_array(idTarefa, matriz_solucao_com_tempos[0], qtdTarefas);
  push_array(startTime, matriz_solucao_com_tempos[1], qtdTarefas);
  push_array(startTime + duracao[idTarefa], matriz_solucao_com_tempos[2], qtdTarefas);

  // printf("Task: %d | Start time: %d", idTarefa, startTime);

  printf("\n------------- Matriz de solução -------------\n");
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < qtdTarefas - 1; j++)
    {
      int value = matriz_solucao_com_tempos[i][j];
      if (i == 0)
      {
        value = value + 1;
      }
      printf("%d ", value);
    }
    printf("\n");
  }
}

int main(int argc, char *argv[])
{
  int seed = 8; // time(NULL)
  srand(seed);

  lerDados("./instancias/j12060_7.sm");
  handleOrdenarTarefasPorSucessor();
  // calcularDuracaoTarefasMaisTempoPredecessores();
  // memcpy(&maiorDuracaoTarefas[0], &tarefaQtdSucessores[0], sizeof(tarefaQtdSucessores[0]));

  zerar_vetor(matriz_solucao_com_tempos[0], qtdTarefas, -1);
  zerar_vetor(matriz_solucao_com_tempos[1], qtdTarefas, -1);
  zerar_vetor(matriz_solucao_com_tempos[2], qtdTarefas, -1);

  double alfa = 0.5;
  int qtdEscalonamento = qtdTarefas;

  zerar_vetor(matriz_tarefas_escalonamento[0], qtdEscalonamento, -1);
  zerar_vetor(matriz_tarefas_escalonamento[1], qtdEscalonamento, 0);
  memcpy(&matriz_tarefas_escalonamento[0], &tarefaQtdSucessores[0], sizeof(tarefaQtdSucessores[0]));

  // Remove a ultima tarefa da solução
  int colunaUltimo = findIndexByValue(qtdTarefas - 1, qtdEscalonamento, matriz_tarefas_escalonamento[0]);
  // Remove a coluna com base no valor encontrado
  if (colunaUltimo != -1)
  {
    for (int j = colunaUltimo; j < qtdEscalonamento; j++)
    {
      matriz_tarefas_escalonamento[0][j] = matriz_tarefas_escalonamento[0][j + 1];
      matriz_tarefas_escalonamento[1][j] = matriz_tarefas_escalonamento[1][j + 1];
    }
    qtdEscalonamento--;
  }

  // Insere 0 na solução e remove o 0 da matriz de matriz_tarefas_escalonamento
  inserirTarefaNaSolucao(0);
  int colunaPrimeiro = findIndexByValue(0, qtdEscalonamento, matriz_tarefas_escalonamento[0]);
  // Remove a coluna com base no valor encontrado
  if (colunaPrimeiro != -1)
  {
    for (int j = colunaPrimeiro; j < qtdEscalonamento - 1; j++)
    {
      matriz_tarefas_escalonamento[0][j] = matriz_tarefas_escalonamento[0][j + 1];
      matriz_tarefas_escalonamento[1][j] = matriz_tarefas_escalonamento[1][j + 1];
    }
    qtdEscalonamento--;
  }

  while (qtdEscalonamento > 0)
  {
    for (int i = 0; i < qtdEscalonamento; i++)
    {
      int idTarefa = matriz_tarefas_escalonamento[0][i];
      bool predecessores_entraram = false;

      tPrececessores predecessores = getPredecessores(idTarefa);
      zerar_vetor(predecessores.list, qtdTarefas, 0);

      if (!todosPredecessoresJaEntraram(idTarefa) && idTarefa != -1)
      {
        matriz_tarefas_escalonamento[1][i] = PESO_PENALIZACAO_PRECEDENCIA + i;
      }
      else
      {
        int maiorDuracao = 0;
        for (int indexPredecessor = 0; indexPredecessor < predecessores.qtdPrecedessores; indexPredecessor++)
        {
          int idPredecessor = predecessores.list[indexPredecessor];
          if (idPredecessor != 0)
          {
            maiorDuracao = MAX(duracao[idPredecessor], maiorDuracao);
          }
        }
        matriz_tarefas_escalonamento[1][i] = maiorDuracao + duracao[idTarefa];
      }
    }

    bool flag = true;
    int indexAux;
    int qtdSucessorAux;
    while (flag)
    {
      flag = false;
      for (int i = 0; i < qtdEscalonamento; i++)
      {
        if (matriz_tarefas_escalonamento[1][i] > matriz_tarefas_escalonamento[1][i] && (i + 1) < qtdEscalonamento)
        {
          flag = true;
          indexAux = matriz_tarefas_escalonamento[0][i + 1];
          qtdSucessorAux = matriz_tarefas_escalonamento[1][i + 1];

          matriz_tarefas_escalonamento[0][i + 1] = matriz_tarefas_escalonamento[0][i];
          matriz_tarefas_escalonamento[1][i + 1] = matriz_tarefas_escalonamento[1][i];

          matriz_tarefas_escalonamento[0][i] = indexAux;
          matriz_tarefas_escalonamento[1][i] = qtdSucessorAux;
        }
      }
    }

    int qtdEscalonamentoLRC = ceil(qtdEscalonamento * alfa);

    // Fazendo valer regra de PRECEDENCIA
    int tempoTarefaEscolhida = PESO_PENALIZACAO_PRECEDENCIA;
    int tarefaEscolhida = -1;
    while (tempoTarefaEscolhida >= PESO_PENALIZACAO_PRECEDENCIA)
    {
      int idTarefaEscolhida = (rand() % qtdEscalonamentoLRC - 1) + 1;
      tarefaEscolhida = matriz_tarefas_escalonamento[0][idTarefaEscolhida];
      tempoTarefaEscolhida = matriz_tarefas_escalonamento[1][idTarefaEscolhida];
    }

    printf("\n\nTarefa escolhida: %d", tarefaEscolhida + 1);
    printf("\n------------- Matriz tarefas a entrar -------------\n");
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < qtdEscalonamento; j++)
      {
        int value = matriz_tarefas_escalonamento[i][j];
        if (i == 0)
        {
          value = value + 1;
        }
        printf("%d ", value);
      }
      printf("\n");
    }

    inserirTarefaNaSolucao(tarefaEscolhida);

    int n = qtdEscalonamento;

    int coluna = findIndexByValue(tarefaEscolhida, qtdEscalonamento, matriz_tarefas_escalonamento[0]);

    // Remove a coluna com base no valor encontrado
    if (coluna != -1)
    {
      for (int j = coluna; j < n - 1; j++)
      {
        matriz_tarefas_escalonamento[0][j] = matriz_tarefas_escalonamento[0][j + 1];
        matriz_tarefas_escalonamento[1][j] = matriz_tarefas_escalonamento[1][j + 1];
      }
      n--;
    }

    qtdEscalonamento--;
  }
  // TODO: Validar inclusão aqui do código para pegar tempo do maior na solução
}

/*
 * O objetivo desse método é ordenar as tarefas pelas que tem
 * a MAIOR quantidade de SUCESSORES
 */
void handleOrdenarTarefasPorSucessor()
{
  bool flag = true;
  int indexAux;
  int qtdSucessorAux;
  while (flag)
  {
    flag = false;
    for (int i = 0; i < qtdTarefas; i++)
    {
      if (tarefaQtdSucessores[1][i + 1] > tarefaQtdSucessores[1][i])
      {
        flag = true;
        indexAux = tarefaQtdSucessores[0][i + 1];
        qtdSucessorAux = tarefaQtdSucessores[1][i + 1];

        tarefaQtdSucessores[0][i + 1] = tarefaQtdSucessores[0][i];
        tarefaQtdSucessores[1][i + 1] = tarefaQtdSucessores[1][i];

        tarefaQtdSucessores[0][i] = indexAux;
        tarefaQtdSucessores[1][i] = qtdSucessorAux;
      }
    }
  }
}

void calcularDuracaoTarefasMaisTempoPredecessores()
{
  memcpy(&maiorDuracaoTarefas[0], &tarefaQtdSucessores[0], sizeof(tarefaQtdSucessores[0]));
  memset(&maiorDuracaoTarefas[1], -1, sizeof(maiorDuracaoTarefas[1]));

  for (int i = 0; i < qtdTarefas; i++)
  {
    int idAtual = maiorDuracaoTarefas[0][i];

    tPrececessores predecessores = getPredecessores(idAtual);
    int qtdPredecessores = predecessores.qtdPrecedessores;

    printf("\nTarefa %d ->", idAtual + 1);
    memset(&entraramList, -1, sizeof(entraramList));
    maiorDuracaoTarefas[1][i] = calcularTempoTarefa(idAtual, predecessores.list, qtdPredecessores);
    printf("\nTempo %d \n", maiorDuracaoTarefas[1][i]);
  }
}

int calcularTempoTarefa(int idTarefa, int predecessores[], int qtdPredecessores)
{
  int tempoTotal = duracao[idTarefa];
  for (int i = 0; i < qtdPredecessores; i++)
  {
    int predecessor = predecessores[i];
    if (matrizRelacoesSucessores[predecessor][idTarefa] == 1)
    {
      tPrececessores predecessoresDentro = getPredecessores(predecessor);
      int qtdPredecessoresDentro = predecessoresDentro.qtdPrecedessores;

      if (!includes_array(predecessor, entraramList, qtdTarefas))
      {
        printf(" %d(%d) |", predecessor + 1, duracao[predecessor]);
        push_array(predecessor, entraramList, qtdTarefas);
        tempoTotal += calcularTempoTarefa(predecessor, predecessoresDentro.list, qtdPredecessoresDentro);
      }
    }
  }

  return tempoTotal;
}

tPrececessores getPredecessores(const int tarefa)
{
  tPrececessores predecessores;
  predecessores.qtdPrecedessores = 0;

  for (int j = 0; j < qtdTarefas; j++)
  {
    if (matrizRelacoesSucessores[j][tarefa] == 1)
    {
      predecessores.list[predecessores.qtdPrecedessores] = j;
      predecessores.qtdPrecedessores++;
    }
  }

  return predecessores;
}

// Leitura
void lerDados(const string arq)
{
  FILE *arquivo = fopen(arq.c_str(), "r");
  lerQtdTarefas(arquivo);
  lerQtdRecursos(arquivo);
  lerRelacoesPrecedencia(arquivo);
  lerDuracaoTarefasEConsumoRecursos(arquivo);
  lerQuantidadeCadaRecurso(arquivo);

  fclose(arquivo);
}
void lerQtdTarefas(FILE *arquivo)
{
  do
  {
    fscanf(arquivo, "%s", &linha);
  } while (strcmp(linha, "):") != 0);
  fscanf(arquivo, "%s", &linha);
  qtdTarefas = atoi(linha);
}
void lerQtdRecursos(FILE *arquivo)
{
  do
  {
    fscanf(arquivo, "%s", &linha);
  } while (strcmp(linha, "renewable") != 0);
  fscanf(arquivo, "%s", &linha);
  fscanf(arquivo, "%s", &linha);

  qtdRecursos = atoi(linha);
}
void lerRelacoesPrecedencia(FILE *arquivo)
{
  for (int i = 0; i < qtdTarefas; i++)
  {
    memset(&matrizRelacoesSucessores[i], 0, sizeof(matrizRelacoesSucessores[i]));
  }

  do
  {
    fscanf(arquivo, "%s", &linha);
  } while (strcmp(linha, "RELATIONS:") != 0);

  for (int i = 0; i < 4; i++)
  {
    fscanf(arquivo, "%s", &linha);
  }

  for (int i = 0; i < qtdTarefas; i++)
  {
    fscanf(arquivo, "%s\t", &linha);
    fscanf(arquivo, "%s\t", &linha);

    fscanf(arquivo, "%s\t", &linha);
    int qtdSucessores = atoi(linha);

    tarefaQtdSucessores[0][i] = i;
    tarefaQtdSucessores[1][i] = qtdSucessores;

    for (int j = 0; j < qtdSucessores; j++)
    {
      fscanf(arquivo, "%s\t", &linha);
      int idSucessor = atoi(linha);

      matrizRelacoesSucessores[i][idSucessor - 1] = 1;
    }
  }
}
void lerDuracaoTarefasEConsumoRecursos(FILE *arquivo)
{
  memset(&duracao, -1, sizeof(duracao));

  do
  {
    fscanf(arquivo, "%s", &linha);
  } while (strcmp(linha, "REQUESTS/DURATIONS:") != 0);

  for (int i = 0; i < (2 + (qtdRecursos * 2) + 2); i++)
  {
    fscanf(arquivo, "%s", &linha);
  }

  for (int i = 0; i < qtdTarefas; i++)
  {
    fscanf(arquivo, "%s", &linha);
    int idTarefaAtual = atoi(linha);

    fscanf(arquivo, "%s", &linha);
    fscanf(arquivo, "%s", &linha);
    int duracaoTarefaAtual = atoi(linha);

    duracao[idTarefaAtual - 1] = duracaoTarefaAtual;

    for (int i = 0; i < qtdRecursos; i++)
    {
      fscanf(arquivo, "%s\t", &linha);
      consumoRecursos[idTarefaAtual - 1][i] = atoi(linha);
    }
  }
}
void lerQuantidadeCadaRecurso(FILE *arquivo)
{
  fscanf(arquivo, "%s\n", &linha);
  do
  {
    fscanf(arquivo, "%s\n", &linha);
  } while (strcmp(linha, "RESOURCEAVAILABILITIES:") != 0);

  for (int i = 0; i < (qtdRecursos * 2) - 1; i++)
  {
    fscanf(arquivo, "%s\t", &linha);
  }
  fscanf(arquivo, "%s\t", &linha);

  for (int i = 0; i < qtdRecursos; i++)
  {
    fscanf(arquivo, "%s\t", &linha);
    recursoDisponivel[i] = atoi(linha);
  }
}

// Métodos auxiliares
bool verificarSeEstaContidoVetor(const int value, const int qtd, const int vetor[])
{
  for (int i = 0; i < qtd; i++)
  {
    if (value == vetor[i])
    {
      return true;
    }
  }

  return false;
}

int findIndexByValue(const int value, const int qtd, const int vetor[])
{
  for (int i = 0; i < qtd; i++)
  {
    if (vetor[i] == value)
    {
      return i;
    }
  }

  return -1;
}

void push_array(int id, int array[], int qtd)
{
  for (int i = 0; i < qtd; i++)
  {
    if (array[i] == -1)
    {
      array[i] = id;

      return;
    }
  }
}

bool includes_array(int id, int array[], int qtd)
{
  for (int i = 0; i < qtd; i++)
  {
    if (array[i] == id)
    {
      return true;
    }
  }

  return false;
}

void zerar_vetor(int *array, const int tamanho, const int value)
{
  memset(array, value, tamanho * sizeof(int));
}
