import time
from networkx.algorithms.components.connected import is_connected
import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
from web3 import *
from solc import compile_source

NUM_NODES = 100
#Number of nodes in graph

#This makes a valid etherum network
def connectWeb3():
    return Web3(IPCProvider('test-eth1/geth.ipc', timeout = 100000))

def compile_source_file(file_path):
   with open(file_path, 'r') as f:
      source = f.read()
   return compile_source(source, optimize = True)

#This deploys contract (Similar to one given in demo code)
def deployEmptyContract(contract_source_path, w3, account):
    compiled_sol = compile_source_file(contract_source_path)
    contract_id, contract_interface3 = compiled_sol.popitem()
    txn_hash = w3.eth.contract(
            abi=contract_interface3['abi'],
            bytecode=contract_interface3['bin']).constructor(4).transact({'txType':"0x0", 'from':account, 'gas':1000000000})
    return txn_hash, contract_interface3

#This calls above function to deploy our contract
def deployContracts(empty_source_path, w3, account):
    tx_hash3, contract_interface3 = deployEmptyContract(empty_source_path, w3, account)

    
    receipt3 = w3.eth.getTransactionReceipt(tx_hash3)

    while ((receipt3 is None)) :
        time.sleep(1)
        receipt3 = w3.eth.getTransactionReceipt(tx_hash3)

    return contract_interface3, receipt3['contractAddress']

#Below provides the interface to call the dapp functions   
class UtilityUI:

    def __init__(self, utility_source_path):
        self.num_txn = 1
        self.num_thread = 4
        self.w3 = connectWeb3()
        self.br = "---------------------------------------------------------------"

        if not self.w3.isConnected():
            print("Connection Failed. Try again!")
        else:
            print("Connection Successful.")
        
        # sleeping for 3 seconds
        time.sleep(3)

        self.w3.miner.start(self.num_thread)
        self.acc = self.w3.eth.accounts[0]
        
        print(self.br)
        print("The account is: ", self.acc)
        print(self.br)

        self.w3.default_account = self.acc
        self.contract_interface3, self.allAcc = deployContracts(utility_source_path, self.w3, self.acc)
        self.contract = self.w3.eth.contract(address=self.allAcc, abi=self.contract_interface3['abi'])
        self.transactions = []

    #This function tracks the output of event emmission from dapp.sol file
    def getLogStatus(self, txn_hash):
        # sleeping for 0.01 seconds
        time.sleep(0.01)

        rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

        while rcpt is None:
            time.sleep(0.5)
            rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

        rcpt = self.w3.eth.getTransactionReceipt(txn_hash)
        logs = self.contract.events.MyEvent().processReceipt(rcpt)
        len_logs = len(logs)
        if ((len_logs <= 0) or ('args' not in logs[0]) or ('status' not in logs[0]['args'])):
            return False
        
        return logs[0]['args']['status']
    #Registering user by calling registerUser of dapp
    def registerUser(self, use_id, user_name):
        txn_hash = self.contract.functions.registerUser(use_id, user_name).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
        self.num_txn += 1
        return self.getLogStatus(txn_hash)
    #Creating Account by calling creatAcc of dapp
    def createAcc(self, user_id_1, user_id_2, balance):
        txn_hash = self.contract.functions.createAcc(user_id_1, user_id_2, balance).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
        self.num_txn += 1
        return self.getLogStatus(txn_hash)
    #Closing Account by calling closeAccount of dapp    
    def closeAccount(self, user_id_1, user_id_2):
        txn_hash = self.contract.functions.closeAccount(user_id_1, user_id_2).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
        self.num_txn += 1
        return self.getLogStatus(txn_hash)
    #Transfering amount by calling sendAmount of dapp
    def sendAmount(self, user_id_1, user_id_2):
        txn_hash = self.contract.functions.sendAmount(user_id_1, user_id_2).transact({'txType':"0x3", 'from':self.acc, 'gas':1000000000000})
        self.num_txn += 1
        status = self.getLogStatus(txn_hash)
        if status:
            print("Transfer Successful.")
        else:
            print("Tranfer Failed. Try again!")
        return status
    #Erasing txns at the end
    def eraseTransactions(self):
        for txn in self.transactions:
            rcpt = None
            while rcpt == None:
                try:
                    rcpt = self.w3.eth.getTransactionReceipt(txn)
                    if rcpt == None:
                        time.sleep(1)
                except Exception as e:
                    print(e)
                    time.sleep(1)
    #Loop to register required number of users
    def registerAllUsers(self, user_id_arr, user_name_arr):
        #registerStatus denotes one-hot representation of registered users
        registerStatus = np.zeros(NUM_NODES)
        txns = []

        for i in range(NUM_NODES):
            txn_hash = self.contract.functions.registerUser(user_id_arr[i], user_name_arr[i]).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
            self.num_txn += 1
            txns.append(txn_hash)

        for i in range(NUM_NODES):

            if np.sum(registerStatus) == NUM_NODES:
                break

            elif registerStatus[i]:
                continue

            else:
                while not registerStatus[i]:
                    txn_hash = txns[i]
                    rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

                    while rcpt is None:
                        time.sleep(0.5)
                        rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

                    rcpt = self.w3.eth.getTransactionReceipt(txn_hash)
                    logs = self.contract.events.MyEvent().processReceipt(rcpt)
                    len_logs = len(logs)
                    if ((len_logs <= 0) or ('args' not in logs[0]) or ('status' not in logs[0]['args'])):
                        txn_hash = self.contract.functions.registerUser(user_id_arr[i], user_name_arr[i]).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
                        self.num_txn += 1
                        txns[i] = txn_hash
                        print(f"Trying Again for User {user_id_arr[i]}...")
                        continue
                    
                    status = logs[0]['args']['status']
                    if status:
                        print(f"User {user_id_arr[i]} registered successfully!")
                        registerStatus[i] = 1
                    else:
                        txn_hash = self.contract.functions.registerUser(user_id_arr[i], user_name_arr[i]).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
                        self.num_txn += 1
                        txns[i] = txn_hash
                        print(f"Trying Again for User {user_id_arr[i]}...")
                    
        return registerStatus
    #generating value from exponential distribution
    def sampleExp(self):
        result = int(np.random.exponential(10))
        if result/2 == 0:
            result = 2
        return result
    #Creating account between users which have edges between them.
    def makeConnectedGraph(self, graph):
        registerStatus = []
        txns = []
        edges = []
        counter = 0
        iter = 0
        
        for i,j in graph.edges:
            e = [i,j]
            edges.append(e)
            balance = self.sampleExp()
            txn_hash = self.contract.functions.createAcc(e[0], e[1], balance).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
            self.num_txn += 1
            txns.append(txn_hash)
            counter += 1
            registerStatus.append(0)
            print(f"Users {e[0]} and {e[1]} connected, balance = {balance}")
        
        print(self.br)
        print(f"Total connections are: {counter}")
        print(self.br)
        
        for i in range(counter):
            if iter == counter:
                break

            elif registerStatus[i]:
                continue

            else:
                while not registerStatus[i]:
                    txn_hash = txns[i]
                    rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

                    while rcpt is None:
                        time.sleep(0.5)
                        rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

                    rcpt = self.w3.eth.getTransactionReceipt(txn_hash)
                    logs = self.contract.events.MyEvent().processReceipt(rcpt)
                    len_logs = len(logs)
                    if ((len_logs <= 0) or ('args' not in logs[0]) or ('status' not in logs[0]['args'])):
                        balance = self.sampleExp()
                        txn_hash = self.contract.functions.createAcc(edges[i][0], edges[i][1], balance).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
                        self.num_txn += 1
                        txns[i] = txn_hash
                        print(f"Trying Again to connect {edges[i][0]} and {edges[i][1]}...")
                        continue
                    
                    status = logs[0]['args']['status']
                    if status:
                        print(f"Users {edges[i][0]} and {edges[i][1]} connected successfully!")
                        registerStatus[i] = 1
                        iter += 1
                    else:
                        balance = self.sampleExp()
                        txn_hash = self.contract.functions.createAcc(edges[i][0], edges[i][1], balance).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
                        self.num_txn += 1
                        txns[i] = txn_hash
                        print(f"Trying Again to connect {edges[i][0]} and {edges[i][1]}...")

        print(self.br)
        print(f"Total connections are: {counter}")
        print(self.br)

        return registerStatus
    #will be used for clearing generated network at the end.
    def clearGraph(self, graph):
        registerStatus = []
        txns = []
        edges = []
        iter = 0
        counter = 0
        
        for i, j in graph.edges:
            e = [i,j]
            edges.append(e)
            txn_hash = self.contract.functions.closeAccount(e[0], e[1]).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
            self.num_txn += 1
            txns.append(txn_hash)
            counter += 1
            registerStatus.append(0)
            print(f"Users {e[0]} and {e[1]} no longer connected")

        for i in range(counter):
            if iter == counter:
                break

            elif registerStatus[i]:
                continue

            else:
                while not registerStatus[i]:
                    txn_hash = txns[i]
                    rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

                    while rcpt is None:
                        time.sleep(0.5)
                        rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

                    rcpt = self.w3.eth.getTransactionReceipt(txn_hash)
                    logs = self.contract.events.MyEvent().processReceipt(rcpt)
                    len_logs = len(logs)
                    if ((len_logs <= 0) or ('args' not in logs[0]) or ('status' not in logs[0]['args'])):
                        txn_hash = self.contract.functions.closeAccount(edges[i][0], edges[i][1]).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
                        self.num_txn += 1
                        txns[i] = txn_hash
                        print(f"Trying Again to disconnect {edges[i][0]} and {edges[i][1]}...")
                        continue
                    
                    status = logs[0]['args']['status']
                    if status:
                        print(f"Users {edges[i][0]} and {edges[i][1]} disconnected successfully!")
                        registerStatus[i] = 1
                        iter += 1
                    else:
                        txn_hash = self.contract.functions.closeAccount(edges[i][0], edges[i][1]).transact({'txType':"0x3", 'from':self.acc, 'gas':3000000})
                        self.num_txn += 1
                        txns[i] = txn_hash
                        print(f"Trying Again to disconnect {edges[i][0]} and {edges[i][1]}...")
                            
        return registerStatus
    #Function used for  transferring 1 coin from sender to receiver
    def startTransactions(self, mapping):
        registerStatus = []
        txns = []
        iter = 0
        counter = 0
        
        for e in mapping:
            print(f"Transferring one coin from {int(x)} to {int(y)}")
            txn_hash = self.contract.functions.sendAmount(e[0], e[1]).transact({'txType' : "0x3", 'from' : self.acc, 'gas' : 300000000})
            self.num_txn += 1
            counter += 1
            txns.append(txn_hash)
            registerStatus.append(False)
        
        for i in range(counter):
            if iter == counter:
                break

            elif registerStatus[i]:
                continue

            else:
                while not registerStatus[i]:
                    txn_hash = txns[i]
                    rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

                    while rcpt is None:
                        time.sleep(0.5)
                        rcpt = self.w3.eth.getTransactionReceipt(txn_hash)

                    rcpt = self.w3.eth.getTransactionReceipt(txn_hash)
                    if ('logs' not in rcpt) or (len(rcpt['logs']) <= 0):
                        continue                    
                    logs = self.contract.events.MyEvent().processReceipt(rcpt)
                    len_logs = len(logs)
                    if ((len_logs <= 0) or ('args' not in logs[0]) or ('status' not in logs[0]['args'])):
                        continue
                    
                    status = logs[0]['args']['status']
                    if status:
                        print(f"Tranferred one coin from {mapping[i][0]} to {mapping[i][1]}")
                    else:
                        print(f"Transaction Failed. Try again!")
                    
                    registerStatus[i] = status
                    iter += 1

        return registerStatus
#generating graph based on power law given number of nodes as input
def genPowerLawGraph(num_nodes):  
    graph = nx.Graph()
    graph.add_nodes_from([0,1])
    graph.add_edge(0, 1)

    degrees = np.zeros(num_nodes)
    degrees[0] = 1
    degrees[1] = 1
    
    alpha = 3.0/2
    print(f"Alpha for Power Law Distribution = {alpha}")
    for i in range(2, num_nodes):
        graph.add_node(i)
        aggregate = (degrees ** alpha) / np.sum(degrees ** alpha)
        edges = np.random.random(num_nodes) <= aggregate
        for j in range(0, i):
            if edges[j] == 1:
                degrees[j] += 1
                graph.add_edge(j, i)
        degrees[i] = np.sum(edges)
        
        if degrees[i] == 0:
            # Ensure that graph is connected
            graph.add_edge(np.random.randint(i), i)
            
    assert(is_connected(graph) == True)
    return graph
######################################################################## MAIN ########################################
source_path = 'dapp.sol'
dapp = UtilityUI(source_path)

user_id_arr = [i for i in range(NUM_NODES)]
user_name_arr = ["user_id_" + str(i) for i in range(NUM_NODES)]
dapp.registerAllUsers(user_id_arr, user_name_arr)

graph = genPowerLawGraph(NUM_NODES)
dapp.makeConnectedGraph(graph)

ratios = []
num_success = 0
epochs = 1000
temp = 100

for i in range(epochs):
    txn = i+1
    x, y = np.random.choice(NUM_NODES, 2, replace = False)
    print(f"{txn} out of {epochs}: From {int(x)} to {int(y)}")
    num_success += dapp.sendAmount(int(x), int(y))
    print(dapp.br)
    if(txn % temp) == 0:
        ratios.append(num_success/txn)
        print(dapp.br)
        print(f"Ratio till {txn} transactions", ratios[-1])
        print(dapp.br)
#dapp.br adds the rquired ---------------------------------------------------- separator
print(dapp.br)
print(dapp.br)
print(dapp.br)
# clearing graph
dapp.clearGraph(graph)

print(dapp.br, "\n\n\n\n")
print(ratios)

X = [(i+1)*temp for i in range(epochs/temp)]
Y = np.array(ratios)
plt.plot(X,Y) #plotting graph between number of transactions and success ratio
plt.xlabel('Number of transactions')
plt.ylabel('Success Rate of transactions')
plt.savefig('plot.png')