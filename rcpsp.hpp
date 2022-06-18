#ifndef PMM_HPP_INCLUDED
#define PMM_HPP_INCLUDED

#define MAX_QTD_RECURSO 10
#define MAX_QTD_TAREFAS 200
#include <string>

typedef struct tSolucao
{
    int tarefasTempoInicio[MAX_QTD_TAREFAS][2];
    int funObj;
} Solucao;

//
typedef struct relacaoPrecedencia
{
    int qtdSucessores;
    int sucessores[MAX_QTD_TAREFAS];
} RelacaoPrecedencia;

// Variaveis leitura
char linha[100];
int qtdTarefas;
int qtdRecursos;
int duracao[MAX_QTD_TAREFAS];
RelacaoPrecedencia relacoesPrecedencia[MAX_QTD_TAREFAS];
int consumoRecursos[MAX_QTD_TAREFAS][MAX_QTD_RECURSO];

// Preenchido após ordenação
int tarefasPorDuracaoRecurso[2][MAX_QTD_TAREFAS];
int recursoDisponivelAtual[MAX_QTD_RECURSO];

void lerDados(std::string arq);
void getQtdTarefas(FILE *arquivo);
void getRelacoesPrecedencia(FILE *arquivo);
void getQtdRecursos(FILE *arquivo);
void getDuracaoTarefasEConsumoRecursos(FILE *arquivo);
void getQuantidadeCadaRecurso(FILE *arquivo);
void ordenarPrecedencia();
void ordenarTarefasRecursos();

// Metodos auxiliares
/* Verifica se esta contido no Vetor. */
bool verificarSeEstaContidoVetor(const int value, const int quantidade, const int vetor[]);
void copiarSolucao(Solucao &solucaoNova, Solucao &solucaoAntiga);

void calcFO(Solucao &s);

#endif // PMM_HPP_INCLUDED
