#ifndef PMM_HPP_INCLUDED
#define PMM_HPP_INCLUDED

#define MAX_QTD_RECURSO 10
#define MAX_QTD_TAREFAS 200
#define PESO_PENALIZACAO_PRECEDENCIA 100000
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


//Heuristica construtiva
void handleHeuristicaConstrutiva(double alfa);

void simulated_annealing(Solucao &solucao_inicial, int temp_inicial, int temp_final, double taxa_resf, int num_sol_viz);
Solucao gerar_vizinho(Solucao solucao_atual);

//FO
int calcularFO(Solucao &s);
int calcularFOPrecedencia(Solucao &s);
int calcularFORecurso(Solucao &s);
bool todosPredecessoresJaEntraram(int idTarefa, int qtdTarefasAnalizar, int *vetor);

// Metodos utilitários
bool verificarSeEstaContidoVetor(const int value, const int qtd, const int vetor[]);
int findIndexByValue(const int value, const int qtd, const int vetor[]);
tPrececessores getPredecessores(const int tarefa);

void push_array(int id, int array[], int qtd);
bool includes_array(int id, int array[], int qtd);
void zerar_vetor(int *array, const int tamanho, const int value);
void remover_elemento_matriz(int elemento, int* matriz, int linhas, int colunas);

#endif // PMM_HPP_INCLUDED
