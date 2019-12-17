/*
Compilação g++ <nomeDoArquivo.cpp> -o <executavel> ; ./executavel 
*/
#include<iostream>
#include<time.h> // biblioteca para usar função time, como semente para função randômica
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
    int arrivalTimeInCPU;  //tempo que o Processo chega na fila para ser escalonada pro processador
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
  cout << "[ ";
  while(i!=NULL){
    i->getItem().showProcess();
    i = i->getNext();
  }
  cout << "]" << endl;
}

class RoundRobinScheduler{
  private:
    DynamicQueue<Process> * processsQueue;
    int quantum;
  public:
    RoundRobinScheduler(){
    processsQueue = new DynamicQueue<Process>();
    quantum = rand()%20 + 1;
    }
    DynamicQueue<Process> * getProcesssQueue(){
      return processsQueue;
    }
    void setProcesssQueue(DynamicQueue<Process> * processsQueue){
      this->processsQueue = processsQueue;
    }
    int getQuantum(){
      return quantum;
    }
    void setQuantum(int quantum){
      this->quantum = quantum;
    }
    void round_robin_scheduling();
      void show_round_robin_scheduling();
};

void RoundRobinScheduler::show_round_robin_scheduling(){
  cout << "|ArrivalTime\t|NecessaryTime\t|TimesOnCPU\t|" << endl;
}

//metodo que implemente o round robin,
//remobe o processo da fila e o coloca de volta, até que o mesmo seja executado por completo
//e conta quantas vezes o mesmo passou pelo processador
void RoundRobinScheduler::round_robin_scheduling(){
  show_round_robin_scheduling();
  while(!getProcesssQueue()->emptyQueue()){
    Process auxProcess;
    auxProcess.setNecessaryTimeInCPU(getProcesssQueue()->getFront()->getItem().getNecessaryTimeInCPU());
    auxProcess.setArrivalTimeInCPU(getProcesssQueue()->getFront()->getItem().getArrivalTimeInCPU());
    auxProcess.setTimesOnCPU(getProcesssQueue()->getFront()->getItem().getTimesOnCPU());
    auxProcess.setSaveNecessaryTime(getProcesssQueue()->getFront()->getItem().getSaveNecessaryTime());
    if(auxProcess.getNecessaryTimeInCPU() > 0){
      auxProcess.setNecessaryTimeInCPU(auxProcess.getNecessaryTimeInCPU() - getQuantum());
      int counterTimesOnCPU = auxProcess.getTimesOnCPU();
      counterTimesOnCPU++; 
      auxProcess.setTimesOnCPU(counterTimesOnCPU);
      getProcesssQueue()->dequeue();
      getProcesssQueue()->enqueue(auxProcess);
    }
    else{
      auxProcess.showProcess();
      getProcesssQueue()->dequeue();
    }
  }
}

int main(){
  srand(time(NULL));
  int quantityOfProcesss;
  quantityOfProcesss = rand()%100 + 1; //gera um numero aleatorio que define a quantidade de processos
  RoundRobinScheduler * rr = new RoundRobinScheduler();
  cout << "Quantity of Processs = " << quantityOfProcesss << endl;
  cout << "Quantum = " << rr->getQuantum() << endl;
  for(int i = 0; i < quantityOfProcesss; i++){
    Process auxProcess;
    auxProcess.setArrivalTimeInCPU(i);
    int value = rand()%200 + 1; // gera o tempo que cada processo terá para completar execução no processador
    auxProcess.setNecessaryTimeInCPU(value); //pelo menos um vez o processo precisa consumir da CPU
    auxProcess.setSaveNecessaryTime(value);
    //pois como o mesmo é consumido na memória, o valor se perde apos o roundRobin
    rr->getProcesssQueue()->enqueue(auxProcess);
  }  
  rr->round_robin_scheduling();
  delete rr;
  return 0;
}