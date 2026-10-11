# Sistema de Gestão de RMA, Garantia e Estoque de Peças para a NVIDIA

Aplicativo de **terminal em C** com **SQLite**, desenvolvido como Trabalho de Conclusão de Curso de Análise e Desenvolvimento de Sistemas.

## Problema

No suporte corporativo, a NVIDIA (e seus parceiros) tratam devoluções de hardware por RMA (*Return Material Authorization*): o cliente abre um chamado, a garantia é conferida, a unidade identificada por número de série entra em análise e, se aprovado, há reparo ou troca com consumo de peças. Sem um sistema interno simples, o controle de status, histórico e estoque fica disperso.

## Objetivo

Simular a gestão interna de:

- cadastro de clientes e produtos;
- números de série e vendas;
- peças de reposição;
- abertura e tramitação de RMAs;
- verificação de garantia;
- histórico de atendimento;
- relatórios gerenciais.

O software é funcional, modular e adequado à apresentação em banca, sem interface gráfica.

## Tecnologias

| Item | Escolha |
| --- | --- |
| Linguagem | C11 |
| Banco | SQLite 3 (código amalgamation em `third_party/`) |
| Interface | Console / menu numérico |
| SO | Linux |
| Compilador | GCC |
| Build | Makefile |

Não há frameworks gráficos nem bibliotecas além do SQLite. Entradas do usuário vão para o banco só por *prepared statements*.

## Como compilar

Na raiz do repositório:

```bash
make
```

Equivalente: `make build` ou `make all`. A pasta `build/` é criada automaticamente. O SQLite é compilado junto (`third_party/sqlite3.c`), então não é necessário instalar `sqlite-devel` no sistema.

## Como executar

```bash
make run
```

ou:

```bash
./build/nvidia_rma
```

**Importante:** rode a partir de `nvidia-rma/`, porque o programa lê `sql/schema.sql` e grava `db/nvidia_rma.db`.

```bash
make clean    # remove a pasta build/
```

## Estrutura de pastas

```
nvidia-rma/
├── src/                 # código C (.c / .h)
│   ├── main.c
│   ├── database.c/.h
│   ├── cliente.c/.h
│   ├── produto.c/.h
│   ├── estoque.c/.h     # seriais, venda e peças
│   ├── rma.c/.h
│   ├── relatorios.c/.h
│   └── utils.c/.h
├── sql/
│   ├── schema.sql
│   └── seed.sql
├── db/                  # nvidia_rma.db (gerado na execução)
├── docs/
│   ├── modelo_dados.md
│   └── manual_uso.md
├── third_party/         # sqlite3.c e sqlite3.h
├── Makefile
└── README.md
```

## Funcionalidades

1. **Clientes** — cadastrar, listar, buscar (ID/nome/e-mail), editar, inativar.
2. **Produtos** — CRUD resumido, ativar/desativar, meses de garantia.
3. **Números de série** — cadastro único, listagem, busca, mudança de status.
4. **Venda/entrega** — vincula serial `DISPONIVEL` a um cliente e marca `VENDIDO`.
5. **Peças** — saldo, mínimo, alerta e recusa de retirada sem estoque.
6. **RMA** — abertura com checagem de garantia, histórico automático, filtros, detalhes, conclusão com peças.
7. **Relatórios** — nove consultas SQL formatadas em colunas.
8. **Dados de teste** — clientes, GPUs/DC/rede fictícios, seriais, RMAs e peças.

Regras principais: um serial por cliente; RMA só em serial `VENDIDO`; um RMA aberto por serial; datas `AAAA-MM-DD`.

## Exemplos de uso

Popular o banco e listar RMAs por status:

```text
8          (popular dados de teste)
ENTER
7          (relatórios)
1          (total por status)
```

Abrir um RMA novo (depois de cadastrar produto, serial e vender):

```text
6 → 1
ID do cliente: 1
Número de série: NV-DEMO-0001
Descrição: Sem vídeo após POST
Prioridade: ALTA
```

Concluir com peça:

```text
6 → 2
ID do RMA: (o gerado)
Novo status: CONCLUIDO
Confirmar: s
Houve substituição de peças? s
ID da peça: 1
Quantidade: 1
ID da peça: 0
Unidade substituída? n
```

Detalhes e documentação de regras estão em `docs/manual_uso.md` e `docs/modelo_dados.md`.
