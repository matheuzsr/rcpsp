#ifndef PMM_HPP_INCLUDED
#define PMM_HPP_INCLUDED

#define MAX_QTD_RECURSO 10
#define MAX_QTD_TAREFAS 200
#define PESO_PENALIZACAO_PRECEDENCIA 1000
#define PESO_PENALIZACAO_RECURSOS 100

#include <string>

typedef struct tSolucao
{
    int tarefasStartTime[2][MAX_QTD_TAREFAS];
    int qtdTarefas;
    int funObj;
    int makespan;
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

// Variaveis de uso
int tempoInicio[MAX_QTD_TAREFAS];
int tempoFim[MAX_QTD_TAREFAS];
int maiorDuracaoTarefas[2][MAX_QTD_TAREFAS];
int desvioPadraoDuracaoTarefas[2][MAX_QTD_TAREFAS];

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

// Metodos utilitários
bool verificarSeEstaContidoVetor(const int value, const int qtd, const int vetor[]);
int findIndexByValue(const int value, const int qtd, const int vetor[]);
tPrececessores getPredecessores(const int tarefa);

#endif // PMM_HPP_INCLUDED
