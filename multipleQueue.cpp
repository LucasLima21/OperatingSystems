/*
Compilação g++ <nomeDoArquivo.cpp> -o <executavel> ; ./executavel 
*/
#include<iostream>
#include<time.h> // biblioteca para usar função time, como semente para função randômica
#include<math.h> // realizar potenciação
using namespace std;
//classe Node, suporte para fazer fila dinâmica 
template <typename Type>
class Node{
private:
  Type item;
  Node<Type> * next;
public:
  Node();
  Node(Type);
  Node<Type> * getNext(){
    return next;
  }
  void setNext(Node<Type> * next){
    this->next = next;
  }
  Type getItem(){
    return item;
  }
  void setItem(Type item){
    this->item = item;
  }
};

template<typename Type>
Node<Type>::Node(){
  next = NULL;
}

template<typename Type>
Node<Type>::Node(Type item){
  this->item = item;
  next = NULL;
}
//classe processo pra simular um processo na fila
class Process{
  private:
    int arrivalTimeInCPU;  //tempo que a Processo chega na fila para ser escalonada pro processador
    int necessaryTimeInCPU; //tempo necessario para que a Processo compelte sua tarefa 
    int timesOnCPU; //mostra quantas vezes o processo teve que ser escalonado para finalizar
    int saveNecessaryTime; // somente para exibir o tempo q era preciso para ele ser processado pelo processador
  public:
    Process();
    int getArrivalTimeInCPU(){
      return arrivalTimeInCPU;
    }
    void setArrivalTimeInCPU(int arrivalTimeInCPU){
      this->arrivalTimeInCPU = arrivalTimeInCPU;
    }
    int getNecessaryTimeInCPU(){
      return necessaryTimeInCPU;
    }
    void setNecessaryTimeInCPU(int necessaryTimeInCPU){
      this->necessaryTimeInCPU = necessaryTimeInCPU;
    }
    int getSaveNecessaryTime(){
      return saveNecessaryTime;
    }
    void setSaveNecessaryTime(int saveNecessaryTime){
      this->saveNecessaryTime = saveNecessaryTime;
    }
    int getTimesOnCPU(){
      return timesOnCPU;
    }
    void setTimesOnCPU(int timesOnCPU){
      this->timesOnCPU = timesOnCPU;
    }
    void showProcess();
};
Process::Process(){
  arrivalTimeInCPU = 0;
  necessaryTimeInCPU = 0;
  timesOnCPU = 0;
  saveNecessaryTime = 0;
}
void Process::showProcess(){
  cout << "|Process[" << getArrivalTimeInCPU() <<"]\t|" << getSaveNecessaryTime() << "\t\t|" << getTimesOnCPU() << "\t\t|" << endl;
}

// implementação da fila e seus métodos
template<typename Type>
class DynamicQueue{
private:
  Node<Type> * front, * back;
public:
  DynamicQueue();

  Node<Type> * getFront(){
    return front->getNext();
  }
  void setFront(Node<Type> * front){
    this->front = front;
  }
  Node<Type> * getBack(){
    return back;
  }
  void setBack(Node<Type> * back){
    this->back = back;
  }
  bool emptyQueue();
  void enqueue(Type);
  void dequeue();
  void showQueue();
};

template<typename Type>
bool DynamicQueue<Type>::emptyQueue(){
  return ((front == back)? 1 : 0);
}
template<typename Type>
DynamicQueue<Type>::DynamicQueue(){
  front = new Node<Type>();
  front->setNext(NULL);
  back = front;
}

template<typename Type>
void DynamicQueue <Type>::enqueue(Type x){
  back->setNext(new Node<Type>());
  back = back->getNext();
  back->setItem(x);
  back->setNext(NULL);
}

template<typename Type>
void DynamicQueue <Type>::dequeue(){
  Node<Type> * aux  = front;
  if(front!=back){
    front = front->getNext();
    delete aux;
  }else cout << "Empty Queue" << endl;
}

template<typename Type>
void DynamicQueue <Type>::showQueue(){
  Node<Type> * i  =  front->getNext();
  while(i!=NULL){
    i->getItem().showProcess();
    i = i->getNext();
  }
}

class MultipleQueueScheduler{
  private:
    DynamicQueue<Process> * multipleQueue;
    // aqui onde irei alocar um vetor de roundRobin, pois cada processo em cada classe de prioridade terá um quanta específico
    // o processo vai, de todo modo todos os processos em uma classe de prioridade ainda serão processados por apenas uma passada do RoundRobin
    int highestPrioirity;
  public:
    MultipleQueueScheduler();
    DynamicQueue<Process> * getMultipleQueue(){
      return multipleQueue;      
    }
    void setMultipleQueue(DynamicQueue<Process> * multipleQueue){
      this->multipleQueue = multipleQueue;
    }
    int getHighestPriority(){
      return highestPrioirity;
    }
    void setHighestPriority(int highestPrioirity){
      this->highestPrioirity = highestPrioirity;
    }        
    void multiple_queue_scheduling(); 
    void show_multiple_queue_scheduling();
};

MultipleQueueScheduler::MultipleQueueScheduler(){
  highestPrioirity = 9; //definindo a mais alta classe de prioridade como 9, como considero a posição 0 do vetor
  //eu tenho de 0 a 8 classes de prioridade, como os valores definidos de tempo para cada processo finalizar execução
  // são de no maximo 256, então garanto que boa parte dos processo que estiverem com este valor de quantum, e estiverem
  // com alta prioridade, sejam executados por completo.
  multipleQueue = new DynamicQueue<Process>[highestPrioirity];  
}

void MultipleQueueScheduler::show_multiple_queue_scheduling(){
  cout << "|ArrivalTime\t|NecessaryTime\t|TimesOnCPU\t" << endl;
  // todos os processos que possuem maior prioridade rodam com quanta 1, depois 2, e assim sucessivamente
  // aqui irei definir o quanta de cada classe de prioridade
}

// metodo que implemente o escalonamento por filas multiplas
// onde ha um vetor com classes de prioridades baseada no seus indices
// uma fila em cada classe de prioridade
// e quando um processo utiliza o processado do mais alto nivel de classe
// ele vai para o nivel mais baixo, recebendo mais quanta
// até q ele seja finalizado ou nao
void MultipleQueueScheduler::multiple_queue_scheduling(){
  cout << " \nSituação antes da utilização do Escalonamento por Filas Multiplas \n" << endl;
  int quanta = 0;
  for(int i = getHighestPriority() - 1; i >= 0; i--){
    cout << "\n|Quanta = "<< pow(2,quanta) << "\t|Priority Class = " << i << "\t\t|" << endl;
    show_multiple_queue_scheduling();
    getMultipleQueue()[i].showQueue();
    quanta++;
  }

  quanta = 0;
  cout << "\n\n\n\n\t### MULTIPLE QUEUE SCHEDULING ###" << endl;
  for(int i = getHighestPriority() - 1; i >= 0; i--){
    cout << "\n[Quanta = "<< pow(2,quanta) << "\t|Priority Class = " << i << "\t]" << endl;
    while(!getMultipleQueue()[i].emptyQueue()){
      Process auxProcess;
      auxProcess.setNecessaryTimeInCPU(getMultipleQueue()[i].getFront()->getItem().getNecessaryTimeInCPU());
      auxProcess.setArrivalTimeInCPU(getMultipleQueue()[i].getFront()->getItem().getArrivalTimeInCPU());
      auxProcess.setTimesOnCPU(getMultipleQueue()[i].getFront()->getItem().getTimesOnCPU());
      auxProcess.setSaveNecessaryTime(getMultipleQueue()[i].getFront()->getItem().getSaveNecessaryTime());
      if(auxProcess.getNecessaryTimeInCPU() > 0){
        auxProcess.setNecessaryTimeInCPU(auxProcess.getNecessaryTimeInCPU() - pow(2,quanta));
        int counterTimesOnCPU = auxProcess.getTimesOnCPU();
        counterTimesOnCPU++; 
        auxProcess.setTimesOnCPU(counterTimesOnCPU);
        counterTimesOnCPU = 0;
        getMultipleQueue()[i].dequeue();  
        if(i == 0){
          if(auxProcess.getNecessaryTimeInCPU() <= 0){
            getMultipleQueue()[i].dequeue();
            cout << "\nProcesso[" << auxProcess.getArrivalTimeInCPU() << "] foi processado " << auxProcess.getTimesOnCPU() << " vezes pela CPU\n"  << endl;
          }
          getMultipleQueue()[i].enqueue(auxProcess);
        }
        else{
          if(auxProcess.getNecessaryTimeInCPU() <= 0){
            cout << "\nProcesso[" << auxProcess.getArrivalTimeInCPU() << "] foi processado " << auxProcess.getTimesOnCPU() << " vezes pela CPU\n"  << endl;
          }
          else{
            getMultipleQueue()[i-1].enqueue(auxProcess);

          }
        }
      }
      else{
        getMultipleQueue()[i].dequeue();
        //auxProcess.showProcess();
      }
    }
    quanta++;
  }
}

int main(){
  srand(time(NULL));
  int quantityOfProcesss;
  quantityOfProcesss = rand()%30 + 1; //gera um numero aleatorio que define a quantidade de processos
  MultipleQueueScheduler * mq = new MultipleQueueScheduler();
  cout << "Quantity of Processs = " << quantityOfProcesss << endl;
  cout << "HighestPriory = " << mq->getHighestPriority() - 1<< endl;
  for(int i = 0; i < quantityOfProcesss; i++){
    Process auxProcess;
    auxProcess.setArrivalTimeInCPU(i);
    int value = rand()%255 + 1;   // processos com tempo de no máximo 2^8; ou seja 1, ,2, 4, 8, .... 128, 256 quanta;
    //se o processo possui tempo maximo de 256, então, garanto que ele seja processado em 8 classes de prioridade
    auxProcess.setNecessaryTimeInCPU(value); //pelo menos um vez o processo precisa consumir da CPU
    auxProcess.setSaveNecessaryTime(value);
    //pois como o mesmo é consumido na memória, o valor se perde apos o roundRobin
    int priority = rand()%mq->getHighestPriority(); // decide aleatoriamente, conforme a quantidade de classes multiplas
    //em qual classe de prioridade o processo será colocado
    mq->getMultipleQueue()[priority].enqueue(auxProcess);
  }  
  mq->multiple_queue_scheduling();
  //OBS.: haverão casos em que processos que estejam na classe mais baixa de prioridade que é a 0, e que tenham
  delete mq;
  return 0;
}