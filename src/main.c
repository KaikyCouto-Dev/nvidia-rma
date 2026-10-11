#include "cliente.h"
#include "database.h"
#include "estoque.h"
#include "produto.h"
#include "relatorios.h"
#include "rma.h"
#include "utils.h"

#include <stdio.h>

#define DB_PATH "db/nvidia_rma.db"

static void menu_principal(void)
{
    printf("\n");
    utils_separador();
    printf("  Sistema de Gestao de RMA, Garantia e Estoque de Pecas\n");
    printf("  NVIDIA — Trabalho de Conclusao de Curso (ADS)\n");
    utils_separador();
    printf("1 - Gerenciar clientes\n");
    printf("2 - Gerenciar produtos\n");
    printf("3 - Gerenciar estoque por numero de serie\n");
    printf("4 - Registrar venda/entrega de produto ao cliente\n");
    printf("5 - Gerenciar pecas de reposicao\n");
    printf("6 - Gerenciar RMAs\n");
    printf("7 - Relatorios\n");
    printf("8 - Popular banco com dados de teste\n");
    printf("0 - Sair\n");
}

int main(void)
{
    int op = -1;

    if (!database_abrir(DB_PATH)) {
        return 1;
    }
    if (!database_inicializar()) {
        database_fechar();
        return 1;
    }

    while (op != 0) {
        utils_limpar_tela();
        menu_principal();
        peca_alertar_minimos();
        if (!utils_ler_int("Opcao: ", &op)) {
            op = -1;
            continue;
        }
        switch (op) {
        case 1:
            cliente_menu();
            break;
        case 2:
            produto_menu();
            break;
        case 3:
            estoque_menu_series();
            break;
        case 4:
            estoque_menu_venda();
            utils_pausar();
            break;
        case 5:
            estoque_menu_pecas();
            break;
        case 6:
            rma_menu();
            break;
        case 7:
            relatorios_menu();
            break;
        case 8:
            database_popular_teste();
            utils_pausar();
            break;
        case 0:
            printf("Encerrando o sistema.\n");
            break;
        default:
            printf("Opcao invalida.\n");
            utils_pausar();
            break;
        }
    }

    database_fechar();
    return 0;
}
