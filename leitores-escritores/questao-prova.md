# Questões

Dada a solução abaixo para o problema dos leitores e escritores, responda as seguintes perguntas. Para cada pergunta, explique sua resposta apontando as linhas do código.

## Solução apenas com os dois locks

a) (1,0 pontos) É garantido que cada escritor possui acesso exclusivo ao banco de dados?

**Resposta:** Sim, é garantido que cada escritor possui acesso exclusivo ao banco de dados. Isso é assegurado pelo uso do mutex `lock_bd` nas linhas 37-39, onde o escritor adquire o lock antes de escrever no banco de dados e o libera após terminar a escrita. Como todos os escritores e leitores usam o mesmo mutex para acessar o banco de dados (linhas 24-26 para leitores), não é possível que dois escritores (ou um escritor e um leitor) acessem o banco simultaneamente.

b) (1,0 pontos) É possível que um número qualquer de leitores acessem o banco de dados simultaneamente?

**Resposta:** Não, não é possível que múltiplos leitores acessem o banco de dados simultaneamente nesta implementação. Como os leitores também usam o mesmo mutex `lock_bd` (linhas 24-26) que os escritores, e cada leitor mantém o lock durante toda a sua operação de leitura, apenas um leitor pode acessar o banco de dados por vez. Este é um comportamento restritivo, pois em uma solução ideal para o problema de leitores-escritores, múltiplos leitores deveriam poder acessar o banco simultaneamente.

c) (1,0 pontos) Os escritores possuem prioridade no acesso ao banco de dados?

**Resposta:** Não, os escritores não possuem prioridade no acesso ao banco de dados. O código implementa um sistema de acesso baseado apenas em mutex, sem mecanismos de priorização (como semáforos adicionais ou contadores). Tanto leitores quanto escritores competem igualmente pelo mesmo mutex `lock_bd` (linhas 24 e 37), e a ordem de acesso depende da programação do sistema operacional e de como as threads são escalonadas, seguindo uma política de primeiro a chegar, primeiro a ser servido (FCFS - First Come, First Served).

d) (1,0 pontos) É livre de starvation?

**Resposta:** Não, a solução não é livre de starvation. Como não há mecanismo de priorização ou fila de espera explícita, é possível que leitores ou escritores sofram starvation. Por exemplo, se houver uma sequência contínua de leitores chegando, um escritor pode ficar indefinidamente esperando para adquirir o lock (linha 37). Da mesma forma, se houver uma sequência contínua de escritores, os leitores podem ficar indefinidamente esperando. O sistema operacional pode favorecer certas threads dependendo de sua política de escalonamento, o que pode resultar em starvation para algumas threads.

## Solução apenas com contador de leitores

a) (1,0 pontos) É garantido que cada escritor possui acesso exclusivo ao banco de dados?

**Resposta:** Sim, é garantido que cada escritor possui acesso exclusivo ao banco de dados. Na nova implementação, quando um escritor adquire o `lock_bd` (linha 37-39), nenhum outro escritor ou leitor pode acessar o banco de dados. Os leitores são impedidos de acessar porque o primeiro leitor tenta adquirir o mesmo lock (linha 26) e fica bloqueado enquanto o escritor mantiver o lock. Como todos os escritores competem pelo mesmo mutex, a exclusividade é garantida.

b) (1,0 pontos) É possível que um número qualquer de leitores acessem o banco de dados simultaneamente?

**Resposta:** Sim, a nova implementação permite que múltiplos leitores acessem o banco de dados simultaneamente. Isso é possível porque apenas o primeiro leitor adquire o `lock_bd` (linhas 25-27), enquanto os demais leitores verificam que o contador `num_leitores` já é maior que 0 e seguem diretamente para a leitura. A variável `num_leitores` é protegida por um mutex separado (`lock_num_leitores`), o que permite que múltiplos leitores se registrem e acessem o banco concorrentemente, enquanto o lock estiver mantido pelo grupo como um todo.

c) (1,0 pontos) Os escritores possuem prioridade no acesso ao banco de dados?

**Resposta:** Não, os escritores não possuem prioridade no acesso ao banco de dados. A implementação ainda segue uma abordagem de "primeiro a chegar, primeiro a ser servido" (FCFS). Quando um escritor tenta adquirir o lock, ele pode ser bloqueado se houver qualquer número de leitores ativos (indicado por `num_leitores > 0`). Da mesma forma, se um escritor estiver usando o banco de dados, novos leitores terão que esperar. Não há mecanismo específico que priorize escritores sobre leitores na ordem de acesso.

d) (1,0 pontos) É livre de starvation?

**Resposta:** Não, a solução ainda não é livre de starvation. Continua sendo possível que escritores sofram starvation se houver um fluxo contínuo de leitores chegando. Como não há limite para o número de leitores que podem acessar o banco simultaneamente ou um mecanismo para dar prioridade a escritores que estão esperando, um escritor pode teoricamente esperar indefinidamente enquanto novos leitores continuam chegando antes do último leitor atual sair. Da mesma forma, os leitores podem sofrer starvation se houver um fluxo contínuo de escritores.
