#include<stdio.h>   // biblioteca padrão do C
#include<stdlib.h>  // biblioteca padrão do C
#include<sys/shm.h>     // biblioteca para criação da memória compartilhada
#include<unistd.h>      // biblioteca para criação de processos e utilização do pid
#include<time.h>        // biblioteca para uso do tempo como semente da função randomica
#define SAME_KEY 334    // chave temporária para gerar chave uma outra chave que irá gerar o id da memória compartilhada
#define TRUE 1          //definição de TRUE
#define FALSE 0         //definição de FALSE
#define EXTENT 11        //defininção de tamanho do buffer
//assinatura dos processos para que um possa chamar o outro dentro de si
void producer(int * buffer, int id_commonMemory);
void consumer(int * buffer, int id_commonMemory);

//produz um item aleatorio entre 0 e EXTENT-10
int producer_item(){
    int item = rand()%100;
    return item;
}

// rotina que poe itens no buffer na posição relativa do contador 
//sendo que pro buffer só há EXTENT-1 posicoes
// processo consumidor acessa a memória compartilhada
void putItemBuffer(int * buffer, int id_commonMemory, int item){
    buffer = (int *) shmat(id_commonMemory, 0, 0); //acessa a memoria, atraves da chave, para escrite e leitura
    if(buffer == NULL){
        perror("shmat error");
        exit(1);
    }
    buffer[buffer[EXTENT-1]] = item;
}

// rotina que remove itens do buffer e põe -1 no item que fora removido
// retorna o item que foi removido
// aqui o processo consumidor acessa a memória compartilhada
int removeItemBuffer(int *buffer, int id_commonMemory){
    int item;
    buffer = (int *) shmat(id_commonMemory, 0, 0); //acessa a memoria, atraves da chave, para escrite e leitura
    if(buffer == NULL){
        perror("shmat error");
        exit(1);
    }
    item = buffer[buffer[EXTENT-1]];
    buffer[buffer[EXTENT-1]] = -1;
    return item;
}

// produtor que ira produzir e colocar itens no buffer
// os itens são de 0 a 99 randomicamente
void producer(int * buffer, int id_commonMemory){
    int item;
    item = producer_item();
    fflush(NULL);
    if(buffer[EXTENT-1] == (EXTENT - 1)){
        sleep(1); //dorme produtor  
    }
    buffer[EXTENT-1]++;
    putItemBuffer(buffer,id_commonMemory, item);
    if(buffer[EXTENT-1] == 1){
        consumer(buffer, id_commonMemory);     // chama processo consumidor, acorda-o (wakeUp)
    }
    printf("item produzido = %d\n", item);
}

// processo consumidor que ira remover o item do buffer e colocar -1 no local para marcar que fora consumido
void consumer(int * buffer, int id_commonMemory){
    int item;
    if(buffer[EXTENT-1] == 0){
        sleep(1); // dorme consumidor            
    }
    item = removeItemBuffer(buffer, id_commonMemory);
    buffer[EXTENT-1]--;
    if(buffer[EXTENT-1] == (EXTENT - 2)){
        producer(buffer, id_commonMemory);     //chama processo produtor, acorda-o (wakeUP)      
    }
    printf("Item consumido = %d\n", item);
    fflush(NULL);
}

// funcao main que chama a subrotina que executa os processos e algoritmos
int main(){
    key_t key;
    int id_commonMemory;
    int * buffer;
    key = ftok("/mnt/c/Users/Lucas/Desktop/SO", SAME_KEY);    // cria a chave de acesso
    //tens que passar o diretorio onde esta este arquivo para a string acima
    id_commonMemory = shmget(key, sizeof(int)*EXTENT, IPC_CREAT | 0666);      // acessa a area de memoria, se nao existe, ele cria
    //key é a chave de acesso da memoria, e segundo paramentro é o tamanho alocado, 0666 é a permissao de acesso usual no linux
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
            consumer(buffer, id_commonMemory);
            sleep(1); //delayy para ver o resultado
        }
    }
    else{      // processo 0 é o produtor
        while(TRUE){
            producer(buffer, id_commonMemory);
            sleep(1); //delayy para ver o resultado
        }
    }
    shmctl(id_commonMemory, IPC_RMID, NULL);
    //observação se o contador que é a ultima posição do buffer extrapolar
    // os limites superior e inferior do mesmo , ele vai da segmentation fault, pois o processo é posto apenas para dormir
    // e dependende da velocidade do processo consumidor ou produtor
    return 0;
}