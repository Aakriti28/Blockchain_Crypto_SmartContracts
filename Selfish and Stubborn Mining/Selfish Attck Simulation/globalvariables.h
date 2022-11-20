#ifndef __GLOBALVARIABLES_H__
#define __GLOBALVARIABLES_H__

#include <bits/stdc++.h>
using namespace std;

#define block_display(x) do { std::cerr << #x << ": " << x << std::endl; } while (0)

// ofstream file descriptors
ofstream blockWrite, startMoneyWrite, endMoneyWrite;

string newline = "=====================================================";

double totalTime;
double blockGenLambda, transactionGenLambda;

double zeta = 0.5;
int lead =0;
double alpha = 0.4;
bool forking = 0;

int malicious_count = 0;

int numPeers;							// Number of peers in the network
double z;									// Percentage of nodes that are slow
double probConnect	= 0.5;	// Probability that the two given peers are connected
double initialMaxAmount;					// Maximum initial amount

vector<vector<double>> propagationDelay;			// Propagation delay between two nodes i and j

int transactionIdCounter = 0, blockIdCounter = 1;	// Genesis block is the first block created

int messageSize = 1e3, blockSize = 8e6;					// Transaction is of size 1 KB, and Block is of size 1 MB

#endif