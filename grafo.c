#include <stdio.h>
#include <stdlib.h>
#include <graphviz/cgraph.h>
#include "grafo.h"

typedef struct nodo_lista *nodo_lista_p;
typedef struct lista *lista_p;
typedef struct vertice *vertice_p;
typedef struct aresta *aresta_p;

#pragma pack (1)
struct nodo_lista {
  void *conteudo;
  nodo_lista_p proximo;
} nodo_lista_t;

struct lista {
  int tam;
  nodo_lista_p primeiro; 
} lista_t;

struct vertice {
  char *nome;
  char tipo;
  lista_p entrada, saida;
} vertice_t;

struct aresta {
  long int peso;
  vertice_p origem, destino;
} aresta_t;

//------------------------------------------------------------------------------
// (apontador para) estrutura de dados para representar um grafo
// 
// o grafo pode ser direcionado ou não
// 
// o grafo tem um nome, que é uma "string"
struct grafo {
  char *nome;
  lista_p vertices, arestas;
};

//------------------------------------------------------------------------------
// Funcao de erro padrao
//
// Recebe como entrada uma mensagem de erro
//
// Gera como saida uma mensagem de erro
inline static void err_exit(const char *msg) {
  printf("[Fatal Error]: %s \nExiting... \n", msg);
  exit(1);
}

//------------------------------------------------------------------------------
// Aloca e devolve uma lista de apontadores
//
// Devolve a lista em caso de sucesso, ou NULL, caso contrário
static lista_p cria_lista(void) {
  lista_p lista = (lista_p)malloc(sizeof(lista_t));
  if(!lista)
    err_exit("Erro ao criar lista!");

  lista->primeiro = NULL;
  lista->tam = 0;

  return lista;
}

//------------------------------------------------------------------------------
// Desaloca uma lista de apontadores
//
// Recebe como entradada uma lista de apontadores
//
// Devolve 1 em caso de sucesso, ou 0 caso, contrário
static int destroi_lista(lista_p lista) {
  if(!lista)
    return 0;

  nodo_lista_p nodo;

  while(lista->primeiro) {
    nodo = lista->primeiro;
    lista->primeiro = lista->primeiro->proximo;

    free(nodo->conteudo);
    free(nodo);
  }

  free(lista);

  return 1;
}

//------------------------------------------------------------------------------
// Desaloca uma lista de apontadores
//
// Recebe como entradada uma lista de apontadores
//
// Devolve a lista em caso de sucesso, ou NULL, caso contrário
static nodo_lista_p cria_nodo_lista(void *conteudo) {
  nodo_lista_p nodo = (nodo_lista_p)malloc(sizeof(nodo_lista_t));
  if(!nodo)
    err_exit("Erro ao criar nodo!");

  nodo->conteudo = conteudo;
  nodo->proximo = NULL;

  return nodo;
}

//------------------------------------------------------------------------------
// Adiciona um nodo em uma lista de apontadores
//
// Recebe como entradada uma lista de apontadores e um nodo
static void add_nodo_lista(lista_p lista, nodo_lista_p nodo) {
  nodo_lista_p p = lista->primeiro;

  if(p != NULL) {
    while(p->proximo != NULL) {
      p = p->proximo;
    }

    p->proximo = nodo;
  } else {
    lista->primeiro = nodo;
  }

  lista->tam++;
}

//------------------------------------------------------------------------------
// Aloca a estrutura de um grafo
//
// Devolve um grafo em caso de sucesso, ou NULL, caso contrário
static grafo cria_grafo(void) {
  grafo g = (grafo)malloc(sizeof(struct grafo));
  if(!g)
    err_exit("Erro ao criar novo grafo!");

  g->nome = (char *)malloc(256 * sizeof(char));
  if(!g->nome)
    err_exit("Erro ao criar grafo->nome!");

  g->vertices = cria_lista();
  g->arestas = cria_lista();

  return g;
}

//------------------------------------------------------------------------------
// desaloca toda a memória usada em *g
// 
// devolve 1 em caso de sucesso,
//         ou 
//         0, caso contrário
int destroi_grafo(grafo g) {
  if(!g)
    return 0;

  if((destroi_lista(g->arestas) & destroi_lista(g->vertices)) == 1) {
    free(g->nome);
    free(g);

    return 1;
  }
  
  return 0;
}

//------------------------------------------------------------------------------
// Aloca a estrutura de um vertice
//
// Recebe como entradada o nome e o tipo do vertice
//
// Devolve o vertice em caso de sucesso, ou NULL, caso contrário
static vertice_p cria_vertice(char *nome, char *tipo) {
  vertice_p v = (vertice_p)malloc(sizeof(vertice_t));
  if(!v)
    err_exit("Erro ao criar vertice!");

  v->nome = (char *)malloc(256*sizeof(char));
  if(!v->nome)
    err_exit("Erro ao criar vertice->nome!");

  strcpy(v->nome, nome);

  v->tipo = tipo[0];

  v->entrada = cria_lista();
  v->saida = cria_lista();

  return v;
}

//------------------------------------------------------------------------------
// Aloca a estrutura de uma aresta
//
// Recebe como entradada o vertice de origem, o vertice de destino e o peso
//
// Devolve a aresta em caso de sucesso, ou NULL, caso contrário
static aresta_p cria_aresta(vertice_p origem, vertice_p destino, long int peso) {
  aresta_p e = (aresta_p)malloc(sizeof(aresta_t));
  if(!e)
    err_exit("Erro ao criar aresta!");

  e->peso = peso;
  e->origem = origem;
  e->destino = destino;

  return e;
}

//------------------------------------------------------------------------------
// Busca uma aresta a no grafo g
//
// Recebe como entradada um grafo e uma aresta
//
// Devolve 1 caso exista a aresta, ou 0, caso contrário
static int busca_aresta (grafo g, aresta_p a) {
  if(!g)
    err_exit("Erro ao carregar o grafo!");

  nodo_lista_p n;

  for (n = g->arestas->primeiro; n != NULL; n = n->proximo) {
    aresta_p e = n->conteudo;

    if (((a->origem == e->origem) || (a->origem == e->destino)) && ((a->destino == e->origem) || (a->destino == e->destino)))
      return 1;
  }

  return 0;
}

//------------------------------------------------------------------------------
// Cria uma lista de consumidores contidas no grafo g
//
// Recebe com entradada um grafo
//
// Devolve uma lista de consumidores em caso de sucesso, ou NULL, caso contrário
static lista_p separa_consumidores(grafo g) {
  if(!g)
    err_exit("Erro ao carregar o grafo!");

  nodo_lista_p nodo;

  lista_p consumidores = cria_lista();

  for(nodo = g->vertices->primeiro; nodo != NULL; nodo = nodo->proximo) {
    vertice_p v = nodo->conteudo;
    if(v->tipo == 'c') {
      add_nodo_lista(consumidores, cria_nodo_lista(v));
    }
  }

  return consumidores;
}

//------------------------------------------------------------------------------
// Verifica se existe uma aresta {c,p} no grafo g
//
// Recebe com entradada um grafo g, um vertice c e um vertice p
//
// Devolve uma aresta em caso de sucesso, ou NULL, caso contrário
static aresta_p comprou(grafo g, vertice_p c, vertice_p p) {
  if(!g)
    err_exit("Erro ao carregar o grafo!");

  nodo_lista_p nodo_e;
  aresta_p e;

  for(nodo_e = g->arestas->primeiro; nodo_e != NULL; nodo_e = nodo_e->proximo) {
    e = nodo_e->conteudo;

    if(e->origem == c && e->destino == p) {
      return e;
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------
// Verifica se existe um vertice v no grafo g
//
// Recebe com entradada um grafo g e um vertice v
//
// Devolve 1 caso exista, ou 0 caso, contrário
static int contem_vertice(grafo g, vertice_p v) {
  if(!g)
    err_exit("Erro ao carregar o grafo!");

  nodo_lista_p nodo_v;
  vertice_p ve;

  for(nodo_v = g->vertices->primeiro; nodo_v != NULL; nodo_v = nodo_v->proximo) {
    ve = nodo_v->conteudo;
    if(ve == v ) {
      return 1;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
// Verifica se existe um nodo n na lista l
//
// Recebe com entradada uma lista l e um nodo n
//
// Devolve 1 caso exista, ou 0 caso, contrário
static int contem_nodo(lista_p l, nodo_lista_p n) {
  if(!l)
    err_exit("Erro ao carregar a lista!");

  nodo_lista_p nodo;

  for(nodo = l->primeiro; nodo != NULL; nodo = nodo->proximo) {
    if(nodo->conteudo == n->conteudo) {
      return 1;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
// lê um grafo no formato dot de input
// 
// devolve o grafo lido,
//         ou 
//         NULL, em caso de erro 

grafo le_grafo(FILE *input) {
  Agraph_t *graph;
  Agnode_t *node;
  Agedge_t *edge;

  nodo_lista_p n;
  aresta_p e;
  vertice_p destino, origem;

  char *tipo = (char *)malloc(4*sizeof(char));
  strcpy(tipo, "tipo");

  // Cria o grafo g
  grafo g = cria_grafo();

  // Le o arquivo de entrada
  graph = agread(input, NULL);
  
  // Atribui o nome do grafo
  strcpy(g->nome, agnameof(graph));

  // Adiciona os vertices ao grafo g
  for (node = agfstnode(graph); node != NULL; node = agnxtnode(graph, node)) {
    add_nodo_lista(g->vertices, cria_nodo_lista(cria_vertice(agnameof(node), agget(node, tipo))));
  }

  // Para cada nodo do grafo .dot
  for (node = agfstnode(graph); node != NULL; node = agnxtnode(graph, node)) {
    // Para cada aresta do grafo .dot
    for (edge = agfstedge(graph, node); edge != NULL; edge = agnxtedge(graph, edge, node)) {

      // Verifica se o vertice de g e tail na aresta do grado .dot
      for (n = g->vertices->primeiro; n != NULL; n = n->proximo) {
        origem = n->conteudo;
        if (strcmp(origem->nome, agnameof(agtail(edge))) == 0)
          break;
      }

      // Verifica se o vertice de g e head na aresta do grado .dot
      for (n = g->vertices->primeiro; n != NULL; n = n->proximo) {
        destino = n->conteudo;
        if (strcmp(destino->nome, agnameof(aghead(edge))) == 0)
          break;
      }

      // Cria uma aresta {tail,head} com peso 1
      e = cria_aresta(origem, destino, 1);

      // Se a aresta não existe em e
      if (!busca_aresta(g, e)) {

        // Adiciona e a lista de arestas de g
        add_nodo_lista(g->arestas, cria_nodo_lista(e));

        // Adiciona e a lista de arestas de saida do vertice de origem
        add_nodo_lista(origem->saida, cria_nodo_lista(e));

        // Adiciona e a lista de arestas de entrada do vertice de destino
        add_nodo_lista(destino->entrada, cria_nodo_lista(e));
      }    
    }
  }
 
  agclose(graph);

  return g;
}
//------------------------------------------------------------------------------
// escreve o grafo g em output usando o formato dot.
//
// devolve o grafo escrito,
//         ou 
//         NULL, em caso de erro 

grafo escreve_grafo(FILE *output, grafo g) {
  if(!g)
    err_exit("Erro ao carregar o grafo!");

  // Imprime o cabecalho do arquivo .dot contendo o nome do grafo
  fprintf(output, "%s ", "strict");
  fprintf(output, "graph %s {", g->nome);

  // Imprime os vertices do grafo g
  for(nodo_lista_p n = g->vertices->primeiro; n != NULL; n = n->proximo) {
    vertice_p v = n->conteudo;
    fprintf(output, "\n\t%s", v->nome);
    fprintf(output, "[\"tipo\"=%c]", v->tipo);
  }

  fprintf(output, "\n\n");

  // Imprime as arestas do grafo g
  for(nodo_lista_p n = g->arestas->primeiro; n != NULL; n = n->proximo) {
    aresta_p a = n->conteudo;

    fprintf(output, "\t%s ", a->origem->nome);

    fprintf(output, "-- ");
    fprintf(output, "%s", a->destino->nome);

    fprintf(output, "[\"weight\"=%li]\n", a->peso);
  }

  fprintf(output, "}\n");

  return g;
}

//------------------------------------------------------------------------------
// devolve o grafo de recomendações de g
//
// cada aresta {c,p} de H representa uma recomendação do produto p
// para o consumidor c, e tem um atributo "weight" que é um inteiro
// representando a intensidade da recomendação do produto p para o
// consumidor c.
//
// cada vértice de g tem um atributo "tipo" cujo valor é 'c' ou 'p',
// conforme o vértice seja consumidor ou produto, respectivamente
grafo recomendacoes(grafo g){
  if(!g)
    err_exit("Erro ao carregar o grafo!");

  // e: edge; c: consumidor; p: produto; v: visitado;
  nodo_lista_p nodo_c1, nodo_e1, nodo_e2, nodo_e3, v1;
  vertice_p c1, c2, p1, p2;
  aresta_p e1, e2, e3, compra, recomendacao;
  lista_p visitados;
  int foi_visitado;

  // Cria uma lista de consumidores
  lista_p consumidores = separa_consumidores(g);

  // Cria o grafo H de recomendações
  grafo h = cria_grafo();
  strcpy(h->nome, "recomendacoes");

  // Para cada consumidor na lista de consumidores
  for(nodo_c1 = consumidores->primeiro; nodo_c1 != NULL; nodo_c1 = nodo_c1->proximo) {
    c1 = nodo_c1->conteudo;

    visitados = cria_lista();

    // Para cada aresta que sai deste consumidor
    for(nodo_e1 = c1->saida->primeiro; nodo_e1 != NULL; nodo_e1 = nodo_e1->proximo) {
      e1 = nodo_e1->conteudo;

      // Pega o produto que este consumidor comprou
      p1 = e1->destino;

      //Para cada aresta deste produto
      for(nodo_e2 = p1->entrada->primeiro; nodo_e2 != NULL; nodo_e2 = nodo_e2->proximo) {
        e2 = nodo_e2->conteudo;

        // Pega outro consumidor que tambem comprou o produto
        c2 = e2->origem;
        
        if(c1 != c2) {

          // Verifica se o consumidor c1 ja recebeu alguma recomendacao do consumidor c2
          v1 = cria_nodo_lista(c2);
          foi_visitado = contem_nodo(visitados, v1);

          if(!foi_visitado) {
            add_nodo_lista(visitados, v1);
          }

          // Para cada aresta deste outro consumidor
          for(nodo_e3 = c2->saida->primeiro; nodo_e3 != NULL; nodo_e3 = nodo_e3->proximo) {
            e3 = nodo_e3->conteudo;

            // Pega um outro produto que este outro consumidor comprou
            p2 = e3->destino;

            // Adiciona este outro produto ao grafo de recomendacoes
            // Se o primeiro consumidor nao comprou este produto ainda
            if((compra = comprou(g, c1, p2)) == NULL) {

              // Se ninguem o recomendou este produto ainda
              if((recomendacao = comprou(h, c1, p2)) == NULL) {

                // Se o produto ainda nao existe no grafo de recomendacoes, adicine
                if(!contem_vertice(h, p2)) {
                  add_nodo_lista(h->vertices, cria_nodo_lista(p2));
                }

                if(!contem_vertice(h, c1)) {
                  // Adiciona o consumidor ao grafo de recomendacoes
                  add_nodo_lista(h->vertices, cria_nodo_lista(c1));
                }

                // Adicione a aresta de recomendacao no grafo de recomendacoes
                add_nodo_lista(h->arestas, cria_nodo_lista(cria_aresta(c1, p2, 1)));
              } else if(!foi_visitado) {
                // Se o produto já foi recomendado anteriormente, some peso a aresta
                recomendacao->peso++;
              }
            } else {
              if(!contem_vertice(h, p2)) {
                // Se o consumidor já havia comprado o produto, apenas adicione ele ao grafo de recomendacoes
                add_nodo_lista(h->vertices, cria_nodo_lista(p2));
              }
              // Adiciona o consumidor ao grafo de recomendacoes
              if(!contem_vertice(h, c1)) {
                add_nodo_lista(h->vertices, cria_nodo_lista(c1));
              }
            }
          }
        }
      }
    }
  }

  return h;
}

//------------------------------------------------------------------------------
