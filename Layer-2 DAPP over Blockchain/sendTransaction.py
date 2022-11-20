import sys
import time
import pprint

from web3 import *
from solc import compile_source
import os




def compile_source_file(file_path):
   with open(file_path, 'r') as f:
      source = f.read()
   return compile_source(source)



def sendEmptyLoopTransaction(address):
       
    contract_source_path = os.environ['HOME']+'/HW3/emptyLoop.sol'
    compiled_sol = compile_source_file(contract_source_path)

    contract_id, contract_interface = compiled_sol.popitem()

    sort_contract = w3.eth.contract(
    address=address,
    abi=contract_interface['abi'])
    tx_hash = sort_contract.functions.runLoop().transact({'txType':"0x3", 'from':w3.eth.accounts[0], 'gas':2409638})
    return tx_hash


print("Starting Transaction Submission")
w3 = Web3(IPCProvider(os.environ['HOME']+'/HW3/test-eth1/geth.ipc', timeout=100000))


i=0

while i < 1:
    with open(os.environ['HOME']+'/HW3/contractAddressList') as fp:
        for line in fp:
            #print(line)
            a,b = line.rstrip().split(':', 1)
            if a=="empty":  
                tx_hash3 = sendEmptyLoopTransaction(b) 
            time.sleep(0.01)

    print("inside loop")

    i=i+1

time.sleep(50)

w3.miner.start(1)


receipt3 = w3.eth.getTransactionReceipt(tx_hash3)

while ((receipt3 is None)) : 
    time.sleep(1)

    receipt3 = w3.eth.getTransactionReceipt(tx_hash3)
    # break


receipt3 = w3.eth.getTransactionReceipt(tx_hash3)

if receipt3 is not None:
    print("empty:{0}".format(receipt3['gasUsed']))

w3.miner.stop()
