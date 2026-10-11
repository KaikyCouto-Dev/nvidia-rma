# Modelo de dados — Sistema de Gestão de RMA NVIDIA

O banco é SQLite (`db/nvidia_rma.db`), criado a partir de `sql/schema.sql`. As datas são gravadas como texto ISO 8601 (`AAAA-MM-DD`). Chaves estrangeiras ficam ativas (`PRAGMA foreign_keys = ON`).

Campo extra em `clientes`: `ativo` (0/1) para exclusão lógica, exigida pelo módulo de clientes.

## Tabelas

### clientes
Cadastro de empresas/contatos que abrem chamados.

| Coluna | Tipo | Observação |
| --- | --- | --- |
| id | INTEGER PK | AUTOINCREMENT |
| nome | TEXT NOT NULL | Contato |
| empresa | TEXT | Opcional |
| email | TEXT UNIQUE NOT NULL | Validado na aplicação (`@` e `.`) |
| telefone, cidade, estado | TEXT | Opcionais |
| data_cadastro | TEXT NOT NULL | ISO 8601 |
| ativo | INTEGER NOT NULL | 1 = ativo, 0 = inativo |

### produtos
Catálogo (GPU, workstation, data center, rede, módulo).

| Coluna | Tipo | Observação |
| --- | --- | --- |
| id | INTEGER PK | |
| nome, categoria, modelo | TEXT NOT NULL | |
| preco | REAL NOT NULL | ≥ 0 |
| meses_garantia | INTEGER NOT NULL | Usado no cálculo de garantia |
| ativo | INTEGER NOT NULL | 1 ativo / 0 inativo |

### estoque_series
Cada unidade física tem um número de série único.

**Status possíveis:** `DISPONIVEL`, `VENDIDO`, `EM_RMA`, `SUBSTITUIDO`, `DESCARTADO`.

### clientes_produtos
Vínculo de venda/entrega. `estoque_serie_id` é **UNIQUE**: um serial só pode estar com um cliente por vez.

### pecas
Peças de reposição. `quantidade_minima` dispara alerta quando o saldo fica abaixo.

### rmas
Solicitação de Return Material Authorization.

**Status possíveis:** `ABERTO`, `EM_ANALISE`, `APROVADO`, `REJEITADO`, `AGUARDANDO_PECA`, `ENVIADO`, `CONCLUIDO`, `CANCELADO`.

**Prioridades:** `BAIXA`, `MEDIA`, `ALTA`, `CRITICA`.

### historico_rma
Uma linha por evento. Toda abertura e toda mudança de status gera registro automático.

### pecas_utilizadas
Consumo de peças em um RMA (em geral na conclusão). A quantidade é descontada de `pecas`.

## Relacionamentos

```
clientes 1──N clientes_produtos N──1 estoque_series N──1 produtos
clientes 1──N rmas N──1 estoque_series
rmas 1──N historico_rma
rmas 1──N pecas_utilizadas N──1 pecas
```

- Um cliente possui vários seriais comprados e vários RMAs.
- Um serial pertence a um produto e, no máximo, a um vínculo de venda.
- Um RMA aponta para o cliente e para o serial; o produto é obtido via `estoque_series`.

## Regras de negócio refletidas no modelo e no código

1. Número de série único (`UNIQUE` em `estoque_series.numero_serie`).
2. Um serial ligado a um único cliente (`UNIQUE` em `clientes_produtos.estoque_serie_id`).
3. RMA só abre se o serial está `VENDIDO` e pertence ao cliente informado.
4. Não há dois RMAs “em andamento” (status diferente de `CONCLUIDO`, `REJEITADO`, `CANCELADO`) para o mesmo serial.
5. Abertura: serial → `EM_RMA` + histórico inicial.
6. Fora da garantia: aviso e observação `FORA DE GARANTIA`; a abertura ainda é permitida.
7. `REJEITADO` / `CANCELADO`: serial volta para `VENDIDO`.
8. `CONCLUIDO`: grava `data_conclusao`; pode registrar peças e definir `VENDIDO` ou `SUBSTITUIDO`.
9. Baixa de peça recusada se a quantidade for insuficiente.
10. Cliente com RMA não é apagado fisicamente; apenas inativado (`ativo = 0`).
11. Consultas com dado do usuário usam *prepared statements* (sem concatenar SQL).
