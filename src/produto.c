#include "produto.h"
#include "database.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

static void produto_listar_interno(sqlite3_stmt *stmt)
{
    printf("\n%-4s %-28s %-16s %-20s %10s %8s %s\n",
           "ID", "Nome", "Categoria", "Modelo", "Preco", "Garantia", "Ativo");
    utils_separador();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *nome = sqlite3_column_text(stmt, 1);
        const unsigned char *cat = sqlite3_column_text(stmt, 2);
        const unsigned char *mod = sqlite3_column_text(stmt, 3);
        printf("%-4d %-28.28s %-16.16s %-20.20s %10.2f %7d m %s\n",
               sqlite3_column_int(stmt, 0),
               nome ? (const char *)nome : "",
               cat ? (const char *)cat : "",
               mod ? (const char *)mod : "",
               sqlite3_column_double(stmt, 4),
               sqlite3_column_int(stmt, 5),
               sqlite3_column_int(stmt, 6) ? "sim" : "nao");
    }
}

void produto_exibir_por_id(int id)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT id, nome, categoria, modelo, preco, meses_garantia, ativo "
        "FROM produtos WHERE id = ?;";

    if (!database_preparar(&stmt, sql)) {
        return;
    }
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Produto %d nao encontrado.\n", id);
        sqlite3_finalize(stmt);
        return;
    }
    printf("ID:              %d\n", sqlite3_column_int(stmt, 0));
    printf("Nome:            %s\n", sqlite3_column_text(stmt, 1));
    printf("Categoria:       %s\n", sqlite3_column_text(stmt, 2));
    printf("Modelo:          %s\n", sqlite3_column_text(stmt, 3));
    printf("Preco:           %.2f\n", sqlite3_column_double(stmt, 4));
    printf("Garantia:        %d meses\n", sqlite3_column_int(stmt, 5));
    printf("Ativo:           %s\n", sqlite3_column_int(stmt, 6) ? "sim" : "nao");
    sqlite3_finalize(stmt);
}

int produto_existe_ativo(int id)
{
    sqlite3_stmt *stmt = NULL;
    int ok = 0;

    if (!database_preparar(&stmt, "SELECT ativo FROM produtos WHERE id = ?;")) {
        return 0;
    }
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ok = sqlite3_column_int(stmt, 0) == 1;
    }
    sqlite3_finalize(stmt);
    return ok;
}

static void produto_cadastrar(void)
{
    Produto p;
    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "INSERT INTO produtos (nome, categoria, modelo, preco, meses_garantia, ativo) "
        "VALUES (?, ?, ?, ?, ?, 1);";

    memset(&p, 0, sizeof p);
    utils_titulo("Cadastrar produto");
    if (!utils_ler_string("Nome: ", p.nome, sizeof p.nome, 1)) return;
    if (!utils_ler_string("Categoria: ", p.categoria, sizeof p.categoria, 1)) return;
    if (!utils_ler_string("Modelo: ", p.modelo, sizeof p.modelo, 1)) return;
    if (!utils_ler_double("Preco: ", &p.preco) || p.preco < 0) {
        printf("Preco invalido.\n");
        return;
    }
    if (!utils_ler_int("Meses de garantia: ", &p.meses_garantia) || p.meses_garantia < 0) {
        printf("Garantia invalida.\n");
        return;
    }

    if (!database_preparar(&stmt, sql)) return;
    sqlite3_bind_text(stmt, 1, p.nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, p.categoria, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, p.modelo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, p.preco);
    sqlite3_bind_int(stmt, 5, p.meses_garantia);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        database_print_erro("Falha ao cadastrar produto");
    } else {
        printf("Produto cadastrado com ID %lld.\n", database_ultimo_id());
    }
    sqlite3_finalize(stmt);
}

static void produto_listar(void)
{
    sqlite3_stmt *stmt = NULL;
    utils_titulo("Lista de produtos");
    if (!database_preparar(&stmt,
                           "SELECT id, nome, categoria, modelo, preco, meses_garantia, ativo "
                           "FROM produtos ORDER BY id;")) {
        return;
    }
    produto_listar_interno(stmt);
    sqlite3_finalize(stmt);
}

static void produto_buscar(void)
{
    int op;
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Buscar produto");
    printf("1 - Por ID\n2 - Por nome\n");
    if (!utils_ler_int_faixa("Opcao: ", &op, 1, 2)) return;

    if (op == 1) {
        int id;
        if (!utils_ler_int("ID: ", &id)) return;
        produto_exibir_por_id(id);
        return;
    }

    {
        char nome[TAM_NOME];
        char like[TAM_NOME + 4];
        if (!utils_ler_string("Nome (parcial): ", nome, sizeof nome, 1)) return;
        snprintf(like, sizeof like, "%%%s%%", nome);
        if (!database_preparar(&stmt,
                               "SELECT id, nome, categoria, modelo, preco, meses_garantia, ativo "
                               "FROM produtos WHERE nome LIKE ? ORDER BY nome;")) {
            return;
        }
        sqlite3_bind_text(stmt, 1, like, -1, SQLITE_TRANSIENT);
        produto_listar_interno(stmt);
        sqlite3_finalize(stmt);
    }
}

static void produto_editar(void)
{
    int id, meses;
    char nome[TAM_NOME], categoria[TAM_CATEGORIA], modelo[TAM_MODELO];
    double preco;
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Editar produto");
    if (!utils_ler_int("ID do produto: ", &id)) return;
    produto_exibir_por_id(id);

    if (!utils_ler_string("Nome: ", nome, sizeof nome, 1)) return;
    if (!utils_ler_string("Categoria: ", categoria, sizeof categoria, 1)) return;
    if (!utils_ler_string("Modelo: ", modelo, sizeof modelo, 1)) return;
    if (!utils_ler_double("Preco: ", &preco) || preco < 0) {
        printf("Preco invalido.\n");
        return;
    }
    if (!utils_ler_int("Meses de garantia: ", &meses) || meses < 0) {
        printf("Garantia invalida.\n");
        return;
    }
    if (!utils_confirmar("Confirmar alteracao")) {
        printf("Alteracao cancelada.\n");
        return;
    }

    if (!database_preparar(&stmt,
                           "UPDATE produtos SET nome=?, categoria=?, modelo=?, preco=?, meses_garantia=? "
                           "WHERE id=?;")) {
        return;
    }
    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, categoria, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, modelo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, preco);
    sqlite3_bind_int(stmt, 5, meses);
    sqlite3_bind_int(stmt, 6, id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        database_print_erro("Falha ao atualizar produto");
    } else if (sqlite3_changes(database_conexao()) == 0) {
        printf("Produto nao encontrado.\n");
    } else {
        printf("Produto atualizado.\n");
    }
    sqlite3_finalize(stmt);
}

static void produto_ativar_desativar(void)
{
    int id, ativo;
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Ativar / desativar produto");
    if (!utils_ler_int("ID do produto: ", &id)) return;
    produto_exibir_por_id(id);

    if (!database_preparar(&stmt, "SELECT ativo FROM produtos WHERE id = ?;")) return;
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Produto nao encontrado.\n");
        sqlite3_finalize(stmt);
        return;
    }
    ativo = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (!utils_confirmar(ativo ? "Desativar este produto" : "Ativar este produto")) {
        printf("Operacao cancelada.\n");
        return;
    }

    if (!database_preparar(&stmt, "UPDATE produtos SET ativo = ? WHERE id = ?;")) return;
    sqlite3_bind_int(stmt, 1, ativo ? 0 : 1);
    sqlite3_bind_int(stmt, 2, id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        database_print_erro("Falha ao alterar status do produto");
    } else {
        printf("Produto agora esta %s.\n", ativo ? "inativo" : "ativo");
    }
    sqlite3_finalize(stmt);
}

void produto_menu(void)
{
    int op = -1;
    while (op != 0) {
        utils_limpar_tela();
        utils_titulo("Gerenciar produtos");
        printf("1 - Cadastrar produto\n");
        printf("2 - Listar produtos\n");
        printf("3 - Buscar produto\n");
        printf("4 - Editar produto\n");
        printf("5 - Ativar/desativar produto\n");
        printf("0 - Voltar\n");
        if (!utils_ler_int("Opcao: ", &op)) {
            op = -1;
            continue;
        }
        switch (op) {
        case 1: produto_cadastrar(); utils_pausar(); break;
        case 2: produto_listar(); utils_pausar(); break;
        case 3: produto_buscar(); utils_pausar(); break;
        case 4: produto_editar(); utils_pausar(); break;
        case 5: produto_ativar_desativar(); utils_pausar(); break;
        case 0: break;
        default: printf("Opcao invalida.\n"); utils_pausar(); break;
        }
    }
}
