# PROJETO FINAL - PROGRAMAÇÃO CONCORRENTE

## Problema do RU (Restaurante Universitário)

### Agentes

- Estudantes sem prioridade
- Estudantes com prioridade
- Cozinheiros
- Faxineiros
- *Bandejas

### Requisitos

- O RU tem um número limitado de bandejas disponíveis
- Os cozinheiros são responsáveis por preparar a comida
- Estudante só pode servir se tiver bandeja disponível
- Podem se alimentar até 3 vezes, com um tempo de digestão aleatório

### Regras de prioridade

- Estudantes regulares só podem servir se não houver bolsistas esperando
- Funcionários só podem servir se não houver estudantes esperando
- Cada estudante pega apenas uma bandeja, e cada bandeja só pode ser usada por uma pessoa
