#include<stdio.h>   // biblioteca padrão do C
#include<stdlib.h>  // biblioteca padrão do C
#include<sys/shm.h>     // biblioteca para criação da memória compartilhada
#include<sys/wait.h>    //usar função wait
#include<signal.h>      //usar função signal
#include<sys/types.h>   //utilizar com pid
#include<unistd.h>      // biblioteca para criação de processos e utilização do pid
#include<time.h>        // biblioteca para uso do tempo como semente da função randomica
#define SAME_KEY 335    // chave temporária para gerar chave uma outra chave que irá gerar o id da memória compartilhada
#define TRUE 1          //definição de TRUE
#define FALSE 0         //definição de FALSE
#define EXTENT 11        //defininção de tamanho do buffer


//algoritmos do monitor 
void monitor_putItemBuffer(int * buffer, int id_commonMemory, int item, int full, int empty);
void monitor_removeItemBuffer(int * buffer, int id_commonMemory, int full, int empty);
//produz um item aleatorio entre 0 e 100
int producer_item(){
    int item = rand()%100;
    return item;
}

//retorna o vetor que no caso é o buffer
int * bufferArray(int id_commonMemory){
    int * buffer;
    buffer = (int *) shmat(id_commonMemory, 0, 0);
    return buffer;
}
// rotina que poe itens no buffer na posição relativa do contador que é a ultima posição reservada do buffer
// aqui o monitor permine ao processo produtor acessar a memória compartilhada e inserir o item gerado
void monitor_putItemBuffer(int * buffer, int id_commonMemory, int item, int full ,int empty){
    buffer = bufferArray(id_commonMemory);
    if(buffer == NULL){
        perror("shmat error");
        exit(1);
    }
    if(buffer[EXTENT-1] == EXTENT - 1){
        wait(NULL);
    }
    buffer[EXTENT-1]++;
    buffer[buffer[EXTENT-1]] = item;
    if(buffer[EXTENT-1] == 1){
        monitor_removeItemBuffer(buffer, id_commonMemory, full, empty);
    }
}

// rotina que remove itens do buffer e põe -1 no item que fora removido
// aqui o monitor permite ao processo consumidor acessar a memoria compartilhada
void monitor_removeItemBuffer(int * buffer, int id_commonMemory, int full, int empty){
    int item;
    buffer = bufferArray(id_commonMemory);
    if(buffer == NULL){
        perror("shmat error");
        exit(1);
    }
    if(buffer[EXTENT-1] == 0){
        wait(NULL);
    }
    item = buffer[buffer[EXTENT-1]];
    printf("item consumido = %d\n", item);
    buffer[buffer[EXTENT-1]] = -1;
    if(buffer[EXTENT-1] == EXTENT - 2){
        monitor_putItemBuffer(buffer, id_commonMemory, item, full, empty);
    }
    buffer[EXTENT-1]--;
}


int main(){
    key_t key;
    int id_commonMemory, full = EXTENT-1, empty = 0;
    int * buffer;
    key = ftok("/mnt/c/Users/Lucas/Desktop/SO", SAME_KEY);    // cria a chave de acesso
    //tens que passar o diretorio onde esta este arquivo para a string acima
    id_commonMemory = shmget(key, sizeof(int)*EXTENT, IPC_CREAT | 0666);      // acessa a area de memoria, se nao existe, ele cria
    //0666 é a permissao de acesso usual no linux
    printf("Identificador do segmento recuperado = %d\n", id_commonMemory);
    fflush(NULL);
    printf("Este segmento esta associado a chave unica = %d\n", key);
    fflush(NULL);
    //caso nao consiga criar
    if(id_commonMemory == -1){
        perror("shmget error");
        exit(1);
    }
    buffer = (int *) shmat(id_commonMemory, 0, 0); //inicializando o contador
    buffer[EXTENT-1] = 0; // BUFFER é o COUNT posição final  do vetor(buffer) armazenado para o contador que irá percorrer no buffer
    srand(time(NULL)); // pega o tempo como semente e joga na função randomica do C, para inserir na  memoria compartilhada pelo processo 1
    pid_t pidNumber; // para obter o numero do processo que acessa a memória durante a execução
    // criando processos...
    if(fork() == 0){  //processo 1 é o consumidor
        while(TRUE){
            monitor_removeItemBuffer(buffer, id_commonMemory,full, empty);
            sleep(1); //delayy para ver o resultado
        }
    }
    else{      // processo 0 é o produtor
        while(TRUE){
            int item; 
            item = producer_item();
            printf("item produzido = %d\n", item);
            monitor_putItemBuffer(buffer, id_commonMemory, item, full,empty);
            sleep(1); //delayy para ver o resultado
            
        }
    }
    buffer[EXTENT-1] = 0;
    shmctl(id_commonMemory, IPC_RMID, NULL);
    return 0;
}