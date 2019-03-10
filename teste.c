#include <stdlib.h>
#include <stdio.h>
#include "grafo.h"

//------------------------------------------------------------------------------

int main(void) {

  grafo g = le_grafo(stdin);

  if ( !g )

    return 1;

  grafo rec = recomendacoes(g);

  //escreve_grafo(stdout, g);
  escreve_grafo(stdout, rec);

  return ! destroi_grafo(g);
}
