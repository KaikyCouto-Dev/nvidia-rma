# Manual de uso — Sistema de Gestão de RMA NVIDIA

## Pré-requisitos

- Linux
- GCC
- Pacote `make`

Na raiz do projeto:

```bash
make
make run
```

O banco `db/nvidia_rma.db` é criado na primeira execução. Execute sempre a partir da pasta `nvidia-rma/` para o programa encontrar `sql/schema.sql`.

## Menu principal

```
1 - Gerenciar clientes
2 - Gerenciar produtos
3 - Gerenciar estoque por número de série
4 - Registrar venda/entrega de produto ao cliente
5 - Gerenciar peças de reposição
6 - Gerenciar RMAs
7 - Relatórios
8 - Popular banco com dados de teste
0 - Sair
```

Digite o número da opção e pressione ENTER. Submenus usam `0` para voltar.

Para demonstração rápida, use a opção **8** uma vez. Os dados são fictícios (e-mails `*.fake` e seriais `NV-FAKE-...`).

## Clientes (opção 1)

1. **Cadastrar:** nome e e-mail obrigatórios. O e-mail precisa conter `@` e `.` depois do `@`.
2. **Listar:** mostra ativos e inativos.
3. **Buscar:** por ID, trecho do nome ou e-mail exato.
4. **Editar:** informa o ID e os novos campos.
5. **Excluir:** exclusão lógica (`ativo = 0`). Se houver RMA, a exclusão física é bloqueada e só a inativação é oferecida.

## Produtos (opção 2)

Cadastre nome, categoria, modelo, preço (≥ 0) e meses de garantia. É possível listar, buscar por ID/nome, editar e ativar/desativar. Produto inativo não recebe novo número de série.

## Estoque por número de série (opção 3)

1. Informe o ID de um produto **ativo**.
2. Informe um serial único (duplicata é recusada).
3. A unidade entra como `DISPONIVEL`.

Também é possível listar, buscar pelo serial e alterar o status (`DISPONIVEL`, `VENDIDO`, `EM_RMA`, `SUBSTITUIDO`, `DESCARTADO`). Alterações críticas pedem confirmação.

## Venda / entrega (opção 4)

1. Número de série (precisa estar `DISPONIVEL`).
2. ID do cliente ativo.
3. Data da compra (`AAAA-MM-DD` ou vazio = hoje).

O sistema grava o vínculo em `clientes_produtos` e muda o serial para `VENDIDO`. Seriais já vendidos, em RMA, substituídos ou descartados não podem ser vendidos.

## Peças (opção 5)

Cadastre a peça com quantidade e mínimo. Na listagem, itens abaixo do mínimo aparecem com alerta. No ajuste de quantidade, valor negativo é **saída**; se o saldo ficar negativo, a retirada é recusada.

## RMAs (opção 6)

### Exemplo prático de abertura

Cenário: cliente 1 comprou o serial `NV-FAKE-4090-0004` (depois de cadastrar o serial e vender).

1. Menu **6 → 1** (Abrir RMA).
2. ID do cliente: `1`.
3. Número de série: o serial vendido àquele cliente.
4. O sistema calcula a garantia com `data_compra + meses_garantia`.
   - Dentro da garantia: mensagem de confirmação.
   - Fora da garantia: aviso e pergunta se deseja abrir mesmo assim; a observação `FORA DE GARANTIA` é gravada.
5. Descrição do problema, por exemplo: `Artefatos na tela em carga máxima`.
6. Prioridade: `ALTA` (vazio assume `MEDIA`).

Efeitos: RMA `ABERTO`, serial `EM_RMA`, linha no histórico.

Não é permitido:

- abrir RMA para serial que não está `VENDIDO`;
- abrir para serial de outro cliente;
- abrir segundo RMA enquanto outro ainda não foi concluído, rejeitado ou cancelado.

### Alterar status (6 → 2)

Informe o ID do RMA e o novo status. Toda mudança gera histórico.

- **CONCLUIDO:** grava data de conclusão; pergunta se houve substituição de peças (ID + quantidade, `0` encerra); pergunta se a unidade foi substituída (`SUBSTITUIDO`) ou retornada (`VENDIDO`).
- **REJEITADO** ou **CANCELADO:** serial volta para `VENDIDO`. Mudanças críticas pedem `s/n`.

### Consulta

- **Listar todos** ou **filtrar** por status, cliente, nome de produto ou período (`AAAA-MM-DD`).
- **Detalhes:** cliente, produto, serial, garantia, histórico e peças usadas.

## Relatórios (opção 7)

| # | Relatório | Uso |
| --- | --- | --- |
| 1 | Total de RMAs por status | Visão do pipeline |
| 2 | RMAs dos últimos 30 dias | Volume recente |
| 3 | Tempo médio de conclusão | `julianday(conclusão) - julianday(abertura)` |
| 4 | Produtos com mais RMAs | Qual SKU mais falha |
| 5 | Clientes com mais RMAs | Contas com mais chamados |
| 6 | Peças mais usadas | Consumo de reposição |
| 7 | Peças abaixo do mínimo | Compras pendentes |
| 8 | Seriais em RMA | Unidades paradas no laboratório |
| 9 | Taxas aprovado / rejeitado / cancelado | Percentuais sobre o total |

Os resultados saem em colunas no terminal.

## Fluxo completo sugerido para banca

1. Opção 8 — popular dados de teste.
2. Opção 2 — conferir produtos NVIDIA fictícios.
3. Opção 3 — cadastrar `NV-DEMO-0001` em um produto.
4. Opção 4 — vender `NV-DEMO-0001` ao cliente 1.
5. Opção 6 — abrir RMA, mudar para `EM_ANALISE`, depois `APROVADO`, depois `CONCLUIDO` com peça.
6. Opção 7 — gerar os relatórios 1, 4, 6 e 8.
