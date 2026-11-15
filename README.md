# Implementação 5 — Algoritmo do Banqueiro

Implementação em C do algoritmo do Banqueiro (Operating System Concepts,
Silberschatz, 10ª ed., p. P-45), com número de clientes e de recursos
definidos dinamicamente pela entrada.

## Compilar

make

## Executar

./banker <instâncias de cada recurso>

Exemplo (3 recursos, com 10, 5 e 7 instâncias):

./banker 10 5 7

O programa lê `customer.txt` (máximo de cada cliente) e `commands.txt`
(sequência de comandos `RQ`, `RL` e `*`) no diretório atual, e grava o
resultado em `result.txt`.

## Limpar arquivos compilados

make clean

## Arquivos

- `banker.c` — implementação completa (leitura/validação de argumentos
  e arquivos de entrada, algoritmo de segurança, geração de `result.txt`)
- `Makefile` — compilação e limpeza
- `customer.txt`, `commands.txt` — arquivos de entrada de exemplo
- `evidencias.log` — log das sessões de teste no terminal