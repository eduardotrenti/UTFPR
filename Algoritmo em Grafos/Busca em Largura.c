#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct adjacencias{
    int id;
    struct adjacencias *proximo;
}adjacencias;

typedef struct vertice{
    int id;
    struct vertice *proximo;
    adjacencias *lista_adjacencia;
}vertice;

typedef struct Grafo{
    int qtd_vertice;
    struct vertice *inicio;
}Grafo;

Grafo* inicializar_grafo(){ //funcao para inicializar o grafo
    Grafo *g = (Grafo*) malloc(sizeof(Grafo));
    g->qtd_vertice = 0;
    g->inicio = NULL;
    return g;
}

int grafo_inicializado( Grafo *g){ //caso o grafo nao tenha sido inicializado
    return ( (g == NULL) ||( g->inicio == NULL));
}



int vertice_existe(Grafo *g,int id){ //funao para verificar se vertice ja foi criado antes
    vertice *aux_vertice = g->inicio;
    if(!grafo_inicializado(g)){
        while(aux_vertice!=NULL && aux_vertice->id < id){ //percorrer
            if(aux_vertice->id == id){//achou o vertice
                return 1;
            }
            aux_vertice = aux_vertice->proximo;
        }
    }
    return ((aux_vertice != NULL) && (aux_vertice->id == id));
}

vertice* buscar_vertice(Grafo *g,int id){ //funcao para pegar vertice

    vertice *v= (vertice*) malloc(sizeof(vertice));
    v->id = 0;
    v->lista_adjacencia = NULL;
    v->proximo = NULL;
    if(vertice_existe(g,id)==1 && id>0){

        /*if(g->inicio == NULL || id<=0)
            return v;
        */
        vertice *aux_vertice = g->inicio;

        while(aux_vertice->proximo != NULL && aux_vertice->id < id)
            aux_vertice = aux_vertice ->proximo;
            //printf(" \n Valor aux vertice :%d\n",aux_vertice->id);
        if(aux_vertice->id == id)
            return aux_vertice;
    }


    return v;
}

int tem_adjacencia(Grafo *g, vertice *v, int id){ //saber se existe a adjacencia entre vertice

    if(v->lista_adjacencia == NULL || id<=0){ //se o vertice nao tem adjacencia, ou id invalido
        return 0;
    }

    adjacencias *aux_adj = v->lista_adjacencia;

    while(aux_adj->proximo != NULL && aux_adj->id < id){
        aux_adj = aux_adj->proximo;
    }
    if(aux_adj->id == id)
        return 1;

    return 0;
}

vertice* inserir_vertice(Grafo *g,int id){ // inserir vertice
    vertice *v = (vertice*) malloc(sizeof(vertice));
    v->id = id;
    v->lista_adjacencia = NULL;
    v->proximo = NULL;

    if((id>0) && (!vertice_existe(g,id))){
        if(g->inicio == NULL ||  g->inicio->id > id){
            v->proximo = g->inicio;
            g->inicio = v;
            g->qtd_vertice++;
            printf("\nVertice:%d Inserido\n",v->id);
            return v;
        }else{
            vertice *aux_vertice = g->inicio;

            while(aux_vertice->proximo != NULL && aux_vertice->proximo->id < id){
                aux_vertice = aux_vertice->proximo;

            }

            v->proximo = aux_vertice->proximo;
            aux_vertice->proximo = v;
            g->qtd_vertice++;
            printf("\nVertice:%d Inserido\n",v->id);
            return v;
        }
    }
    printf("\n Vertice:%d ja existe\n",id);
    return buscar_vertice(g,id);
}



void inserir_aresta(Grafo *g, int id1, int id2){

    if((id1 != id2) && (id1 > 0) && (id2 > 0)){

        vertice *v1 = inserir_vertice(g,id1);
        vertice *v2 = inserir_vertice(g,id2);

        if((!tem_adjacencia(g,v1,id2)) && (!tem_adjacencia(g,v2,id1))){
            adjacencias *criar_adj1 = (adjacencias*) malloc(sizeof(adjacencias));
            criar_adj1->id = id2;
            criar_adj1->proximo = NULL;

            adjacencias *criar_adj2 = (adjacencias*) malloc(sizeof(adjacencias));;
            criar_adj2->id = id1;
            criar_adj2->proximo = NULL;

            adjacencias *aux_adj = v1->lista_adjacencia;

            if(v1->lista_adjacencia == NULL || v1->lista_adjacencia->id > id2){
                criar_adj1->proximo = v1->lista_adjacencia;
                v1->lista_adjacencia = criar_adj1;
            }else{
                while(aux_adj->proximo != NULL && aux_adj->proximo->id < id2){
                    aux_adj = aux_adj->proximo;
                }
                criar_adj1->proximo = aux_adj->proximo;
                aux_adj->proximo = criar_adj1;
            }

            aux_adj = v2->lista_adjacencia;

            if(v2->lista_adjacencia == NULL || v2->lista_adjacencia->id > id1){
                criar_adj2->proximo = v2->lista_adjacencia;
                v2->lista_adjacencia = criar_adj2;
            }else{
                while(aux_adj->proximo != NULL && aux_adj->proximo->id < id1)
                    aux_adj = aux_adj->proximo;
                criar_adj2->proximo = aux_adj->proximo;
                aux_adj->proximo = criar_adj2;
            }

        }else{
            printf("\n Ja existe esta aresta\n");
        }
    }else{
        printf("\n Ids com valores iguais, ou menores que 1\n");
    }

}

void remover_aresta(Grafo *g,int id1, int id2){

    if(!grafo_inicializado(g) && vertice_existe(g,id1) && vertice_existe(g,id2) && (id1>0) && (id2>0) && (id1 != id2)){
        vertice *v1 = inserir_vertice(g, id1);
        vertice *v2 = inserir_vertice(g, id2);
        if( (tem_adjacencia(g,v1,id2)==1) && (tem_adjacencia(g,v2,id1) == 1)){

            adjacencias *aux_adj = v1->lista_adjacencia;
            adjacencias *aux_adj2 = aux_adj;

            if(v1->lista_adjacencia->id == id2){
                v1->lista_adjacencia = v1->lista_adjacencia->proximo;
                printf("\nAresta %d removida 1\n",id2);
                free(aux_adj);
            }else{
                while(aux_adj != NULL && aux_adj->id < id2){
                    aux_adj2 = aux_adj;
                    aux_adj = aux_adj->proximo;
                }
                if(aux_adj!= NULL && aux_adj->id == id2){
                    aux_adj2->proximo = aux_adj->proximo;
                    free(aux_adj);
                }
            }

            aux_adj = v2->lista_adjacencia;
            aux_adj2 = aux_adj;

            if(v2->lista_adjacencia->id == id1){
                v2->lista_adjacencia = v2->lista_adjacencia->proximo;
                printf("\nAresta %d removida 3 1\n",id1);
                free(aux_adj);
            }else{
                while(aux_adj != NULL && aux_adj->id < id1){
                    aux_adj2 = aux_adj;
                    aux_adj = aux_adj->proximo;
                }
                if( aux_adj != NULL && aux_adj->id == id1){
                    aux_adj2->proximo = aux_adj->proximo;
                    printf("\nAresta %d removida 4\n",id1);
                    free(aux_adj);
                }
            }
        }

    }else
        printf("\nIds invalidos ou vertices nao existem\n");

}

void remover_vertice(Grafo *g, int id){
    if(!grafo_inicializado(g) ){
        vertice *aux_vertice = g->inicio;

        if( id == g->inicio->id){

            adjacencias *aux_adj = aux_vertice->lista_adjacencia;
            adjacencias *aux_adj2 = aux_adj;

            while(aux_adj != NULL){
                aux_adj2 = aux_adj;
                aux_adj = aux_adj->proximo;
                remover_aresta(g,id,aux_adj2->id);
            }

            g->inicio = g->inicio->proximo;
            g->qtd_vertice--;
            free(aux_vertice);
        }else{
            vertice *aux_vertice2;
            while(aux_vertice != NULL && aux_vertice->id < id){
                aux_vertice2 = aux_vertice;
                aux_vertice = aux_vertice->proximo;
            }

            if(aux_vertice != NULL && aux_vertice->id == id){
                adjacencias *aux_adj = aux_vertice->lista_adjacencia;
                adjacencias *aux_adj2 = aux_adj;
                while(aux_adj != NULL){
                    aux_adj2 = aux_adj;
                    aux_adj = aux_adj->proximo;
                    remover_aresta(g,id,aux_adj2->id);
                }

                aux_vertice2->proximo = aux_vertice->proximo;
                printf("\nVertice:%d removido\n",aux_vertice->id);
                g->qtd_vertice--;
                free(aux_vertice);
            }
        }
    }else
        printf("\nVertice Nao Existe\n");
}

void visualizar_grafo(Grafo *g){
    if(!grafo_inicializado(g)){

        printf("\n===== Visualizando Grafo ====== \n");
        vertice *aux_vertice = g->inicio;
        adjacencias *aux_adjacencias;
        while(aux_vertice!=NULL){
            printf("\n\n[%d] ",aux_vertice->id);
            aux_adjacencias = aux_vertice->lista_adjacencia;
            while(aux_adjacencias != NULL){
                printf("--> %d ",aux_adjacencias->id);
                aux_adjacencias = aux_adjacencias->proximo;
            }
            aux_vertice = aux_vertice->proximo;
            printf("\n\n");
        }
        printf("\n=============================== \n");
    }else
        printf("\nGrafo nao Existe\n");

}


void infomarGrauVertice(Grafo *g, int id){
    if(!grafo_inicializado(g)){

        vertice *aux_vertice =  buscar_vertice(g,id); //pegar o vertice com o id informado

        if(aux_vertice != NULL && aux_vertice->id == id){

            adjacencias *aux_adj = aux_vertice->lista_adjacencia; // ver adjacencias do vetor
            int grauVertice = 0;
            while(aux_adj != NULL){
                grauVertice++;
                aux_adj = aux_adj->proximo;
            }
            printf("Grau do vertice informado %d\n", grauVertice);
        }else{
            printf("Vertice informado: %d, nao foi encontrado!\n", id);
        }
    }else{
        printf("Nenhum vertice criado!\n");
    }
}

int* buscaLargura(Grafo *g,int idBusca) {

    int *verticesVisitados = (int *) calloc(g->qtd_vertice + 1, sizeof(int)); // 1 se o vertice foi visitado

    if (grafo_inicializado(g)) {
        printf("Grafo nao inicializado.\n");
        return verticesVisitados;
    }

    vertice *aux_vertice = buscar_vertice(g, idBusca);

    if (aux_vertice == NULL || aux_vertice->id != idBusca){
        printf("Vertice %d nao encontrado.\n", idBusca);
        return verticesVisitados;
    }

    int *filaVertice = (int *)malloc((g->qtd_vertice + 1) * sizeof(int)); //  vertices que vao ser visitados,
    int inicioFila = 0, fimFila = 0;//inicioFila para pegar o primeiro elemento da Fila, e fimFila para adicionar elemento na fila
    // fila insere no fim, e pega do inicio

    filaVertice[fimFila] = idBusca;//adicionar no fim da fila, o id inserido
    fimFila++;//final da fila atualizado
    verticesVisitados[idBusca] = 1; //vertice encontrado

    printf("Comeca no vertice: %d:\n", idBusca);

    while (inicioFila < fimFila) { //ate a fila estar vazia de novo
        int v_atualID = filaVertice[inicioFila]; //pegar id do elemento no inicio da fila
        inicioFila++; //atualizando inicio
        printf("%d ", v_atualID); // printando vertices encontrados

        vertice *v = buscar_vertice(g, v_atualID); // pegar vertice com id
        adjacencias *aux_adj = v->lista_adjacencia; // lista com os vizinhos do vertice

        // Visitar todos os vizinhos do vértice atual
        while (aux_adj != NULL) {
            if (!verticesVisitados[aux_adj->id]) {
                filaVertice[fimFila] = aux_adj->id;
                fimFila++;
                verticesVisitados[aux_adj->id] = 1;
            }
            aux_adj = aux_adj->proximo;
        }
    }

    printf("\n");
    free(filaVertice);
    return verticesVisitados;
}


int verificaGrafoConexo(Grafo *g,int id){
    if (!grafo_inicializado(g)) {
        int *listaVertice = (int*)calloc(g->qtd_vertice+1, sizeof(int));

        listaVertice = buscaLargura(g,id);//realizamos a busca para verificar se todos os vertices serão visitados

        for (int i=1;i<= g->qtd_vertice;i++) {
            if (listaVertice[i] == 0) {
                printf(" Nao conexo\n");
                free(listaVertice);
                return 0;
            }
        }
        printf("Grafo conexo\n");
        free(listaVertice);
        return 1;
    } else {
        printf("Grafo nao existe\n");
        return 0;
    }
}



void grafoMatriz(Grafo *g){
    if(!grafo_inicializado(g)){
        int **m= (int**)malloc((g->qtd_vertice+1) * sizeof(int*));
        for(int i=0; i<=g->qtd_vertice; i++){
            m[i] = (int*)calloc((g->qtd_vertice+1), sizeof(int));
        }

        vertice *aux_vertice = g->inicio;

        while(aux_vertice != NULL){
            adjacencias *aux_adj = aux_vertice->lista_adjacencia;
            while(aux_adj != NULL){
                m[aux_vertice->id][aux_adj->id] = 1;
                aux_adj = aux_adj->proximo;
            }
            aux_vertice = aux_vertice->proximo;
        }

        printf("\n =============== Imprimindo Matriz ============== \n   _______");
        printf("\n _|");
        for(int i=1; i<= g->qtd_vertice; i++){
            printf("%d ", i);
        }
        printf("\n");

        for(int i=1; i<=g->qtd_vertice; i++){
            printf("|%d ", i);
            for(int j=1; j<=g->qtd_vertice; j++){
                printf("%d ", m[i][j]);
            }
            printf("\n");
        }

        for(int i=0; i< g->qtd_vertice; i++){
            free(m[i]);
        }
        free(m);
        printf("\n");
    }
    else{
        printf("Grafo nao existe!\n");
    }
}


int main(){
    Grafo *g = inicializar_grafo();
    int s=0,id=0,id2=0;

  /*     1
       /   \
      2     3
       \   /
         4   */

    inserir_vertice(g,1);
    inserir_vertice(g,2);
    inserir_vertice(g,3);
    inserir_aresta(g,1,2);
    inserir_aresta(g,1,3);
    //inserir_aresta(g,2,3);
    inserir_aresta(g,2,4);
    inserir_aresta(g,3,4);

    do{
        printf("\n1. Inserir Vertices \n2. Inserir Arestas \n3. Visualizar Grafo. \n4. Remover Vertices \n5. Remover Arestas \n6. Informar grau de um vertice \n7. Informar se o grafo e conexo \n8. Converter grafo para Matriz de Adjacencia \n9. Caminhamento em Amplitude (Busca em Largura)\n10. Sair \n");
        scanf("%d",&s);
        switch(s){
            case 1:
                //inserir vertice
                printf("\nInforme o id do vertice:\n");
                scanf("%d",&id);
                inserir_vertice(g,id);
                break;
            case 2:
                printf("\nInforme os ids para criar a aresta:\n");
                scanf("%d %d",&id,&id2);
                inserir_aresta(g,id,id2);
                break;
            case 3:
                visualizar_grafo(g);
                break;
            case 4:
                printf("\nInforme o id do vertice a ser removido: \n");
                scanf("%d",&id);
                remover_vertice(g,id);
                break;
            case 5:
                printf("\nInforme os ids para remover a aresta:\n");
                scanf("%d %d",&id,&id2);
                remover_aresta(g,id,id2);
                break;
            case 6:
                printf("\nInforme o id do vertice:\n");
                scanf("%d",&id);
                infomarGrauVertice(g,id);
                break;
            case 7:
                printf("\nInforme o id do vertice para busca em largura:\n");
                scanf("%d",&id);
                verificaGrafoConexo(g,id);
                break;
            case 8:
                grafoMatriz(g);
                break;
            case 9:
                printf("\nInforme o id do vertice para busca em largura:\n");
                scanf("%d",&id);
                free(buscaLargura(g,id));
            default:
                break;
        }
    }while(s!=10);
    return 0;
}
