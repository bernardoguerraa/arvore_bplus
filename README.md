# Folha de Árvore B+ em Arquivo Binário

Este projeto implementa as operações de uma **folha de Árvore B+**, utilizando uma **lista duplamente encadeada gerenciada diretamente em um arquivo binário**. Todas as operações de inserção, remoção, pesquisa e visualização são realizadas em disco, sem a necessidade de carregar toda a estrutura de dados para a memória principal.

Este trabalho foi desenvolvido para a disciplina de **Algoritmos e Estruturas de Dados 2 (AEDS 2)** da **UNIFEI - Campus Itabira**, sob a orientação do professor **Rafael Francisco dos Santos**.

---

## 🧠 Conceito Principal

A principal característica deste projeto é a simulação de alocação dinâmica de memória usando um arquivo como se fosse um grande vetor. Em vez de ponteiros de memória (como `new` e `delete`), o sistema utiliza **índices (cursores)** para conectar os nós da lista.

-   **Gerenciamento em Arquivo:** As operações são feitas diretamente no arquivo `pagina.dat`, o que é ideal para persistência de dados e para lidar com volumes de informação maiores que a memória RAM disponível.
-   **Cabeçalho de Controle:** A primeira posição do arquivo (`índice 0`) é reservada para um **cabeçalho** que armazena metadados essenciais: quantidade de registros, ponteiros para o primeiro e último elemento, e o início da lista de nós livres.
-   **Lista de Nós Livres (Free List):** Quando um registro é removido, sua posição no arquivo não é perdida. Em vez disso, ela é adicionada a uma lista encadeada de "espaços livres", pronta para ser reutilizada por uma futura inserção. Isso evita a fragmentação do arquivo e otimiza o uso do espaço.

---

## ✨ Funcionalidades

O programa oferece um menu interativo com as seguintes operações:

-   **Inserir:** Adiciona um novo registro no final da lista.
-   **Inserir Ordenado:** Adiciona um novo registro mantendo a ordem crescente das chaves.
-   **Remover:** Remove um registro com base na sua chave e o move para a lista de nós livres.
-   **Pesquisar:** Busca um registro pela chave e exibe seus dados.
-   **Imprimir Registros:** Exibe todos os registros válidos, na ordem em que estão na lista.
-   **Imprimir Estrutura:** Mostra o estado completo do arquivo, incluindo os metadados do cabeçalho e todos os nós (ocupados e livres).
-   **Imprimir Livres:** Exibe a lista encadeada de nós disponíveis para reutilização.

---

## ▶️ Como Usar

### 1. Compilar o Código
Utilize um compilador C++ (como o g++) para compilar o arquivo-fonte.
### 2. Executar o programa
./arvore_bplus
### 3. Interagir com o menu
Após a inicialização, um menu será exibido para que você possa escolher a operação desejada.

=== MENU ===
1. Inserir
2. Inserir ordenado
3. Remover
4. Pesquisar
5. Imprimir registros
6. Imprimir estrutura
7. Imprimir livres
0. Sair
Opcao:

---

## 📚 Aprendizado e Importância
Este projeto é fundamental para entender como estruturas de dados podem ser implementadas em memória secundária (disco), garantindo a persistência dos dados entre execuções do programa.

 - Gerenciamento de Memória Simulado: Demonstra técnicas de gerenciamento manual de espaço, como o uso de uma lista de nós livres (Free List), que são conceitos essenciais no desenvolvimento de sistemas de banco de dados e sistemas operacionais.

 - Ponteiros vs. Cursores: Oferece uma visão prática sobre a diferença entre ponteiros de memória e cursores (índices) para referenciar dados, uma abstração crucial para a manipulação de registros em arquivos.

 - Manipulação de Arquivos Binários: Solidifica o conhecimento em manipulação de arquivos binários em C++ (fstream), uma habilidade valiosa para qualquer aplicação que precise de performance e controle de baixo nível sobre o armazenamento.

---

## 👨‍💻 Autores
Luiz Otávio de Queiroz Lage Silva

Bernardo Carvalho Guerra Martins da Costa
