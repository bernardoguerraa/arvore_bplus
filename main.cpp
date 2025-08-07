/**
 * @file arvore_bplus.cpp
 * @brief Implementação de uma folha de Árvore B+ em arquivo binário
 * @author Luiz Otávio de Queiroz Lage Silva
 * @author Bernardo Carvalho Guerra Martins da Costa
 * @date 2025-06-29
 * 
 * Trabalho de AEDS 2 - UNIFEI Campus Itabira
 * Professor: Rafael Francisco dos Santos
 * 
 * Este código implementa as operações básicas de uma folha de Árvore B+ usando
 * arquivos binários, com inserção, remoção, pesquisa e impressão de registros.
 * Todas as operações são feitas diretamente no arquivo sem carregar todos os
 * dados para memória principal.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

/**
 * @struct dados
 * @brief Armazena os dados de um registro
 * @var chave Identificador único do registro (valor inteiro)
 * @var nome Campo de dados associado à chave (30 caracteres fixos)
 */
struct dados {
    int chave;      ///< Chave única para identificação do registro
    char nome[30];  ///< Nome ou valor associado à chave
};

/**
 * @union celula
 * @brief Union que representa tanto o cabeçalho quanto os nós da lista
 * 
 * Pode armazenar:
 * - Cabeçalho: contém metadados sobre a estrutura do arquivo
 * - Nó da lista: contém os dados e ponteiros para outros nós
 */
union celula {
    /**
     * @struct cabecalho
     * @brief Estrutura de controle do arquivo
     */
    struct {
        int quant;  ///< Quantidade de registros ativos no arquivo
        int first;  ///< Índice do primeiro registro na lista
        int last;   ///< Índice do último registro na lista
        int free;   ///< Índice do primeiro nó na lista de espaços livres
        int tam;    ///< Capacidade total de registros no arquivo
    } cabecalho;
    
    /**
     * @struct lista
     * @brief Estrutura de um nó de dados
     */
    struct {
        int next;   ///< Índice do próximo nó na lista (-1 se último)
        int prev;   ///< Índice do nó anterior na lista (-1 se primeiro)
        dados reg;  ///< Dados armazenados neste nó
    } lista;
};

/**
 * @brief Inicializa um novo arquivo para a Árvore B+
 * @param arq Referência para o arquivo já aberto
 * @param n Número máximo de registros que o arquivo pode armazenar
 * 
 * Esta função:
 * 1. Configura o cabeçalho com valores iniciais
 * 2. Inicializa todos os nós como livres
 * 3. Encadeia os nós livres em uma lista
 * 
 * Complexidade: O(n) onde n é o número máximo de registros
 */
void inicializar(fstream &arq, int n) {
    celula cab, l;
    // Posiciona no início do arquivo
    arq.seekp(0, arq.beg);

    // Configuração inicial do cabeçalho
    cab.cabecalho.quant = 0;    // Nenhum registro inserido
    cab.cabecalho.first = -1;   // Lista vazia
    cab.cabecalho.last = -1;    // Lista vazia
    cab.cabecalho.free = 1;     // Primeira posição livre é 1
    cab.cabecalho.tam = n;      // Capacidade total

    // Escreve o cabeçalho no arquivo
    arq.write((char*)&cab, sizeof(cab));

    // Inicializa todos os nós como livres
    for (int i = 1; i <= cab.cabecalho.tam; i++) {
        // Encadeamento dos nós livres:
        // Cada nó aponta para o próximo, exceto o último que aponta para -1
        l.lista.next = (i == n) ? -1 : i + 1;
        l.lista.prev = -1;      // Nós livres não usam prev
        l.lista.reg.chave = -1; // Marcação explícita como livre
        arq.write((char*)&l, sizeof(l));
    }
}

/**
 * @brief Imprime toda a estrutura do arquivo
 * @param arq Referência para o arquivo já aberto
 * 
 * Mostra:
 * - Todos os metadados do cabeçalho
 * - Todos os registros (ativos e livres) com seus dados completos
 * 
 * Complexidade: O(n) onde n é o número máximo de registros
 */
void imprimirEstrutura(fstream &arq) {
    celula cab, l;
    // Lê o cabeçalho
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    cout << "\n=== ESTRUTURA COMPLETA ==="
         << "\nCabecalho:"
         << "\n  Quant: " << cab.cabecalho.quant
         << "\n  First: " << cab.cabecalho.first
         << "\n  Last: " << cab.cabecalho.last
         << "\n  Free: " << cab.cabecalho.free
         << "\n  Tam: " << cab.cabecalho.tam
         << "\n\nRegistros:";

    // Imprime todos os registros, um por um
    for (int i = 1; i <= cab.cabecalho.tam; i++) {
        arq.read((char*)&l, sizeof(l));
        cout << "\n  Pos " << i << ": "
             << "Chave=" << (l.lista.reg.chave == -1 ? "[LIVRE]" : to_string(l.lista.reg.chave))
             << ", Nome=" << (l.lista.reg.chave == -1 ? "[LIVRE]" : l.lista.reg.nome)
             << ", Next=" << l.lista.next
             << ", Prev=" << l.lista.prev;
    }
    cout << "\n";
}

/**
 * @brief Imprime apenas os registros válidos na ordem da lista
 * @param arq Referência para o arquivo já aberto
 * 
 * Percorre a lista de registros ativos seguindo os ponteiros next
 * a partir do primeiro registro (first) até o último (last).
 * 
 * Complexidade: O(n) onde n é o número de registros ativos
 */
void imprimirLista(fstream &arq) {
    celula cab, l;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    cout << "\n=== REGISTROS VALIDOS ==="
         << "\nCabecalho:"
         << "\n  Quant: " << cab.cabecalho.quant
         << "\n  First: " << cab.cabecalho.first
         << "\n  Last: " << cab.cabecalho.last
         << "\n  Free: " << cab.cabecalho.free
         << "\n\nRegistros:";

    if (cab.cabecalho.first == -1) {
        cout << "\nLista vazia!\n";
        return;
    }

    // Percorre a lista a partir do primeiro registro
    int pos = cab.cabecalho.first;
    while (pos != -1) {
        arq.seekg(sizeof(celula) + (pos-1)*sizeof(celula), arq.beg);
        arq.read((char*)&l, sizeof(l));
        cout << "\n  Pos " << pos << ": "
             << "Chave=" << l.lista.reg.chave
             << " | Nome=" << l.lista.reg.nome
             << " | Next=" << l.lista.next
             << " | Prev=" << l.lista.prev;
        
        if (pos == cab.cabecalho.last) break;
        pos = l.lista.next;
    }
    cout << "\n";
}

/**
 * @brief Imprime a lista de nós livres
 * @param arq Referência para o arquivo já aberto
 * 
 * Percorre a lista de nós livres seguindo os ponteiros next
 * a partir do primeiro nó livre (free) até o final.
 * 
 * Complexidade: O(n) onde n é o número de nós livres
 */
void imprimirFree(fstream &arq) {
    celula cab, l;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    cout << "\n=== REGISTROS LIVRES ==="
         << "\nCabecalho:"
         << "\n  Free: " << cab.cabecalho.free
         << "\n\nRegistros livres:";

    int pos = cab.cabecalho.free;
    while (pos != -1) {
        arq.seekg(sizeof(celula) + (pos-1)*sizeof(celula), arq.beg);
        arq.read((char*)&l, sizeof(l));
        cout << "\n  Pos " << pos << " -> Next: " << l.lista.next;
        pos = l.lista.next;
    }
    cout << "\n";
}

/**
 * @brief Pesquisa um registro pela chave
 * @param arq Referência para o arquivo já aberto
 * @param chave Chave a ser pesquisada
 * @param resultado Referência para armazenar o registro encontrado
 * @return true se encontrou, false caso contrário
 * 
 * Percorre a lista de registros ativos até encontrar a chave.
 * 
 * Complexidade: O(n) onde n é o número de registros ativos
 */
bool pesquisa(fstream &arq, int chave, celula &resultado) {
    celula cab;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    int atual = cab.cabecalho.first;
    while (atual != -1) {
        arq.seekg(sizeof(celula) + (atual-1)*sizeof(celula), arq.beg);
        arq.read((char*)&resultado, sizeof(resultado));
        if (resultado.lista.reg.chave == chave) {
            return true;
        }
        atual = resultado.lista.next;
    }
    return false;
}

/**
 * @brief Insere um novo registro na primeira posição livre
 * @param arq Referência para o arquivo já aberto
 * @param d Dados a serem inseridos
 * 
 * Esta função:
 * 1. Verifica se a chave já existe
 * 2. Verifica se há espaço livre
 * 3. Insere no primeiro nó livre
 * 4. Atualiza os ponteiros e o cabeçalho
 * 
 * Complexidade: O(n) para pesquisa + O(1) para inserção
 */
void inserir(fstream &arq, dados d) {
    celula l, cab, existente;
    
    // Verifica se chave já existe
    if (pesquisa(arq, d.chave, existente)) {
        cout << "Erro: Chave ja existente!\n";
        return;
    }

    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    // Verifica se há espaço livre
    if (cab.cabecalho.free == -1) {
        cout << "Erro: Arquivo cheio!\n";
        return;
    }

    // Obtém o primeiro nó livre
    arq.seekg(sizeof(celula) + (cab.cabecalho.free-1)*sizeof(celula), arq.beg);
    arq.read((char*)&l, sizeof(l));
    int free = l.lista.next;  // Salva o próximo livre

    // Configura o novo registro
    l.lista.reg = d;
    l.lista.next = -1;        // Será o último da lista
    l.lista.prev = cab.cabecalho.last;  // Aponta para o antigo último

    // Atualiza o antigo último registro
    if (cab.cabecalho.last != -1) {
        celula lastNode;
        arq.seekg(sizeof(celula) + (cab.cabecalho.last-1)*sizeof(celula), arq.beg);
        arq.read((char*)&lastNode, sizeof(lastNode));
        lastNode.lista.next = cab.cabecalho.free;  // Aponta para o novo
        arq.seekp(sizeof(celula) + (cab.cabecalho.last-1)*sizeof(celula), arq.beg);
        arq.write((char*)&lastNode, sizeof(lastNode));
    }

    // Escreve o novo registro
    arq.seekp(sizeof(celula) + (cab.cabecalho.free-1)*sizeof(celula), arq.beg);
    arq.write((char*)&l, sizeof(l));

    // Atualiza cabeçalho
    if (cab.cabecalho.first == -1) {
        cab.cabecalho.first = cab.cabecalho.free;  // Primeiro registro
    }
    
    cab.cabecalho.last = cab.cabecalho.free;  // Novo último
    cab.cabecalho.free = free;                // Atualiza lista livre
    cab.cabecalho.quant++;                    // Incrementa contador

    // Escreve cabeçalho atualizado
    arq.seekp(0, arq.beg);
    arq.write((char*)&cab, sizeof(cab));
}

/**
 * @brief Insere um novo registro mantendo a ordenação por chave
 * @param arq Referência para o arquivo já aberto
 * @param d Dados a serem inseridos
 * 
 * Esta função:
 * 1. Verifica se a chave já existe
 * 2. Encontra a posição correta para manter a ordenação
 * 3. Atualiza os ponteiros dos nós vizinhos
 * 4. Atualiza o cabeçalho
 * 
 * Complexidade: O(n) onde n é o número de registros ativos
 */
void inserirOrdenado(fstream &arq, dados d) {
    celula cab, l, novo, existente;
    
    // Verifica se chave já existe
    if (pesquisa(arq, d.chave, existente)) {
        cout << "Erro: Chave ja existente!\n";
        return;
    }

    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    // Verifica se há espaço livre
    if (cab.cabecalho.free == -1) {
        cout << "Erro: Arquivo cheio!\n";
        return;
    }

    // Obtém o primeiro nó livre
    arq.seekg(sizeof(celula) + (cab.cabecalho.free-1)*sizeof(celula), arq.beg);
    arq.read((char*)&novo, sizeof(novo));
    int free = novo.lista.next;  // Salva o próximo livre

    // Configura o novo nó
    novo.lista.reg = d;
    novo.lista.next = -1;
    novo.lista.prev = -1;

    // Caso lista vazia
    if (cab.cabecalho.first == -1) {
        cab.cabecalho.first = cab.cabecalho.free;
        cab.cabecalho.last = cab.cabecalho.free;
    } else {
        // Procura posição de inserção
        int atual = cab.cabecalho.first;
        int anterior_pos = -1;

        while (atual != -1) {
            arq.seekg(sizeof(celula) + (atual-1)*sizeof(celula), arq.beg);
            arq.read((char*)&l, sizeof(l));
            if (l.lista.reg.chave > d.chave) break;
            anterior_pos = atual;
            atual = l.lista.next;
        }

        // Inserção no início
        if (atual == cab.cabecalho.first) {
            novo.lista.next = cab.cabecalho.first;
            novo.lista.prev = -1;
            
            // Atualiza o antigo primeiro
            arq.seekg(sizeof(celula) + (cab.cabecalho.first-1)*sizeof(celula), arq.beg);
            arq.read((char*)&l, sizeof(l));
            l.lista.prev = cab.cabecalho.free;
            arq.seekp(sizeof(celula) + (cab.cabecalho.first-1)*sizeof(celula), arq.beg);
            arq.write((char*)&l, sizeof(l));
            
            cab.cabecalho.first = cab.cabecalho.free;
        } 
        // Inserção no final
        else if (atual == -1) {
            novo.lista.prev = cab.cabecalho.last;
            
            // Atualiza o antigo último
            arq.seekg(sizeof(celula) + (cab.cabecalho.last-1)*sizeof(celula), arq.beg);
            arq.read((char*)&l, sizeof(l));
            l.lista.next = cab.cabecalho.free;
            arq.seekp(sizeof(celula) + (cab.cabecalho.last-1)*sizeof(celula), arq.beg);
            arq.write((char*)&l, sizeof(l));
            
            cab.cabecalho.last = cab.cabecalho.free;
        }
        // Inserção no meio
        else {
            celula anterior, proximo;
            novo.lista.prev = anterior_pos;
            novo.lista.next = atual;

            // Atualiza nó anterior
            arq.seekg(sizeof(celula) + (anterior_pos-1)*sizeof(celula), arq.beg);
            arq.read((char*)&anterior, sizeof(anterior));
            anterior.lista.next = cab.cabecalho.free;
            arq.seekp(sizeof(celula) + (anterior_pos-1)*sizeof(celula), arq.beg);
            arq.write((char*)&anterior, sizeof(anterior));

            // Atualiza nó posterior
            arq.seekg(sizeof(celula) + (atual-1)*sizeof(celula), arq.beg);
            arq.read((char*)&proximo, sizeof(proximo));
            proximo.lista.prev = cab.cabecalho.free;
            arq.seekp(sizeof(celula) + (atual-1)*sizeof(celula), arq.beg);
            arq.write((char*)&proximo, sizeof(proximo));
        }
    }

    // Escreve o novo nó
    arq.seekp(sizeof(celula) + (cab.cabecalho.free-1)*sizeof(celula), arq.beg);
    arq.write((char*)&novo, sizeof(novo));

    // Atualiza cabeçalho
    cab.cabecalho.free = free;
    cab.cabecalho.quant++;
    arq.seekp(0, arq.beg);
    arq.write((char*)&cab, sizeof(cab));
}

/**
 * @brief Remove um registro pela chave
 * @param arq Referência para o arquivo já aberto
 * @param chave Chave do registro a ser removido
 * @return true se removeu com sucesso, false se não encontrou
 * 
 * Esta função:
 * 1. Localiza o registro a ser removido
 * 2. Atualiza os ponteiros dos nós vizinhos
 * 3. Adiciona o nó removido à lista de livres
 * 4. Atualiza o cabeçalho
 * 
 * Complexidade: O(n) onde n é o número de registros ativos
 */
bool remover(fstream &arq, int chave) {
    celula cab, l;
    arq.seekg(0, arq.beg);
    arq.read((char*)&cab, sizeof(cab));

    int atual = cab.cabecalho.first;
    int anterior_pos = -1;

    // Procura o registro
    while (atual != -1) {
        arq.seekg(sizeof(celula) + (atual-1)*sizeof(celula), arq.beg);
        arq.read((char*)&l, sizeof(l));
        
        if (l.lista.reg.chave == chave) {
            // Atualiza nó anterior
            if (l.lista.prev != -1) {
                celula anterior;
                arq.seekg(sizeof(celula) + (l.lista.prev-1)*sizeof(celula), arq.beg);
                arq.read((char*)&anterior, sizeof(anterior));
                anterior.lista.next = l.lista.next;
                arq.seekp(sizeof(celula) + (l.lista.prev-1)*sizeof(celula), arq.beg);
                arq.write((char*)&anterior, sizeof(anterior));
            } else {
                // Era o primeiro - atualiza cabeçalho
                cab.cabecalho.first = l.lista.next;
            }

            // Atualiza nó posterior
            if (l.lista.next != -1) {
                celula proximo;
                arq.seekg(sizeof(celula) + (l.lista.next-1)*sizeof(celula), arq.beg);
                arq.read((char*)&proximo, sizeof(proximo));
                proximo.lista.prev = l.lista.prev;
                arq.seekp(sizeof(celula) + (l.lista.next-1)*sizeof(celula), arq.beg);
                arq.write((char*)&proximo, sizeof(proximo));
            } else {
                // Era o último - atualiza cabeçalho
                cab.cabecalho.last = l.lista.prev;
            }

            // Libera o nó
            l.lista.next = cab.cabecalho.free;
            l.lista.prev = -1;
            l.lista.reg.chave = -1;  // Marca como livre
            strcpy(l.lista.reg.nome, "[LIVRE]");

            // Escreve nó liberado
            arq.seekp(sizeof(celula) + (atual-1)*sizeof(celula), arq.beg);
            arq.write((char*)&l, sizeof(l));

            // Atualiza lista de livres
            cab.cabecalho.free = atual;
            cab.cabecalho.quant--;

            // Se lista ficou vazia
            if (cab.cabecalho.quant == 0) {
                cab.cabecalho.first = -1;
                cab.cabecalho.last = -1;
            }

            // Atualiza cabeçalho
            arq.seekp(0, arq.beg);
            arq.write((char*)&cab, sizeof(cab));
            return true;
        }
        anterior_pos = atual;
        atual = l.lista.next;
    }
    return false;
}

/**
 * @brief Função principal
 * 
 * Responsável por:
 * - Abrir/criar o arquivo de dados
 * - Exibir menu interativo
 * - Chamar as operações conforme seleção do usuário
 * 
 * Menu de opções:
 * 1. Inserir registro
 * 2. Inserir ordenado
 * 3. Remover registro
 * 4. Pesquisar registro
 * 5. Imprimir registros válidos
 * 6. Imprimir estrutura completa
 * 7. Imprimir nós livres
 * 0. Sair
 */
int main() {
    fstream arq;
    dados d;
    int op, chave;
    celula resultado;

    cout << "Abrindo arquivo pagina.dat...\n";
    arq.open("pagina.dat", ios::binary | fstream::in | fstream::out);
    
    // Se arquivo não existe, cria um novo
    if (!arq.is_open()) {
        cout << "Arquivo nao existe. Criando novo...\n";
        arq.open("pagina.dat", ios::binary | fstream::in | fstream::out | fstream::trunc);
        if (!arq.is_open()) {
            cerr << "Erro ao criar arquivo!\n";
            return 1;
        }
        cout << "Digite o numero maximo de registros: ";
        int n;
        cin >> n;
        inicializar(arq, n);
    }

    // Menu interativo
    do {
        cout << "\n=== MENU ==="
             << "\n1. Inserir"
             << "\n2. Inserir ordenado"
             << "\n3. Remover"
             << "\n4. Pesquisar"
             << "\n5. Imprimir registros"
             << "\n6. Imprimir estrutura"
             << "\n7. Imprimir livres"
             << "\n0. Sair"
             << "\nOpcao: ";
        cin >> op;

        switch (op) {
            case 1:
                cout << "Chave: "; cin >> d.chave;
                cout << "Nome: "; cin >> d.nome;
                inserir(arq, d);
                break;
                
            case 2:
                cout << "Chave: "; cin >> d.chave;
                cout << "Nome: "; cin >> d.nome;
                inserirOrdenado(arq, d);
                break;
                
            case 3:
                cout << "Chave a remover: "; cin >> chave;
                if (remover(arq, chave)) {
                    cout << "Registro removido com sucesso!\n";
                } else {
                    cout << "Chave nao encontrada!\n";
                }
                break;
                
            case 4:
                cout << "Chave a pesquisar: "; cin >> chave;
                if (pesquisa(arq, chave, resultado)) {
                    cout << "Registro encontrado:\n"
                         << "Chave: " << resultado.lista.reg.chave
                         << " | Nome: " << resultado.lista.reg.nome << "\n";
                } else {
                    cout << "Registro nao encontrado!\n";
                }
                break;
                
            case 5:
                imprimirLista(arq);
                break;
                
            case 6:
                imprimirEstrutura(arq);
                break;
                
            case 7:
                imprimirFree(arq);
                break;
                
            case 0:
                cout << "Encerrando programa...\n";
                break;
                
            default:
                cout << "Opcao invalida!\n";
        }
    } while (op != 0);

    arq.close();
    return 0;
}