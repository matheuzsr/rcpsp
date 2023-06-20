#ifndef PMM_HPP_INCLUDED
#define PMM_HPP_INCLUDED

#define MAX_QTD_RECURSO 10
#define MAX_QTD_TAREFAS 200
#define PESO_PENALIZACAO_PRECEDENCIA 100
#define PESO_PENALIZACAO_RECURSOS 100
#define TEMPO_MAXIMO 10000

#include <string>

typedef struct tSolucao
{
    int matriz_solucao_com_tempos[3][MAX_QTD_TAREFAS];
    int matriz_solucao_recursos_consumidos_tempo[MAX_QTD_RECURSO][TEMPO_MAXIMO];
    int funObj;
} Solucao;

typedef struct tPrececessores
{
    int qtdPrecedessores;
    int list[MAX_QTD_TAREFAS];
} Prececessores;

typedef struct tSucessores
{
    int qtdSucessores;
    int list[MAX_QTD_TAREFAS];
} Sucessores;

// Variaveis leitura
char linha[100];
int qtdTarefas;
int qtdRecursos;
int duracao[MAX_QTD_TAREFAS];
int recursoDisponivel[MAX_QTD_RECURSO];
int matrizRelacoesSucessores[MAX_QTD_TAREFAS][MAX_QTD_TAREFAS];
int tarefaQtdSucessores[2][MAX_QTD_TAREFAS];
int consumoRecursos[MAX_QTD_TAREFAS][MAX_QTD_RECURSO];
int matriz_tarefas_escalonamento[2][MAX_QTD_TAREFAS];

// Variaveis de uso
int tempoInicio[MAX_QTD_TAREFAS];
int tempoFim[MAX_QTD_TAREFAS];
int maiorDuracaoTarefas[2][MAX_QTD_TAREFAS];
int tarefas_entrar_matriz_solucao[2][MAX_QTD_TAREFAS];

/* Posicão 0 [id tarefa]
 * Posicão 1 [start_time]
 * Posição 2 [end_time]
 */
int matriz_solucao_com_tempos[3][MAX_QTD_TAREFAS];
int matriz_solucao_tarefas_tempo[MAX_QTD_TAREFAS][TEMPO_MAXIMO];
int matriz_solucao_recursos_consumidos_tempo[MAX_QTD_RECURSO][TEMPO_MAXIMO];

// Var usada abaixo no calcular
int entraramList[MAX_QTD_TAREFAS];

void lerDados(std::string arq);
void lerQtdTarefas(FILE *arquivo);
void lerRelacoesPrecedencia(FILE *arquivo);
void lerQtdRecursos(FILE *arquivo);
void lerDuracaoTarefasEConsumoRecursos(FILE *arquivo);
void lerQuantidadeCadaRecurso(FILE *arquivo);
int calcularTempoTarefa(int idTarefa, int predecessores[], int qtdPredecessores);

void handleOrdenarTarefasPorSucessor();
void calcularDuracaoTarefasMaisTempoPredecessores();
void inserirNaSolucao();
void preencherMatrizBinariaTarefaTempo(int tarefa, int startTime);
void preencherMatrizRecursoTempo(int tarefa, int startTime);

// Heuristica GRASP
void heuristicaGrasp(double alfa, const double tempo_limite, double &tempo_melhor, double &tempo_total, std::string file_name);
// Heuristica construtiva
void handleHeuristicaConstrutiva(double alfa);
Solucao simulated_annealing(Solucao solucao_inicial, double temp_inicial, double temp_final, double taxa_resf, int num_sol_viz, double start_time, double tempo_limite);
Solucao gerar_vizinho(Solucao solucao_atual);
Solucao gerar_vizinho_tempo(Solucao solucao_atual);
Solucao gerar_vizinho_tempo_novo(Solucao solucao_atual);
Solucao gerar_vizinho_tempo_rand(Solucao solucao_atual);
void remover_uso_recursos(Solucao &solucao_atual, int index_tarefa);
void inserir_uso_recursos(Solucao &solucao_atual, int index_tarefa);

// FO
int calcularFO(Solucao &s);
int calcularFOPrecedencia(Solucao &s);
int calcularFORecurso(Solucao &s);
bool todosPredecessoresJaEntraram(int idTarefa, int qtdTarefasAnalizar, int *vetor);
bool todosPredecessoresJaEntraramBaseadoTempo(int idTarefa, int indexTarefa, Solucao s);

// Metodos utilitários
void arrumarSolucao(Solucao &solucao);
bool verificarSeEstaContidoVetor(const int value, const int qtd, const int vetor[]);
int findIndexByValue(const int value, const int qtd, const int vetor[]);
tPrececessores getPredecessores(const int tarefa);
tSucessores getSucessores(const int tarefa);
void copiarSolucao(Solucao &solucaoNova, Solucao &solucaoAntiga);
void escreverSolucao(Solucao &solucao, std::string arq);

// Metricas
void escreverMetricas(std::string arq, int fo_construtiva, int fo_SA, double tempo_gasto, bool is_melhorada_SA);
void escreverSeedMetricas(std::string arq, int seed);
void escreverFinalMetricas(Solucao solucao, std::string arq, double tempo_gasto);

void push_array(int id, int array[], int qtd);
bool includes_array(int id, int array[], int qtd);
void zerar_vetor(int *array, const int tamanho, const int value);
void remover_elemento_matriz(int elemento, int *matriz, int linhas, int colunas);

#endif // PMM_HPP_INCLUDED
