#include<bits/stdc++.h>
#include "globalvariables.h"
#include<algorithm>
using namespace std;

/////////////////////////////////////////////////////////////////utility functions/////////////////////////////////////////////

int getIndexLongest(int peerIndex)
{
    //get the local index of longest length block

    int longestLen=0;
    int indexLongest;
    int corrBlockID;

    for(int i=0;i<peersList[peerIndex].blocks.size();i++)
    {
        if(peersList[peerIndex].blocks[i].posBlockchain>longestLen){
            longestLen=peersList[peerIndex].blocks[i].posBlockchain;
            indexLongest=i;
            corrBlockID=peersList[peerIndex].blocks[i].blockId;
        }
    }

    // find the latest block with the longest length
    double timestamp=double(INT_MAX);
    for(int i=0;i<peersList[peerIndex].blocks.size();i++)
    {
        if(peersList[peerIndex].blocks[i].posBlockchain==longestLen){
            //when first is found
            if(timestamp> totalTime){
                timestamp = peersList[peerIndex].blocks[i].creationTime;
                indexLongest=i;
                corrBlockID=peersList[peerIndex].blocks[i].blockId;
            }
                //when one block is already found, find next
            else{
                if(timestamp>peersList[peerIndex].blocks[i].creationTime)
                {
                    timestamp = peersList[peerIndex].blocks[i].creationTime;
                    indexLongest=i;
                    corrBlockID=peersList[peerIndex].blocks[i].blockId;
                }
            }
        }
    }
    return indexLongest;

}


double unifRand()
{
    // generate a random number between 0 and 1
    std::mt19937_64 range;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    range.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
    return unif(range);
}

double expSampleVal(double lambda)
{
    // sample from an exponential distribution with param = lambda
    return log(1-unifRand())/((-1)*(lambda));
}

double computeLatency(int i,int j,double m)
{
    // compute the latency as described in ps
    double cij = (peersList[i].peerType && peersList[i].peerType) ? 1e8 : 5e6;
    return propagationDelay[i][j]+(m/cij)+expSampleVal((double)cij/96000);
}

double computeCoins(int hID,int gID)
{
    // compute coins for every node
    double currentCoins=0;
    int currBlockIndex=getIndexLongest(hID);
    while(currBlockIndex!=(-1))
    {
        currentCoins+=peersList[hID].blocks[currBlockIndex].peer2Amount[gID];
        currBlockIndex=peersList[hID].blocks[currBlockIndex].previousBlockIndex;
    }
    return currentCoins;
}

//////////////////////////////////////////////////////////initialization functions///////////////////////////////////////////////////

void makePeers()
{
    // initialise the network with given peers
    for(int i=0;i<numPeers-1;i++)
    {
        peer tempPeer;
        tempPeer.peerId=i;
        bool fastFlag=true;
        if(unifRand()<(z/100))
        {
            fastFlag= false;
        }
        tempPeer.peerType=fastFlag;
        tempPeer.blockGenLambda=blockGenLambda;
        tempPeer.transactionLambda=transactionGenLambda;
        tempPeer.node_type = 0;
        peersList.push_back(tempPeer);
    }

    peer tempPeer;
    tempPeer.peerId=numPeers-1;
    bool fastFlag=true;
    tempPeer.peerType=fastFlag;
    tempPeer.blockGenLambda=(alpha/(1-alpha))*(numPeers-1)*blockGenLambda;
    tempPeer.transactionLambda=transactionGenLambda;
    tempPeer.node_type = 1;
    peersList.push_back(tempPeer);

}

void makeConnectedGraph()
{
    //ensure graph is connected
    for(int i=0;i<numPeers-1;i++)
    {
        for(int j=i+1;j<numPeers-1;j++)
        {
            if(unifRand()<probConnect)
            {
                peersList[i].neighbours.push_back(j);
                peersList[j].neighbours.push_back(i);
            }
        }
    }

    // compute the number of connected peers using BFS
    int numConnectedPeers=0;
    vector<bool> visited(numPeers,false);
    queue<int> BFS;
    BFS.push(0);
    visited[0]=true;
    numConnectedPeers+=1;
    while(!BFS.empty())
    {
        int currPeer=BFS.front();
        BFS.pop();
        for(auto neighIndex: peersList[currPeer].neighbours)
        {
            if(!visited[neighIndex])
            {
                visited[neighIndex]=true;
                numConnectedPeers+=1;
                BFS.push(neighIndex);
            }
        }
    }
    // reinitialise if not connected

    if(numConnectedPeers!=numPeers-1)
    {
        for(int i=0;i<numPeers;i++)
        {
            peersList[i].neighbours.clear();
        }
        makeConnectedGraph();
    }
    else
    {
        int num_neighbours = zeta*numPeers;

        vector<int>indices;
        for(int i=0; i<numPeers-1; i++)
        {
            indices.push_back(i);
        }

        random_shuffle(indices.begin(), indices.end());

        for(int i=0; i<num_neighbours; i++)
        {
            peersList[numPeers-1].neighbours.push_back(indices[i]);
            peersList[indices[i]].neighbours.push_back(numPeers-1);
        }
    }


}

void makepropagationDelayVec()
{
    // initialise propagation delays RV drwan from uniform distribution 10-500
    propagationDelay.resize(numPeers, vector<double>(numPeers));
    for(int i=0;i<numPeers;i++)
    {
        for(int j=i+1;j<numPeers;j++)
        {
            double tempDelay=10+490*unifRand();
            propagationDelay[i][j]=tempDelay/1000;
            propagationDelay[j][i]=tempDelay/1000;
        }
    }
}

void genesisBlock()
{
    // add the first block
    block genBlock(0,0.0,-1,-1,1);

    genBlock.previousBlockIndex=-1;

    startMoneyWrite.open("initialCoins.txt");
    for(int i=0;i<numPeers;i++)
    {
        double randAmou=initialMaxAmount*unifRand();
        startMoneyWrite<<"Start Amount with Peer "<<i<<": "<<randAmou<<endl;
        genBlock.transactionSet.insert(transaction(transactionIdCounter,-1,i,randAmou));
        genBlock.peer2Amount[i]+=randAmou;
        transactionIdCounter+=1;
    }
    startMoneyWrite.close();

    // write info to file

    blockWrite<<newline<<endl;
    blockWrite<<"Block Hash : "<<genBlock.blockId<<endl;
    blockWrite<<"Created By Peer : "<<genBlock.peerID<<endl;
    blockWrite<<"Time of Creation : "<<genBlock.creationTime<<endl;
    blockWrite<<"Number of Transactions : "<<genBlock.transactionSet.size()<<endl;
    blockWrite<<"Previous Block Hash : "<<genBlock.previousBlockID<<endl;
    blockWrite<<newline<<endl;

    for(int i=0;i<numPeers;i++)
    {
        peersList[i].blocks.push_back(genBlock);
    }
}



void initSimulation()
{
    // trigger generation of blocks and transactions
    for(int i=0;i<numPeers;i++)
    {
        //generating Block
        event genBlock(3,expSampleVal(peersList[i].blockGenLambda), 0.0,0,i,block(),transaction());
        schedulerQ.push(genBlock);

        //generating Transaction
        event genTransaction(1,expSampleVal(peersList[i].transactionLambda), 0.0,0,i,block(),transaction());
        schedulerQ.push(genTransaction);
    }
}

void initialiseNetwork(){
    // call initialisation functions
    makepropagationDelayVec();
    makePeers();
    makeConnectedGraph();
    genesisBlock();
}


/////////////////////////////////////////////////////////////////Events Generation and Reception/////////////////////////////////////////////

void genTransact(event e)
{
    // generate a transation from the sneder to an arbitrary peer
    int senderId=e.currPeer;
    int receipientId=unifRand()*numPeers;
    while(receipientId==senderId)
    {
        receipientId=unifRand()*numPeers;
    }

    double transactionAmount=unifRand()*computeCoins(senderId,senderId);
    transaction newtrans(transactionIdCounter,senderId,receipientId,transactionAmount);
    transactionIdCounter+=1;
    peersList[senderId].allTransactions.insert(newtrans);

    // add an implicitely added transaction
    event nextTransGen(1,e.minedTime+expSampleVal(peersList[senderId].transactionLambda),e.minedTime,senderId,senderId,block(),transaction());
    schedulerQ.push(nextTransGen);

    // propagate the transation to neighbours
    for(int i=0;i< peersList[senderId].neighbours.size();i++)
    {
        int recieverId=peersList[senderId].neighbours[i];
        double latencyValue= computeLatency(senderId,recieverId,messageSize);
        event recEvent(2,e.minedTime+latencyValue,e.minedTime,senderId,recieverId,block(),newtrans);
        schedulerQ.push(recEvent);
    }

}

void recvTransact(event e)
{
    // propagate the transaction to neighbours except if already received
    bool transactionAlreadyRecieved=false;
    for (auto curreTransaction:peersList[e.currPeer].allTransactions )
    {
        if(curreTransaction.transactionId==e.currTransaction.transactionId)
        {
            transactionAlreadyRecieved=true;
            break;
        }
    }

    if(!transactionAlreadyRecieved)
    {
        peersList[e.currPeer].allTransactions.insert(e.currTransaction);
        int senderId=e.currPeer;
        for(int i=0;i<peersList[senderId].neighbours.size();i++)
        {
            int destinId=peersList[senderId].neighbours[i];
            double latencyValue=computeLatency(senderId,destinId,messageSize);
            event recEvent(2,e.minedTime+latencyValue,e.minedTime,senderId,destinId,block(),e.currTransaction);
            schedulerQ.push(recEvent);
        }

    }

}
void genBlockEvent(event e)
{
    // check if a block is scheduled before the given block 
    bool receivedBlocksInMiddle=false;
    for( auto it = peersList[e.currPeer].block2Time.begin();
         it != peersList[e.currPeer].block2Time.end(); ++it )
    {
        double timeR = it->second;
        if(timeR<e.minedTime && timeR>e.createTime)
        {
            receivedBlocksInMiddle=true;
            break;
        }
    }

    // generate a new block and add to the longest chain

    if(!receivedBlocksInMiddle)
    {
        if(peersList[e.currPeer].node_type==1 && forking == 0)
        {
            int longestLen=0;
            int indexLongest;
            int corrBlockID;
            
            if(peersList[e.currPeer].private_blocks.size()==0)
            {

                for(int i=0;i<peersList[e.currPeer].blocks.size();i++)
                {
                    if(peersList[e.currPeer].blocks[i].posBlockchain>longestLen)
                    {
                        longestLen=peersList[e.currPeer].blocks[i].posBlockchain;
                        indexLongest=i;
                        corrBlockID=peersList[e.currPeer].blocks[i].blockId;
                    }
                }

                double timestamp=double(INT_MAX);
                for(int i=0;i<peersList[e.currPeer].blocks.size();i++)
                {
                    if(peersList[e.currPeer].blocks[i].posBlockchain==longestLen)
                    {
                        //when first is found
                        if(timestamp> totalTime)
                        {
                            timestamp = peersList[e.currPeer].blocks[i].creationTime;
                            indexLongest=i;
                            corrBlockID=peersList[e.currPeer].blocks[i].blockId;
                        }
                            //Find the earliest one
                        else{
                            if(timestamp>peersList[e.currPeer].blocks[i].creationTime){
                                timestamp = peersList[e.currPeer].blocks[i].creationTime;
                                indexLongest=i;
                                corrBlockID=peersList[e.currPeer].blocks[i].blockId;
                            }
                        }
                    }
                }
                peersList[e.currPeer].head_len = longestLen;
                peersList[e.currPeer].head_id = corrBlockID;
                peersList[e.currPeer].head_index = indexLongest;
            }
            else
            {
                longestLen = peersList[e.currPeer].head_len+ peersList[e.currPeer].private_blocks.size();
                corrBlockID = peersList[e.currPeer].private_blocks[peersList[e.currPeer].private_blocks.size()-1].blockId;   
                indexLongest = peersList[e.currPeer].head_index + peersList[e.currPeer].private_blocks.size();

            }
            block generatedBlock(blockIdCounter,e.minedTime,corrBlockID,e.currPeer,longestLen+1);
            blockIdCounter+=1;
            generatedBlock.previousBlockIndex=indexLongest;

            peersList[e.currPeer].allTransactions.insert(transaction(transactionIdCounter,-1,e.currPeer,50));
            transactionIdCounter+=1;

            //add all unspent transactions to this block
            set<transaction> unspentTransactions=peersList[e.currPeer].allTransactions;
            int currBlockIndex=indexLongest;
            while(peersList[e.currPeer].blocks[currBlockIndex].previousBlockID!=(-1))
            {
                for(auto temTrans:peersList[e.currPeer].blocks[currBlockIndex].transactionSet)
                {
                    unspentTransactions.erase(temTrans);
                }
                currBlockIndex=peersList[e.currPeer].blocks[currBlockIndex].previousBlockIndex;
            }

            vector<double> coinValuesAtHost(numPeers,0);
            for(int i1=0;i1<numPeers;i1++)
            {
                coinValuesAtHost[i1]=computeCoins(e.currPeer,i1);
            }
            // iterate through all utxo transactions

            for(auto txn:unspentTransactions)
            {
                if(txn.senderID==-1)
                {
                    coinValuesAtHost[txn.receiverID]+=txn.coins;
                    generatedBlock.peer2Amount[txn.receiverID]+=txn.coins;

                    generatedBlock.transactionSet.insert(txn);

                    // stop when the block size limit reached
                    if (generatedBlock.transactionSet.size() >= 1024)
                        break;
                }     
                else
                {
                    if( txn.coins>coinValuesAtHost[txn.senderID])
                    {
                        unspentTransactions.erase(txn);
                    }
                    else
                    {
                        coinValuesAtHost[txn.senderID]-=txn.coins;
                        coinValuesAtHost[txn.receiverID]+=txn.coins;
                        generatedBlock.peer2Amount[txn.senderID]-=txn.coins;
                        generatedBlock.peer2Amount[txn.receiverID]+=txn.coins;

                        generatedBlock.transactionSet.insert(txn);
                        if (generatedBlock.transactionSet.size() >= 1024)
                            break;
                    }
                }
            }

            peersList[e.currPeer].private_blocks.push_back(generatedBlock);
            malicious_count++;
            lead++;
            return;
        }

        int longestLen=0;
        int indexLongest;
        int corrBlockID;

        for(int i=0;i<peersList[e.currPeer].blocks.size();i++)
        {
            if(peersList[e.currPeer].blocks[i].posBlockchain>longestLen)
            {
                longestLen=peersList[e.currPeer].blocks[i].posBlockchain;
                indexLongest=i;
                corrBlockID=peersList[e.currPeer].blocks[i].blockId;
            }
        }

        double timestamp=double(INT_MAX);
        for(int i=0;i<peersList[e.currPeer].blocks.size();i++)
        {
            if(peersList[e.currPeer].blocks[i].posBlockchain==longestLen)
            {
                //when first is found
                if(timestamp> totalTime)
                {
                    timestamp = peersList[e.currPeer].blocks[i].creationTime;
                    indexLongest=i;
                    corrBlockID=peersList[e.currPeer].blocks[i].blockId;
                }
                    //Find the earliest one
                else{
                    if(timestamp>peersList[e.currPeer].blocks[i].creationTime){
                        timestamp = peersList[e.currPeer].blocks[i].creationTime;
                        indexLongest=i;
                        corrBlockID=peersList[e.currPeer].blocks[i].blockId;
                    }
                }
            }
        }


        //create a new block that refers to this block
        block generatedBlock(blockIdCounter,e.minedTime,corrBlockID,e.currPeer,longestLen+1);



        block_display(blockIdCounter);
        block_display(e.minedTime);

        peersList[e.currPeer].block2Time[blockIdCounter]=e.minedTime;
        blockIdCounter+=1;

        //Add mining fee transaction
        peersList[e.currPeer].allTransactions.insert(transaction(transactionIdCounter,-1,e.currPeer,50));
        transactionIdCounter+=1;

        //add all unspent transactions to this block
        set<transaction> unspentTransactions=peersList[e.currPeer].allTransactions;
        int currBlockIndex=indexLongest;
        while(peersList[e.currPeer].blocks[currBlockIndex].previousBlockID!=(-1))
        {
            for(auto temTrans:peersList[e.currPeer].blocks[currBlockIndex].transactionSet)
            {
                unspentTransactions.erase(temTrans);
            }
            currBlockIndex=peersList[e.currPeer].blocks[currBlockIndex].previousBlockIndex;
        }

        vector<double> coinValuesAtHost(numPeers,0);
        for(int i1=0;i1<numPeers;i1++)
        {
            coinValuesAtHost[i1]=computeCoins(e.currPeer,i1);
        }
        // iterate through all utxo transactions

        for(auto txn:unspentTransactions)
        {
            if(txn.senderID==-1)
            {
                coinValuesAtHost[txn.receiverID]+=txn.coins;
                generatedBlock.peer2Amount[txn.receiverID]+=txn.coins;

                generatedBlock.transactionSet.insert(txn);

                // stop when the block size limit reached
                if (generatedBlock.transactionSet.size() >= 1024)
                    break;
            } 
            
            else
            {
                if( txn.coins>coinValuesAtHost[txn.senderID])
                {
                    unspentTransactions.erase(txn);
                }
                else
                {
                    coinValuesAtHost[txn.senderID]-=txn.coins;
                    coinValuesAtHost[txn.receiverID]+=txn.coins;
                    generatedBlock.peer2Amount[txn.senderID]-=txn.coins;
                    generatedBlock.peer2Amount[txn.receiverID]+=txn.coins;

                    generatedBlock.transactionSet.insert(txn);
                    if (generatedBlock.transactionSet.size() >= 1024)
                        break;
                }
            }
        }

    

        blockWrite<<newline<<endl;
        blockWrite<<"Block Hash : "<<generatedBlock.blockId<<endl;
        blockWrite<<"Created By Peer : "<<generatedBlock.peerID<<endl;
        blockWrite<<"Time of Creation : "<<generatedBlock.creationTime<<endl;
        blockWrite<<"Number of Transactions : "<<generatedBlock.transactionSet.size()<<endl;
        blockWrite<<"Previous Block Hash : "<<generatedBlock.previousBlockID<<endl;
        blockWrite<<newline<<endl;

        //adding additional block to current peer 
        generatedBlock.previousBlockIndex=indexLongest;
        peersList[e.currPeer].blocks.push_back(generatedBlock);

        //broadcasting block to all my peers
        for(int i=0;i<peersList[e.currPeer].neighbours.size();i++)
        {
            int recievePeerId=peersList[e.currPeer].neighbours[i];
            double latencyValue=computeLatency(e.currPeer,recievePeerId,blockSize);
            event recieveEvent(4,e.minedTime+latencyValue,e.minedTime,e.currPeer,recievePeerId,generatedBlock,transaction());
            schedulerQ.push(recieveEvent);
        }

    }

    forking = 0;
}
void recvBlockEvent(event e)
{
    // receive a generated block and send to neighbours if not already received
    bool alreadyRecievedBlock=false;
    for(int i=0;i< peersList[e.currPeer].blocks.size();i++)
    {
        if(peersList[e.currPeer].blocks[i].blockId==e.currBlock.blockId)
        {
            alreadyRecievedBlock=true;
            break;
        }
    }

    if(!alreadyRecievedBlock)
    {
        if(peersList[(e.currPeer)].node_type == 1)
        {

            if (lead == 1) forking = 1;
            if (lead>1) lead--;
            if(lead == 1)
            {
                for(auto generatedBlock : peersList[(e.currPeer)].private_blocks)
                {
                    peersList[e.currPeer].block2Time[generatedBlock.blockId]=e.minedTime;
                    block_display(generatedBlock.blockId);
                    block_display(e.minedTime);

                    blockWrite<<newline<<endl;
                    blockWrite<<"Block Hash : "<<generatedBlock.blockId<<endl;
                    blockWrite<<"Created By Peer : "<<generatedBlock.peerID<<endl;
                    blockWrite<<"Time of Creation : "<<generatedBlock.creationTime<<endl;
                    blockWrite<<"Number of Transactions : "<<generatedBlock.transactionSet.size()<<endl;
                    blockWrite<<"Previous Block Hash : "<<generatedBlock.previousBlockID<<endl;
                    blockWrite<<newline<<endl;

                    peersList[e.currPeer].blocks.push_back(generatedBlock);

                    for(int i=0;i<peersList[e.currPeer].neighbours.size();i++)
                    {
                        int recievePeerId=peersList[e.currPeer].neighbours[i];
                        double latencyValue=computeLatency(e.currPeer,recievePeerId,blockSize);
                        event recieveEvent(4,e.minedTime+latencyValue,e.minedTime,e.currPeer,recievePeerId,generatedBlock,transaction());
                        schedulerQ.push(recieveEvent);
                        
                    }

                }

                peersList[(e.currPeer)].private_blocks.clear();
                lead = 0;      
            }
            else if (lead > 1) {
                auto generatedBlock = peersList[e.currPeer].private_blocks[0];
                peersList[e.currPeer].private_blocks.erase(peersList[e.currPeer].private_blocks.begin());
                peersList[e.currPeer].block2Time[generatedBlock.blockId]=e.minedTime;
                block_display(generatedBlock.blockId);
                block_display(e.minedTime);

                blockWrite<<newline<<endl;
                blockWrite<<"Block Hash : "<<generatedBlock.blockId<<endl;
                blockWrite<<"Created By Peer : "<<generatedBlock.peerID<<endl;
                blockWrite<<"Time of Creation : "<<generatedBlock.creationTime<<endl;
                blockWrite<<"Number of Transactions : "<<generatedBlock.transactionSet.size()<<endl;
                blockWrite<<"Previous Block Hash : "<<generatedBlock.previousBlockID<<endl;
                blockWrite<<newline<<endl;

                peersList[e.currPeer].blocks.push_back(generatedBlock);

                for(int i=0;i<peersList[e.currPeer].neighbours.size();i++)
                {
                    int recievePeerId=peersList[e.currPeer].neighbours[i];
                    double latencyValue=computeLatency(e.currPeer,recievePeerId,blockSize);
                    event recieveEvent(4,e.minedTime+latencyValue,e.minedTime,e.currPeer,recievePeerId,generatedBlock,transaction());
                    schedulerQ.push(recieveEvent);
                    
                }
            }
        }
        double t_K=expSampleVal( peersList[e.currPeer].blockGenLambda );

        //Triggering Block Generation Event
        //Here t_k refers to T_K given in ps
        event genBlockT(3,e.minedTime+t_K,e.minedTime,e.currPeer,e.currPeer,block(),transaction());
        schedulerQ.push(genBlockT);

        //find the len and blockk id of the Peer which is being referred here
        int longestLen=0;
        int indexLongest;
        int corrBlockID;
        for(int i=0;i<peersList[e.currPeer].blocks.size();i++)
        {
            if(peersList[e.currPeer].blocks[i].blockId==e.currBlock.previousBlockID)
            {
                longestLen=peersList[e.currPeer].blocks[i].posBlockchain;
                indexLongest=i;
                corrBlockID=peersList[e.currPeer].blocks[i].blockId;
                break;
            }
        }
        e.currBlock.posBlockchain=longestLen+1;
        peersList[e.currPeer].block2Time[e.currBlock.blockId]=e.minedTime;
        e.currBlock.previousBlockIndex=indexLongest;
        peersList[e.currPeer].blocks.push_back(e.currBlock);

        if (peersList[e.currPeer].node_type != 1) {
            for(int i=0;i<peersList[e.currPeer].neighbours.size();i++)
            {
                int destinPeer=peersList[e.currPeer].neighbours[i];
                if(destinPeer!=e.senderPeer)
                {
                    double latencyValue= computeLatency(e.currPeer,destinPeer,blockSize);
                    event recBevent(4,e.minedTime+latencyValue,e.minedTime,e.currPeer,destinPeer,e.currBlock,transaction());
                    schedulerQ.push(recBevent);

                }
            }
        }
        

    }

}

/////////////////////////////////////////////////////////////////Print functions/////////////////////////////////////////////
double theoretical_ratio(double a, double gam)
{

    double Rpool = (a*(1-a)*(1-a)*(4*a + gam*(1 - 2*a)) - a*a*a)/(1 - a*(1 + (2-a)*a));
    return Rpool;

}

void printRatio()
{
    int longestLen=0;
    int indexLongest;
    int corrBlockID;

    for(int i=0;i<peersList[1].blocks.size();i++)
    {
        if(peersList[1].blocks[i].posBlockchain>longestLen){
            longestLen=peersList[1].blocks[i].posBlockchain;
            indexLongest=i;
            corrBlockID=peersList[1].blocks[i].blockId;
        }
    }

    // find the latest block with the longest length
    double timestamp=double(INT_MAX);
    for(int i=0;i<peersList[1].blocks.size();i++)
    {
        if(peersList[1].blocks[i].posBlockchain==longestLen){
            //when first is found
            if(timestamp> totalTime){
                timestamp = peersList[1].blocks[i].creationTime;
                indexLongest=i;
                corrBlockID=peersList[1].blocks[i].blockId;
            }
                //when one block is already found, find next
            else{
                if(timestamp>peersList[1].blocks[i].creationTime)
                {
                    timestamp = peersList[1].blocks[i].creationTime;
                    indexLongest=i;
                    corrBlockID=peersList[1].blocks[i].blockId;
                }
            }
        }
    }
    int total_adversary = malicious_count;
    int total_blocks = blockIdCounter;
    int main_blocks = longestLen;
    int main_adversary = 0;
    cout<<"while start\n";

    while(indexLongest!=(-1))
    {
        int k = peersList[1].blocks[indexLongest].peerID;
        if (k >= 0) {
            main_adversary+= peersList[k].node_type;
            // cout<<main_adversary<<" "<<peersList[1].blocks[indexLongest].peerID<<"\n";
            indexLongest=peersList[1].blocks[indexLongest].previousBlockIndex;
        }
        else {
            break;
        } 
    }
    cout<<"while end\n";

    double mpu_adv = (1.0*main_adversary)/total_adversary;
    double mpu_overall =  (1.0*main_blocks)/total_blocks; 
    double eff_alpha = (1.0*main_adversary)/main_blocks;
    double max_alpha = (1.0 * (main_adversary + peersList[numPeers-1].private_blocks.size()))/(main_blocks + peersList[numPeers-1].private_blocks.size());
    double Rpool0 = theoretical_ratio(alpha, 0.0);
    double Rpool1 = theoretical_ratio(alpha, 1.0);

    ofstream outfile;
    outfile.open("MPU.txt");

    outfile<<"MPU_ADVERSARY:                    "<<mpu_adv<<"\n";
    outfile<<"MPU_OVERALL:                      "<<mpu_overall<<"\n"; 
    outfile<<"TOTAL ADVERSARY:                  "<<total_adversary<<"\n"; 
    outfile<<"MAIN ADVERSARY:                   "<<main_adversary<<"\n";
    outfile<<"EFFECTIVE ALPHA:                  "<<eff_alpha<<"\n";
    outfile<<"PRIVATE SIZE:                     "<<peersList[numPeers-1].private_blocks.size()<<"\n";
    outfile<<"UPPER BOUND ON ALPHA:             "<<max_alpha<<"\n";
    outfile<<"THEORETICAL RATIO AT GAMMA = 1:   "<<Rpool1<<"\n";
    outfile<<"THEORETICAL RATIO AT GAMMA = 0:   "<<Rpool0<<"\n";

    outfile.close();

    return;

}

void printTree()
{
    for(int i=0;i<numPeers;i++)
    {
        ofstream outfile;
        outfile.open ("Blockchain_"+to_string(i)+".txt");
        outfile<<"Peer#"<<i<<"\n";
        outfile<<newline<<"\n";
        outfile<<newline<<"\n";
        for(int k=0; k<peersList[i].blocks.size();k++)
        {
            outfile<<"Block#"<<k<<"\n";
            outfile<<newline<<"\n";
            outfile<<"Block ID:             "<<peersList[i].blocks[k].blockId<<"\n";
            outfile<<"Created by peer:      "<<peersList[i].blocks[k].peerID<<"\n";
            outfile<<"Length in BlockChain: "<<peersList[i].blocks[k].posBlockchain<<"\n";
            outfile<<"Previous Block:       "<<peersList[i].blocks[k].previousBlockID<<"\n";
            outfile<<"Created at:           "<<peersList[i].blocks[k].creationTime<<"\n";
            outfile<<"Received at:           "<<peersList[i].block2Time[peersList[i].blocks[k].blockId]<<"\n";
            for(auto txn:peersList[i].blocks[k].transactionSet)
            {
                outfile<<"\tTxnID "<<txn.transactionId<<": "<<txn.senderID<<" pays "<<txn.receiverID<<" "<<txn.coins<<" coins"<<"\n";
            }
        }
        outfile.close();
    }
}

void printUTXO()
{
    for(int abc=0;abc<numPeers;abc++)
    {

        int indexLongest=getIndexLongest(abc);


        set<transaction> unspentTransactions=peersList[abc].allTransactions;
        int currBlockIndex=indexLongest;
        while(peersList[abc].blocks[currBlockIndex].previousBlockID!=(-1))
        {
            for(auto temTrans:peersList[abc].blocks[currBlockIndex].transactionSet)
            {
                unspentTransactions.erase(temTrans);
            }
            currBlockIndex=peersList[abc].blocks[currBlockIndex].previousBlockIndex;
        }

        ofstream outfile;
        outfile.open ("UTXO_"+to_string(abc)+".txt");
        outfile<<"Peer#"<<abc<<"\n";
        outfile<<newline<<"\n";
        for(auto txnSet:unspentTransactions)
        {
            outfile<<"\tTxnID "<<txnSet.transactionId<<": "<<txnSet.senderID<<" pays "<<txnSet.receiverID<<" "<<txnSet.coins<<" coins"<<"\n";
        }
        outfile.close();
    }
}

void printEndMoney()
{
    endMoneyWrite.open("FinalCoins.txt");
    float total_blocks = peersList[1].blocks.size();
    vector<float> num_each(numPeers);

    for(int i=0;i<numPeers;i++)
    {
        endMoneyWrite<<"Final amount with Peer "<<i<<": "<<computeCoins(i,i)<<endl;
        endMoneyWrite<<"Number of Blocks Generated:   "<<peersList[i].blocks.size()<<"\n";
        num_each[i] = 0;

    }

    endMoneyWrite<<"Total number of Blocks generated:"<<total_blocks<<"\n";

    for(int i=0;i<peersList[1].blocks.size();i++)
    {
        num_each[peersList[1].blocks[i].peerID+1]++;
    }

    for(int i=0;i<numPeers;i++)
    {

        endMoneyWrite<<"Ratio for "<<i<<"    :"<<(float)num_each[i]/total_blocks<<"\n";
    }

    endMoneyWrite.close();

}

void printHonestBlockchain()
{   
    ofstream outfile;
    outfile.open ("blockChainOn"+to_string(1)+".py");
    outfile<<"import networkx as nx"<<endl;
    outfile<<"import matplotlib.pyplot as plt"<<endl;
    outfile<<"fig, ax = plt.subplots(figsize=(20,20))"<<endl;
    outfile<<"G=nx.Graph()"<<endl;
    for(int j=0;j<peersList[1].blocks.size();j++)
    {
    if(peersList[1].blocks[j].previousBlockIndex!=(-1))
    {
        if (peersList[numPeers-1].blocks[j].peerID == (numPeers-1)){
            outfile<<"G.add_edge("<<peersList[numPeers-1].blocks[j].blockId<<","<<peersList[numPeers-1].blocks[j].previousBlockIndex<<" ,color='r',weight=2)"<<endl;
        }
        else{
            outfile<<"G.add_edge("<<peersList[numPeers-1].blocks[j].blockId<<","<<peersList[numPeers-1].blocks[j].previousBlockIndex<<",color='k',weight=1)"<<endl;
        }
    }
    }
    outfile<<"colors = nx.get_edge_attributes(G,'color').values() \nweights = nx.get_edge_attributes(G,'weight').values()"<<endl;

    outfile<<"nx.draw(G,with_labels = True, arrows=True, pos=nx.spring_layout(G), edge_color=colors, width=list(weights))"<<endl;
    outfile<<"import matplotlib.pyplot as plt"<<endl;
    string figureName="blockchaingraph"+to_string(1);
    outfile<<"plt.savefig('"<<figureName<<".svg', format='svg', dpi=1200)";
    outfile.close();
}

void printMaliciousBlockchain()
{   
    ofstream outfile;
    outfile.open ("blockChainOn"+to_string(numPeers-1)+".py");
    outfile<<"import networkx as nx"<<endl;
    outfile<<"import matplotlib.pyplot as plt"<<endl;
    outfile<<"fig, ax = plt.subplots(figsize=(20,20))"<<endl;
    outfile<<"G=nx.Graph()"<<endl;
    for(int j=0;j<peersList[numPeers-1].blocks.size();j++)
    {
    if(peersList[numPeers-1].blocks[j].previousBlockIndex!=(-1))
    {
        if (peersList[numPeers-1].blocks[j].peerID == (numPeers-1)){
            // cout<<"aaaaaaaaaaa\n";
            outfile<<"G.add_edge("<<peersList[numPeers-1].blocks[j].blockId<<","<<peersList[numPeers-1].blocks[j].previousBlockIndex<<" ,color='r',weight=2)"<<endl;
        }
        else{
            // cout<<peersList[numPeers-1].blocks[j].peerID<<"\n";
            outfile<<"G.add_edge("<<peersList[numPeers-1].blocks[j].blockId<<","<<peersList[numPeers-1].blocks[j].previousBlockIndex<<",color='k',weight=1)"<<endl;
        }
        
    }
    }
    for(int j=0;j<peersList[numPeers-1].private_blocks.size();j++)
    {
    if(peersList[numPeers-1].private_blocks[j].previousBlockIndex!=(-1))
    {
        outfile<<"G.add_edge("<<peersList[numPeers-1].private_blocks[j].blockId<<","<<peersList[numPeers-1].private_blocks[j].previousBlockIndex<<" ,color='b',weight=5)"<<endl;
    }
    }
    outfile<<"colors = nx.get_edge_attributes(G,'color').values() \nweights = nx.get_edge_attributes(G,'weight').values()"<<endl;

    outfile<<"nx.draw(G,with_labels = True, arrows=True, pos=nx.spring_layout(G), edge_color=colors, width=list(weights))"<<endl;
    outfile<<"import matplotlib.pyplot as plt"<<endl;
    string figureName="blockchaingraph"+to_string(numPeers-1);
    outfile<<"plt.savefig('"<<figureName<<".svg', format='svg', dpi=1200)";
    outfile.close();
}

void printAll() {
    printRatio();
    cout<<"ratio done"<<endl;
    printTree();
    cout<<"tree done"<<endl;
    // printUTXO();
    // cout<<"utxo done\n";
    printEndMoney();
    cout<<"money done"<<endl;
    printHonestBlockchain();
    printMaliciousBlockchain();
    
}

