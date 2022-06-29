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


//#define MODO_DEBUG = true;


#define MAX(X, Y) ((X > Y) ? X : Y)
using namespace std;

int main(int argc, char *argv[])
{
  Solucao sol;

  lerDados("./instancias/j12060_7.sm");
  heuristicaConstrutiva(sol);

  calcFO(sol);
  //escreverSolucao(sol, "./solucao/j12060_7.sol");

   lerSolucao("./solucao/j12060_7.sol");
   calcFO(sol);
  return 0;
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
void heuristicaConstrutiva(Solucao &sol)
{
  ordenarPrecedencia();
  ordenarTarefasRecursos();
  setTarefasStartTimeOrdenadoPrecedenciaSolucaoEMakespan(sol);
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
void ordenarTarefasRecursos()
{
  int tempoAtual = 0;
  bool sairWhile = true;

  int recursoDisponivelAtual[qtdTarefas];
  memcpy(&recursoDisponivelAtual, &recursoDisponivel, sizeof(recursoDisponivel));

  while (sairWhile)
  {
    bool podeEntrar = false;
    for (int i = 0; i < qtdTarefas; i++)
    {
      int tarefaAtual = tarefasStartTimeOrdenadaPrecedencia[0][i];

      // maluco ta saindo aqui
      if (tarefasStartTimeOrdenadaPrecedencia[1][i] != -1 && (tarefasStartTimeOrdenadaPrecedencia[1][i] + duracao[i]) == tempoAtual)
      {
        for (int j = 0; j < qtdRecursos; j++)
        {
          recursoDisponivelAtual[j] = recursoDisponivelAtual[j] + consumoRecursos[tarefaAtual - 1][j];
        }
#ifdef MODO_DEBUG
        printf("\nEu tarefa:(%d) SAI no tempo %d", tarefaAtual, tempoAtual);
#endif
      }

      if (tarefasStartTimeOrdenadaPrecedencia[1][i] == -1 && todosAnterioresOrdenadosJaEntraram(i))
      {
        // maluco ta entrando aqui
        for (int j = 0; j < qtdRecursos; j++)
        {
          if (consumoRecursos[tarefaAtual - 1][j] < recursoDisponivelAtual[j])
          {
            podeEntrar = true;
          }
          else
          {
            podeEntrar = false;
            break;
          }
        }

        if (podeEntrar)
        {
          tarefasStartTimeOrdenadaPrecedencia[1][i] = tempoAtual;

          for (int j = 0; j < qtdRecursos; j++)
          {
            recursoDisponivelAtual[j] = recursoDisponivelAtual[j] - consumoRecursos[tarefaAtual - 1][j];
          }
#ifdef MODO_DEBUG
          printf("\nEu tarefa:(%d) ENTREI no tempo %d", tarefaAtual, tempoAtual);
#endif
        }
      }
    }

    tempoAtual++;
    sairWhile = !todosAnterioresOrdenadosJaEntraram(qtdTarefas - 1);
  }
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
void setTarefasStartTimeOrdenadoPrecedenciaSolucaoEMakespan(Solucao &sol)
{
  int idUltimaTarefa = tarefasStartTimeOrdenadaPrecedencia[0][qtdTarefas - 2];
  int tempoUltimaTarefaReal = tarefasStartTimeOrdenadaPrecedencia[1][qtdTarefas - 2];

  // TODO: (Dúvida) Na ultima posição do array ordenado p/ precedencia estara sempre o time do fim da ultima tarefa?
  tarefasStartTimeOrdenadaPrecedencia[1][qtdTarefas - 1] = tempoUltimaTarefaReal + duracao[idUltimaTarefa - 1] - 1;

  memcpy(&sol.tarefasStartTime, &tarefasStartTimeOrdenadaPrecedencia, sizeof(tarefasStartTimeOrdenadaPrecedencia));

  int flag = 1;
  int aux[2];

  while (flag)
  {
    flag = 0;
    for (int i = 0; i < qtdTarefas; i++)
    {
      if (sol.tarefasStartTime[0][i] > sol.tarefasStartTime[0][i + 1] && sol.tarefasStartTime[0][i + 1] != -1)
      {
        flag = 1;
        aux[0] = sol.tarefasStartTime[0][i];
        aux[1] = sol.tarefasStartTime[1][i];

        sol.tarefasStartTime[0][i] = sol.tarefasStartTime[0][i + 1];
        sol.tarefasStartTime[1][i] = sol.tarefasStartTime[1][i + 1];

        sol.tarefasStartTime[0][i + 1] = aux[0];
        sol.tarefasStartTime[1][i + 1] = aux[1];
      }
    }
  }

  sol.qtdTarefas = qtdTarefas;
  sol.makespan = tarefasStartTimeOrdenadaPrecedencia[1][qtdTarefas - 1];
}

// Calculo FO
void calcFO(Solucao &s)
{
  s.funObj = 0;
  int penalizacaoPrecedencia = calcularPenalizacaoPrecedencia(s);
  int penalizacaoEstouroRecurso = calcularPenalizacaoEstouroRecurso(s);

  s.funObj = s.makespan + (PESO_PENALIZACAO_PRECEDENCIA * penalizacaoPrecedencia) + (PESO_PENALIZACAO_RECURSOS * penalizacaoEstouroRecurso);
}

int calcularPenalizacaoPrecedencia(Solucao &s)
{
  int penalizacaoPrecedencia = 0;

  memcpy(&tarefasStartTimeOrdenadaAposSolucao[0], &s.tarefasStartTime[0], sizeof(s.tarefasStartTime[0]));
  memcpy(&tarefasStartTimeOrdenadaAposSolucao[1], &s.tarefasStartTime[1], sizeof(s.tarefasStartTime[1]));
  ordenarSolucaoStartTime();

  for (int i = 0; i < s.qtdTarefas; i++)
  {
    int idSucessor = tarefasStartTimeOrdenadaAposSolucao[0][i];

    for (int j = 0; j < s.qtdTarefas; j++)
    {
      if (matrizRelacoesPrecedencia[j][idSucessor - 1] == 1 && !verificarSeEstaContidoVetor(j + 1, i + 1, tarefasStartTimeOrdenadaAposSolucao[0]))
      {
        penalizacaoPrecedencia += 1;
      }
    }
  }
 printf("\n Penaliza precedencia: %d", penalizacaoPrecedencia);

  return penalizacaoPrecedencia;
}

//*************************************************************************** Penalizando recursos
int calcularPenalizacaoEstouroRecurso(Solucao &s)
{
  int recursoDisponivelAtual[qtdTarefas];
  memcpy(&recursoDisponivelAtual, &recursoDisponivel, sizeof(recursoDisponivel));

  int tempoAtual = 0;
  int tempoFinal = s.tarefasStartTime[1][s.qtdTarefas - 1];
  int matrizExecutandoNoTempo[qtdTarefas][tempoFinal];
  int penalizacaoEstouroRecurso = 0;

  //zerando a matriz
  for (int i = 0; i < qtdTarefas; i++)
  {
      for(int j = 0; j < tempoFinal; i++)
    memset(&matrizExecutandoNoTempo[i], 0, sizeof(matrizExecutandoNoTempo[i]));
  }

  for (int j = 0; j < tempoFinal - 1; j++) //percorrendo o tempo
    {
    //percorrendo a matriz e preenchendo
        for (int i = 0; i < s.qtdTarefas - 1; i++)
        {
            for (int j = 0; j < tempoFinal - 1; j++)
            {
                if (s.tarefasStartTime[1][i] = tempoAtual)
                {
                    matrizExecutandoNoTempo[i][j] == 1;
                    for (int j = 0; j < duracao[i]-1; j++)
                    {
                        matrizExecutandoNoTempo[i][j] == 1;
                    }
                }
            }
        }

        tempoAtual++;
    }

    printf("\n");
    printf("Matriz de tarefas executando por tempo\n");
    //imprimindo matriz
    for (int i = 0; i < s.qtdTarefas - 1; i++)
        {
            for (int j = 0; j < tempoFinal - 1; j++)
            {
                printf("%d",matrizExecutandoNoTempo[i][j]);
            }
            printf("\n");
        }


    //percorrendo a matriz somando os recusos
    int somaRecursosUsando =0;
    int diminuiPenaliza =0;
       for (int j = 0; j < tempoFinal - 1; j++)
        {
             for (int i = 0; i < s.qtdTarefas - 1; i++)
            {
                if (matrizExecutandoNoTempo[i][j] = 1)//quer dizer que tem tarefa usando o recurso
                {
                     //verificar quantos recursos minha tarefa i usa
                     somaRecursosUsando = somaRecursosUsando + consumoRecursos[i][MAX_QTD_RECURSO]; //verificar
                }

            }
            //dimunir o recurso da tarefa do meu recurso disponivel
            
            diminuiPenaliza = (somaRecursosUsando - recursoDisponivelAtual[qtdRecursos]); // verificar
            //guardar o resultado = penalização
        }

        penalizacaoEstouroRecurso = diminuiPenaliza;

 printf("\n Penaliza:(%d)", penalizacaoEstouroRecurso);

  return penalizacaoEstouroRecurso;
}

void ordenarSolucaoStartTime()
{
  int flag = 1;
  int aux0, aux1;
  while (flag)
  {
    flag = 0;
    for (int i = 0; i < qtdTarefas; i++)
    {
      if (tarefasStartTimeOrdenadaAposSolucao[1][i] > tarefasStartTimeOrdenadaAposSolucao[1][i + 1] && tarefasStartTimeOrdenadaAposSolucao[1][i + 1] != -1)
      {
        flag = 1;
        aux0 = tarefasStartTimeOrdenadaAposSolucao[0][i];
        aux1 = tarefasStartTimeOrdenadaAposSolucao[1][i];

        tarefasStartTimeOrdenadaAposSolucao[0][i] = tarefasStartTimeOrdenadaAposSolucao[0][i + 1];
        tarefasStartTimeOrdenadaAposSolucao[1][i] = tarefasStartTimeOrdenadaAposSolucao[1][i + 1];

        tarefasStartTimeOrdenadaAposSolucao[0][i + 1] = aux0;
        tarefasStartTimeOrdenadaAposSolucao[1][i + 1] = aux1;
      }
    }
  }
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
