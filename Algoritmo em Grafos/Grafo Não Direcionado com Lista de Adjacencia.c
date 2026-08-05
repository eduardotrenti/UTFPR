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
                //printf("\nAresta %d removida 3 1\n",id1);
            }else{
                while(aux_adj != NULL && aux_adj->id < id1){
                    aux_adj2 = aux_adj;
                    aux_adj = aux_adj->proximo;
                }
                if( aux_adj != NULL && aux_adj->id == id1){
                    aux_adj2->proximo = aux_adj->proximo;
                    printf("\nAresta %d removida 4\n",id1);
                    free(aux_adj);
                    //printf("\nAresta %d removida 4\n",id1);
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
                //printf("\n No primeiro caso id : %d e aux_adj2->id:%d\n",id,aux_adj2->id);
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
                //printf("\n Antes do while remover vertice id : %d e aux_adj2->id:%d\n",id,aux_adj2->id);
                while(aux_adj != NULL){
                    aux_adj2 = aux_adj;
                    aux_adj = aux_adj->proximo;
                    //printf("\n While remover vertice id : %d e aux_adj2->id:%d\n",id,aux_adj2->id);
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


int main(){
    Grafo *g = inicializar_grafo();
    int s=0,id=0,id2=0;

    inserir_vertice(g,1);
    inserir_vertice(g,2);
    inserir_vertice(g,3);
    inserir_aresta(g,1,2);
    inserir_aresta(g,1,3);
    inserir_aresta(g,2,3);

    do{
        printf("\n1. Inserir Vertices \n2. Inserir Arestas \n3. Visualizar Grafo. \n4. Remover Vertices \n5. Remover Arestas \n6. Sair\n");
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
            default:
                break;
        }
    }while(s!=6);
    return 0;
}
