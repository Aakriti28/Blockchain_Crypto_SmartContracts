# Blockchain Simulator

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