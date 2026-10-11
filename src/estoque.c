#include "estoque.h"
#include "cliente.h"
#include "database.h"
#include "produto.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

int estoque_serie_por_numero(const char *numero, int *id, int *produto_id, char *status, size_t status_tam)
{
    sqlite3_stmt *stmt = NULL;
    int ok = 0;
    const char *sql =
        "SELECT id, produto_id, status FROM estoque_series WHERE numero_serie = ?;";

    if (!database_preparar(&stmt, sql)) {
        return 0;
    }
    sqlite3_bind_text(stmt, 1, numero, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (id) *id = sqlite3_column_int(stmt, 0);
        if (produto_id) *produto_id = sqlite3_column_int(stmt, 1);
        if (status && status_tam > 0) {
            const unsigned char *st = sqlite3_column_text(stmt, 2);
            snprintf(status, status_tam, "%s", st ? (const char *)st : "");
        }
        ok = 1;
    }
    sqlite3_finalize(stmt);
    return ok;
}

int estoque_atualizar_status_serie(int serie_id, const char *status)
{
    sqlite3_stmt *stmt = NULL;
    int ok = 0;

    if (!utils_status_serie_valido(status)) {
        printf("Status de numero de serie invalido.\n");
        return 0;
    }
    if (!database_preparar(&stmt, "UPDATE estoque_series SET status = ? WHERE id = ?;")) {
        return 0;
    }
    sqlite3_bind_text(stmt, 1, status, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, serie_id);
    if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(database_conexao()) > 0) {
        ok = 1;
    } else {
        database_print_erro("Falha ao atualizar status do numero de serie");
    }
    sqlite3_finalize(stmt);
    return ok;
}

int estoque_cliente_da_serie(int serie_id, int *cliente_id, char *data_compra, size_t tam_data)
{
    sqlite3_stmt *stmt = NULL;
    int ok = 0;
    const char *sql =
        "SELECT cliente_id, data_compra FROM clientes_produtos WHERE estoque_serie_id = ?;";

    if (!database_preparar(&stmt, sql)) {
        return 0;
    }
    sqlite3_bind_int(stmt, 1, serie_id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (cliente_id) *cliente_id = sqlite3_column_int(stmt, 0);
        if (data_compra && tam_data > 0) {
            const unsigned char *d = sqlite3_column_text(stmt, 1);
            snprintf(data_compra, tam_data, "%s", d ? (const char *)d : "");
        }
        ok = 1;
    }
    sqlite3_finalize(stmt);
    return ok;
}

static void serie_exibir(int id)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT s.id, s.numero_serie, s.status, s.data_entrada, p.id, p.nome, p.modelo "
        "FROM estoque_series s JOIN produtos p ON p.id = s.produto_id WHERE s.id = ?;";

    if (!database_preparar(&stmt, sql)) return;
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Numero de serie ID %d nao encontrado.\n", id);
        sqlite3_finalize(stmt);
        return;
    }
    printf("ID serie:     %d\n", sqlite3_column_int(stmt, 0));
    printf("N. serie:     %s\n", sqlite3_column_text(stmt, 1));
    printf("Status:       %s\n", sqlite3_column_text(stmt, 2));
    printf("Entrada:      %s\n", sqlite3_column_text(stmt, 3));
    printf("Produto ID:   %d\n", sqlite3_column_int(stmt, 4));
    printf("Produto:      %s (%s)\n", sqlite3_column_text(stmt, 5), sqlite3_column_text(stmt, 6));
    sqlite3_finalize(stmt);
}

static void serie_cadastrar(void)
{
    int produto_id;
    char serial[TAM_SERIAL], hoje[TAM_DATA];
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Cadastrar numero de serie");
    if (!utils_ler_int("ID do produto: ", &produto_id)) return;
    if (!produto_existe_ativo(produto_id)) {
        printf("Produto inexistente ou inativo.\n");
        return;
    }
    if (!utils_ler_string("Numero de serie: ", serial, sizeof serial, 1)) return;

    utils_data_hoje(hoje, sizeof hoje);
    if (!database_preparar(&stmt,
                           "INSERT INTO estoque_series (produto_id, numero_serie, status, data_entrada) "
                           "VALUES (?, ?, 'DISPONIVEL', ?);")) {
        return;
    }
    sqlite3_bind_int(stmt, 1, produto_id);
    sqlite3_bind_text(stmt, 2, serial, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, hoje, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        if (sqlite3_extended_errcode(database_conexao()) == SQLITE_CONSTRAINT_UNIQUE) {
            printf("Numero de serie duplicado. Cadastro recusado.\n");
        } else {
            database_print_erro("Falha ao cadastrar numero de serie");
        }
    } else {
        printf("Numero de serie cadastrado com ID %lld.\n", database_ultimo_id());
    }
    sqlite3_finalize(stmt);
}

static void serie_listar(void)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT s.id, s.numero_serie, s.status, p.nome, s.data_entrada "
        "FROM estoque_series s JOIN produtos p ON p.id = s.produto_id "
        "ORDER BY s.id;";

    utils_titulo("Numeros de serie");
    if (!database_preparar(&stmt, sql)) return;
    printf("\n%-4s %-22s %-12s %-28s %s\n", "ID", "N. serie", "Status", "Produto", "Entrada");
    utils_separador();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-4d %-22.22s %-12.12s %-28.28s %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2),
               sqlite3_column_text(stmt, 3),
               sqlite3_column_text(stmt, 4));
    }
    sqlite3_finalize(stmt);
}

static void serie_buscar(void)
{
    char serial[TAM_SERIAL];
    int id, produto_id;
    char status[TAM_STATUS];

    utils_titulo("Buscar numero de serie");
    if (!utils_ler_string("Numero de serie: ", serial, sizeof serial, 1)) return;
    if (!estoque_serie_por_numero(serial, &id, &produto_id, status, sizeof status)) {
        printf("Numero de serie nao encontrado.\n");
        return;
    }
    serie_exibir(id);
}

static void serie_alterar_status(void)
{
    char serial[TAM_SERIAL], novo[TAM_STATUS], atual[TAM_STATUS];
    int id, produto_id;

    utils_titulo("Alterar status do numero de serie");
    if (!utils_ler_string("Numero de serie: ", serial, sizeof serial, 1)) return;
    if (!estoque_serie_por_numero(serial, &id, &produto_id, atual, sizeof atual)) {
        printf("Numero de serie nao encontrado.\n");
        return;
    }
    serie_exibir(id);
    utils_menu_status_serie();
    if (!utils_ler_string("Novo status: ", novo, sizeof novo, 1)) return;
    if (!utils_status_serie_valido(novo)) {
        printf("Status invalido.\n");
        return;
    }
    if (!utils_confirmar("Confirmar alteracao de status")) {
        printf("Operacao cancelada.\n");
        return;
    }
    if (estoque_atualizar_status_serie(id, novo)) {
        printf("Status atualizado de %s para %s.\n", atual, novo);
    }
}

void estoque_menu_venda(void)
{
    char serial[TAM_SERIAL], status[TAM_STATUS], data[TAM_DATA];
    int serie_id, produto_id, cliente_id;
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Registrar venda / entrega");
    if (!utils_ler_string("Numero de serie: ", serial, sizeof serial, 1)) return;
    if (!estoque_serie_por_numero(serial, &serie_id, &produto_id, status, sizeof status)) {
        printf("Numero de serie nao encontrado.\n");
        return;
    }
    if (strcmp(status, "DISPONIVEL") != 0) {
        printf("Nao e possivel vender este numero de serie (status atual: %s).\n", status);
        printf("Venda permitida apenas para unidades DISPONIVEL.\n");
        return;
    }
    if (!utils_ler_int("ID do cliente: ", &cliente_id)) return;
    if (!cliente_existe_ativo(cliente_id)) {
        printf("Cliente inexistente ou inativo.\n");
        return;
    }
    if (!utils_ler_string("Data da compra (AAAA-MM-DD, vazio = hoje): ", data, sizeof data, 0)) return;
    if (utils_vazio(data)) {
        utils_data_hoje(data, sizeof data);
    } else if (!utils_validar_data(data)) {
        printf("Data invalida. Use o formato AAAA-MM-DD.\n");
        return;
    }

    if (!utils_confirmar("Confirmar venda/entrega")) {
        printf("Operacao cancelada.\n");
        return;
    }

    database_executar("BEGIN;");
    if (!database_preparar(&stmt,
                           "INSERT INTO clientes_produtos (cliente_id, estoque_serie_id, data_compra) "
                           "VALUES (?, ?, ?);")) {
        database_executar("ROLLBACK;");
        return;
    }
    sqlite3_bind_int(stmt, 1, cliente_id);
    sqlite3_bind_int(stmt, 2, serie_id);
    sqlite3_bind_text(stmt, 3, data, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("Nao foi possivel vincular o numero de serie ao cliente ");
        printf("(ja existe vinculo ou violacao de chave).\n");
        database_print_erro("Detalhe");
        sqlite3_finalize(stmt);
        database_executar("ROLLBACK;");
        return;
    }
    sqlite3_finalize(stmt);

    if (!estoque_atualizar_status_serie(serie_id, "VENDIDO")) {
        database_executar("ROLLBACK;");
        return;
    }
    database_executar("COMMIT;");
    printf("Venda registrada. Serial %s agora esta VENDIDO para o cliente %d.\n", serial, cliente_id);
}

static void peca_listar(void)
{
    sqlite3_stmt *stmt = NULL;
    utils_titulo("Pecas de reposicao");
    if (!database_preparar(&stmt,
                           "SELECT id, nome, categoria, quantidade, quantidade_minima, ativo "
                           "FROM pecas ORDER BY id;")) {
        return;
    }
    printf("\n%-4s %-28s %-16s %8s %8s %s\n",
           "ID", "Nome", "Categoria", "Qtd", "Min", "Alerta");
    utils_separador();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int qtd = sqlite3_column_int(stmt, 3);
        int min = sqlite3_column_int(stmt, 4);
        printf("%-4d %-28.28s %-16.16s %8d %8d %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2),
               qtd, min,
               qtd < min ? "ABAIXO DO MINIMO" : (sqlite3_column_int(stmt, 5) ? "" : "inativa"));
    }
    sqlite3_finalize(stmt);
}

void peca_alertar_minimos(void)
{
    int n = database_contar(
        "SELECT COUNT(*) FROM pecas WHERE ativo = 1 AND quantidade < quantidade_minima;");
    if (n > 0) {
        printf("\n*** ALERTA: %d peca(s) abaixo da quantidade minima. ***\n", n);
    }
}

static void peca_cadastrar(void)
{
    char nome[TAM_NOME], categoria[TAM_CATEGORIA];
    int qtd, min;
    sqlite3_stmt *stmt = NULL;

    utils_titulo("Cadastrar peca");
    if (!utils_ler_string("Nome: ", nome, sizeof nome, 1)) return;
    if (!utils_ler_string("Categoria: ", categoria, sizeof categoria, 1)) return;
    if (!utils_ler_int("Quantidade inicial: ", &qtd) || qtd < 0) {
        printf("Quantidade invalida.\n");
        return;
    }
    if (!utils_ler_int("Quantidade minima: ", &min) || min < 0) {
        printf("Quantidade minima invalida.\n");
        return;
    }

    if (!database_preparar(&stmt,
                           "INSERT INTO pecas (nome, categoria, quantidade, quantidade_minima, ativo) "
                           "VALUES (?, ?, ?, ?, 1);")) {
        return;
    }
    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, categoria, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, qtd);
    sqlite3_bind_int(stmt, 4, min);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        database_print_erro("Falha ao cadastrar peca");
    } else {
        printf("Peca cadastrada com ID %lld.\n", database_ultimo_id());
        if (qtd < min) {
            printf("ALERTA: quantidade inicial abaixo do minimo.\n");
        }
    }
    sqlite3_finalize(stmt);
}

static void peca_atualizar_qtd(void)
{
    int id, delta, atual, min, nova;
    sqlite3_stmt *stmt = NULL;

    peca_listar();
    if (!utils_ler_int("ID da peca: ", &id)) return;
    if (!database_preparar(&stmt,
                           "SELECT quantidade, quantidade_minima, ativo FROM pecas WHERE id = ?;")) {
        return;
    }
    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Peca nao encontrada.\n");
        sqlite3_finalize(stmt);
        return;
    }
    atual = sqlite3_column_int(stmt, 0);
    min = sqlite3_column_int(stmt, 1);
    sqlite3_finalize(stmt);

    printf("Quantidade atual: %d (minimo: %d)\n", atual, min);
    printf("Informe o ajuste (positivo entra, negativo sai).\n");
    if (!utils_ler_int("Ajuste: ", &delta)) return;
    nova = atual + delta;
    if (nova < 0) {
        printf("Retirada recusada: quantidade insuficiente (disponivel: %d).\n", atual);
        return;
    }

    if (!database_preparar(&stmt, "UPDATE pecas SET quantidade = ? WHERE id = ?;")) return;
    sqlite3_bind_int(stmt, 1, nova);
    sqlite3_bind_int(stmt, 2, id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        database_print_erro("Falha ao atualizar estoque da peca");
    } else {
        printf("Nova quantidade: %d\n", nova);
        if (nova < min) {
            printf("ALERTA: peca abaixo da quantidade minima (%d).\n", min);
        }
    }
    sqlite3_finalize(stmt);
}

int peca_baixar_estoque(int peca_id, int quantidade)
{
    sqlite3_stmt *stmt = NULL;
    int atual, min, nova;

    if (quantidade <= 0) {
        printf("Quantidade deve ser maior que zero.\n");
        return 0;
    }
    if (!database_preparar(&stmt,
                           "SELECT quantidade, quantidade_minima FROM pecas WHERE id = ? AND ativo = 1;")) {
        return 0;
    }
    sqlite3_bind_int(stmt, 1, peca_id);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Peca %d nao encontrada ou inativa.\n", peca_id);
        sqlite3_finalize(stmt);
        return 0;
    }
    atual = sqlite3_column_int(stmt, 0);
    min = sqlite3_column_int(stmt, 1);
    sqlite3_finalize(stmt);

    if (quantidade > atual) {
        printf("Estoque insuficiente da peca %d (disponivel: %d, solicitado: %d).\n",
               peca_id, atual, quantidade);
        return 0;
    }
    nova = atual - quantidade;
    if (!database_preparar(&stmt, "UPDATE pecas SET quantidade = ? WHERE id = ?;")) return 0;
    sqlite3_bind_int(stmt, 1, nova);
    sqlite3_bind_int(stmt, 2, peca_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        database_print_erro("Falha ao baixar peca");
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_finalize(stmt);
    if (nova < min) {
        printf("ALERTA: peca %d ficou abaixo do minimo (%d). Saldo: %d.\n", peca_id, min, nova);
    }
    return 1;
}

void estoque_menu_series(void)
{
    int op = -1;
    while (op != 0) {
        utils_limpar_tela();
        utils_titulo("Estoque por numero de serie");
        printf("1 - Cadastrar numero de serie\n");
        printf("2 - Listar numeros de serie\n");
        printf("3 - Buscar numero de serie\n");
        printf("4 - Alterar status\n");
        printf("0 - Voltar\n");
        if (!utils_ler_int("Opcao: ", &op)) { op = -1; continue; }
        switch (op) {
        case 1: serie_cadastrar(); utils_pausar(); break;
        case 2: serie_listar(); utils_pausar(); break;
        case 3: serie_buscar(); utils_pausar(); break;
        case 4: serie_alterar_status(); utils_pausar(); break;
        case 0: break;
        default: printf("Opcao invalida.\n"); utils_pausar(); break;
        }
    }
}

void estoque_menu_pecas(void)
{
    int op = -1;
    while (op != 0) {
        utils_limpar_tela();
        utils_titulo("Pecas de reposicao");
        peca_alertar_minimos();
        printf("1 - Cadastrar peca\n");
        printf("2 - Listar pecas\n");
        printf("3 - Atualizar quantidade\n");
        printf("0 - Voltar\n");
        if (!utils_ler_int("Opcao: ", &op)) { op = -1; continue; }
        switch (op) {
        case 1: peca_cadastrar(); utils_pausar(); break;
        case 2: peca_listar(); peca_alertar_minimos(); utils_pausar(); break;
        case 3: peca_atualizar_qtd(); utils_pausar(); break;
        case 0: break;
        default: printf("Opcao invalida.\n"); utils_pausar(); break;
        }
    }
}
