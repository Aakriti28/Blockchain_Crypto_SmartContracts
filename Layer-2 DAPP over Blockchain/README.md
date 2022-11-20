# README

1. place test-eth1, password.txt, runEthereumNode.sh, run.py file inside HW3 directory

2. Run the following command and copy the address to the genesis.json in the alloc section that adds the balance to the geth account.
```
geth --datadir $HOME/HW3/test-eth1/ --password $HOME/HW3/password.txt account new
```

3. Run the following command to set up the Ethereum node.

```
sh runEthereumNode.sh
```

4. execute the run.py file from a different terminal window

```
python3 run.py
```