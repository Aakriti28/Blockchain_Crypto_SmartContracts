import networkx as nx
import matplotlib.pyplot as plt
fig, ax = plt.subplots(figsize=(20,20))
G=nx.Graph()
G.add_edge(1,0 ,color='r',weight=2)
G.add_edge(4,0,color='k',weight=1)
G.add_edge(5,0,color='k',weight=1)
G.add_edge(2,0,color='k',weight=1)
G.add_edge(3,0,color='k',weight=1)
G.add_edge(6,1,color='k',weight=1)
G.add_edge(7,6,color='k',weight=1)
G.add_edge(8,7,color='k',weight=1)
G.add_edge(9,8 ,color='r',weight=2)
G.add_edge(10,8,color='k',weight=1)
G.add_edge(11,9 ,color='r',weight=2)
G.add_edge(12,9,color='k',weight=1)
G.add_edge(13,11 ,color='r',weight=2)
G.add_edge(14,11,color='k',weight=1)
G.add_edge(15,13,color='k',weight=1)
G.add_edge(16,15 ,color='r',weight=2)
G.add_edge(17,15,color='k',weight=1)
G.add_edge(18,16 ,color='r',weight=2)
G.add_edge(19,16,color='k',weight=1)
G.add_edge(20,18 ,color='r',weight=2)
G.add_edge(21,18,color='k',weight=1)
G.add_edge(22,20 ,color='r',weight=2)
G.add_edge(23,20,color='k',weight=1)
G.add_edge(24,22,color='k',weight=1)
G.add_edge(25,24 ,color='r',weight=2)
G.add_edge(26,24,color='k',weight=1)
G.add_edge(27,25,color='k',weight=1)
G.add_edge(28,27,color='k',weight=1)
G.add_edge(29,28,color='k',weight=1)
G.add_edge(30,29,color='k',weight=1)
G.add_edge(31,30 ,color='r',weight=2)
G.add_edge(32,30,color='k',weight=1)
colors = nx.get_edge_attributes(G,'color').values() 
weights = nx.get_edge_attributes(G,'weight').values()
nx.draw(G,with_labels = True, arrows=True, pos=nx.spring_layout(G), edge_color=colors, width=list(weights))
import matplotlib.pyplot as plt
plt.savefig('blockchaingraph1.svg', format='svg', dpi=1200)