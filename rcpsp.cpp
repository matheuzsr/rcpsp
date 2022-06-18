#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <memory.h>
#include <math.h>
#include "rcpsp.hpp"
#include <iostream>
#include <fstream>

#define MAX(X, Y) ((X > Y) ? X : Y)
using namespace std;

int main(int argc, char *argv[])
{
#ifdef MODO_TESTE
#else
  int seed = 125442;
  srand(seed);
  lerDados("./instancias/j12060_7.sm");

  Solucao sol;
#endif
  return 0;
}

void lerDados(const string arq)
{
  FILE *arquivo = fopen(arq.c_str(), "r");
  getQtdTarefas(arquivo);
  getQtdRecursos(arquivo);
  getRelacoesPrecedencia(arquivo);
  getDuracaoTarefasEConsumoRecursos(arquivo);
  getQuantidadeCadaRecurso(arquivo);

  ordenarPrecedencia();
  fclose(arquivo);
}

void getQtdTarefas(FILE *arquivo)
{
  do
  {
    fscanf(arquivo, "%s", &linha);
  } while (strcmp(linha, "):") != 0);
  fscanf(arquivo, "%s", &linha);
  qtdTarefas = atoi(linha);
}
void getQtdRecursos(FILE *arquivo)
{
  do
  {
    fscanf(arquivo, "%s", &linha);
  } while (strcmp(linha, "renewable") != 0);
  fscanf(arquivo, "%s", &linha);
  fscanf(arquivo, "%s", &linha);

  qtdRecursos = atoi(linha);
}
void getRelacoesPrecedencia(FILE *arquivo)
{
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
    relacoesPrecedencia[i].qtdSucessores = atoi(linha);

    for (int j = 0; j < relacoesPrecedencia[i].qtdSucessores; j++)
    {
      fscanf(arquivo, "%s\t", &linha);
      relacoesPrecedencia[i].sucessores[j] = atoi(linha);
    }
  }
}
void getDuracaoTarefasEConsumoRecursos(FILE *arquivo)
{
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
void getQuantidadeCadaRecurso(FILE *arquivo)
{
  fscanf(arquivo, "%s\n", &linha);
  printf("%s", linha);
  do
  {
    fscanf(arquivo, "%s\n", &linha);
    printf("%s", linha);
  } while (strcmp(linha, "RESOURCEAVAILABILITIES:") != 0);

  for (int i = 0; i < (qtdRecursos * 2) - 1; i++)
  {
    fscanf(arquivo, "%s\t", &linha);
    printf("%s", linha);
  }
  fscanf(arquivo, "%s\t", &linha);

  for (int i = 0; i < qtdRecursos; i++)
  {
    fscanf(arquivo, "%s\t", &linha);
    recursoDisponivelAtual[i] = atoi(linha);
  }
}

void ordenarPrecedencia()
{
  RelacaoPrecedencia relacoesPrecedenciaOrdenado[MAX_QTD_TAREFAS];
  memcpy(&relacoesPrecedenciaOrdenado, &relacoesPrecedencia, sizeof(relacoesPrecedencia));

  int flag = 1;
  RelacaoPrecedencia aux;

  // TODO: alterar para uma outra função genérica, que recebe uma função que faz a condicional que ta dentro do if
  while (flag)
  {
    flag = 0;
    for (int i = 0; i < qtdTarefas; i++)
    {
      if (relacoesPrecedenciaOrdenado[i].qtdSucessores <
          relacoesPrecedenciaOrdenado[i + 1].qtdSucessores)
      {
        flag = 1;
        aux = relacoesPrecedenciaOrdenado[i];
        relacoesPrecedenciaOrdenado[i] = relacoesPrecedenciaOrdenado[i + 1];
        relacoesPrecedenciaOrdenado[i + 1] = aux;
      }
    }
  }

  // TODO: alterar para uma outra função genérica, que recebe uma função que faz a condicional que ta dentro do if
  flag = 1;
  while (flag)
  {
    flag = 0;
    for (int i = 0; i < qtdTarefas; i++)
    {
      if (relacoesPrecedenciaOrdenado[i].qtdSucessores == relacoesPrecedenciaOrdenado[i + 1].qtdSucessores &&
          (relacoesPrecedenciaOrdenado[i].sucessores[0] >
           relacoesPrecedenciaOrdenado[i + 1].sucessores[0]))
      {
        flag = 1;
        aux = relacoesPrecedenciaOrdenado[i];
        relacoesPrecedenciaOrdenado[i] = relacoesPrecedenciaOrdenado[i + 1];
        relacoesPrecedenciaOrdenado[i + 1] = aux;
      }
    }
  }

  /*
   * TODO:Falta finalizar a inserção do dado lido na matriz, [tarefa][recursos]
   * de maneira sequencial
   * após isso será inciada as que inciam no mesmo tempo
   */
  int tarefaAtualOrdenada = 0;

  memset(&tarefasPorDuracaoRecurso[0], -1, sizeof(tarefasPorDuracaoRecurso[0]));

  for (int i = 0; i < qtdTarefas; i++)
  {
    if (!verificarSeEstaContidoVetor(i + 1, qtdTarefas, tarefasPorDuracaoRecurso[0]))
    {
      tarefasPorDuracaoRecurso[0][tarefaAtualOrdenada] = i + 1;
      tarefaAtualOrdenada = tarefaAtualOrdenada + 1;
    }

    for (int j = 0; j < relacoesPrecedenciaOrdenado[i].qtdSucessores; j++)
    {
      if (!verificarSeEstaContidoVetor(relacoesPrecedenciaOrdenado[i].sucessores[j], qtdTarefas, tarefasPorDuracaoRecurso[0]))
      {
        tarefasPorDuracaoRecurso[0][tarefaAtualOrdenada] = relacoesPrecedenciaOrdenado[i].sucessores[j];
        tarefaAtualOrdenada = tarefaAtualOrdenada + 1;
      }
    }
  }
  printf("hello");
}

bool verificarSeEstaContidoVetor(const int value, const int quantidade, const int vetor[])
{
  for (int i = 0; i < quantidade; i++)
  {
    if (value == vetor[i])
    {
      return true;
    }
  }

  return false;
}

void ordenarTarefasRecursos()
{
}

void copiarSolucao(Solucao &solucaoNova, Solucao &solucaoAntiga)
{
  memcpy(&solucaoNova, &solucaoAntiga, sizeof(solucaoAntiga));
}