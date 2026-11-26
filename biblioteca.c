
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define A 100
#define PRAZO 14

typedef struct Usuario{
    char email[A];
    char nome[A];
    struct Usuario *next;
}Usuario;

typedef struct Livro{
    char titulo[A];
    char autor[A];
    int ano_publicado;
    int codigo;
    int status;
    int emprestimos;
    char email[A];
    time_t data_emprestimo;
    time_t data_limite;
    struct Livro *next;
}Livro;

// Arvore auxiliar para pesquisar os codigos dos livros
typedef struct Arvore{
    int codigo;
    Livro *livro;
    struct Arvore *esq;
    struct Arvore *dir;
}Arvore;

typedef struct Pessoa{
    struct Usuario *usuarios;
    struct Livro *sistema;
    struct Arvore *arvore;
    int proximo_codigo;
}Pessoa;


// Prototipos de funcoes ===================

void menu(Pessoa *p); // Prototipos de função foram usados devido a um problema recorrente
void cadastro(Pessoa *p); // de arquitetura do código, o erro de "declaração implícita" estava
void consulta(Pessoa *p); // acontecendo a todo momento, então para evitar estresse desnecessario
void atualizar(Pessoa *p); // foi implementado o prototipo de funções.
void exclusao(Pessoa *p);  // Como as funções já são declaradas no inicio, este erro de "declaração implícita"
void estatisticas(Pessoa *p); //deixou de acontecer.

void cadastrar_livro(Pessoa *p);
void cadastrar_usuario(Pessoa *p);

void consultar_livro(Pessoa *p);
void consultar_usuario(Pessoa *p);
void consultar_emprestimo(Pessoa *p);

void atualizar_livro(Pessoa *p);
void atualizar_usuario(Pessoa *p);

void excluir_livro(Pessoa *p);
void excluir_usuario(Pessoa *p);

void emprestimo(Pessoa *p);
void devolucao(Pessoa *p);

void liberar_memoria(Pessoa *p);

Livro *buscar_livro(Pessoa *p, int codigo);
Usuario *buscar_usuario(Pessoa *p, char email[A]);

Arvore *inserir_arvore(Arvore *raiz, Livro *livro, int *ok);
Arvore *buscar_arvore(Arvore *raiz, int codigo);
Arvore *remover_arvore(Arvore *raiz, int codigo);
Arvore *menor_arvore(Arvore *raiz);
void liberar_arvore(Arvore *raiz);

void mostrar_data(time_t data);

// Insere o codigo do livro na arvore.
// Cada codigo menor fica a esquerda e cada codigo maior fica a direita.
Arvore *inserir_arvore(Arvore *raiz, Livro *livro, int *ok){ //Se ok for 1, algum problema aconteceu.

    if(raiz == NULL){

        Arvore *novo = malloc(sizeof(Arvore));

        if(novo == NULL){
            *ok = 0;
            return NULL;
        }

        novo->codigo = livro->codigo;
        novo->livro = livro;
        novo->esq = NULL;
        novo->dir = NULL;

        return novo;
    }

    if(livro->codigo < raiz->codigo){
        raiz->esq = inserir_arvore(raiz->esq, livro, ok);
    }

    else if(livro->codigo > raiz->codigo){
        raiz->dir = inserir_arvore(raiz->dir, livro, ok);
    }

    else{
        *ok = 0;
    }

    return raiz;
}


// Pesquisa um codigo na arvore binaria
Arvore *buscar_arvore(Arvore *raiz, int codigo){

    while(raiz != NULL){

        if(codigo == raiz->codigo){
            return raiz;
        }

        if(codigo < raiz->codigo){
            raiz = raiz->esq;
        }

        else{
            raiz = raiz->dir;
        }
    }

    return NULL;
}


// Encontra o menor elemento de uma arvore
Arvore *menor_arvore(Arvore *raiz){

    while(raiz != NULL && raiz->esq != NULL){
        raiz = raiz->esq;
    }

    return raiz;
}


// Remove um codigo da arvore binaria
Arvore *remover_arvore(Arvore *raiz, int codigo){

    if(raiz == NULL){
        return NULL;
    }

    if(codigo < raiz->codigo){
        raiz->esq = remover_arvore(raiz->esq, codigo);
    }

    else if(codigo > raiz->codigo){
        raiz->dir = remover_arvore(raiz->dir, codigo);
    }

    else{

        Arvore *aux;

        // Caso 1: no sem filhos
        if(raiz->esq == NULL && raiz->dir == NULL){

            free(raiz);
            return NULL;
        }

        // Caso 2: no com apenas um filho
        if(raiz->esq == NULL){

            aux = raiz->dir;
            free(raiz);
            return aux;
        }

        if(raiz->dir == NULL){

            aux = raiz->esq;
            free(raiz);
            return aux;
        }

        // Caso 3: no com dois filhos
        // Substitui pelo menor elemento da subarvore direita
        aux = menor_arvore(raiz->dir);

        raiz->codigo = aux->codigo;
        raiz->livro = aux->livro;

        raiz->dir = remover_arvore(raiz->dir, aux->codigo);
    }

    return raiz;
}


// Libera todos os nos da arvore
void liberar_arvore(Arvore *raiz){

    if(raiz != NULL){

        liberar_arvore(raiz->esq);
        liberar_arvore(raiz->dir);

        free(raiz);
    }
}


// =========================================


// BUSCAS ==================================

// A busca de livros agora utiliza a arvore binaria
Livro *buscar_livro(Pessoa *p, int codigo){

    Arvore *aux = buscar_arvore(p->arvore, codigo);

    if(aux == NULL){
        return NULL;
    }

    return aux->livro;
}


// Busca um usuario pelo email
Usuario *buscar_usuario(Pessoa *p, char email[A]){

    Usuario *atual = p->usuarios;

    while(atual != NULL){

        if(strcmp(atual->email, email) == 0){
            return atual;
        }

        atual = atual->next;
    }

    return NULL;
}


// =========================================


// CADASTROS ===============================

void cadastrar_usuario(Pessoa *p){

    Usuario *novo = malloc(sizeof(Usuario));

    if(novo == NULL){
        printf("Erro de memoria.\n");
        return;
    }

    printf("Email: ");
    scanf("%s", novo->email);

    if(buscar_usuario(p, novo->email) != NULL){

        printf("Email ja cadastrado.\n");
        free(novo);
        return;
    }

    printf("Nome: ");
    scanf("%s", novo->nome);

    novo->next = p->usuarios;
    p->usuarios = novo;

    printf("Usuario cadastrado com sucesso!\n");
}


void cadastrar_livro(Pessoa *p){

    Livro *novo = malloc(sizeof(Livro));

    if(novo == NULL){
        printf("Erro de memoria.\n");
        return;
    }

    printf("Titulo: ");
    scanf("%s", novo->titulo);

    printf("Autor: ");
    scanf("%s", novo->autor);

    printf("Ano de publicacao: ");
    scanf("%d", &novo->ano_publicado);

    novo->codigo = p->proximo_codigo;
    novo->status = 0;
    novo->emprestimos = 0;

    strcpy(novo->email, "");

    novo->data_emprestimo = 0;
    novo->data_limite = 0;

    // Primeiro insere o livro na arvore.
    // Se a memoria falhar, o cadastro nao sera realizado.
    int ok = 1;

    Arvore *nova_raiz = inserir_arvore(p->arvore, novo, &ok);

    if(ok == 0){

        printf("Erro ao inserir codigo na arvore.\n");
        free(novo);
        return;
    }

    p->arvore = nova_raiz;

    // Insere o livro na lista mantendo a ordem dos codigos
    Livro *atual = p->sistema;
    Livro *anterior = NULL;

    while(atual != NULL && atual->codigo < novo->codigo){

        anterior = atual;
        atual = atual->next;
    }

    if(anterior == NULL){

        novo->next = p->sistema;
        p->sistema = novo;
    }

    else{

        novo->next = atual;
        anterior->next = novo;
    }

    p->proximo_codigo++;

    printf("Livro cadastrado com sucesso!\n");
    printf("Codigo do livro: %d\n", novo->codigo);
}


void cadastro(Pessoa *p){

    int opcao;

    do{

        printf("\n===== CADASTRO =====\n");
        printf("1. Livro\n");
        printf("2. Usuario\n");
        printf("0. Voltar\n");
        printf("Digite: ");
        scanf("%d", &opcao);

        switch(opcao){

            case 1:
                cadastrar_livro(p);
                break;

            case 2:
                cadastrar_usuario(p);
                break;

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
        }

    }while(opcao != 0);
}

void consultar_livro(Pessoa *p){

    int opcao;
    int codigo;
    char autor[A];

    Livro *atual;

    printf("\n===== CONSULTA DE LIVROS =====\n");
    printf("1. Buscar por codigo\n");
    printf("2. Buscar por autor\n");
    printf("Digite: ");
    scanf("%d", &opcao);

    switch(opcao){

        case 1:

            printf("Codigo: ");
            scanf("%d", &codigo);

            atual = buscar_livro(p, codigo);

            if(atual == NULL){
                printf("Livro nao encontrado.\n");
            }

            else{

                printf("\nTitulo: %s\n", atual->titulo);
                printf("Autor: %s\n", atual->autor);
                printf("Ano: %d\n", atual->ano_publicado);
                printf("Codigo: %d\n", atual->codigo);

                if(atual->status == 0){
                    printf("Status: Disponivel\n");
                }

                else{

                    printf("Status: Emprestado\n");
                    printf("Email: %s\n", atual->email);

                    printf("Data do emprestimo: ");
                    mostrar_data(atual->data_emprestimo);

                    printf("Prazo de devolucao: ");
                    mostrar_data(atual->data_limite);
                }
            }

            break;

        case 2:

            printf("Autor: ");
            scanf("%s", autor);

            atual = p->sistema;

            int encontrou = 0;

            while(atual != NULL){

                if(strcmp(atual->autor, autor) == 0){

                    printf("\nTitulo: %s\n", atual->titulo);
                    printf("Ano: %d\n", atual->ano_publicado);
                    printf("Codigo: %d\n", atual->codigo);

                    encontrou = 1;
                }

                atual = atual->next;
            }

            if(encontrou == 0){
                printf("Livro nao encontrado.\n");
            }

            break;

        default:
            printf("Opcao invalida.\n");
    }
}


void consultar_usuario(Pessoa *p){

    int opcao;
    char email[A];
    char nome[A];

    Usuario *atual;
    int encontrou = 0;

    printf("\n===== CONSULTA DE USUARIOS =====\n");
    printf("1. Buscar por email\n");
    printf("2. Buscar por nome\n");
    printf("Digite: ");
    scanf("%d", &opcao);

    switch(opcao){

        case 1:

            printf("Email: ");
            scanf("%s", email);

            atual = buscar_usuario(p, email);

            if(atual == NULL){
                printf("Usuario nao cadastrado.\n");
            }

            else{

                printf("Nome: %s\n", atual->nome);
                printf("Email: %s\n", atual->email);
            }

            break;

        case 2:

            printf("Nome: ");
            scanf("%s", nome);

            atual = p->usuarios;

            while(atual != NULL){

                if(strcmp(atual->nome, nome) == 0){

                    printf("\nNome: %s\n", atual->nome);
                    printf("Email: %s\n", atual->email);

                    encontrou = 1;
                }

                atual = atual->next;
            }

            if(encontrou == 0){
                printf("Usuario nao cadastrado.\n");
            }

            break;

        default:
            printf("Opcao invalida.\n");
    }
}


void consultar_emprestimo(Pessoa *p){

    char email[A];

    Usuario *usuario;
    Livro *atual;

    printf("Email do usuario: ");
    scanf("%s", email);

    usuario = buscar_usuario(p, email);

    if(usuario == NULL){

        printf("Usuario nao cadastrado.\n");
        return;
    }

    atual = p->sistema;

    int encontrou = 0;

    printf("\nLivros emprestados para %s:\n", usuario->nome);

    while(atual != NULL){

        if(atual->status == 1 &&
           strcmp(atual->email, email) == 0){

            printf("\nTitulo: %s\n", atual->titulo);
            printf("Autor: %s\n", atual->autor);
            printf("Codigo: %d\n", atual->codigo);

            printf("Prazo de devolucao: ");
            mostrar_data(atual->data_limite);

            encontrou = 1;
        }

        atual = atual->next;
    }

    if(encontrou == 0){
        printf("Este usuario nao possui livros emprestados.\n");
    }
}


void consulta(Pessoa *p){

    int opcao;

    do{

        printf("\n===== CONSULTA =====\n");
        printf("1. Livros\n");
        printf("2. Usuarios\n");
        printf("3. Emprestimos\n");
        printf("0. Voltar\n");
        printf("Digite: ");
        scanf("%d", &opcao);

        switch(opcao){

            case 1:
                consultar_livro(p);
                break;

            case 2:
                consultar_usuario(p);
                break;

            case 3:
                consultar_emprestimo(p);
                break;

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
        }

    }while(opcao != 0);
}

void atualizar_livro(Pessoa *p){

    int codigo;
    int opcao;

    Livro *atual;

    printf("Codigo do livro: ");
    scanf("%d", &codigo);

    atual = buscar_livro(p, codigo);

    if(atual == NULL){

        printf("Livro nao encontrado.\n");
        return;
    }

    printf("\n1. Titulo\n");
    printf("2. Autor\n");
    printf("3. Ano\n");
    printf("Digite: ");
    scanf("%d", &opcao);

    switch(opcao){

        case 1:
            printf("Novo titulo: ");
            scanf("%s", atual->titulo);
            break;

        case 2:
            printf("Novo autor: ");
            scanf("%s", atual->autor);
            break;

        case 3:
            printf("Novo ano: ");
            scanf("%d", &atual->ano_publicado);
            break;

        default:
            printf("Opcao invalida.\n");
            return;
    }

    printf("Livro atualizado com sucesso!\n");
}


void atualizar_usuario(Pessoa *p){

    char email[A];

    Usuario *atual;

    printf("Email do usuario: ");
    scanf("%s", email);

    atual = buscar_usuario(p, email);

    if(atual == NULL){

        printf("Usuario nao cadastrado.\n");
        return;
    }

    printf("Novo nome: ");
    scanf("%s", atual->nome);

    printf("Usuario atualizado com sucesso!\n");
}


void atualizar(Pessoa *p){

    int opcao;

    do{

        printf("\n===== ATUALIZACAO =====\n");
        printf("1. Livros\n");
        printf("2. Usuarios\n");
        printf("0. Voltar\n");
        printf("Digite: ");
        scanf("%d", &opcao);

        switch(opcao){

            case 1:
                atualizar_livro(p);
                break;

            case 2:
                atualizar_usuario(p);
                break;

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
        }

    }while(opcao != 0);
}

void excluir_livro(Pessoa *p){

    int codigo;

    Livro *atual = p->sistema;
    Livro *anterior = NULL;

    printf("Codigo do livro: ");
    scanf("%d", &codigo);

    // Pesquisa o livro na lista encadeada
    while(atual != NULL && atual->codigo != codigo){

        anterior = atual;
        atual = atual->next;
    }

    if(atual == NULL){

        printf("Livro nao encontrado.\n");
        return;
    }

    if(atual->status == 1){

        printf("Nao e possivel excluir um livro emprestado.\n");
        return;
    }

    // Remove o livro da lista
    if(anterior == NULL){
        p->sistema = atual->next;
    }

    else{
        anterior->next = atual->next;
    }

    // Remove tambem o codigo da arvore binaria
    p->arvore = remover_arvore(p->arvore, codigo);

    free(atual);

    printf("Livro excluido com sucesso!\n");
}


void excluir_usuario(Pessoa *p){

    char email[A];

    Usuario *atual = p->usuarios;
    Usuario *anterior = NULL;

    Livro *livro = p->sistema;

    printf("Email do usuario: ");
    scanf("%s", email);

    // Impede de excluir usuarios que possuem emprestimos ativos
    while(livro != NULL){

        if(livro->status == 1 &&
           strcmp(livro->email, email) == 0){

            printf("Usuario possui livros emprestados.\n");
            return;
        }

        livro = livro->next;
    }

    while(atual != NULL &&
          strcmp(atual->email, email) != 0){

        anterior = atual;
        atual = atual->next;
    }

    if(atual == NULL){

        printf("Usuario nao cadastrado.\n");
        return;
    }

    if(anterior == NULL){
        p->usuarios = atual->next;
    }

    else{
        anterior->next = atual->next;
    }

    free(atual);

    printf("Usuario excluido com sucesso!\n");
}


void exclusao(Pessoa *p){

    int opcao;

    do{

        printf("\n===== EXCLUSAO =====\n");
        printf("1. Livro\n");
        printf("2. Usuario\n");
        printf("0. Voltar\n");
        printf("Digite: ");
        scanf("%d", &opcao);

        switch(opcao){

            case 1:
                excluir_livro(p);
                break;

            case 2:
                excluir_usuario(p);
                break;

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
        }

    }while(opcao != 0);
}

// Ajuda de LLM
//Motivo, não foi usado "#include <time.h>" em sala, então se apresentou dificuldade nesta parte.
//==============================================
void mostrar_data(time_t data){

    struct tm *info = localtime(&data);
    char data_texto[20];

    if(info == NULL){
        printf("Data indisponivel.\n");
        return;
    }

    strftime(data_texto, sizeof(data_texto), "%d/%m/%Y", info);

    printf("%s\n", data_texto);
}
//==========================================

void emprestimo(Pessoa *p){

    int codigo;
    char email[A];

    Livro *livro;
    Usuario *usuario;

    printf("Codigo do livro: ");
    scanf("%d", &codigo);

    livro = buscar_livro(p, codigo);

    if(livro == NULL){

        printf("Livro nao encontrado.\n");
        return;
    }

    if(livro->status == 1){

        printf("Livro ja esta emprestado.\n");
        return;
    }

    printf("Email do usuario: ");
    scanf("%s", email);

    usuario = buscar_usuario(p, email);

    if(usuario == NULL){

        printf("Usuario nao cadastrado.\n");
        return;
    }

    livro->status = 1;
    strcpy(livro->email, email);

    // Registra a data atual do emprestimo
    livro->data_emprestimo = time(NULL);

    // Define o prazo de devolucao para 14 dias depois
    livro->data_limite = livro->data_emprestimo +
                         (PRAZO * 24 * 60 * 60);

    // Incrementa o historico de emprestimos do livro
    livro->emprestimos++;

    printf("Emprestimo realizado com sucesso!\n");

    printf("Data do emprestimo: ");
    mostrar_data(livro->data_emprestimo);

    printf("Prazo de devolucao: ");
    mostrar_data(livro->data_limite);
}


void devolucao(Pessoa *p){

    int codigo;

    Livro *livro;

    printf("Codigo do livro: ");
    scanf("%d", &codigo);

    livro = buscar_livro(p, codigo);

    if(livro == NULL){

        printf("Livro nao encontrado.\n");
        return;
    }

    if(livro->status == 0){

        printf("O livro nao estava emprestado.\n");
        return;
    }

    livro->status = 0;
    strcpy(livro->email, "");

    // Limpa as datas do emprestimo atual
    // O contador historico permanece armazenado
    livro->data_emprestimo = 0;
    livro->data_limite = 0;

    printf("Devolucao realizada com sucesso!\n");
}

void estatisticas(Pessoa *p){

    Livro *livro = p->sistema;
    Livro *mais_emprestado = NULL;

    Usuario *usuario = p->usuarios;
    Usuario *mais_ativo = NULL;

    int maior_quantidade = 0;
    int quantidade;
    int total_atrasados = 0;

    time_t agora = time(NULL);

    printf("\n===== ESTATISTICAS =====\n");

    // 1. Encontra o livro com maior historico de emprestimos
    while(livro != NULL){

        if(mais_emprestado == NULL ||
           livro->emprestimos > maior_quantidade){

            maior_quantidade = livro->emprestimos;
            mais_emprestado = livro;
        }

        livro = livro->next;
    }

    printf("\n--- LIVRO MAIS EMPRESTADO ---\n");

    if(mais_emprestado == NULL ||
       maior_quantidade == 0){

        printf("Ainda nao existem emprestimos registrados.\n");
    }

    else{

        printf("Titulo: %s\n", mais_emprestado->titulo);
        printf("Autor: %s\n", mais_emprestado->autor);
        printf("Codigo: %d\n", mais_emprestado->codigo);
        printf("Quantidade de emprestimos: %d\n",
               mais_emprestado->emprestimos);
    }


    // 2. Conta quantos livros cada usuario possui emprestados
    maior_quantidade = 0;

    while(usuario != NULL){

        quantidade = 0;
        livro = p->sistema;

        while(livro != NULL){

            if(livro->status == 1 &&
               strcmp(livro->email, usuario->email) == 0){

                quantidade++;
            }

            livro = livro->next;
        }

        if(quantidade > maior_quantidade){

            maior_quantidade = quantidade;
            mais_ativo = usuario;
        }

        usuario = usuario->next;
    }

    printf("\n--- USUARIO COM MAIS EMPRESTIMOS ATIVOS ---\n");

    if(mais_ativo == NULL || maior_quantidade == 0){

        printf("Nenhum usuario possui emprestimos ativos.\n");
    }

    else{

        printf("Nome: %s\n", mais_ativo->nome);
        printf("Email: %s\n", mais_ativo->email);
        printf("Livros emprestados: %d\n", maior_quantidade);
    }


    // 3. Procura todos os livros emprestados cujo prazo venceu
    printf("\n--- LIVROS ATRASADOS ---\n");

    livro = p->sistema;

    while(livro != NULL){

        if(livro->status == 1 &&
           agora > livro->data_limite){

            printf("\nTitulo: %s\n", livro->titulo);
            printf("Codigo: %d\n", livro->codigo);
            printf("Usuario: %s\n", livro->email);

            printf("Prazo de devolucao: ");
            mostrar_data(livro->data_limite);

            total_atrasados++;
        }

        livro = livro->next;
    }

    if(total_atrasados == 0){
        printf("Nenhum livro atrasado.\n");
    }

    printf("\nTotal de livros atrasados: %d\n", total_atrasados);
}

void liberar_memoria(Pessoa *p){

    Usuario *usuario;
    Livro *livro;

    while(p->usuarios != NULL){

        usuario = p->usuarios;
        p->usuarios = p->usuarios->next;

        free(usuario);
    }

    while(p->sistema != NULL){

        livro = p->sistema;
        p->sistema = p->sistema->next;

        free(livro);
    }

    // Libera separadamente os nos da arvore
    liberar_arvore(p->arvore);
    p->arvore = NULL;

    printf("Memoria liberada.\n");
}

void menu(Pessoa *p){

    int opcao;

    do{

        printf("\n===== SISTEMA DE BIBLIOTECA =====\n");
        printf("1. Cadastro\n");
        printf("2. Consulta\n");
        printf("3. Atualizacao\n");
        printf("4. Exclusao\n");
        printf("5. Emprestimo\n");
        printf("6. Devolucao\n");
        printf("7. Estatisticas\n");
        printf("0. Sair\n");

        printf("Digite: ");
        scanf("%d", &opcao);

        switch(opcao){

            case 1:
                cadastro(p);
                break;

            case 2:
                consulta(p);
                break;

            case 3:
                atualizar(p);
                break;

            case 4:
                exclusao(p);
                break;

            case 5:
                emprestimo(p);
                break;

            case 6:
                devolucao(p);
                break;

            case 7:
                estatisticas(p);
                break;

            case 0:
                liberar_memoria(p);
                printf("Programa encerrado.\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }

    }while(opcao != 0);
}

int main(){

    Pessoa p;

    p.usuarios = NULL;
    p.sistema = NULL;
    p.arvore = NULL;
    p.proximo_codigo = 1;

    menu(&p);

    return 0;
}
