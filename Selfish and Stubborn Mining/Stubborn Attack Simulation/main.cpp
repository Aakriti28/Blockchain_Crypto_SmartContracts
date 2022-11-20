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
    numPeers                    = 20;
    z                           = 50;
    probConnect                 = 0.3;

    initialMaxAmount            = 100;
    transactionGenLambda        = 0.5;
    blockGenLambda              = 0.0025;
    zeta                        = 0.5;
    alpha                       = 0.5;


    cout<<"Usage: <numPeers> <totalTime> <transactionGenLambda> <blockGenLambda> <zeta> <alpha>\n";

    if(argc == 7)
    {
        numPeers                    =atof(argv[1]);
        totalTime                   =atof(argv[2]);
        transactionGenLambda        =atof(argv[3]);
        blockGenLambda              =atof(argv[4]);
        zeta                        =atof(argv[5]);
        alpha                       =atof(argv[6]);                       
    }


    initialiseNetwork();
    cout<<"Initialised network.\n";
    initSimulation();
    cout<<"Simulation started.\n";
    simulate();
    cout<<"Simulation done.\n";
    printAll();
    cout<<"Print money.\n";

    blockWrite.close();
    return 0;
}