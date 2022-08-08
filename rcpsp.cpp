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

#define MODO_DEBUG = true;
#define SEMENTE_ALEATORIA = true;
#define METRICAS_TRABALHO_1 = true;

#define MAX(X, Y) ((X > Y) ? X : Y)
#define MIN(X, Y) ((X < Y) ? X : Y)
#define ROUND(number) ((number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5))
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
  /* Passe um valor de 0 à 1 */
  float LRC = 0.8;

  lerDados("./instancias/j10.sm");
  heuristicaGrasp(sol, LRC);
  printf("oi");
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
  memcpy(&sol.tarefasStartTime[0], &tarefasStartTimeOrdenadaPrecedencia[0], sizeof(tarefasStartTimeOrdenadaPrecedencia[0]));
  memset(&sol.tarefasStartTime[1], -1, sizeof(sol.tarefasStartTime[1]));

  for (int i = 1; i < qtdTarefas - 1; i++)
  {
    int idTarefaAtual = tarefasStartTimeOrdenadaPrecedencia[0][i];
    sol.tarefasStartTime[1][i] = i;
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

bool todosAnterioresOrdenadosJaSairam(Solucao s, int indiceTarefaAtual, int tempoAtual)
{
  for (int i = 0; i < qtdTarefas; i++)
  {
    int tarefaAtual = s.tarefasStartTime[0][indiceTarefaAtual];
    bool ehPredecessor = matrizRelacoesPrecedencia[i][tarefaAtual - 1] == 1;
    int predecessor = tarefasStartTimeOrdenadaPrecedencia[0][i];
    int idPredecessorSolucao = encontrarPosicaoTarefa(s, predecessor);

    if (!(ehPredecessor && idPredecessorSolucao != -1) && tempoAtual != 0)
      return false;

    int tempoGastoPredecessor = duracao[predecessor - 1] + s.tarefasStartTime[1][idPredecessorSolucao];

    // Valudar se a tarefa atual != predecessor
    if (tempoGastoPredecessor >= 0 && (tempoGastoPredecessor < tempoAtual || tempoAtual != 0))
      return false;
  }

  return true;

  // for (int i = 0; i < indiceTarefaAtual; i++)
  // {
  //   // int predecessor = tarefasStartTimeOrdenadaPrecedencia[0][i];
  //   int idPredecessorSolucao = encontrarPosicaoTarefa(s, predecessor);
  //   bool ehDeFatoPredecessor = matrizRelacoesPrecedencia[predecessor - 1][i] == 1;

  //   if (idPredecessorSolucao == -1)
  //     return false;

  //   int tempoGastoPredecessor = duracao[predecessor - 1] + s.tarefasStartTime[1][idPredecessorSolucao];

  //   // if (tarefasStartTimeOrdenadaPrecedencia[1][i] == -1 || tempoGastoPredecessor < tempoAtual ||(tempoAtual == 0 && predecess or == s.tarefasStartTime[0][indiceTarefaAtual]))
  //   if (!ehDeFatoPredecessor || duracao[predecessor - 1] != 0 && ((tarefasStartTimeOrdenadaPrecedencia[1][i] == -1 || tempoGastoPredecessor > tempoAtual) || predecessor == s.tarefasStartTime[0][indiceTarefaAtual]))
  //   {
  //     return false;
  //   }
  // }

  // return true;
}

// Meta heuristica - Grasp
void heuristicaGrasp(Solucao &solGrasp, float LRC)
{
  Solucao solucaoLocalMelhor;
  copiarSolucao(solucaoLocalMelhor, solGrasp);

  while (true)
  {
    heuristicaConstrutivaSemMovimentar(solGrasp);
    heuristicaAleatoria(solGrasp, LRC);
    memset(&solGrasp.tarefasStartTime[1], 0, sizeof(solGrasp.tarefasStartTime[1]));

    buscaLocal(solGrasp);
    recalculoParaEscalonarSolucaoAleatoria(solGrasp);
    calcFOSemPenalizacaoPrecedencia(solGrasp);

    if (solGrasp.funObj < solucaoLocalMelhor.funObj)
      copiarSolucao(solucaoLocalMelhor, solGrasp);
  }
}

void heuristicaAleatoria(Solucao &s, float LRC)
{
  int qtdTarefasAleatorias = ROUND((qtdTarefas - 2) * LRC);

  for (int i = 1; i <= qtdTarefasAleatorias; i++)
  {
    int aux[2];
    int tarefaAleatoria = (rand() % (qtdTarefas - 2)) + 2;
    int idTarefaAleatoria = encontrarPosicaoTarefa(s, tarefaAleatoria);
    int prioridadeTarefa = encontrarPrioridadeTarefa(s, tarefaAleatoria);

    aux[0] = s.tarefasStartTime[0][i];
    aux[1] = s.tarefasStartTime[1][i];

    s.tarefasStartTime[0][i] = tarefaAleatoria;
    s.tarefasStartTime[1][i] = prioridadeTarefa;

    s.tarefasStartTime[0][idTarefaAleatoria] = aux[0];
    s.tarefasStartTime[1][idTarefaAleatoria] = aux[1];
  }

  ordenarParteTarefasPelasPrioridades(s, 1, qtdTarefasAleatorias);
  ordenarParteTarefasPelasPrioridades(s, qtdTarefasAleatorias, qtdTarefas - 1);

  encaixarParteEstaticaNaAleatoria(s, qtdTarefasAleatorias + 1);
}
void encaixarParteEstaticaNaAleatoria(Solucao sol, const int idInicioParteEstatica)
{
  int aux[2];
  int auxMovimentacao[2];

  for (int i = idInicioParteEstatica; i < qtdTarefas; i++)
  {
    int idTarefa = sol.tarefasStartTime[0][i];
    int prioridadeTarefa = sol.tarefasStartTime[1][i];

    for (int j = 1; j < qtdTarefas - 1; j++)
    {
      int prioridadeTarefaAleatoria = sol.tarefasStartTime[1][j];

      if (prioridadeTarefa < prioridadeTarefaAleatoria)
      {
        aux[0] = sol.tarefasStartTime[0][j];
        aux[1] = prioridadeTarefaAleatoria;

        sol.tarefasStartTime[0][j] = idTarefa;
        sol.tarefasStartTime[1][j] = prioridadeTarefa;

        auxMovimentacao[0] = sol.tarefasStartTime[0][j + 1];
        auxMovimentacao[1] = sol.tarefasStartTime[1][j + 1];

        for (int jSeguinte = j + 1; jSeguinte < qtdTarefas - 1; jSeguinte++)
        {
          sol.tarefasStartTime[0][jSeguinte] = aux[0];
          sol.tarefasStartTime[1][jSeguinte] = aux[1];

          sol.tarefasStartTime[0][jSeguinte + 1] = auxMovimentacao[0];
          sol.tarefasStartTime[1][jSeguinte + 1] = auxMovimentacao[1];
        }
      }

      sol.tarefasStartTime[0][j] = aux[0];
      sol.tarefasStartTime[1][j] = aux[1];
    }

    printf(" %d ", sol.tarefasStartTime[1][i]);
  }
}
void ordenarParteTarefasPelasPrioridades(Solucao &s, const int inicio, const int fim)
{
  int flag = 1;
  int aux[2];
  while (flag)
  {
    flag = 0;
    for (int i = inicio; i <= fim; i++)
    {
      if (s.tarefasStartTime[1][i] > s.tarefasStartTime[1][i + 1] && s.tarefasStartTime[1][i + 1] != -1)
      {
        flag = 1;
        aux[0] = s.tarefasStartTime[0][i];
        aux[1] = s.tarefasStartTime[1][i];

        s.tarefasStartTime[0][i] = s.tarefasStartTime[0][i + 1];
        s.tarefasStartTime[1][i] = s.tarefasStartTime[1][i + 1];

        s.tarefasStartTime[0][i + 1] = aux[0];
        s.tarefasStartTime[1][i + 1] = aux[1];
        printf("\n");
      }
    }
  }
  printf("\n");
}

int encontrarPrioridadeTarefa(Solucao s, const int idTarefaProcurada)
{
  for (int i = 0; i < qtdTarefas; i++)
  {
    if (idTarefaProcurada == s.tarefasStartTime[0][i])
    {
      return s.tarefasStartTime[1][i];
    }
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

void buscaLocal(Solucao &s)
{
  Solucao solucaoLocalMelhor;
  copiarSolucao(solucaoLocalMelhor, s);

  int tarefaB = (rand() % (qtdTarefas - 2)) + 2;
  int idTarefaAleatoria = encontrarPosicaoTarefa(s, tarefaB);

  for (int i = 1; i < qtdTarefas - 1; i++)
  {
    int tarefaA = s.tarefasStartTime[0][i];

    if (tarefaA != tarefaB)
    {
      int posicaoTarefaA = encontrarPosicaoTarefa(s, tarefaA);
      int posicaoTarefaB = encontrarPosicaoTarefa(s, tarefaB);

      int maiorPosicao = MAX(posicaoTarefaA, posicaoTarefaB);
      int menorPosicao = MIN(posicaoTarefaA, posicaoTarefaB);

      bool podeTrocar = true;
      while (podeTrocar)
      {
        for (int predecessor = 0; predecessor < qtdTarefas; predecessor++)
        {
          // Validando os predecessores da tarefaA
          bool ehPredecessorTarefaA = matrizRelacoesPrecedencia[predecessor][tarefaA - 1] == 1;
          if (ehPredecessorTarefaA || predecessor + 1 == tarefaB)
          {
            int posicaoPredecessor = encontrarPosicaoTarefa(s, predecessor + 1);
            if (posicaoTarefaB < posicaoPredecessor)
              podeTrocar = false;
          }

          // Validando os predecessores da tarefaB
          bool ehPredecessorTarefaB = matrizRelacoesPrecedencia[predecessor][tarefaB - 1] == 1;
          if (ehPredecessorTarefaB || predecessor + 1 == tarefaB)
          {
            int posicaoPredecessor = encontrarPosicaoTarefa(s, predecessor + 1);
            if (posicaoTarefaA < posicaoPredecessor)
              podeTrocar = false;
          }
        }

        if (podeTrocar == true)
        {
          int aux = s.tarefasStartTime[0][maiorPosicao];
          s.tarefasStartTime[0][maiorPosicao] = s.tarefasStartTime[0][menorPosicao];
          s.tarefasStartTime[0][menorPosicao] = aux;
          printf("Troquei");

          memset(&s.tarefasStartTime[1], 0, sizeof(s.tarefasStartTime[1]));
          recalculoParaEscalonarSolucaoAleatoria(s);
          calcFOSemPenalizacaoPrecedencia(s);

          if (s.funObj < solucaoLocalMelhor.funObj)
            copiarSolucao(solucaoLocalMelhor, s);
        }

        podeTrocar = false;
      }
    }
  }

  copiarSolucao(s, solucaoLocalMelhor);
}

void recalculoParaEscalonarSolucaoAleatoria(Solucao &s)
{
  int maiorTempoTarefasSemEscalonar = getCalcularMaiorTempoTarefas();
  //****************matriz tarefasPorMaiorTempo
  int matrizExecutandoNoTempo[maiorTempoTarefasSemEscalonar][qtdTarefas];

  //******************* zerando a matriz
  for (int i = 0; i < maiorTempoTarefasSemEscalonar; i++)
  {
    memset(&matrizExecutandoNoTempo[i], 0, sizeof(matrizExecutandoNoTempo[i]));
  }

  //***************** Preenchendo a matriz com as tarefas uma atras da outra
  int tempoAtual = 0;

  while (tempoAtual < maiorTempoTarefasSemEscalonar)
  {
    for (int tarefaAtual = 0; tarefaAtual < qtdTarefas; tarefaAtual++)
    {
      if (s.tarefasStartTime[1][tarefaAtual] == tempoAtual)
      {
        matrizExecutandoNoTempo[tempoAtual][tarefaAtual] = 1;

        for (int tempoExecutando = tempoAtual + 1; tempoExecutando < (tempoAtual + duracao[tarefaAtual]); tempoExecutando++)
        {
          matrizExecutandoNoTempo[tempoExecutando][tarefaAtual] = 1;
        }
      }
    }
    tempoAtual++;
  }

  //***************** Tentando escalonar as tarefas, comparando duas a duas

  int somaRecursosUsando[qtdRecursos];

  int idAtual = -1;
  int idAtual_2 = -1;

  for (int tarefaAtual = 0; tarefaAtual < qtdTarefas; tarefaAtual++)
  {
    idAtual = s.tarefasStartTime[1][tarefaAtual];
    idAtual_2 = s.tarefasStartTime[1][tarefaAtual + 1];

    // verificar se os idAtual e idAtual_2 podem entrar no mesmo tempo

    // Primeiro verificar se idAtual é predecesor de idAtual_2
    bool idAtualEhPredecessor = matrizRelacoesPrecedencia[idAtual - 1][idAtual_2 - 1] == 1; // significa que é predecessor ou seja não pode entrar junto

    // Segundo conferir se tenho recurso suficiente para as duas tarefas executarem juntas
    if (!idAtualEhPredecessor) // então pode executar junto
    {
      int somaRecursosUsando[qtdRecursos];
      memset(&somaRecursosUsando, 0, sizeof(somaRecursosUsando));

      for (int i = 0; i < qtdRecursos; i++)
      {
        somaRecursosUsando[i] = consumoRecursos[idAtual][i] + consumoRecursos[idAtual_2][i];
      }
      for (int i = 0; i < qtdRecursos; i++)
      {
        if (somaRecursosUsando[i] < recursoDisponivel[i])
        {

          //***************************** PODE ENTRAR JUNTO
          // Tenho que percorrer a matriz matrizExecutandoNoTempo[][] preenchendo 1 na linha
          // da tareda idAtual_2 a partir do startime da tarefa idAtual
        }
      }
    }
  }
}

// Calculo FO (sem penalizaverificarSeEstaContidoVetorzão)
void calcFOSemPenalizacaoPrecedencia(Solucao &s)
{
  s.funObj = 0;
  int penalizacaoEstouroRecurso = calcularPenalizacaoEstouroRecurso(s);

  s.funObj = s.makespan + (PESO_PENALIZACAO_RECURSOS * penalizacaoEstouroRecurso);
}

int calcularPenalizacaoEstouroRecurso(Solucao &s)
{
  int recursoDisponivelAtual[qtdTarefas];
  memcpy(&recursoDisponivelAtual, &recursoDisponivel, sizeof(recursoDisponivel));

  int tempoAtual = 0;
  int tempoFinal = s.tarefasStartTime[1][qtdTarefas - 1];
  int matrizExecutandoNoTempo[tempoFinal][qtdTarefas];
  int penalizacaoEstouroRecurso = 0;

  // zerando a matriz
  for (int i = 0; i < tempoFinal; i++)
  {
    memset(&matrizExecutandoNoTempo[i], 0, sizeof(matrizExecutandoNoTempo[i]));
  }

  while (tempoAtual < tempoFinal)
  {
    for (int tarefaAtual = 0; tarefaAtual < qtdTarefas; tarefaAtual++)
    {
      if (s.tarefasStartTime[1][tarefaAtual] == tempoAtual)
      {
        matrizExecutandoNoTempo[tempoAtual][tarefaAtual] = 1;

        for (int tempoExecutando = tempoAtual + 1; tempoExecutando < (tempoAtual + duracao[tarefaAtual]); tempoExecutando++)
        {
          matrizExecutandoNoTempo[tempoExecutando][tarefaAtual] = 1;
        }
      }
    }
    tempoAtual++;
  }

  int somaRecursosUsando[qtdRecursos];
  int diminuiPenaliza = 0;

  for (int tempAtual = 0; tempAtual < tempoFinal - 1; tempAtual++)
  {
    memset(&somaRecursosUsando, 0, sizeof(somaRecursosUsando));

    for (int tarfAtual = 0; tarfAtual < s.qtdTarefas - 1; tarfAtual++)
    {
      if (matrizExecutandoNoTempo[tempAtual][tarfAtual] == 1)
      {
        for (int i = 0; i < qtdRecursos; i++)
        {
          somaRecursosUsando[i] = consumoRecursos[tarfAtual][i] + somaRecursosUsando[i];
        }
      }
    }

    for (int i = 0; i < qtdRecursos; i++)
    {
      if (somaRecursosUsando[i] > recursoDisponivel[i])
      {
        penalizacaoEstouroRecurso = (somaRecursosUsando[i] - recursoDisponivel[i]) + penalizacaoEstouroRecurso;
      }
    }
  }

  return penalizacaoEstouroRecurso;
}

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
