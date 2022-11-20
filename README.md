# Simulation of a Peer to Peer Cryptocurrency Network
Assignments and Projects done during the course - Introduction to Blockchains, Cryptocurrency and Smart Contracts taught by [Prof. Vinay Ribeiro](https://www.cse.iitb.ac.in/~vinayr/).

## Instructions to run the simulator:
Compile the `main.cpp` file using `g++ main.cpp`.
```bash
./a.out <SimulationTime> <NumberOfNodes> <percentSlowNodes> <lambda for transaction> <lambda for block>
```

## Directory Structure
The directory contains the following files:

- `main.cpp` : Contains the main blockchain simulator code. Takes in the following arguments:
1. Simulation time (in seconds)
2. Number of nodes in the network
3. Percentage of slow nodes (in % between 0 to 100)
4. Probability with which two arbitrary nodes are neighbours in the network

- `functions.h`	: Contains the utility functions required for the simulation.

- `globalvariables.h` : Contains the the global variables that are initialised and updated by variaous other functions.

- `structures.h` : Contains the following user-defined structures: node, block, event, transaction and their constructors.

## Simulation Output
1. `blockData.txt` : Information of the generated blocks
2. `BlockChain_*.txt` : Blockchain tree for each node, with other details
3. `initialCoins.txt` : Coins owned by each node at the start of simulation
4. `FinalCoins.txt` : Coins owned by each node at the end of simulation

# Selfish and Stubborn Mining Attacks

## Instructions to run the simulator:
Compile the `main.cpp` file using `g++ main.cpp`.
```bash
./a.out <NumberOfNodes> <SimulationTime> <lambda for transaction> <lambda for block> <zeta> <alpha>
```

## Directory Structure
The directory contains two folders `selfish` and `stubborn`. Each directory has the following files:

- `main.cpp` : Contains the main blockchain simulator code. Takes in the following arguments as stated above.

- `functions.h`	: Contains the utility functions required for the simulation.

- `globalvariables.h` : Contains the the global variables that are initialised and updated by variaous other functions.

- `structures.h` : Contains the following user-defined structures: node, block, event, transaction and their constructors.

- `mg.sh` : A bash script used to make blockchain tree plots.

## Simulation Output
1. `blockData.txt` : Information of the generated blocks
2. `BlockChain_*.txt` : Blockchain tree for each node, with other details
3. `initialCoins.txt` : Coins owned by each node at the start of simulation
4. `FinalCoins.txt` : Coins owned by each node at the end of simulation
5. `MPU.txt` : Contains the required ratios (MPU, effective alpha, etc.) for analysis

# Layer-2 DAPP over Blockchains

1. Run the following command and copy the address to the genesis.json in the alloc section that adds the balance to the geth account.
```
geth --datadir $HOME/HW3/test-eth1/ --password ./password.txt account new
```

2. Run the following command to set up the Ethereum node.

```
sh runEthereumNode.sh
```

3. execute the run.py file from a different terminal window

```
python3 run.py
```

