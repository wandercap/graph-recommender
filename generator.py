#!/usr/bin/env python
# -*- coding: utf-8 -*-

# HOW TO USE
# Chame o gerador com 1 ou 2 argumentos;
# Os argumentos devem ser numeros inteiros;
# 1 argumento N: Gera N casos de teste aleatórios
# 2 argumentos M, N: Gera um caso de teste aleatório para M consumidores e N produtos
# Se nao houver argumentos, ele gera um caso aleatório base de 10 consumidores e 10 produtos

import random
import sys

quantidade = 1

if len(sys.argv) == 2:
    quantidade = int(sys.argv[1])
    consumidores = random.randint(1, 100)
    produtos = random.randint(1, 100)
if len(sys.argv) >= 3:
    consumidores = int(sys.argv[1])
    produtos = int(sys.argv[2])
else:
    consumidores = 10
    produtos = 10

for i in range(quantidade):
    arestas = []

    file = open("{}c_{}p.dot".format(consumidores, produtos), "w")

    file.write("strict graph {\n")

    for i in range(consumidores):
        file.write("\tc{} [tipo=c];\n".format(i+1))

    for i in range(produtos):
        file.write("\tp{} [tipo=p];\n".format(i+1))

    for i in range(consumidores):
        for j in range(random.randint(1, produtos)):
            k = random.randint(1, produtos)
            if not [i, k] in arestas:
                arestas.append([i, k])
                file.write("\tc{} -- p{};\n".format(i+1, k))

    file.write("}\n")
    
    consumidores = random.randint(1, 100)
    produtos = random.randint(1, 100)
