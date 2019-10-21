/*
Equipe:
    Lucas da Silva Lima - 1515080170
    André Ricardo Marques Rogério - 1615080011
*/
/*Compilação gcc <nomeDoArquivo.c> -o <executavel> ; ./executavel */
#include<stdio.h>   // biblioteca padrão do C
#include<stdlib.h>  // biblioteca padrão do C
#include<sys/shm.h>     // biblioteca para criação da memória compartilhada
#include<unistd.h>      // biblioteca para criação de processos e utilização do pid
#include<time.h>        // biblioteca para uso do tempo como semente da função randomica
#define SAME_KEY 333    // chave temporária para gerar chave uma outra chave que irá gerar o id da memória compartilhada
#define TRUE 1          //definição de TRUE
#define FALSE 0         //definição de FALSE

int interested[2], turn; //variaveis globais para serem verificadas pelos processos
// método que entra na regiao critica, acessa a memoria compartilhada
// e o processo que acessar essa regiao é mostrado na tela a alteração do dado dessa regiao
void enterCriticalRegion(int * pointer, int id_commonMemory, int process){
    // primeiro argumento é a chave para acessar o endereço 
    // o segundo por ser 0, o sistema escolhe um espaço de endereço disponivel pro processo
    // 0 indica leitura e escrita nesta area
    pointer = (int *) shmat(id_commonMemory, 0, 0);      // acessa a area de espaço de endereçamento do processo
    //ponteiro que acessa a memoria compartilhada, se dois processos tentarem acessar ao mesmo tempo, deve da erro !!
    
    //caso nao consiga acessar
    if( pointer == NULL){
        perror("shmat error");
        exit(1);
    }
    //if só para exibir uma resposta que possa ver funcionando tanto processo 1 quanto o 0
    if(process == 1){
        printf("[Processo %d] - Ler Memoria compartilhada = %d\n" , process, *pointer);
        *pointer = rand()%100;
        printf("[Processo %d] - Valor alterado = %d\n", process, *pointer);
    }
    else{
        printf("[Processo %d] - Ler Memoria compartilhada = %d\n" , process, *pointer);
        *pointer = 111;
        printf("[Processo %d] - Valor alterado = %d\n", process, *pointer);
    }    
}

void leaveCritialRegion(int process){  // processo sai da regiao critica e libera para o outro processo entrar
    interested[process] = FALSE;
}

int createSharedMemory(){
    key_t key;
    int id_commonMemory;
    key = ftok("/mnt/c/Users/Lucas/Desktop/SO", SAME_KEY);    // cria a chave de acesso
    id_commonMemory = shmget(key, sizeof(int), IPC_CREAT | 0666);      // acessa a area de memoria, se nao existe, ele cria
    //0666 é a permissao de acesso usual no linux
    printf("Identificador do segmento recuperado = %d", id_commonMemory);
    printf("\n");
    printf("Este segmento esta associado a chave unica = %d", key);
    printf("\n");
    //caso nao consiga criar
    if(id_commonMemory == -1){
        perror("shmget error");
        exit(1);
    }
    return id_commonMemory;
}


int main(){
    key_t key;
    int id_commonMemory;
    int * pointer;
    key = ftok("/mnt/c/Users/Lucas/Desktop/SO", SAME_KEY);    // cria a chave de acesso
    //insere na string o caminho do diretorio onde ficara salvo este arquivo
    id_commonMemory = shmget(key, sizeof(int), IPC_CREAT | 0666);      // acessa a area de memoria, se nao existe, ele cria
    //0666 é a permissao de acesso usual no linux
    printf("Identificador do segmento recuperado = %d", id_commonMemory);
    printf("\n");
    printf("Este segmento esta associado a chave unica = %d", key);
    printf("\n");
    //caso nao consiga criar
    if(id_commonMemory == -1){
        perror("shmget error");
        exit(1);
    }
    pid_t pidNumber; // para obter o numero do processo que acessa a memória durante a execução
    srand(time(NULL)); // pega o tempo como semente e joga na função randomica do C, para inserir na  memoria compartilhada pelo processo 1
    // criando processos...
    if(fork() == 0){
        while(TRUE){
            printf("\n");
            printf("[Processo 1] - com o pid = %d", getpid()); 
            printf("\n");
            // algoritmo de Peterson
            interested[1] = TRUE;
            turn = 0;
            while(interested[0] == 1 && turn == 0){
                printf("[Processo 1] - Memória Ocupada/Em Uso, espere!\n");
            }
            printf("[Processo 1] - Entra na Região Crítica\n");
            enterCriticalRegion(pointer,id_commonMemory, 1);
            printf("[Processo 1] - Fim da Região Crítica\n");
            leaveCritialRegion(1); // libera a memória compartilhada   
            sleep(1); //delayy para ver o resultado
        }        
    }
    else{
        while(TRUE){
            printf("\n");
            printf("[Processo 0] com o pid = %d", getpid());
            printf("\n");
            // algoritmo de Peterson
            interested[0] = TRUE;
            turn = 1;
            while(interested[1] == TRUE && turn == 1){
                printf("[Processo 0] - Memória Ocupada/Em Uso, espere!\n");
            }
            printf("[Processo 0] - Entra na Região Crítica\n");
            enterCriticalRegion(pointer,id_commonMemory, 0);
            printf("[Processo 0] - Fim da Região Crítica\n");
            leaveCritialRegion(0); //libera a memoria compartilhada
            sleep(1); // delay para ver o resultado
        }
    }
    
    shmctl(id_commonMemory, IPC_RMID, NULL);
}