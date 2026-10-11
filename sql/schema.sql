-- =============================================================================
-- Schema: Sistema de Gestão de RMA, Garantia e Estoque de Peças (NVIDIA)
-- SQLite 3 — chaves estrangeiras, constraints e índices
-- =============================================================================

PRAGMA foreign_keys = ON;

-- Clientes corporativos. "ativo" permite exclusão lógica (0 = inativo).
CREATE TABLE IF NOT EXISTS clientes (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    nome          TEXT    NOT NULL,
    empresa       TEXT,
    email         TEXT    UNIQUE NOT NULL,
    telefone      TEXT,
    cidade        TEXT,
    estado        TEXT,
    data_cadastro TEXT    NOT NULL,
    ativo         INTEGER NOT NULL DEFAULT 1 CHECK (ativo IN (0, 1))
);

-- Catálogo de produtos NVIDIA (GPUs, placas de rede, módulos etc.).
CREATE TABLE IF NOT EXISTS produtos (
    id             INTEGER PRIMARY KEY AUTOINCREMENT,
    nome           TEXT    NOT NULL,
    categoria      TEXT    NOT NULL,
    modelo         TEXT    NOT NULL,
    preco          REAL    NOT NULL CHECK (preco >= 0),
    meses_garantia INTEGER NOT NULL CHECK (meses_garantia >= 0),
    ativo          INTEGER NOT NULL DEFAULT 1 CHECK (ativo IN (0, 1))
);

-- Unidades físicas identificadas por número de série.
CREATE TABLE IF NOT EXISTS estoque_series (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    produto_id   INTEGER NOT NULL,
    numero_serie TEXT    UNIQUE NOT NULL,
    status       TEXT    NOT NULL DEFAULT 'DISPONIVEL'
        CHECK (status IN ('DISPONIVEL', 'VENDIDO', 'EM_RMA', 'SUBSTITUIDO', 'DESCARTADO')),
    data_entrada TEXT    NOT NULL,
    FOREIGN KEY (produto_id) REFERENCES produtos(id)
);

-- Vínculo atual de um número de série a um cliente (um serial por vez).
CREATE TABLE IF NOT EXISTS clientes_produtos (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    cliente_id       INTEGER NOT NULL,
    estoque_serie_id INTEGER NOT NULL UNIQUE,
    data_compra      TEXT    NOT NULL,
    FOREIGN KEY (cliente_id)       REFERENCES clientes(id),
    FOREIGN KEY (estoque_serie_id) REFERENCES estoque_series(id)
);

-- Peças de reposição usadas em reparos de RMA.
CREATE TABLE IF NOT EXISTS pecas (
    id                INTEGER PRIMARY KEY AUTOINCREMENT,
    nome              TEXT    NOT NULL,
    categoria         TEXT    NOT NULL,
    quantidade        INTEGER NOT NULL DEFAULT 0 CHECK (quantidade >= 0),
    quantidade_minima INTEGER NOT NULL DEFAULT 5 CHECK (quantidade_minima >= 0),
    ativo             INTEGER NOT NULL DEFAULT 1 CHECK (ativo IN (0, 1))
);

-- Solicitações de Return Material Authorization.
CREATE TABLE IF NOT EXISTS rmas (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    cliente_id       INTEGER NOT NULL,
    estoque_serie_id INTEGER NOT NULL,
    data_abertura    TEXT    NOT NULL,
    descricao_problema TEXT  NOT NULL,
    status           TEXT    NOT NULL DEFAULT 'ABERTO'
        CHECK (status IN (
            'ABERTO', 'EM_ANALISE', 'APROVADO', 'REJEITADO',
            'AGUARDANDO_PECA', 'ENVIADO', 'CONCLUIDO', 'CANCELADO'
        )),
    prioridade       TEXT    NOT NULL DEFAULT 'MEDIA'
        CHECK (prioridade IN ('BAIXA', 'MEDIA', 'ALTA', 'CRITICA')),
    data_conclusao   TEXT,
    observacoes      TEXT,
    FOREIGN KEY (cliente_id)       REFERENCES clientes(id),
    FOREIGN KEY (estoque_serie_id) REFERENCES estoque_series(id)
);

-- Linha do tempo de cada RMA (toda mudança de status gera um registro).
CREATE TABLE IF NOT EXISTS historico_rma (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    rma_id          INTEGER NOT NULL,
    data_evento     TEXT    NOT NULL,
    status_anterior TEXT,
    status_novo     TEXT    NOT NULL,
    descricao       TEXT    NOT NULL,
    FOREIGN KEY (rma_id) REFERENCES rmas(id)
);

-- Peças consumidas em um RMA (normalmente na conclusão).
CREATE TABLE IF NOT EXISTS pecas_utilizadas (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    rma_id          INTEGER NOT NULL,
    peca_id         INTEGER NOT NULL,
    quantidade      INTEGER NOT NULL CHECK (quantidade > 0),
    data_utilizacao TEXT    NOT NULL,
    FOREIGN KEY (rma_id) REFERENCES rmas(id),
    FOREIGN KEY (peca_id) REFERENCES pecas(id)
);

CREATE INDEX IF NOT EXISTS idx_clientes_email        ON clientes(email);
CREATE INDEX IF NOT EXISTS idx_clientes_nome         ON clientes(nome);
CREATE INDEX IF NOT EXISTS idx_produtos_nome         ON produtos(nome);
CREATE INDEX IF NOT EXISTS idx_series_produto        ON estoque_series(produto_id);
CREATE INDEX IF NOT EXISTS idx_series_status         ON estoque_series(status);
CREATE INDEX IF NOT EXISTS idx_series_numero         ON estoque_series(numero_serie);
CREATE INDEX IF NOT EXISTS idx_cp_cliente            ON clientes_produtos(cliente_id);
CREATE INDEX IF NOT EXISTS idx_rmas_cliente          ON rmas(cliente_id);
CREATE INDEX IF NOT EXISTS idx_rmas_serie            ON rmas(estoque_serie_id);
CREATE INDEX IF NOT EXISTS idx_rmas_status           ON rmas(status);
CREATE INDEX IF NOT EXISTS idx_rmas_abertura         ON rmas(data_abertura);
CREATE INDEX IF NOT EXISTS idx_historico_rma         ON historico_rma(rma_id);
CREATE INDEX IF NOT EXISTS idx_pecas_utilizadas_rma  ON pecas_utilizadas(rma_id);
CREATE INDEX IF NOT EXISTS idx_pecas_utilizadas_peca ON pecas_utilizadas(peca_id);
