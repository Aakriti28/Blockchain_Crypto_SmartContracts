#include<bits/stdc++.h>
#include "globalvariables.h"
using namespace std;


/////////////////////////////////////////////////transaction data structure///////////////////////////////////////////
struct transaction
{
    int transactionId;
    int senderID;
    int receiverID;
    double coins;

    transaction(){}

    transaction(int _transactionID,int _senderID,int _receiverID,double _coins)
    {
        transactionId=_transactionID;
        senderID=_senderID;
        receiverID=_receiverID;
        coins=_coins;
    }

    bool operator==(const transaction& txn) const
    {
        return transactionId== txn.transactionId;
    }

    bool operator<(const transaction& txn) const
    {
        return transactionId < txn.transactionId;
    }

};
/////////////////////////////////////////////////block data structure///////////////////////////////////////////
struct block
{
    int blockId;
    double creationTime;
    int previousBlockID;
    int previousBlockIndex;
    int peerID;
    int posBlockchain;
    set<transaction> transactionSet;
    map<int,double> peer2Amount;
    block()
    {

    }

    block(int _blockID, double _creationTime,int _previousBlock, int _peerID, int _posBlockchain)
    {
        blockId=_blockID;
        creationTime=_creationTime;
        previousBlockID=_previousBlock;
        peerID=_peerID;
        posBlockchain=_posBlockchain;
        for(int i=0;i<numPeers;i++)
        {
            peer2Amount[i]=0;
        }
    }

};

/////////////////////////////////////////////////peer data structure////////////////////////////////////////////////

struct peer
{
    int peerId;
    vector<int> neighbours;
    bool peerType;
    double blockGenLambda;
    double transactionLambda;
    int node_type;
    map<int,double> block2Time;
    vector<block>blocks;
    vector<block>private_blocks;
    int head_len;
    int head_id;
    int head_index;
    set<transaction> allTransactions;
};

/////////////////////////////////////////////////event data structure////////////////////////////////////////////////////
struct event
{
    int eventID;
    double minedTime;
    double createTime;
    int senderPeer;
    int currPeer;
    block currBlock;
    transaction currTransaction;
    event(int _eventID,double _minedTime,double _createTime,int _senderPeer,int _currPeer,block _currBlock,transaction _currTransaction)
    {
        eventID=_eventID;
        minedTime=_minedTime;
        createTime=_createTime;
        senderPeer=_senderPeer;
        currPeer=_currPeer;
        currBlock=_currBlock;
        currTransaction=_currTransaction;
    }

    bool operator<(const event& eve) const
    {
        //so that we get earliest scheduled at the top
        return minedTime > eve.minedTime;
    }


};

priority_queue<event> schedulerQ;
vector<peer> peersList;

