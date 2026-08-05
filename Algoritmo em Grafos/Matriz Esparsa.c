#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct tipo_elemento {
    int valor;
    int ID_Linha;
    struct tipo_elemento *proximo;
} tipo_elemento;


typedef struct tipo_linha {
    int ID_Coluna;
    struct tipo_linha *proximo;
    struct tipo_elemento *inicio;
} tipo_linha;

typedef struct {
    int qtd_linhas;
    int qtd_colunas;
    struct tipo_linha *inicio;
}Tipo_Mat_Esparsa;

Tipo_Mat_Esparsa* criar_matriz_esparsa(){ // funcao para criar matriz esparsa
    Tipo_Mat_Esparsa *matE = (Tipo_Mat_Esparsa*) malloc(sizeof(Tipo_Mat_Esparsa)); //alocando matriz esparsa
    matE->qtd_colunas = 0;
    matE->qtd_linhas = 0;
    matE->inicio = NULL;
    return matE;
}


int matriz_vazia(Tipo_Mat_Esparsa *matE){ // funcao para verificar se a matriz esparsa foi criada
    if( matE->inicio == NULL){
        return 1; //matriz vazia
    }else{
        return 0; // matriz ja criada
    }
}




void apaga_matriz(Tipo_Mat_Esparsa *matE){ // funcao para liberar a memoria da matriz esparsa


    if(!matriz_vazia(matE)){ // se a matriz naos foi apagada

        tipo_linha *auxColuna; // percorrer as colunas da matriz
        tipo_elemento *auxLinha;//aux para percorrer as linhas

        while(matE->inicio != NULL){
            auxColuna = matE->inicio;

            auxLinha = auxColuna->inicio;// aux para percorrer as linhas

            while(auxColuna->inicio != NULL){
                auxLinha = auxColuna->inicio;// aux para ajudar na liberação de memoria das linhas
                auxColuna->inicio = auxLinha->proximo;// auxLinha recebe o proximo elemento
                free(auxLinha); //liberando a memoria para cada linha da coluna
            }

            matE->inicio = auxColuna->proximo; // auxColuna recebe a proxima coluna
            free(auxColuna); // liberar cada coluna depois de seus elementos
        }

        free(matE); // liberação da matriz
    }
}

//funcao para descobrir se a coluna na posicao ja existe
int coluna_criada(Tipo_Mat_Esparsa *matE,int coluna){
    tipo_linha *auxColuna = matE->inicio;

    while(auxColuna != NULL){
        if(auxColuna->ID_Coluna == coluna){
            return 1; // coluna na posicao ja foi criada
        }else if(auxColuna->ID_Coluna > coluna){ // ja passou do id para busca
            return 0;
        }
        auxColuna = auxColuna->proximo;
    }

    //coluna nao foi criada ainda
    return 0;

}

void inserir_coluna(int coluna, Tipo_Mat_Esparsa *matE){ // inserir coluna
    if((coluna_criada(matE,coluna)==0) && (coluna>=0)) { // se a coluna nao foi criada e tem um valor valido

        tipo_linha *aux = matE->inicio;

        tipo_linha *criar = (tipo_linha*) malloc(sizeof(tipo_linha)); // criar coluna
        criar->ID_Coluna = coluna;
        criar->inicio = NULL;
        criar->proximo = NULL;

        if(matE->inicio == NULL || matE->inicio->ID_Coluna > coluna){ // se for o primeiro elemento ou o id é menor que o primeiro elemento
            criar->proximo = matE->inicio;
            matE->inicio=criar;
        }else{

            while((aux->proximo != NULL) && (aux->proximo->ID_Coluna < coluna)){
                aux = aux->proximo; // percorrer ate achar a posicao para inserção
            }

            criar->proximo = aux->proximo;
            aux->proximo = criar;

        }
        //Coluna Inserida
    }

}


int buscar_elemento(int coluna, int linha, Tipo_Mat_Esparsa *matE){ //funcao apara buscar elemento
    if(!matriz_vazia(matE) && (matE->qtd_colunas>=coluna) && (matE->qtd_linhas>=linha) && ((coluna >= 0 ) && (linha >= 0)) ){ // posicao precisa ser valida

        tipo_linha *auxColuna = matE->inicio;

        if(auxColuna!=NULL){

            while((auxColuna != NULL) && (auxColuna->ID_Coluna<coluna) ){
                auxColuna=auxColuna->proximo; //percorre ate achar a coluna
            }

            if((auxColuna!= NULL) && (auxColuna->ID_Coluna == coluna) ){ // se achou a coluna
                tipo_elemento *auxLinha = auxColuna->inicio;
                if(auxLinha != NULL){

                    while((auxLinha != NULL) && (auxLinha->ID_Linha < linha)){
                        auxLinha = auxLinha->proximo;//percorrer ate achar a lihna
                    }
                    if(  (auxLinha!= NULL) && (auxLinha->ID_Linha == linha)){
                        return auxLinha->valor; //retorna valor do elemento na posicao
                    }
                }

            }
        }
    }

    // Matriz Vazia, posicao invalida ou elemento nao existe
    return 0;
}

void inserir_elemento_matriz_esparsa(int linha, int coluna, int valor, Tipo_Mat_Esparsa *matE){

    if(valor==0){
        return;
    }

    if((coluna>=0) && (linha>=0)){

        if(matriz_vazia(matE)){ //caso a matriz seja vazia
            matE->qtd_colunas = coluna;
            matE->qtd_linhas = linha;
        }



        if(coluna_criada(matE,coluna) == 0){
            inserir_coluna(coluna,matE);// inserir coluna se nao existe
        }

        tipo_linha *auxColuna = matE->inicio;

        while((auxColuna != NULL)&&(auxColuna->ID_Coluna<coluna)){
            auxColuna = auxColuna->proximo; //percorrer ate achar a posicao
        }
        if( (auxColuna!=NULL) && auxColuna->ID_Coluna == coluna ){ //se achar a posicao da coluna
            tipo_elemento *auxLinha = auxColuna->inicio;

            tipo_elemento *criar_elemento = (tipo_elemento*) malloc(sizeof(tipo_elemento));
            criar_elemento->ID_Linha = linha;
            criar_elemento->valor = valor;
            criar_elemento->proximo = NULL;

            if(auxColuna->inicio==NULL || auxLinha->ID_Linha > linha){ // se for o primeiro elemento da coluna ou i
                criar_elemento->proximo = auxColuna->inicio;// o elemento criado aponta para o primeiro
                auxColuna->inicio = criar_elemento;// o inicio aponta para o elemento criado
            }
            else{
                while( (auxLinha->proximo != NULL ) && (auxLinha->ID_Linha < linha)){
                    auxLinha = auxLinha->proximo; //percorrer ate acahar a posicao da linha
                }

                if(auxLinha->ID_Linha == linha){
                    auxLinha->valor = valor;//se for igual troca o valor na posicao
                    free(criar_elemento);//liberar memoria pois nao sera utilizado
                }else{
                    criar_elemento->proximo = auxLinha->proximo; // o valor criado aponta para o proximo
                    auxLinha->proximo = criar_elemento;//o anterior aponta para o valor criado
                }


            }

            /*
            if((matE->qtd_colunas == 0 && coluna == 0)){
                matE->qtd_colunas = 1;
            }

            if((matE->qtd_linhas == 0 && linha == 0)){
                matE->qtd_linhas = 1;
            }
            */

            //atualizar a dimensão da matriz
            if(coluna > matE->qtd_colunas){
                matE->qtd_colunas = coluna;
            }else if(matE->qtd_colunas == 0){
                matE->qtd_colunas = 1;
            }

            if(linha > matE->qtd_linhas){
                matE->qtd_linhas = linha;
            }else if(matE->qtd_linhas == 0){
                matE->qtd_linhas = 1;
            }



        }
    }

}

void transposta_matriz_esparsa(Tipo_Mat_Esparsa *matE1, Tipo_Mat_Esparsa **matE2){
    if(!matriz_vazia(matE1)){


        (*matE2) = criar_matriz_esparsa();
        //atualizar a dimensao


        tipo_linha *auxColuna = matE1->inicio;

        while(auxColuna!=NULL){
            tipo_elemento *auxLinha = auxColuna->inicio;

            while(auxLinha!=NULL && auxLinha->ID_Linha <= matE1->qtd_linhas){//percorrer as linhas
                inserir_elemento_matriz_esparsa(auxColuna->ID_Coluna,auxLinha->ID_Linha,auxLinha->valor,*matE2);//inserir o valor com as dimencoes trocada
                auxLinha = auxLinha->proximo;
            }


            auxColuna = auxColuna->proximo;//percorrer coluna

        }


    }else{
        *matE2 = criar_matriz_esparsa();
    }


}

//funcao para somar duas matrizes
void soma_Matrizes_esparsa(Tipo_Mat_Esparsa *matE1,Tipo_Mat_Esparsa *matE2,Tipo_Mat_Esparsa **matE3){
       if(!matriz_vazia(matE1) && !matriz_vazia(matE2) && (matE1->qtd_colunas == matE2->qtd_colunas) && (matE1->qtd_linhas == matE2->qtd_linhas)){
            (*matE3) = criar_matriz_esparsa();//atualizar as dimensoes

            int soma;
            for(int i=0;i<=matE1->qtd_linhas;i++){
                soma=0;//atualiza o valor da soma para cada posicao
                for(int j=0;j<= matE1->qtd_colunas;j++){
                    soma = buscar_elemento(j,i,matE1) + buscar_elemento(j,i,matE2); //valor da soma das matrizes na posicao
                    inserir_elemento_matriz_esparsa(i,j,soma,*matE3);//insere elemento na posicao
                }

            }


        }else{
            printf("\nDimensao Errada");
            *matE3 = criar_matriz_esparsa();
        }
}


//funcao para realizar a multiplicacao de duas matrizes
void multiplica_matriz_esparsa(Tipo_Mat_Esparsa *matE1, Tipo_Mat_Esparsa *matE2,Tipo_Mat_Esparsa **matE3){
    if(!matriz_vazia(matE1) && !matriz_vazia(matE2) && (matE1->qtd_colunas == matE2->qtd_linhas)){
        *matE3 = criar_matriz_esparsa();
        int soma=0;
        for(int i=0;i<=matE1->qtd_linhas;i++){
            for(int k=0;k<=matE2->qtd_colunas;k++){
                soma = 0;//atualizar o valor da soma da multiplicacao para cada posicao
                for(int j=0;j<=matE2->qtd_linhas;j++){ // cada linha da matriz 2
                    soma += buscar_elemento(j,i,matE1) * buscar_elemento(k,j,matE2);//soma da multiplicacao na posicao
                }
                inserir_elemento_matriz_esparsa(i,k,soma,*matE3);//insere o valor da soma da multiplicacao na posicao
            }

        }


    }else{
        printf("\nDimensoes Erradas");
        *matE3 = criar_matriz_esparsa();
    }



}



void imprimir_matriz_esparsa(Tipo_Mat_Esparsa *matE){ //funcao para imprimir matriz
    if(!matriz_vazia(matE)){
        printf("\n ---------------------- Imprimindo Matriz Esparsa ---------------------- \n\n");

        for(int i=0;i<=matE->qtd_linhas;i++){

            printf("| ");
            for(int j=0;j<=matE->qtd_colunas;j++){
                printf(" %3d ",buscar_elemento(j,i,matE));
            }
            printf(" |\n");
        }
        printf("\n -------------------------------------------- \n\n");


    }


}

int main(){
    Tipo_Mat_Esparsa *matE = criar_matriz_esparsa();

    inserir_elemento_matriz_esparsa(2,1,4,matE);
    inserir_elemento_matriz_esparsa(2,2,3,matE);
    inserir_elemento_matriz_esparsa(3,3,8,matE);
    inserir_elemento_matriz_esparsa(0,0,12,matE);
    inserir_elemento_matriz_esparsa(1,1,3,matE);
    inserir_elemento_matriz_esparsa(1,0,6,matE);

    Tipo_Mat_Esparsa *matE2;

    inserir_elemento_matriz_esparsa(1,2,11,matE2);
    inserir_elemento_matriz_esparsa(3,0,10,matE2);
    inserir_elemento_matriz_esparsa(3,3,9,matE2);
    inserir_elemento_matriz_esparsa(2,2,7,matE2);
    inserir_elemento_matriz_esparsa(3,1,3,matE2);
    inserir_elemento_matriz_esparsa(1,3,2,matE2);

    Tipo_Mat_Esparsa *matE3;
    Tipo_Mat_Esparsa *matE4;
    Tipo_Mat_Esparsa *matE5;

    soma_Matrizes_esparsa(matE,matE2,&matE3);
    multiplica_matriz_esparsa(matE,matE2,&matE4);
    transposta_matriz_esparsa(matE4 ,&matE5);

    imprimir_matriz_esparsa(matE);
    imprimir_matriz_esparsa(matE2);
    imprimir_matriz_esparsa(matE3);
    imprimir_matriz_esparsa(matE4);
    imprimir_matriz_esparsa(matE5);

    apaga_matriz(matE);
    apaga_matriz(matE2);
    apaga_matriz(matE3);
    apaga_matriz(matE4);
    apaga_matriz(matE5);


    return 0;
}

