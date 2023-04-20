#include <iostream>
#include <bits/stdc++.h>

using namespace std;

class Packet{
private:
    int id;
    int size;
public:
    Packet(int id, int size){
        this->id = id;
        this->size = size;
    }
    int getId(){
        return this->id;
    }
    int getSize(){
        return this->size;
    }
};

class Bucket{
protected:
    int bucketSize;
    int currBucketSize;
    queue<Packet> buffer;
    int currId;

public:
    Bucket() {
        this->currBucketSize = 0;
        this->currId = 1;
    }

    void addPacket(int psize){
        Packet* newPacket = new Packet(currId, psize);
        currId++;
        if(currBucketSize + newPacket->getSize() > bucketSize){
            cout << "Bucket is full. Packet rejected." << endl;
            return;
        }
        buffer.push(*newPacket);
        currBucketSize += newPacket->getSize();
        cout << "Packet " << newPacket->getId() << " added to queue." << endl;
    }

    virtual void transmit(){};
};

class TokenBucket : public Bucket{
    int maxTokens;
    int currTokens;
    int tokenRate;

public:
    TokenBucket(int bucketSize, int maxTokens, int tokenRate){
        this->bucketSize = bucketSize;
        this->maxTokens = maxTokens;
        this->currTokens = 0;
        this->tokenRate = tokenRate;
    }

    void transmit(){
        if(currTokens+tokenRate < maxTokens){
            currTokens = currTokens + tokenRate;
        }
        else{
            currTokens = maxTokens;
        }

        if(buffer.size() == 0){
            cout << "No packets in bucket" << endl;
            return;
        }

        while(!buffer.empty()){
            Packet topPacket = buffer.front();

            if(currTokens < 1) break;
            currTokens--;

            buffer.pop();
            cout << "Packet with id = " << topPacket.getId() << " transmitted." << endl;
        }
    }
};

class LeakyBucket : public Bucket{
    int leakRate;

public:
    LeakyBucket(int bucketSize, int leakRate){
        this->leakRate = leakRate;
        this->bucketSize = bucketSize;
        this->currBucketSize = 0;
        this->currId = 1;
    }

    void transmit(){
        if(buffer.size() == 0){
            cout << "No packets in bucket" << endl;
            return;
        }
        int n = leakRate;
        while(!buffer.empty()){
            Packet topPacket = buffer.front();
            int topPacketSize = topPacket.getSize();
            if(topPacketSize > n) break;
            n = n - topPacketSize;
            currBucketSize -= topPacketSize;
            buffer.pop();
            cout << "Packet with id = " << topPacket.getId() << " transmitted." << endl;
        }
    }
};

void timerFunc(Bucket* bucket){
    while(true){
        cout << "Clock tick... " << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        bucket->transmit();
    }
}

void addPacketsFunc(Bucket* bucket){
    bucket->addPacket(200);
    bucket->addPacket(200);
    bucket->addPacket( 200);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bucket->addPacket( 200);
    bucket->addPacket( 200);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bucket->addPacket( 200);
    bucket->addPacket( 200);
}

int main() {
    LeakyBucket* leakyBucket = new LeakyBucket(1000, 400);
    thread th = thread{timerFunc, std::ref(leakyBucket)};
    thread th2 = thread{addPacketsFunc, std::ref(leakyBucket)};
    th.join();
    th2.join();

    TokenBucket* tokenBucket = new TokenBucket(1000, 8, 2);
    thread t3 = thread{timerFunc, std::ref(tokenBucket)};
    thread th4 = thread{addPacketsFunc, std::ref(tokenBucket)};
    th.join();
    th2.join();

    return 0;
}
