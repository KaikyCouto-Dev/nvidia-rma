#include "relatorios.h"
#include "database.h"
#include "utils.h"

#include <stdio.h>

static void rel_total_por_status(void)
{
    utils_titulo("Total de RMAs por status");
    database_imprimir_consulta(
        "SELECT status AS status, COUNT(*) AS total "
        "FROM rmas GROUP BY status ORDER BY total DESC;");
}

static void rel_abertos_30_dias(void)
{
    utils_titulo("RMAs abertos nos ultimos 30 dias");
    database_imprimir_consulta(
        "SELECT r.id AS id, r.data_abertura AS abertura, r.status AS status, "
        "c.nome AS cliente, s.numero_serie AS serial "
        "FROM rmas r "
        "JOIN clientes c ON c.id = r.cliente_id "
        "JOIN estoque_series s ON s.id = r.estoque_serie_id "
        "WHERE date(r.data_abertura) >= date('now', '-30 days') "
        "ORDER BY r.data_abertura DESC;");
}

static void rel_tempo_medio(void)
{
    utils_titulo("Tempo medio de conclusao (RMAs CONCLUIDO)");
    database_imprimir_consulta(
        "SELECT COUNT(*) AS concluidos, "
        "ROUND(AVG(julianday(data_conclusao) - julianday(data_abertura)), 2) AS media_dias "
        "FROM rmas WHERE status = 'CONCLUIDO' AND data_conclusao IS NOT NULL;");
}

static void rel_produtos_mais_rma(void)
{
    utils_titulo("Produtos com mais RMAs");
    database_imprimir_consulta(
        "SELECT p.nome AS produto, p.modelo AS modelo, COUNT(*) AS total_rmas "
        "FROM rmas r "
        "JOIN estoque_series s ON s.id = r.estoque_serie_id "
        "JOIN produtos p ON p.id = s.produto_id "
        "GROUP BY p.id ORDER BY total_rmas DESC, p.nome;");
}

static void rel_clientes_mais_rma(void)
{
    utils_titulo("Clientes com mais RMAs");
    database_imprimir_consulta(
        "SELECT c.nome AS cliente, c.empresa AS empresa, COUNT(*) AS total_rmas "
        "FROM rmas r JOIN clientes c ON c.id = r.cliente_id "
        "GROUP BY c.id ORDER BY total_rmas DESC, c.nome;");
}

static void rel_pecas_mais_usadas(void)
{
    utils_titulo("Pecas mais utilizadas em substituicoes");
    database_imprimir_consulta(
        "SELECT p.nome AS peca, p.categoria AS categoria, SUM(pu.quantidade) AS total "
        "FROM pecas_utilizadas pu JOIN pecas p ON p.id = pu.peca_id "
        "GROUP BY p.id ORDER BY total DESC, p.nome;");
}

static void rel_pecas_abaixo_minimo(void)
{
    utils_titulo("Pecas abaixo do estoque minimo");
    database_imprimir_consulta(
        "SELECT id, nome, quantidade, quantidade_minima AS minimo, "
        "(quantidade_minima - quantidade) AS deficit "
        "FROM pecas WHERE quantidade < quantidade_minima ORDER BY deficit DESC;");
}

static void rel_series_em_rma(void)
{
    utils_titulo("Numeros de serie atualmente em RMA");
    database_imprimir_consulta(
        "SELECT s.numero_serie AS serial, s.status AS status_serie, p.nome AS produto, "
        "r.id AS rma_id, r.status AS status_rma "
        "FROM estoque_series s "
        "JOIN produtos p ON p.id = s.produto_id "
        "LEFT JOIN rmas r ON r.estoque_serie_id = s.id "
        "AND r.status NOT IN ('CONCLUIDO', 'REJEITADO', 'CANCELADO') "
        "WHERE s.status = 'EM_RMA' "
        "ORDER BY s.numero_serie;");
}

static void rel_taxas(void)
{
    utils_titulo("Taxa de RMAs aprovados, rejeitados e cancelados");
    database_imprimir_consulta(
        "SELECT "
        "  grupo, "
        "  quantidade, "
        "  ROUND(100.0 * quantidade / NULLIF((SELECT COUNT(*) FROM rmas), 0), 2) AS percentual "
        "FROM ("
        "  SELECT 'APROVADO' AS grupo, COUNT(*) AS quantidade FROM rmas WHERE status = 'APROVADO' "
        "  UNION ALL "
        "  SELECT 'CONCLUIDO (fluxo aprovado)', COUNT(*) FROM rmas WHERE status = 'CONCLUIDO' "
        "  UNION ALL "
        "  SELECT 'REJEITADO', COUNT(*) FROM rmas WHERE status = 'REJEITADO' "
        "  UNION ALL "
        "  SELECT 'CANCELADO', COUNT(*) FROM rmas WHERE status = 'CANCELADO' "
        "  UNION ALL "
        "  SELECT 'TOTAL', COUNT(*) FROM rmas "
        ");");
}

void relatorios_menu(void)
{
    int op = -1;
    while (op != 0) {
        utils_limpar_tela();
        utils_titulo("Relatorios");
        printf("1 - Total de RMAs por status\n");
        printf("2 - RMAs abertos nos ultimos 30 dias\n");
        printf("3 - Tempo medio de conclusao dos RMAs concluidos\n");
        printf("4 - Produtos com mais RMAs\n");
        printf("5 - Clientes com mais RMAs\n");
        printf("6 - Pecas mais utilizadas em substituicoes\n");
        printf("7 - Pecas abaixo do estoque minimo\n");
        printf("8 - Numeros de serie atualmente em RMA\n");
        printf("9 - Taxa de RMAs aprovados, rejeitados e cancelados\n");
        printf("0 - Voltar\n");
        if (!utils_ler_int("Opcao: ", &op)) { op = -1; continue; }
        switch (op) {
        case 1: rel_total_por_status(); utils_pausar(); break;
        case 2: rel_abertos_30_dias(); utils_pausar(); break;
        case 3: rel_tempo_medio(); utils_pausar(); break;
        case 4: rel_produtos_mais_rma(); utils_pausar(); break;
        case 5: rel_clientes_mais_rma(); utils_pausar(); break;
        case 6: rel_pecas_mais_usadas(); utils_pausar(); break;
        case 7: rel_pecas_abaixo_minimo(); utils_pausar(); break;
        case 8: rel_series_em_rma(); utils_pausar(); break;
        case 9: rel_taxas(); utils_pausar(); break;
        case 0: break;
        default: printf("Opcao invalida.\n"); utils_pausar(); break;
        }
    }
}
