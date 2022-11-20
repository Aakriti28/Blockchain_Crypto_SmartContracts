#include <bits/stdc++.h>
#include "structures.h"
#include "functions.h"
#include "globalvariables.h"

using namespace std;

void simulate()
{
    double currTime = 0.0;
    while (currTime < totalTime)
    {
        event e = schedulerQ.top();
        schedulerQ.pop();

        currTime = e.minedTime;

        switch(e.eventID)
        {
            case 1:
                genTransact(e);
                break;
            case 2:
                recvTransact(e);
                break;
            case 3:
                genBlockEvent(e);
                break;
            case 4:
                recvBlockEvent(e);
                break;
        }
    }
}


int main(int argc, char* argv[])
{
    blockWrite.open("blockData.txt");
    totalTime                   = 500;
    numPeers                    = 10;
    z                           = 50;
    probConnect                 = 0.5;

    initialMaxAmount            = 100;
    transactionGenLambda        = 100;
    blockGenLambda              = 0.005;
    zeta                        = 0.5;
    alpha                       = 0.4;
    

    if(argc == 4)
    {
        blockGenLambda              =atof(argv[1]);
        zeta                        =atof(argv[2]);
        alpha                       =atof(argv[3]);                       
    }


    initialiseNetwork();
    initSimulation();
    simulate();
    printAll();

    blockWrite.close();
    return 0;
}