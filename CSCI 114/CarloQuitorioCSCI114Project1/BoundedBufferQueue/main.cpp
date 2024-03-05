#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

//class for the queue
class BBQ {
private:
    int maxQSize;
    queue<int> kue;
    mutex mutx;
    condition_variable itemInserted;
    condition_variable itemPopped;

public:
    BBQ(int kueSize){ 
        maxQSize = kueSize; 
    
    }

    int getQueSize(){ 
        return kue.size();
    }

    int getMaxQueSize(){ 
        return maxQSize;
    }

    void insert(int val, int* threadNum, int* producersStopped);
    int remove(int* threadNum, int* consumersStopped);

    float getQueCap() { 
        return float(getQueSize() / getMaxQueSize());
    }

    void print();
};

void BBQ::insert(int x, int *threadNum, int *producersStopped) { //waits until there is room and then inserts into the queue
    unique_lock<mutex> lock(mutx);

    while (kue.size() >= maxQSize) {
        cout << "Waiting to produce by thread number #" << *threadNum << endl;
        (*producersStopped)++;
        itemInserted.wait(lock);
    }

    kue.push(x);
    itemInserted.notify_one();
    lock.unlock();
}

int BBQ::remove(int *threadNum, int *consumersStopped) { //waits until there is an item and then removes it
    int x;
    unique_lock<mutex> lock(mutx);

    while (kue.empty()) {
        cout << "Waiting to consume by thread number #" << *threadNum << endl;
        (*consumersStopped)++;
        itemInserted.wait(lock);
    }

    x = kue.front();
    kue.pop();
    itemPopped.notify_one();
    lock.unlock();
    return x;
}

void BBQ::print() {
    queue<int> print = kue;

    while (!(kue.empty())) {
        cout << kue.front() << " ";
        kue.pop();
    }
    cout << endl;
}



//finds the data associated with every argument
int getArgData(vector<string> arguments, string prefix) {
    for (string arg : arguments) {
        if (arg.size() > prefix.size() + 2 && prefix == arg.substr(1, prefix.size())) {
            return stoi(arg.substr(prefix.size() + 2, arg.size() - prefix.size()));
        }
    }
    return -1;
}

int itemsProduced = 0;
int itemsConsumed = 0;
int producersStopped = 0;
int consumersStopped = 0;
const int threadPairs = 20;
const float maxCap = 0.75;

void producer(BBQ *bbq, int threadNum, int delayRange);
void consumer(BBQ *bbq, int threadNum, int delayRange);

int main(int numArguments, char *argumentString[]) {
    srand(time(NULL));

    //get command line arguments
    vector<string> args;
    for (size_t i = 0; i < numArguments; i++){
        args.push_back(string(argumentString[i]));
    }

    int tpDelay = getArgData(args, "tp");
    int tcDelay = getArgData(args, "tc");
    int maxSize = getArgData(args, "qs");
    //make default size to 25
    maxSize = maxSize == -1 ? 25 : maxSize;

    BBQ *Project = new BBQ(maxSize);

    //make producers and consumers
    vector<thread> producers;
    vector<thread> consumers;

    //producers are even and consumers are odd
    for(size_t i = 0; i < threadPairs; i++){
        if(i%2 == 0){ //allows producers and consumers to alternate executes
            producers.push_back(thread(producer, Project, i, tpDelay));
        }
        else{
            consumers.push_back(thread(consumer, Project, i + threadPairs, tcDelay));
        }
    }

    while(true) //low cost way to make program run forever
       ;

    //does not execute while running forever
    this_thread::sleep_for(chrono::seconds(5));

    cout << "Items produced: " << itemsProduced << endl;
    cout << "Items consumed: " << itemsConsumed << endl;
    cout << "Number of stopped producers: " << producersStopped << endl;
    cout << "Number of stopped consumers: " << consumersStopped << endl;

return 0;
}

void producer(BBQ *bbq, int threadNum, int delayRange) {
    while (true){
        int delayMod = 1;
        if(bbq->getQueCap() > maxCap){ //if the capacity goes past 75% slowdown the producer
            delayMod = 1.0 + (bbq->getQueCap() - delayMod) / (1.0 - delayMod);
        }

        else if (bbq->getQueCap() < (1 - delayMod)){ //if the capacity is near empty speed up producer
            delayMod = bbq->getQueCap() / (1-delayMod);
        }

        int delayTime = (rand() % delayRange) * delayMod;
        this_thread::sleep_for(chrono::milliseconds(delayTime));

        bbq->insert(itemsProduced, &threadNum, &producersStopped);
        itemsProduced++;
        cout << "Item #" << itemsProduced << " produced by thread #" << threadNum << endl;
    }
}

void consumer(BBQ *bbq, int threadNum, int delayRange){
    while (true){
        int delayTime = rand() % (delayRange);
        this_thread::sleep_for(chrono::milliseconds(delayTime));

        int item = bbq->remove(&threadNum, &consumersStopped);
        itemsConsumed++;

        cout << "Item #" << item << " consumed by thread #" << threadNum << endl;
    }
}