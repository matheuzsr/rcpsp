#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <memory.h>
#include <math.h>
#include "rcpsp.hpp"
#include <iostream>
#include <fstream>

#include <string.h>

// #define MODO_DEBUG = true;
#define SEMENTE_ALEATORIA = true;
#define METRICAS_TRABALHO_1 = true;

#define MAX(X, Y) ((X > Y) ? X : Y)
#define MIN(X, Y) ((X < Y) ? X : Y)
using namespace std;

int main(int argc, char *argv[])
{
#ifdef SEMENTE_ALEATORIA
  int seed = 10;
  srand(seed);
#endif

  Solucao sol;
  gerarMetricasTrabalho2(sol);

  return 0;
}

void gerarMetricasTrabalho2(Solucao &sol)
{
  lerDados("./instancias/j10.sm");
  heuristicaGrasp(sol);
}

// Leitura
void lerDados(const string arq)
{
  FILE *arquivo = fopen(arq.c_str(), "r");
  getQtdTarefas(arquivo);
  getQtdRecursos(arquivo);
  getRelacoesPrecedencia(arquivo);
  getDuracaoTarefasEConsumoRecursos(arquivo);
  getQuantidadeCadaRecurso(arquivo);

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
  for (int i = 0; i < qtdTarefas; i++)
  {
    memset(&matrizRelacoesPrecedencia[i], 0, sizeof(matrizRelacoesPrecedencia[i]));
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
    relacoesPrecedencia[i].qtdSucessores = atoi(linha);

    for (int j = 0; j < relacoesPrecedencia[i].qtdSucessores; j++)
    {
      fscanf(arquivo, "%s\t", &linha);
      int idSucessor = atoi(linha);
      relacoesPrecedencia[i].sucessores[j] = idSucessor;

      matrizRelacoesPrecedencia[i][idSucessor - 1] = 1;
    }
  }
}
void getDuracaoTarefasEConsumoRecursos(FILE *arquivo)
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
void getQuantidadeCadaRecurso(FILE *arquivo)
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

// Heuristica
void heuristicaConstrutivaSemMovimentar(Solucao &sol)
{
  ordenarPrecedencia();

  int tempoTarefas = 0;
  for (int i = 0; i < qtdTarefas; i++)
  {
    int idTarefaAtual = tarefasStartTimeOrdenadaPrecedencia[0][i];
    int duracaoTarefaAtual = duracao[idTarefaAtual - 1];

    tempoTarefas = duracaoTarefaAtual + tempoTarefas;
    tarefasStartTimeOrdenadaPrecedencia[1][i] = tempoTarefas;

    tempoTarefas++;
  }

  memcpy(&sol.tarefasStartTime[0], &tarefasStartTimeOrdenadaPrecedencia[0], sizeof(tarefasStartTimeOrdenadaPrecedencia[0]));
  memcpy(&sol.tarefasStartTime[1], &tarefasStartTimeOrdenadaPrecedencia[1], sizeof(tarefasStartTimeOrdenadaPrecedencia[1]));
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

  int tarefaAtualOrdenada = 0;

  memset(&tarefasStartTimeOrdenadaPrecedencia[0], -1, sizeof(tarefasStartTimeOrdenadaPrecedencia[0]));
  memset(&tarefasStartTimeOrdenadaPrecedencia[1], -1, sizeof(tarefasStartTimeOrdenadaPrecedencia[1]));

  for (int i = 0; i < qtdTarefas; i++)
  {
    if (!verificarSeEstaContidoVetor(i + 1, qtdTarefas, tarefasStartTimeOrdenadaPrecedencia[0]))
    {
      tarefasStartTimeOrdenadaPrecedencia[0][tarefaAtualOrdenada] = i + 1;
      tarefaAtualOrdenada = tarefaAtualOrdenada + 1;
    }

    for (int j = 0; j < relacoesPrecedenciaOrdenado[i].qtdSucessores; j++)
    {
      if (!verificarSeEstaContidoVetor(relacoesPrecedenciaOrdenado[i].sucessores[j], qtdTarefas, tarefasStartTimeOrdenadaPrecedencia[0]))
      {
        tarefasStartTimeOrdenadaPrecedencia[0][tarefaAtualOrdenada] = relacoesPrecedenciaOrdenado[i].sucessores[j];
        tarefaAtualOrdenada = tarefaAtualOrdenada + 1;
      }
    }
  }
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

int getCalcularMaiorTempoTarefas()
{
  int duracaoTotal = 0;
  for (int i = 0; i < qtdTarefas - 1; i++)
  {
    duracaoTotal = duracao[i] + duracaoTotal;
  }

  return duracaoTotal;
}

bool todosAnterioresOrdenadosJaEntraram(const int indiceTarefaAtual)
{
  for (int i = 0; i < indiceTarefaAtual; i++)
  {
    if (tarefasStartTimeOrdenadaPrecedencia[1][i] == -1)
    {
      return false;
    }
  }

  return true;
}

// Meta heuristica - Grasp
void heuristicaGrasp(Solucao solGrasp)
{
  heuristicaConstrutivaSemMovimentar(solGrasp);
  heuristicaAleatoria(solGrasp);
  // Chamar o calculo de FO para penalizar recurso e calcular tbm o makespan

  int maiorTempoTarefas = getCalcularMaiorTempoTarefas();
}

void heuristicaAleatoria(Solucao s)
{
  for (int i = 0; i < PESO_ALEATORIEDADE; i++)
  {
    int tarefaA = (rand() % qtdTarefas - 1) + 1;
    int tarefaB = (rand() % qtdTarefas - 1) + 1;

    int posicaoTarefaA = encontrarPosicaoTarefa(s, tarefaA);
    int posicaoTarefaB = encontrarPosicaoTarefa(s, tarefaB);

    int maiorPosicao = MAX(posicaoTarefaA, posicaoTarefaB);
    int menorPosicao = MIN(posicaoTarefaA, posicaoTarefaB);

    int podeTrocar = true;
    while (podeTrocar)
    {
      for (int predecessor = 0; predecessor < qtdTarefas; predecessor++)
      {
        if (matrizRelacoesPrecedencia[predecessor][s.tarefasStartTime[0][maiorPosicao] - 1] == 1)
        {
          int posicaoPredecessor = encontrarPosicaoTarefa(s, predecessor);
          if (s.tarefasStartTime[0][menorPosicao] < posicaoPredecessor)
          {
            podeTrocar = false;
          }
        }
      }

      if (podeTrocar == true)
      {
        int aux = s.tarefasStartTime[0][maiorPosicao];
        s.tarefasStartTime[0][maiorPosicao] = s.tarefasStartTime[0][menorPosicao];
        s.tarefasStartTime[0][menorPosicao] = aux;
        // Agora é necessário recalcular o tempo da sol aleatória
      }

      podeTrocar = false;
    }
  }

  printf("%d", s.makespan);
}

void reCalcularTempo(Solucao s, int idInicioReCalculo)
{
  int tempoAtual = 0;
  int recursosDisponivelAtual[MAX_QTD_RECURSO];
  memcpy(&recursosDisponivelAtual, &recursoDisponivel, sizeof(recursoDisponivel));

  for (int i = 0; i < qtdTarefas - 1; i++)
  {
    int tarefaAtual = tarefasStartTimeOrdenadaPrecedencia[0][i];
    int duracaoTarefaAtual = duracao[tarefaAtual - 1];

    int temRecursoDisponivel = true;
    for (int recurso = 0; recurso < MAX_QTD_RECURSO - 1; recurso++)
    {
      int recursoASerUsado = consumoRecursos[tarefaAtual][recurso] + recursosDisponivelAtual[recurso];
      if (recursoDisponivel[recurso] <= recursoASerUsado)
        temRecursoDisponivel = false;

      // consumoRecursos[tarefaAtual][recurso]
      // if(recursosDisponivelAtual[recurso] == )
    }

    tarefasStartTimeOrdenadaPrecedencia[0][i] = tempoAtual;
  }
}

int encontrarPosicaoTarefa(Solucao s, const int idTarefaProcurada)
{
  for (int i = 0; i < qtdTarefas; i++)
  {
    if (idTarefaProcurada == s.tarefasStartTime[0][i])
    {
      return i;
    }
  }

  return -1;
}

void buscaLocal(Solucao sol) {
  // Sorteia um cara aleatorio tenta trocar ele com todos os outros
  // Guarda a melhor solucao resultante disso
}

// Calculo FO (sem penalizaverificarSeEstaContidoVetorção)
void calcFOSemPenalizacao(Solucao &s) {}

// Métodos auxiliares
void copiarSolucao(Solucao &solucaoNova, Solucao &solucaoAntiga)
{
  memcpy(&solucaoNova, &solucaoAntiga, sizeof(solucaoAntiga));
}
void lerSolucao(std::string arq)
{
  FILE *arquivo = fopen(arq.c_str(), "r");

  fscanf(arquivo, "%s", &linha);
  fscanf(arquivo, "%s", &linha);
  solucaoLida.funObj = atoi(linha);

  fscanf(arquivo, "%s", &linha);
  fscanf(arquivo, "%s", &linha);
  solucaoLida.makespan = atoi(linha);

  fscanf(arquivo, "%s", &linha);
  fscanf(arquivo, "%s", &linha);
  fscanf(arquivo, "%s", &linha);
  fscanf(arquivo, "%s", &linha);

  int i = 0;
  do
  {
    fscanf(arquivo, "%s", &linha);
    solucaoLida.tarefasStartTime[0][i] = atoi(linha);
    fscanf(arquivo, "%s", &linha);
    solucaoLida.tarefasStartTime[1][i] = atoi(linha);
    i++;
  } while (fgetc(arquivo) != EOF);
  solucaoLida.qtdTarefas = i - 1;
}
void escreverSolucao(Solucao &solucao, std::string arq)
{
  FILE *arquivo = fopen(arq.c_str(), "w");

  fprintf(arquivo, "FO: ");
  fprintf(arquivo, "%d\n", solucao.funObj);

  fprintf(arquivo, "Makespan: ");
  fprintf(arquivo, "%d\n", solucao.makespan);

  fprintf(arquivo, "------------------\n");
  fprintf(arquivo, "Job Start Time\n");

  for (int i = 0; i < solucao.qtdTarefas; i++)
  {
    fprintf(arquivo, "%d %d\n",
            solucao.tarefasStartTime[0][i],
            solucao.tarefasStartTime[1][i]);
  }
}
