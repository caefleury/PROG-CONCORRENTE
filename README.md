# Programação Concorrente

Este repositório contém exemplos e implementações de conceitos de programação concorrente desenvolvidos na disciplina de Programação Concorrente na Universidade de Brasília (UnB).

## Estrutura do Repositório

### Condição de Corrida

- **[soma.c](condicao-de-corrida/soma.c)**: Demonstração de condições de corrida e como elas podem afetar o resultado de operações concorrentes.

### Pombo Correio

- **[pombo-correio.c](pombo-correio/pombo-correio.c)**: Implementação do problema clássico do pombo correio, um exemplo de sincronização entre threads produtoras e consumidoras.

### Pthreads

- **[pthreads.c](pthreads/pthreads.c)**: Exemplos de uso da biblioteca POSIX Threads (pthreads) para criar e gerenciar threads em C.

## Como Compilar e Executar

Para compilar os exemplos em C, utilize o gcc com a flag `-pthread`:

```bash
gcc -o nome_do_executavel arquivo_fonte.c -pthread
```

Para executar:

```bash
./nome_do_executavel
```

## Conceitos Abordados

- Threads e processos
- Sincronização (mutexes, semáforos, variáveis de condição)
- Condições de corrida
- Deadlocks e starvation
- Padrões de concorrência (produtor-consumidor, leitores-escritores)

## Pré-requisitos

- Compilador GCC
- Biblioteca POSIX Threads
