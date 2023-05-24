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

#define MODO_DEBUG true

#define MAX(X, Y) ((X > Y) ? X : Y)
using namespace std;

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

bool atendeProximas(int idTarefa, const int inicio, int fim)
{
  for (int i = inicio; i < fim; i++)
  {
    for (int j = 0; j < qtdRecursos; j++)
    {
      int consumoTarefaParaEntrar = consumoRecursos[idTarefa][j];
      int consumoAtualDoTempo = matriz_solucao_recursos_consumidos_tempo[j][i];
      if (consumoAtualDoTempo + consumoTarefaParaEntrar > recursoDisponivel[j])
      {
        return false;
      }
    }
  }

  return true;
}

int getStarTimeShiftTimeSolucaoPorCausaRecurso(int idTarefa)
{
  int startTime = getStartTimeTarefa(idTarefa);

  for (int i = startTime; i < TEMPO_MAXIMO; i++)
  {
    int qtdRecursosAtendidos = 0;
    for (int j = 0; j < qtdRecursos; j++)
    {
      int consumoTarefaParaEntrar = consumoRecursos[idTarefa][j];
      int duracaoTarefaParaEntrar = duracao[idTarefa];
      int consumoAtualDoTempo = matriz_solucao_recursos_consumidos_tempo[j][i];

      if ((consumoAtualDoTempo + consumoTarefaParaEntrar <= recursoDisponivel[j]) && atendeProximas(idTarefa, i, i + duracaoTarefaParaEntrar))
      {
        qtdRecursosAtendidos++;
      }
    }

    if (qtdRecursosAtendidos == qtdRecursos)
    {
      if (startTime != i)
      {
#ifdef MODO_DEBUG
        printf("\nEra %d, agr é %d", startTime, i);
#endif
      }
      return i;
    }
  }
}

void inserirTarefaNaSolucao(int idTarefa)
{
  int startTime = getStarTimeShiftTimeSolucaoPorCausaRecurso(idTarefa);

  push_array(idTarefa, matriz_solucao_com_tempos[0], qtdTarefas);
  push_array(startTime, matriz_solucao_com_tempos[1], qtdTarefas);
  push_array(startTime + duracao[idTarefa], matriz_solucao_com_tempos[2], qtdTarefas);

  // printf("Task: %d | Start time: %d", idTarefa, startTime);

#ifdef MODO_DEBUG
  printf("\n------------- Matriz de solução -------------\n");
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < qtdTarefas; j++)
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
#endif
}

int main(int argc, char *argv[])
{
  int seed = time(NULL); // time(NULL); 1684285591
  srand(seed);

  const char *instancias[] = {
      "j301_1",
      "j301_2",
      "j609_3",
      "j609_8",
      "j1206_4",
      "j1207_2",
  };

  double tempo_limite = 5 * 60;
  double tempo_melhor, tempo_total;

  double alfa = 0.5;

  size_t arraySize = sizeof(instancias) / sizeof(instancias[0]);
  for (size_t i = 0; i < arraySize; ++i)
  {
    std::string instancia = instancias[i];
    lerDados("./instancias/" + instancia + ".sm");
    heuristicaGrasp(alfa, tempo_limite, tempo_melhor, tempo_total, instancia, seed);
  }
}

void heuristicaGrasp(double alfa, const double tempo_limite, double &tempo_melhor, double &tempo_total, std::string instancia, int seed)
{
  Solucao solucao_melhor_global;
  solucao_melhor_global.funObj = 99999;

  clock_t hI, hF;
  tempo_total = tempo_melhor = 0;
  hI = clock();

  time_t startTime = time(NULL);
  while (time(NULL) - startTime < tempo_limite)
  {
    // TODO Trazer esse cara para junto do handleHeuristicaConstrutiva(alfa);
    handleOrdenarTarefasPorSucessor();

    zerar_vetor(matriz_solucao_com_tempos[0], qtdTarefas, -1);
    zerar_vetor(matriz_solucao_com_tempos[1], qtdTarefas, -1);
    zerar_vetor(matriz_solucao_com_tempos[2], qtdTarefas, -1);

    zerar_vetor(matriz_tarefas_escalonamento[0], qtdTarefas, -1);
    zerar_vetor(matriz_tarefas_escalonamento[1], qtdTarefas, 0);

    for (int i = 0; i < qtdRecursos; i++)
    {
      zerar_vetor(matriz_solucao_recursos_consumidos_tempo[i], TEMPO_MAXIMO, 0);
    }

    for (int i = 0; i < qtdTarefas; i++)
    {
      zerar_vetor(matriz_solucao_com_tempos[i], TEMPO_MAXIMO, -1);
    }

    memcpy(&matriz_tarefas_escalonamento[0], &tarefaQtdSucessores[0], sizeof(tarefaQtdSucessores[0]));

    handleHeuristicaConstrutiva(alfa);

    Solucao solucao_construtiva;
    memcpy(&solucao_construtiva.matriz_solucao_com_tempos[0], &matriz_solucao_com_tempos[0], sizeof(matriz_solucao_com_tempos[0]));
    memcpy(&solucao_construtiva.matriz_solucao_com_tempos[1], &matriz_solucao_com_tempos[1], sizeof(matriz_solucao_com_tempos[1]));
    memcpy(&solucao_construtiva.matriz_solucao_com_tempos[2], &matriz_solucao_com_tempos[2], sizeof(matriz_solucao_com_tempos[2]));

    for (int j = 0; j < qtdRecursos; j++)
    {
      memcpy(&solucao_construtiva.matriz_solucao_recursos_consumidos_tempo[j], &matriz_solucao_recursos_consumidos_tempo[j], sizeof(matriz_solucao_recursos_consumidos_tempo[j]));
    }
    solucao_construtiva.funObj = calcularFO(solucao_construtiva);

    // Busca local
    double temp_inicial = 100;
    double temp_final = 0.01;
    double taxa_resf = 0.995;
    int num_sol_viz = 100;
    simulated_annealing(solucao_construtiva, temp_inicial, temp_final, taxa_resf, num_sol_viz);

    if (solucao_construtiva.funObj < solucao_melhor_global.funObj)
    {
      hF = clock();
      tempo_melhor = ((double)(hF - hI)) / CLOCKS_PER_SEC;
      copiarSolucao(solucao_melhor_global, solucao_construtiva);
      escreverMetricas(solucao_melhor_global, "./metricas/" + instancia + ".metric", tempo_melhor, seed);
    }
  }
}

void handleHeuristicaConstrutiva(double alfa)
{
  int qtdEscalonamento = qtdTarefas;

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

      if (!todosPredecessoresJaEntraram(idTarefa, qtdTarefas, matriz_solucao_com_tempos[0]) && idTarefa != -1)
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
        if (matriz_tarefas_escalonamento[1][i] > matriz_tarefas_escalonamento[1][i + 1] && (i + 1) < qtdEscalonamento)
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
#ifdef MODO_DEBUG
    printf("\n\nTarefa escolhida: %d", tarefaEscolhida + 1);
#endif
    // Fazendo valer regra de RECURSO
    // não pode estrapolar recursoDisponivel
    // gasto por cada tarefa consumoRecursos
    int idColuna = findIndexByValue(tarefaEscolhida, qtdEscalonamento, matriz_tarefas_escalonamento[0]);
    inserirTarefaNaSolucao(tarefaEscolhida);

    int indexSolucao = findIndexByValue(tarefaEscolhida, qtdTarefas, matriz_solucao_com_tempos[0]);
    int startTimeTarefaEscolhida = matriz_solucao_com_tempos[1][indexSolucao];
    // preencherMatrizBinariaTarefaTempo(tarefaEscolhida, startTimeTarefaEscolhida);
    preencherMatrizRecursoTempo(tarefaEscolhida, startTimeTarefaEscolhida);

    int n = qtdEscalonamento;
    // Remove a coluna com base no valor encontrado
    if (idColuna != -1)
    {
      for (int j = idColuna; j < n - 1; j++)
      {
        matriz_tarefas_escalonamento[0][j] = matriz_tarefas_escalonamento[0][j + 1];
        matriz_tarefas_escalonamento[1][j] = matriz_tarefas_escalonamento[1][j + 1];
      }
      n--;
    }

    qtdEscalonamento--;
  }
  // TODO: Validar inclusão aqui do código para pegar tempo do maior na solução
  inserirTarefaNaSolucao(11);

#ifdef MODO_DEBUG
  printf("\n------------- Matriz Recurso Tempo -------------\n");
  for (int i = 0; i < qtdRecursos; i++)
  {
    for (int j = 0; j < 50; j++)
    {
      int value = matriz_solucao_recursos_consumidos_tempo[i][j];
      printf("%d ", value);
    }
    printf("\n");
  }
#endif
}

void preencherMatrizRecursoTempo(int tarefa, int startTime)
{
  int endTime = startTime + duracao[tarefa];
  for (int i = startTime; i <= endTime - 1; i++)
    for (int recurso = 0; recurso <= qtdRecursos; recurso++)
    {
      {

        int consumoAtual = matriz_solucao_recursos_consumidos_tempo[recurso][i];

        int consumoTarefa = consumoRecursos[tarefa][recurso];
        matriz_solucao_recursos_consumidos_tempo[recurso][i] = consumoAtual + consumoTarefa;
      }
    }
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

tSucessores getSucessores(const int tarefa)
{
  tSucessores sucessores;
  sucessores.qtdSucessores = 0;

  for (int j = 0; j < qtdTarefas; j++)
  {
    if (matrizRelacoesSucessores[tarefa][j] == 1)
    {
      sucessores.list[sucessores.qtdSucessores] = j;
      sucessores.qtdSucessores++;
    }
  }

  return sucessores;
}

int calcularFO(Solucao &s)
{

  int makespan = 0;
  for (int i = 0; i < qtdTarefas; i++)
  {
    makespan = MAX(s.matriz_solucao_com_tempos[2][i], makespan);
  }

  int recurso = calcularFORecurso(s);
  int precedencia = calcularFOPrecedencia(s);

  int penalizacao = (recurso * PESO_PENALIZACAO_RECURSOS) + (precedencia * PESO_PENALIZACAO_PRECEDENCIA);
  // printf("\n Penalização : %d \n", penalizacao);

  return penalizacao + makespan;
}

int calcularFORecurso(Solucao &s)
{
  int makespan = s.matriz_solucao_com_tempos[3][qtdTarefas];

  int penalizacaoRecurso = 0;
  for (int i = 0; i < makespan; i++)
  {
    for (int j = 0; j < qtdRecursos; j++)
    {
      if (s.matriz_solucao_recursos_consumidos_tempo[j][i] > recursoDisponivel[j])
      {
        penalizacaoRecurso++;
      }
    }
  }

  return penalizacaoRecurso;
}

int calcularFOPrecedencia(Solucao &s)
{
  int penalizacaoPrecedencia = 0;

  for (int i = 0; i < qtdTarefas; i++)
  {
    int idTarefa = s.matriz_solucao_com_tempos[0][i];

    if (!todosPredecessoresJaEntraram(idTarefa, i, s.matriz_solucao_com_tempos[0]))
    {
      penalizacaoPrecedencia++;
    }
  }

  return penalizacaoPrecedencia;
}

bool todosPredecessoresJaEntraram(int idTarefa, int qtdTarefasAnalizar, int *vetor)
{
  tPrececessores predecessores = getPredecessores(idTarefa);

  for (int j = 0; j < predecessores.qtdPrecedessores; j++)
  {
    int predecessor = predecessores.list[j];

    // A segunda parte da condicional é para
    if (!includes_array(predecessor, vetor, qtdTarefasAnalizar) && predecessor != 0)
    {
      return false;
    }
  }

  return true;
}

Solucao simulated_annealing(Solucao solucao_inicial, double temp_inicial, double temp_final, double taxa_resf, int num_sol_viz)
{
  double temp = temp_inicial;
  Solucao solucao_melhor = solucao_inicial;
  Solucao solucao_atual = solucao_inicial;

  while (temp <= temp_final)
  {
    for (int i = 0; i < num_sol_viz; i++)
    {
      Solucao solucao_vizinha = gerar_vizinho_tempo(solucao_atual);

      int dif_fo = solucao_vizinha.funObj - solucao_atual.funObj;

      if (dif_fo < 0)
      {
        copiarSolucao(solucao_atual, solucao_vizinha);
      }
      else
      {
        double probability = exp(-dif_fo / temp);

        double num_aleatorio = (double)rand() / (double)RAND_MAX;
        if (num_aleatorio < probability)
        {
          copiarSolucao(solucao_atual, solucao_vizinha);
        }
      }
      if (solucao_vizinha.funObj < solucao_melhor.funObj)
      {
        copiarSolucao(solucao_melhor, solucao_vizinha);
      }
    }

    temp = temp * taxa_resf;
  }

  return solucao_melhor;
}

bool algum_predecessores_nao_entraram(const int tempo, tPrececessores predecessores, Solucao solucao)
{
  for (int i = 0; i < predecessores.qtdPrecedessores; i++)
  {
    int tarefa_atual = predecessores.list[i];
    int indexMatriz = findIndexByValue(tarefa_atual, qtdTarefas, solucao.matriz_solucao_com_tempos[0]);

    if (solucao.matriz_solucao_com_tempos[2][indexMatriz] > tempo)
    {
      return true;
      break;
    }

    tPrececessores predecessores_do_predecessor = getPredecessores(tarefa_atual);
    if (algum_predecessores_nao_entraram(tempo, predecessores_do_predecessor, solucao))
    {
      return true;
      break;
    }
  }

  return false;
}

Solucao gerar_vizinho_tempo(Solucao solucao_atual)
{
  // int idTarefa = rand() % (qtdTarefas - 1) + 1;
  int idTarefa = rand() % (qtdTarefas - qtdTarefas / 2 - 1) + qtdTarefas / 2 + 1;
  int indexMatriz = findIndexByValue(idTarefa, qtdTarefas, solucao_atual.matriz_solucao_com_tempos[0]);
  int start_time_tarefa = solucao_atual.matriz_solucao_com_tempos[0][indexMatriz];

  tPrececessores predecessores = getPredecessores(idTarefa);
  int tempo = 0;
  while (tempo < start_time_tarefa)
  {
    if (!algum_predecessores_nao_entraram(tempo, predecessores, solucao_atual))
    {
      printf("posso entrar");
    }

    tempo++;
  }
}

Solucao gerar_vizinho(Solucao solucao_atual)
{
  // printf("Gerando vizinho");

  int aux[2];

  bool pode_entrar_alguem = false;

  int num_aleat_1;
  int num_aleat_2;
  int onde_pode_entrar[qtdTarefas];
  while (!pode_entrar_alguem)
  {
    num_aleat_1 = rand() % (qtdTarefas - 1) + 1;
    num_aleat_2 = (rand() % (11 - num_aleat_1 + 1)) + num_aleat_1;

    while (num_aleat_1 == num_aleat_2)
    {
      num_aleat_2 = rand() % (qtdTarefas - 1) + 1;
    }

    int idTarefa = solucao_atual.matriz_solucao_com_tempos[0][num_aleat_1];

    tSucessores sucessores = getSucessores(idTarefa);

    zerar_vetor(onde_pode_entrar, qtdTarefas, -1);
    for (int i = num_aleat_1 + 1; i < num_aleat_2; i++)
    {
      if (!includes_array(solucao_atual.matriz_solucao_com_tempos[0][i], sucessores.list, sucessores.qtdSucessores))
      {
        push_array(i, onde_pode_entrar, qtdTarefas);
      }
      else
      {
        break;
      }
    }

    bool pode_entrar_alguem = false;
    for (int i = 0; i < qtdTarefas; i++)
    {
      if (onde_pode_entrar[i] != -1)
      {
        pode_entrar_alguem = true;
      }
    }
  }

  int num_aleat_2_antes_sucessor = onde_pode_entrar[0];

  aux[0] = solucao_atual.matriz_solucao_com_tempos[0][num_aleat_2_antes_sucessor];
  aux[1] = solucao_atual.matriz_solucao_com_tempos[1][num_aleat_2_antes_sucessor];

  remover_uso_recursos(solucao_atual, num_aleat_1);
  remover_uso_recursos(solucao_atual, num_aleat_2_antes_sucessor);

  solucao_atual.matriz_solucao_com_tempos[0][num_aleat_2_antes_sucessor] = solucao_atual.matriz_solucao_com_tempos[0][num_aleat_1];
  solucao_atual.matriz_solucao_com_tempos[1][num_aleat_2_antes_sucessor] = solucao_atual.matriz_solucao_com_tempos[1][num_aleat_1];
  solucao_atual.matriz_solucao_com_tempos[2][num_aleat_2_antes_sucessor] = solucao_atual.matriz_solucao_com_tempos[1][num_aleat_1] + duracao[solucao_atual.matriz_solucao_com_tempos[0][num_aleat_1]];

  solucao_atual.matriz_solucao_com_tempos[0][num_aleat_1] = aux[0];
  solucao_atual.matriz_solucao_com_tempos[1][num_aleat_1] = aux[1];
  solucao_atual.matriz_solucao_com_tempos[2][num_aleat_1] = aux[1] + duracao[aux[0]];

  inserir_uso_recursos(solucao_atual, num_aleat_1);
  inserir_uso_recursos(solucao_atual, num_aleat_2_antes_sucessor);

  int maior_end_time = 0;
  for (int i = 0; i < qtdTarefas; i++)
  {
    maior_end_time = MAX(solucao_atual.matriz_solucao_com_tempos[2][i], maior_end_time);
  }

  solucao_atual.funObj = calcularFO(solucao_atual);
  return solucao_atual;
}

void remover_uso_recursos(Solucao &solucao_atual, int index_tarefa)
{
  int tarefa = solucao_atual.matriz_solucao_com_tempos[0][index_tarefa];
  int start_time = solucao_atual.matriz_solucao_com_tempos[1][index_tarefa];
  int end_time = solucao_atual.matriz_solucao_com_tempos[2][index_tarefa];

  for (int i = start_time; i < (start_time + duracao[tarefa]); i++)
  {
    for (int j = 0; j < qtdRecursos; j++)
    {
      solucao_atual.matriz_solucao_recursos_consumidos_tempo[j][i] = solucao_atual.matriz_solucao_recursos_consumidos_tempo[j][i] - consumoRecursos[tarefa][j];
    }
  }
}

void inserir_uso_recursos(Solucao &solucao_atual, int index_tarefa)
{
  int tarefa = solucao_atual.matriz_solucao_com_tempos[0][index_tarefa];
  int start_time = solucao_atual.matriz_solucao_com_tempos[1][index_tarefa];
  int end_time = solucao_atual.matriz_solucao_com_tempos[2][index_tarefa];

  for (int i = start_time; i < (start_time + duracao[tarefa]); i++)
  {
    for (int j = 0; j < qtdRecursos; j++)
    {
      solucao_atual.matriz_solucao_recursos_consumidos_tempo[j][i] = solucao_atual.matriz_solucao_recursos_consumidos_tempo[j][i] + consumoRecursos[tarefa][j];
    }
  }
}

void copiarSolucao(Solucao &solucaoNova, Solucao &solucaoAntiga)
{
  memcpy(&solucaoNova, &solucaoAntiga, sizeof(solucaoAntiga));
}

void escreverSolucao(Solucao &solucao, std::string arq)
{
  FILE *arquivo = fopen(arq.c_str(), "w");

  fprintf(arquivo, "FO: ");
  fprintf(arquivo, "%d\n", solucao.funObj);

  fprintf(arquivo, "Makespan: ");
  fprintf(arquivo, "%d\n", solucao.funObj);

  fprintf(arquivo, "------------------\n");
  fprintf(arquivo, "Job Start Time\n");

  for (int i = 0; i < qtdTarefas; i++)
  {
    fprintf(arquivo, "%d %d\n",
            solucao.matriz_solucao_com_tempos[0][i],
            solucao.matriz_solucao_com_tempos[1][i]);
  }
  fclose(arquivo);
}

void escreverMetricas(Solucao &solucao, std::string arq, double tempo_gasto, int seed)
{
  FILE *arquivo = fopen(arq.c_str(), "a");

  fprintf(arquivo, "FO: %d | Tempo gasto:  %.5fs | Seed: %d", solucao.funObj, tempo_gasto, seed);

  fprintf(arquivo, "\nSolucao: \n");
  for (int i = 0; i < qtdTarefas; i++)
  {
    fprintf(arquivo, "%d ", solucao.matriz_solucao_com_tempos[0][i]);
  }
  fprintf(arquivo, "\n");

  for (int i = 0; i < qtdTarefas; i++)
  {
    fprintf(arquivo, "%d ", solucao.matriz_solucao_com_tempos[1][i]);
  }

  fprintf(arquivo, "\n\n");
  fclose(arquivo);
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
