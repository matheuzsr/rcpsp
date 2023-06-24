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

// #define MODO_DEBUG true

#define MAX(X, Y) ((X > Y) ? X : Y)
using namespace std;

int main(int argc, char *argv[])
{
  int exec = atoi(argv[1]);
  printf("Exec %d", exec);
  double alfa = 0.85;

  const char *instancias[] = {
      // "j30/j301_1",
      // "j30/j301_2",
      // "j30/j3024_9",
      // "j30/j3024_10",
      // "j30/j3048_9",
      // "j30/j3048_10",

      // "j60/j601_1",
      // "j60/j601_2",
      // "j60/j6024_9",
      // "j60/j6024_10",
      // "j60/j6048_9",
      // "j60/j6048_10",

      // "j90/j901_1",
      // "j90/j901_2",
      // "j90/j9024_9",
      // "j90/j9024_10",
      // "j90/j9048_9",
      // "j90/j9048_10",

      "j120/j1201_1",
      "j120/j1201_2",
      "j120/j12030_9",
      "j120/j12030_10",
      "j120/j12060_9",
      "j120/j12060_10",

      // "j10",
  };

  const int x = 60 * 60;
  const int tempo_instancias[] = {
      x,
      x,
      x,
      x,
      x,
      x,

      // 2 * x,
      // 2 * x,
      // 2 * x,
      // 2 * x,
      // 2 * x,
      // 2 * x,

      // 3 * x,
      // 3 * x,
      // 3 * x,
      // 3 * x,
      // 3 * x,
      // 3 * x,

      // 4 * x,
      // 4 * x,
      // 4 * x,
      // 4 * x,
      // 4 * x,
      // 4 * x,
  };

  int qtdExecucoes = 2;
  int tempo_limite;
  double tempo_melhor, tempo_total;

  // for (int i = 1; i < qtdExecucoes; i++)
  // {
  int seed = time(NULL); // time(NULL); 1684285591
  srand(seed);

  size_t qtdInstancias = sizeof(instancias) / sizeof(instancias[0]);

  for (int indexInstancia = 0; indexInstancia < qtdInstancias; indexInstancia++)
  {
    printf("\nInst: %d/%d", indexInstancia + 1, qtdInstancias);
    std::cout.flush();

    std::string instancia = instancias[indexInstancia];
    tempo_limite = tempo_instancias[indexInstancia];

    lerDados("./instancias/" + instancia + ".sm");

    std::string file_name = "./metricas/exec_" + std::to_string(exec + 1) + "/" + instancia + ".sol";
    escreverSeedMetricas(file_name, seed);

    Solucao solucao_melhor_global;
    solucao_melhor_global.funObj = 9999999;

    heuristicaGrasp(alfa, tempo_limite, tempo_melhor, tempo_total, solucao_melhor_global, file_name);

    escreverFinalMetricas(solucao_melhor_global, file_name, tempo_melhor);
  }
  // }
}

void heuristicaGrasp(double alfa, const double tempo_limite, double &tempo_melhor, double &tempo_total, Solucao &solucao_melhor_global, std::string file_name)
{
#ifdef MODO_DEBUG
  printf("\n\n>>> (Heurística GRASP) Executando...");
#endif

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

    double tempo_atual;
    if (solucao_construtiva.funObj < solucao_melhor_global.funObj)
    {
      hF = clock();
      double tempo_atual = ((double)(hF - hI)) / CLOCKS_PER_SEC;
      copiarSolucao(solucao_melhor_global, solucao_construtiva);
      tempo_melhor = tempo_atual;
    }

    double temp_inicial = 0.01 * qtdTarefas;
    const double temp_final = 0.01;
    double taxa_resf = 0.995000;
    int num_sol_viz = 1.2 * qtdTarefas;
    Solucao solucao_apos_SA = simulated_annealing(solucao_construtiva, temp_inicial, temp_final, taxa_resf, num_sol_viz, startTime, tempo_limite, solucao_melhor_global.funObj, tempo_melhor, hI);

    if (solucao_apos_SA.funObj < solucao_melhor_global.funObj)
    {
      copiarSolucao(solucao_melhor_global, solucao_apos_SA);
    }
  }
}

void handleHeuristicaConstrutiva(double alfa)
{
#ifdef MODO_DEBUG
  // printf("\n>>> (Heurística Construtiva) Executando...");
#endif
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

    // TODO: Revisar aqui
    if (alfa == 0)
    {
      qtdEscalonamentoLRC = 1;
    }

    // Fazendo valer regra de PRECEDENCIA
    int tempoTarefaEscolhida = PESO_PENALIZACAO_PRECEDENCIA;
    int tarefaEscolhida = -1;
    while (tempoTarefaEscolhida >= PESO_PENALIZACAO_PRECEDENCIA)
    {

      int idTarefaEscolhida = ((rand() % qtdEscalonamentoLRC) - 1) + 1;

      tarefaEscolhida = matriz_tarefas_escalonamento[0][idTarefaEscolhida];
      tempoTarefaEscolhida = matriz_tarefas_escalonamento[1][idTarefaEscolhida];
    }
#ifdef MODO_DEBUG
    // printf("\n\nTarefa escolhida: %d", tarefaEscolhida + 1);
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
  inserirTarefaNaSolucao(qtdTarefas - 1);

#ifdef MODO_DEBUG
  // printf("\n------------- Matriz Recurso Tempo -------------\n");
  for (int i = 0; i < qtdRecursos; i++)
  {
    for (int j = 0; j < 50; j++)
    {
      int value = matriz_solucao_recursos_consumidos_tempo[i][j];
      // printf("%d ", value);
    }
    // printf("\n");
  }
#endif
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
        // printf("\nEra %d, agr é %d", startTime, i);
#endif
      }
      return i;
    }
  }

  return -1;
}

void inserirTarefaNaSolucao(int idTarefa)
{
  int startTime = getStarTimeShiftTimeSolucaoPorCausaRecurso(idTarefa);

  if (startTime != -1)
  {
    push_array(idTarefa, matriz_solucao_com_tempos[0], qtdTarefas);
    push_array(startTime, matriz_solucao_com_tempos[1], qtdTarefas);
    push_array(startTime + duracao[idTarefa], matriz_solucao_com_tempos[2], qtdTarefas);
  }
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

    memset(&entraramList, -1, sizeof(entraramList));
    maiorDuracaoTarefas[1][i] = calcularTempoTarefa(idAtual, predecessores.list, qtdPredecessores);
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
        // printf(" %d(%d) |", predecessor + 1, duracao[predecessor]);
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
  // //printf("\n Penalização : %d \n", penalizacao);

  return penalizacao + makespan;
}

int calcularFORecurso(Solucao &s)
{
  int makespan = s.matriz_solucao_com_tempos[2][qtdTarefas - 1];

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

    if (!todosPredecessoresJaEntraramBaseadoTempo(idTarefa, i, s))
    {
      penalizacaoPrecedencia++;
    }
  }

  return penalizacaoPrecedencia;
}

bool todosPredecessoresJaEntraramBaseadoTempoNovoGerarVizinho(int idTarefa, int indexTarefa, int new_starttime, Solucao s)
{
  tPrececessores predecessores = getPredecessores(idTarefa);

  for (int j = 0; j < predecessores.qtdPrecedessores; j++)
  {
    int predecessor = predecessores.list[j];
    int index_predecessor = findIndexByValue(predecessor, qtdTarefas, s.matriz_solucao_com_tempos[0]);
    int end_time_predecessor = s.matriz_solucao_com_tempos[2][index_predecessor];

    // A segunda parte da condicional é para
    if (new_starttime < end_time_predecessor && predecessor != 0)
    {
      return false;
    }
  }

  return true;
}

bool todosPredecessoresJaEntraramBaseadoTempo(int idTarefa, int indexTarefa, Solucao s)
{
  int starttime_tarefa = s.matriz_solucao_com_tempos[1][indexTarefa];
  tPrececessores predecessores = getPredecessores(idTarefa);

  for (int j = 0; j < predecessores.qtdPrecedessores; j++)
  {
    int predecessor = predecessores.list[j];
    int index_predecessor = findIndexByValue(predecessor, qtdTarefas, s.matriz_solucao_com_tempos[0]);
    int end_time_predecessor = s.matriz_solucao_com_tempos[2][index_predecessor];

    // A segunda parte da condicional é para
    if (starttime_tarefa < end_time_predecessor && predecessor != 0)
    {
      return false;
    }
  }

  return true;
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

Solucao simulated_annealing(Solucao solucao_inicial, double temp_inicial, double temp_final, double taxa_resf, int num_sol_viz, double start_time, double tempo_limite, int fo_melhor_global, double &tempo_melhor, clock_t hI)
{
#ifdef MODO_DEBUG
  printf("\n>>> (Simulated annealing) Executando...");
#endif
  double temp = temp_inicial;

  Solucao solucao_vizinha;
  Solucao solucao_melhor;
  Solucao solucao_atual;
  copiarSolucao(solucao_melhor, solucao_inicial);
  copiarSolucao(solucao_atual, solucao_inicial);

  while (temp >= temp_final)
  {
    for (int i = 0; i < num_sol_viz; i++)
    {
      solucao_vizinha = gerar_vizinho_tempo_novo(solucao_atual);
      int dif_fo = solucao_vizinha.funObj - solucao_atual.funObj;

      if (time(NULL) > (start_time + tempo_limite))
        goto endLoops;

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
        if (solucao_vizinha.funObj < fo_melhor_global)
        {
          clock_t hF = clock();
          tempo_melhor = ((double)(hF - hI)) / CLOCKS_PER_SEC;
        }

        // Salvar aqui ja no arquivo se for melhor global
        copiarSolucao(solucao_melhor, solucao_vizinha);
        break;
      }
    }

    temp = temp * taxa_resf;
  }

endLoops:
  return solucao_melhor;
}

Solucao gerar_vizinho_tempo_novo(Solucao solucao_atual)
{
  Solucao vizinho;
  copiarSolucao(vizinho, solucao_atual);

  bool flag = true;
  int indexAux;
  int startAux;
  int endAux;
  while (flag)
  {
    flag = false;
    for (int i = 0; i < qtdTarefas - 1; i++)
    {
      if (vizinho.matriz_solucao_com_tempos[1][i + 1] < vizinho.matriz_solucao_com_tempos[1][i])
      {
        flag = true;
        indexAux = vizinho.matriz_solucao_com_tempos[0][i + 1];
        startAux = vizinho.matriz_solucao_com_tempos[1][i + 1];
        endAux = vizinho.matriz_solucao_com_tempos[2][i + 1];

        vizinho.matriz_solucao_com_tempos[0][i + 1] = vizinho.matriz_solucao_com_tempos[0][i];
        vizinho.matriz_solucao_com_tempos[1][i + 1] = vizinho.matriz_solucao_com_tempos[1][i];
        vizinho.matriz_solucao_com_tempos[2][i + 1] = vizinho.matriz_solucao_com_tempos[2][i];

        vizinho.matriz_solucao_com_tempos[0][i] = indexAux;
        vizinho.matriz_solucao_com_tempos[1][i] = startAux;
        vizinho.matriz_solucao_com_tempos[2][i] = endAux;
      }
    }
  }

  int num_aleat_1 = rand() % (qtdTarefas - 1) + 1;

  int tarefa_1 = vizinho.matriz_solucao_com_tempos[0][num_aleat_1];

  Sucessores sucessores = getSucessores(tarefa_1);

  int tarefas_para_trocar[qtdTarefas];
  int qtd_tarefas_para_trocar = 0;
  zerar_vetor(tarefas_para_trocar, qtdTarefas, -1);

  for (int i = num_aleat_1 + 1; i < qtdTarefas - 1; i++)
  {
    int tarefa_para_trocar = vizinho.matriz_solucao_com_tempos[0][i];

    if (!includes_array(tarefa_para_trocar, sucessores.list, sucessores.qtdSucessores))
    {
      push_array(i, tarefas_para_trocar, qtdTarefas);
      qtd_tarefas_para_trocar++;
    }
  }

  int index_tarefa_definida_trocar = tarefas_para_trocar[rand() % (qtd_tarefas_para_trocar + 1)];
  int tarefa_definida_trocar = vizinho.matriz_solucao_com_tempos[0][index_tarefa_definida_trocar];

  // Fazendo shift no array
  for (int i = num_aleat_1; i < index_tarefa_definida_trocar; i++)
  {
    vizinho.matriz_solucao_com_tempos[0][i] = vizinho.matriz_solucao_com_tempos[0][i + 1];
  }
  vizinho.matriz_solucao_com_tempos[0][index_tarefa_definida_trocar] = tarefa_1;

  // Arrumando solucao
  arrumarSolucao(vizinho);

  vizinho.funObj = calcularFO(vizinho);

  return vizinho;
}

void arrumarSolucao(Solucao &solucao)
{
  int tempo = 0;
  bool geral_entrou = false, pode_entrar = false;
  int index_tarefa_para_entrar = 0;

  int makespan = solucao.funObj;
  solucao.funObj = 0;
  zerar_vetor(solucao.matriz_solucao_com_tempos[1], qtdTarefas, -1);
  zerar_vetor(solucao.matriz_solucao_com_tempos[2], qtdTarefas, -1);
  for (int j = 0; j < qtdRecursos; j++)
    zerar_vetor(solucao.matriz_solucao_recursos_consumidos_tempo[j], makespan, 0);

  // if(tarefa == -1) {
  //   break;
  // }

  while (!geral_entrou)
  {
    geral_entrou = true;
    pode_entrar = true;

    for (int index_tarefa = index_tarefa_para_entrar; index_tarefa < qtdTarefas; index_tarefa++)
    {
      int tarefa_atual = solucao.matriz_solucao_com_tempos[0][index_tarefa];

      for (int i = 0; i < qtdRecursos; i++)
      {
        // Validando precedencia
        if (!todosPredecessoresJaEntraramBaseadoTempoNovoGerarVizinho(tarefa_atual, index_tarefa, tempo, solucao))
        {
          pode_entrar = false;
          break;
        }

        int consumo_tempo_atual = solucao.matriz_solucao_recursos_consumidos_tempo[i][tempo];
        if ((consumo_tempo_atual + consumoRecursos[tarefa_atual][i]) > recursoDisponivel[i])
        {
          pode_entrar = false;
          break;
        }

        for (int tempo_j = tempo; tempo_j < tempo + duracao[tarefa_atual]; tempo_j++)
        {
          for (int rec_k = 0; rec_k < qtdRecursos; rec_k++)
          {
            int consumo_tempo_atual = solucao.matriz_solucao_recursos_consumidos_tempo[rec_k][tempo_j];
            if ((consumo_tempo_atual + consumoRecursos[tarefa_atual][rec_k]) > recursoDisponivel[rec_k])
            {
              pode_entrar = false;
              break;
            }
          }
        }
      }

      if (!pode_entrar)
        break;

      int start_time = tempo;
      int end_time = tempo + duracao[tarefa_atual];
      solucao.matriz_solucao_com_tempos[1][index_tarefa] = start_time;
      solucao.matriz_solucao_com_tempos[2][index_tarefa] = end_time;
      index_tarefa_para_entrar++;
      for (int tempo_j = tempo; tempo_j < tempo + duracao[tarefa_atual]; tempo_j++)
      {
        for (int rec_index = 0; rec_index < qtdRecursos; rec_index++)
        {
          int consumo_tempo_atual = solucao.matriz_solucao_recursos_consumidos_tempo[rec_index][tempo_j];
          solucao.matriz_solucao_recursos_consumidos_tempo[rec_index][tempo_j] = consumo_tempo_atual + consumoRecursos[tarefa_atual][rec_index];
        }
      }
    }

    for (int i = 1; i < qtdTarefas; i++)
    {
      if (solucao.matriz_solucao_com_tempos[2][i] == -1)
      {
        geral_entrou = false;
        break;
      }
    }

    tempo++;
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

void escreverSeedMetricas(std::string arq, int seed)
{
  FILE *arquivo = fopen(arq.c_str(), "a");

  fprintf(arquivo, "Seed: %d\n\n", seed);
  fclose(arquivo);
}

void escreverMetricas(std::string arq, int fo_construtiva, int fo_SA, double tempo_gasto, bool is_melhorada_SA)
{
  FILE *arquivo = fopen(arq.c_str(), "a");
  std::string boolean_SA = is_melhorada_SA ? "true" : "false";

  fprintf(arquivo, "FO-const: %d | FO-SA: %d | Tempo: %.6fs | SA: %s\n", fo_construtiva, fo_SA, tempo_gasto, boolean_SA.c_str());

  fclose(arquivo);
}

void escreverFinalMetricas(Solucao solucao, std::string arq, double tempo_gasto)
{
  FILE *arquivo = fopen(arq.c_str(), "a");

  fprintf(arquivo, "\nMelhor FO: %d | Tempo: %.6fs", solucao.funObj, tempo_gasto);

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

#ifdef MODO_DEBUG
  // Escrevendo no arquivo o endtime
  fprintf(arquivo, "\n");
  for (int i = 0; i < qtdTarefas; i++)
  {
    fprintf(arquivo, "%d ", solucao.matriz_solucao_com_tempos[2][i]);
  }

  // Escrevendo no arquivo os recursos consumidos
  fprintf(arquivo, "\n");
  for (int i = 0; i < qtdRecursos; i++)
  {
    fprintf(arquivo, "\n");

    for (int j = 0; j < solucao.funObj; j++)
    {
      fprintf(arquivo, "%d ", solucao.matriz_solucao_recursos_consumidos_tempo[i][j]);
    }
  }
#endif

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

    for (int j = 0; j < qtdRecursos; j++)
    {
      fscanf(arquivo, "%s\t", &linha);
      consumoRecursos[idTarefaAtual - 1][j] = atoi(linha);
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
