-- Dados ficticios para demonstracao do TCC. Nenhum cliente ou serial real.
-- Datas no formato ISO 8601 (AAAA-MM-DD). Referencia de "hoje": 2026-10-11.

INSERT INTO clientes (nome, empresa, email, telefone, cidade, estado, data_cadastro, ativo) VALUES
('Ana Paula Ferreira', 'AlphaCloud Labs', 'ana.ferreira@alphacloud.fake', '1130011001', 'Sao Paulo', 'SP', '2024-02-12', 1),
('Bruno Henrique Costa', 'NorteCompute', 'bruno.costa@nortecompute.fake', '9232012002', 'Manaus', 'AM', '2024-03-05', 1),
('Camila Souza Dias', 'Atlas HPC', 'camila.dias@atlashpc.fake', '2133013003', 'Rio de Janeiro', 'RJ', '2024-04-18', 1),
('Diego Martins Lima', 'Pampa Data', 'diego.lima@pampadata.fake', '5134014004', 'Porto Alegre', 'RS', '2024-05-22', 1),
('Eduarda Nogueira', 'Cerrado AI', 'eduarda.nogueira@cerradoai.fake', '6135015005', 'Brasilia', 'DF', '2024-06-09', 1),
('Felipe Andrade', 'Recife Neurons', 'felipe.andrade@recifeneurons.fake', '8136016006', 'Recife', 'PE', '2024-07-30', 1),
('Gabriela Pinto', 'Minas Vision', 'gabriela.pinto@minasvision.fake', '3137017007', 'Belo Horizonte', 'MG', '2024-08-14', 1),
('Henrique Alves', 'Litoral Render', 'henrique.alves@litoralrender.fake', '4138018008', 'Curitiba', 'PR', '2024-09-01', 1),
('Isabela Rocha', 'Bahia Inference', 'isabela.rocha@bahia-inf.fake', '7139019009', 'Salvador', 'BA', '2025-01-11', 1),
('Joao Victor Melo', 'Sampa Studio 3D', 'joao.melo@sampastudio.fake', '1140020010', 'Sao Paulo', 'SP', '2025-02-20', 1),
('Karina Teixeira', 'Orbit Networks', 'karina.teixeira@orbitnet.fake', '1141021011', 'Campinas', 'SP', '2025-03-08', 1),
('Lucas Barbosa', 'Andes Training', 'lucas.barbosa@andestraining.fake', '1142022012', 'Florianopolis', 'SC', '2025-04-02', 1);

INSERT INTO produtos (nome, categoria, modelo, preco, meses_garantia, ativo) VALUES
('NVIDIA GeForce RTX 4090 24GB', 'GPU', 'RTX4090-24G-FAKE', 19990.00, 36, 1),
('NVIDIA RTX 6000 Ada Generation', 'Workstation', 'RTX6000-ADA-FAKE', 28990.00, 36, 1),
('NVIDIA A100 80GB SXM4', 'Data Center', 'A100-SXM4-80-FAKE', 89000.00, 36, 1),
('NVIDIA H100 80GB SXM5', 'Data Center', 'H100-SXM5-80-FAKE', 159000.00, 36, 1),
('NVIDIA L40S 48GB', 'Data Center', 'L40S-48G-FAKE', 64900.00, 36, 1),
('NVIDIA ConnectX-7 InfiniBand', 'Rede', 'MCX75310-FAKE', 8900.00, 24, 1),
('NVIDIA BlueField-3 DPU', 'Rede', 'BF3-DPU-FAKE', 18400.00, 24, 1),
('NVIDIA Spectrum-4 Switch Module', 'Modulo', 'SP4-MOD-FAKE', 22700.00, 12, 1);

INSERT INTO pecas (nome, categoria, quantidade, quantidade_minima, ativo) VALUES
('Ventoinha axial GPU 92mm', 'Cooling', 18, 5, 1),
('Pasta termica HPC 4g', 'Cooling', 40, 10, 1),
('Conector 12VHPWR', 'Energia', 12, 6, 1),
('Modulo GDDR simulado', 'Memoria', 8, 4, 1),
('Dissipador vapor chamber', 'Cooling', 6, 3, 1),
('Cabo NVLink fake', 'Interconnect', 9, 4, 1),
('Riser PCIe 5.0', 'Placa', 15, 5, 1),
('Kit parafusos GPU', 'Mecanico', 50, 10, 1),
('Pad termico GPU', 'Cooling', 22, 8, 1),
('Sensor de temperatura', 'Eletronica', 14, 5, 1),
('I/O shield workstation', 'Mecanico', 7, 4, 1),
('Cooler blower datacenter', 'Cooling', 3, 5, 1);

INSERT INTO estoque_series (produto_id, numero_serie, status, data_entrada) VALUES
((SELECT id FROM produtos WHERE modelo='RTX4090-24G-FAKE'), 'NV-FAKE-4090-0001', 'EM_RMA', '2024-11-02'),
((SELECT id FROM produtos WHERE modelo='RTX4090-24G-FAKE'), 'NV-FAKE-4090-0002', 'EM_RMA', '2024-11-03'),
((SELECT id FROM produtos WHERE modelo='RTX4090-24G-FAKE'), 'NV-FAKE-4090-0003', 'VENDIDO', '2024-11-04'),
((SELECT id FROM produtos WHERE modelo='RTX4090-24G-FAKE'), 'NV-FAKE-4090-0004', 'DISPONIVEL', '2025-01-10'),
((SELECT id FROM produtos WHERE modelo='RTX6000-ADA-FAKE'), 'NV-FAKE-6000-0001', 'EM_RMA', '2024-12-01'),
((SELECT id FROM produtos WHERE modelo='RTX6000-ADA-FAKE'), 'NV-FAKE-6000-0002', 'VENDIDO', '2024-12-02'),
((SELECT id FROM produtos WHERE modelo='RTX6000-ADA-FAKE'), 'NV-FAKE-6000-0003', 'SUBSTITUIDO', '2024-12-03'),
((SELECT id FROM produtos WHERE modelo='RTX6000-ADA-FAKE'), 'NV-FAKE-6000-0004', 'DISPONIVEL', '2025-02-11'),
((SELECT id FROM produtos WHERE modelo='A100-SXM4-80-FAKE'), 'NV-FAKE-A100-0001', 'EM_RMA', '2024-08-15'),
((SELECT id FROM produtos WHERE modelo='A100-SXM4-80-FAKE'), 'NV-FAKE-A100-0002', 'EM_RMA', '2024-08-16'),
((SELECT id FROM produtos WHERE modelo='A100-SXM4-80-FAKE'), 'NV-FAKE-A100-0003', 'VENDIDO', '2024-08-17'),
((SELECT id FROM produtos WHERE modelo='A100-SXM4-80-FAKE'), 'NV-FAKE-A100-0004', 'DISPONIVEL', '2025-03-01'),
((SELECT id FROM produtos WHERE modelo='H100-SXM5-80-FAKE'), 'NV-FAKE-H100-0001', 'EM_RMA', '2025-01-05'),
((SELECT id FROM produtos WHERE modelo='H100-SXM5-80-FAKE'), 'NV-FAKE-H100-0002', 'VENDIDO', '2025-01-06'),
((SELECT id FROM produtos WHERE modelo='H100-SXM5-80-FAKE'), 'NV-FAKE-H100-0003', 'VENDIDO', '2025-01-07'),
((SELECT id FROM produtos WHERE modelo='H100-SXM5-80-FAKE'), 'NV-FAKE-H100-0004', 'DISPONIVEL', '2025-06-20'),
((SELECT id FROM produtos WHERE modelo='L40S-48G-FAKE'), 'NV-FAKE-L40S-0001', 'EM_RMA', '2025-02-02'),
((SELECT id FROM produtos WHERE modelo='L40S-48G-FAKE'), 'NV-FAKE-L40S-0002', 'EM_RMA', '2025-02-03'),
((SELECT id FROM produtos WHERE modelo='L40S-48G-FAKE'), 'NV-FAKE-L40S-0003', 'VENDIDO', '2025-02-04'),
((SELECT id FROM produtos WHERE modelo='L40S-48G-FAKE'), 'NV-FAKE-L40S-0004', 'DISPONIVEL', '2025-07-12'),
((SELECT id FROM produtos WHERE modelo='MCX75310-FAKE'), 'NV-FAKE-CX7-0001', 'VENDIDO', '2023-05-10'),
((SELECT id FROM produtos WHERE modelo='MCX75310-FAKE'), 'NV-FAKE-CX7-0002', 'EM_RMA', '2023-05-11'),
((SELECT id FROM produtos WHERE modelo='MCX75310-FAKE'), 'NV-FAKE-CX7-0003', 'VENDIDO', '2024-09-01'),
((SELECT id FROM produtos WHERE modelo='MCX75310-FAKE'), 'NV-FAKE-CX7-0004', 'DISPONIVEL', '2025-08-01'),
((SELECT id FROM produtos WHERE modelo='BF3-DPU-FAKE'), 'NV-FAKE-BF3-0001', 'EM_RMA', '2024-10-01'),
((SELECT id FROM produtos WHERE modelo='BF3-DPU-FAKE'), 'NV-FAKE-BF3-0002', 'VENDIDO', '2024-10-02'),
((SELECT id FROM produtos WHERE modelo='BF3-DPU-FAKE'), 'NV-FAKE-BF3-0003', 'DESCARTADO', '2024-10-03'),
((SELECT id FROM produtos WHERE modelo='BF3-DPU-FAKE'), 'NV-FAKE-BF3-0004', 'DISPONIVEL', '2025-09-09'),
((SELECT id FROM produtos WHERE modelo='SP4-MOD-FAKE'), 'NV-FAKE-SP4-0001', 'EM_RMA', '2025-08-20'),
((SELECT id FROM produtos WHERE modelo='SP4-MOD-FAKE'), 'NV-FAKE-SP4-0002', 'VENDIDO', '2025-08-21'),
((SELECT id FROM produtos WHERE modelo='SP4-MOD-FAKE'), 'NV-FAKE-SP4-0003', 'DISPONIVEL', '2026-01-15'),
((SELECT id FROM produtos WHERE modelo='SP4-MOD-FAKE'), 'NV-FAKE-SP4-0004', 'DISPONIVEL', '2026-02-01');

INSERT INTO clientes_produtos (cliente_id, estoque_serie_id, data_compra)
SELECT c.id, s.id, v.data_compra
FROM (
    SELECT 'ana.ferreira@alphacloud.fake' AS email, 'NV-FAKE-4090-0001' AS serial, '2025-01-15' AS data_compra UNION ALL
    SELECT 'bruno.costa@nortecompute.fake', 'NV-FAKE-4090-0002', '2025-02-10' UNION ALL
    SELECT 'camila.dias@atlashpc.fake', 'NV-FAKE-4090-0003', '2025-03-01' UNION ALL
    SELECT 'diego.lima@pampadata.fake', 'NV-FAKE-6000-0001', '2025-01-20' UNION ALL
    SELECT 'eduarda.nogueira@cerradoai.fake', 'NV-FAKE-6000-0002', '2025-02-18' UNION ALL
    SELECT 'felipe.andrade@recifeneurons.fake', 'NV-FAKE-6000-0003', '2025-03-22' UNION ALL
    SELECT 'gabriela.pinto@minasvision.fake', 'NV-FAKE-A100-0001', '2024-09-12' UNION ALL
    SELECT 'henrique.alves@litoralrender.fake', 'NV-FAKE-A100-0002', '2024-10-05' UNION ALL
    SELECT 'isabela.rocha@bahia-inf.fake', 'NV-FAKE-A100-0003', '2024-11-11' UNION ALL
    SELECT 'joao.melo@sampastudio.fake', 'NV-FAKE-H100-0001', '2025-04-02' UNION ALL
    SELECT 'karina.teixeira@orbitnet.fake', 'NV-FAKE-H100-0002', '2025-05-19' UNION ALL
    SELECT 'lucas.barbosa@andestraining.fake', 'NV-FAKE-H100-0003', '2025-06-08' UNION ALL
    SELECT 'ana.ferreira@alphacloud.fake', 'NV-FAKE-L40S-0001', '2025-06-15' UNION ALL
    SELECT 'bruno.costa@nortecompute.fake', 'NV-FAKE-L40S-0002', '2025-07-01' UNION ALL
    SELECT 'camila.dias@atlashpc.fake', 'NV-FAKE-L40S-0003', '2025-07-20' UNION ALL
    SELECT 'diego.lima@pampadata.fake', 'NV-FAKE-CX7-0001', '2023-06-01' UNION ALL
    SELECT 'eduarda.nogueira@cerradoai.fake', 'NV-FAKE-CX7-0002', '2023-07-10' UNION ALL
    SELECT 'felipe.andrade@recifeneurons.fake', 'NV-FAKE-CX7-0003', '2024-10-12' UNION ALL
    SELECT 'gabriela.pinto@minasvision.fake', 'NV-FAKE-BF3-0001', '2024-11-01' UNION ALL
    SELECT 'henrique.alves@litoralrender.fake', 'NV-FAKE-BF3-0002', '2024-12-15' UNION ALL
    SELECT 'isabela.rocha@bahia-inf.fake', 'NV-FAKE-SP4-0001', '2025-09-01' UNION ALL
    SELECT 'joao.melo@sampastudio.fake', 'NV-FAKE-SP4-0002', '2025-09-10'
) v
JOIN clientes c ON c.email = v.email
JOIN estoque_series s ON s.numero_serie = v.serial;

INSERT INTO rmas (cliente_id, estoque_serie_id, data_abertura, descricao_problema, status, prioridade, data_conclusao, observacoes)
SELECT c.id, s.id, v.abertura, v.problema, v.status, v.prio, v.conclusao, v.obs
FROM (
    SELECT 'ana.ferreira@alphacloud.fake' e, 'NV-FAKE-4090-0001' s, '2026-09-20' abertura, 'Artefatos visuais e travamentos no driver apos 10 min de carga.' problema, 'ABERTO' status, 'ALTA' prio, NULL conclusao, NULL obs UNION ALL
    SELECT 'bruno.costa@nortecompute.fake', 'NV-FAKE-4090-0002', '2026-09-22', 'Temperatura acima de 95C em idle de render.', 'EM_ANALISE', 'MEDIA', NULL, NULL UNION ALL
    SELECT 'diego.lima@pampadata.fake', 'NV-FAKE-6000-0001', '2026-08-30', 'Falha ECC recorrente em aplicacao CAD.', 'APROVADO', 'ALTA', NULL, NULL UNION ALL
    SELECT 'eduarda.nogueira@cerradoai.fake', 'NV-FAKE-6000-0002', '2026-07-12', 'Cliente relatou ruido; analise nao reproduziu defeito.', 'REJEITADO', 'BAIXA', '2026-07-20', 'Sem falha encontrada' UNION ALL
    SELECT 'felipe.andrade@recifeneurons.fake', 'NV-FAKE-6000-0003', '2026-06-01', 'Unidade com conector 12VHPWR derretido.', 'CONCLUIDO', 'CRITICA', '2026-06-18', NULL UNION ALL
    SELECT 'gabriela.pinto@minasvision.fake', 'NV-FAKE-A100-0001', '2026-09-01', 'Queda de throughput em treinamento LLM.', 'AGUARDANDO_PECA', 'ALTA', NULL, NULL UNION ALL
    SELECT 'henrique.alves@litoralrender.fake', 'NV-FAKE-A100-0002', '2026-09-10', 'Erro NVLink intermitente no cluster.', 'ENVIADO', 'ALTA', NULL, NULL UNION ALL
    SELECT 'isabela.rocha@bahia-inf.fake', 'NV-FAKE-A100-0003', '2026-05-02', 'Solicitacao cancelada pelo cliente apos janela de manutencao.', 'CANCELADO', 'BAIXA', '2026-05-04', NULL UNION ALL
    SELECT 'joao.melo@sampastudio.fake', 'NV-FAKE-H100-0001', '2026-09-28', 'XID 79 e reboot do no GPU.', 'ABERTO', 'CRITICA', NULL, NULL UNION ALL
    SELECT 'karina.teixeira@orbitnet.fake', 'NV-FAKE-H100-0002', '2026-04-10', 'Falha de memoria HBM; pecas substituidas.', 'CONCLUIDO', 'ALTA', '2026-04-28', NULL UNION ALL
    SELECT 'ana.ferreira@alphacloud.fake', 'NV-FAKE-L40S-0001', '2026-10-01', 'Displayport sem sinal em workstation de inferencia.', 'EM_ANALISE', 'MEDIA', NULL, NULL UNION ALL
    SELECT 'bruno.costa@nortecompute.fake', 'NV-FAKE-L40S-0002', '2026-09-15', 'Fan curve irregular e alerta de cooler.', 'APROVADO', 'MEDIA', NULL, NULL UNION ALL
    SELECT 'diego.lima@pampadata.fake', 'NV-FAKE-CX7-0001', '2026-08-01', 'Porta InfiniBand nao negocia 400G.', 'CONCLUIDO', 'ALTA', '2026-08-14', 'FORA DE GARANTIA' UNION ALL
    SELECT 'eduarda.nogueira@cerradoai.fake', 'NV-FAKE-CX7-0002', '2026-09-05', 'CRC elevado no link; unidade permanece em analise.', 'AGUARDANDO_PECA', 'ALTA', NULL, 'FORA DE GARANTIA' UNION ALL
    SELECT 'gabriela.pinto@minasvision.fake', 'NV-FAKE-BF3-0001', '2026-09-18', 'DPU nao sobe firmware apos update.', 'ENVIADO', 'ALTA', NULL, NULL UNION ALL
    SELECT 'isabela.rocha@bahia-inf.fake', 'NV-FAKE-SP4-0001', '2026-10-05', 'Modulo Spectrum-4 com LED amber persistente.', 'ABERTO', 'MEDIA', NULL, 'FORA DE GARANTIA'
) v
JOIN clientes c ON c.email = v.e
JOIN estoque_series s ON s.numero_serie = v.s;

INSERT INTO historico_rma (rma_id, data_evento, status_anterior, status_novo, descricao)
SELECT r.id, h.data_evento, h.status_anterior, h.status_novo, h.descricao
FROM (
    SELECT 'NV-FAKE-4090-0001' serial, '2026-09-20' data_evento, NULL status_anterior, 'ABERTO' status_novo, 'RMA aberto.' descricao UNION ALL
    SELECT 'NV-FAKE-4090-0002', '2026-09-22', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-4090-0002', '2026-09-24', 'ABERTO', 'EM_ANALISE', 'Encaminhado ao laboratorio.' UNION ALL
    SELECT 'NV-FAKE-6000-0001', '2026-08-30', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-6000-0001', '2026-09-02', 'ABERTO', 'EM_ANALISE', 'Logs ECC coletados.' UNION ALL
    SELECT 'NV-FAKE-6000-0001', '2026-09-08', 'EM_ANALISE', 'APROVADO', 'Falha confirmada. Troca autorizada.' UNION ALL
    SELECT 'NV-FAKE-6000-0002', '2026-07-12', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-6000-0002', '2026-07-18', 'ABERTO', 'EM_ANALISE', 'Teste de burn-in 48h.' UNION ALL
    SELECT 'NV-FAKE-6000-0002', '2026-07-20', 'EM_ANALISE', 'REJEITADO', 'Sem defeito reproduzivel.' UNION ALL
    SELECT 'NV-FAKE-6000-0003', '2026-06-01', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-6000-0003', '2026-06-03', 'ABERTO', 'APROVADO', 'Dano termico no conector.' UNION ALL
    SELECT 'NV-FAKE-6000-0003', '2026-06-18', 'APROVADO', 'CONCLUIDO', 'Pecas substituidas e unidade devolvida.' UNION ALL
    SELECT 'NV-FAKE-A100-0001', '2026-09-01', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-A100-0001', '2026-09-06', 'ABERTO', 'AGUARDANDO_PECA', 'Aguardando cooler blower.' UNION ALL
    SELECT 'NV-FAKE-A100-0002', '2026-09-10', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-A100-0002', '2026-09-14', 'ABERTO', 'ENVIADO', 'Unidade despachada ao centro de reparo.' UNION ALL
    SELECT 'NV-FAKE-A100-0003', '2026-05-02', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-A100-0003', '2026-05-04', 'ABERTO', 'CANCELADO', 'Cancelado a pedido do cliente.' UNION ALL
    SELECT 'NV-FAKE-H100-0001', '2026-09-28', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-H100-0002', '2026-04-10', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-H100-0002', '2026-04-12', 'ABERTO', 'EM_ANALISE', 'Dump de memoria coletado.' UNION ALL
    SELECT 'NV-FAKE-H100-0002', '2026-04-20', 'EM_ANALISE', 'APROVADO', 'Modulo de memoria condenado.' UNION ALL
    SELECT 'NV-FAKE-H100-0002', '2026-04-28', 'APROVADO', 'CONCLUIDO', 'Substituicao de pecas concluida.' UNION ALL
    SELECT 'NV-FAKE-L40S-0001', '2026-10-01', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-L40S-0001', '2026-10-03', 'ABERTO', 'EM_ANALISE', 'Cabo e firmware verificados.' UNION ALL
    SELECT 'NV-FAKE-L40S-0002', '2026-09-15', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-L40S-0002', '2026-09-19', 'ABERTO', 'APROVADO', 'Cooler com rolamento danificado.' UNION ALL
    SELECT 'NV-FAKE-CX7-0001', '2026-08-01', NULL, 'ABERTO', 'RMA aberto (FORA DE GARANTIA).' UNION ALL
    SELECT 'NV-FAKE-CX7-0001', '2026-08-05', 'ABERTO', 'APROVADO', 'Aprovado como reparo pago.' UNION ALL
    SELECT 'NV-FAKE-CX7-0001', '2026-08-14', 'APROVADO', 'CONCLUIDO', 'Porta e riser substituidos.' UNION ALL
    SELECT 'NV-FAKE-CX7-0002', '2026-09-05', NULL, 'ABERTO', 'RMA aberto (FORA DE GARANTIA).' UNION ALL
    SELECT 'NV-FAKE-CX7-0002', '2026-09-09', 'ABERTO', 'AGUARDANDO_PECA', 'Aguardando pad termico e sensor.' UNION ALL
    SELECT 'NV-FAKE-BF3-0001', '2026-09-18', NULL, 'ABERTO', 'RMA aberto.' UNION ALL
    SELECT 'NV-FAKE-BF3-0001', '2026-09-21', 'ABERTO', 'ENVIADO', 'Enviado para regravacao de firmware.' UNION ALL
    SELECT 'NV-FAKE-SP4-0001', '2026-10-05', NULL, 'ABERTO', 'RMA aberto (FORA DE GARANTIA).'
) h
JOIN estoque_series s ON s.numero_serie = h.serial
JOIN rmas r ON r.estoque_serie_id = s.id;

INSERT INTO pecas_utilizadas (rma_id, peca_id, quantidade, data_utilizacao)
SELECT r.id, p.id, u.qtd, u.data
FROM (
    SELECT 'NV-FAKE-6000-0003' serial, 'Conector 12VHPWR' peca, 1 qtd, '2026-06-18' data UNION ALL
    SELECT 'NV-FAKE-6000-0003', 'Pasta termica HPC 4g', 1, '2026-06-18' UNION ALL
    SELECT 'NV-FAKE-H100-0002', 'Modulo GDDR simulado', 2, '2026-04-28' UNION ALL
    SELECT 'NV-FAKE-H100-0002', 'Pad termico GPU', 2, '2026-04-28' UNION ALL
    SELECT 'NV-FAKE-CX7-0001', 'Riser PCIe 5.0', 1, '2026-08-14' UNION ALL
    SELECT 'NV-FAKE-CX7-0001', 'Sensor de temperatura', 1, '2026-08-14'
) u
JOIN estoque_series s ON s.numero_serie = u.serial
JOIN rmas r ON r.estoque_serie_id = s.id
JOIN pecas p ON p.nome = u.peca;

UPDATE pecas SET quantidade = quantidade - 1 WHERE nome = 'Conector 12VHPWR';
UPDATE pecas SET quantidade = quantidade - 1 WHERE nome = 'Pasta termica HPC 4g';
UPDATE pecas SET quantidade = quantidade - 2 WHERE nome = 'Modulo GDDR simulado';
UPDATE pecas SET quantidade = quantidade - 2 WHERE nome = 'Pad termico GPU';
UPDATE pecas SET quantidade = quantidade - 1 WHERE nome = 'Riser PCIe 5.0';
UPDATE pecas SET quantidade = quantidade - 1 WHERE nome = 'Sensor de temperatura';
