pragma solidity >=0.4.25;

contract DAPP {

    event MyEvent(bool status);

    // mapping from user id to name, idx respectively
    mapping(uint => string) userName;
    mapping(uint => uint) userIndex;

    uint COUNTER = 0;
    uint[3][][110] neighbours;

    function edgeIndex(uint user_id_1, uint user_id_2) public returns (uint) {
        for(uint i=0; i<neighbours[user_id_1].length; i++) {
            if(neighbours[user_id_1][i][0] == user_id_2){
                return i;
            }
        }
        return uint(neighbours[user_id_1].length);
    }

    function registerUser(uint user_id, string user_name) public returns (bool) {

        if (userIndex[user_id] != 0) {
            emit MyEvent(false);
            return false;
        }

        userName[user_id] = user_name;
        COUNTER++;
        userIndex[user_id] = COUNTER;
        
        emit MyEvent(true);
        return true;
    }

    function createAcc(uint user_id_1, uint user_id_2, uint balance) public returns (bool) {  
        uint s1 = userIndex[user_id_1];
        uint s2 = userIndex[user_id_2];

        bool VALID_ACCOUNT = ((s1 == 0) || (s2 == 0) || (s1 == s2));
        if (VALID_ACCOUNT == true) {
            emit MyEvent(false);
            return false;
        }

        // add edge if edge do not exist
        bool EDGE_EXIST = (edgeIndex(s1, s2) != neighbours[s1].length || edgeIndex(s2, s1) != neighbours[s2].length);
        if (EDGE_EXIST) {
            emit MyEvent(false);
            return false;
        }
        else {
            uint L2 = uint(neighbours[s2].length);
            neighbours[s1].push([s2, balance/2, L2]);
            uint L1 = uint(neighbours[s1].length);
            neighbours[s2].push([s1, balance/2, L1-1]);

            emit MyEvent(true);
            return true;
        }
    }

    function shortestPath(uint s1, uint s2) public returns (uint[3][]) {

        // Each node visited at most once : size of neighbours.length or COUNTER is sufficient
        uint[3][] memory parentArr = new uint[3][](COUNTER + 100);
        if(s1 == s2){
            return parentArr;
        }

        uint[] memory queue = new uint[](COUNTER + 100);
        uint initial_ = 0;
        uint final_ = 1;
        queue[initial_] = s1;
        while(initial_ < final_){
            uint currNode = queue[initial_];
            uint currDist = parentArr[currNode][0] + 1;
            initial_++;
            //neighbours[s2] structure = [node_index, balance, idx of opposite direction edge in neighbours[node_index]]
            for(uint i = 0; i < neighbours[currNode].length; i++){
                uint currNeighbour = neighbours[currNode][i][0];
                
                // If Starting node or self-loop or
                // If balance in this account is 0, cannot push through this edge.
                if((currNeighbour == s1) || (currNeighbour == currNode) || (neighbours[currNode][i][1] == 0) || ((currNeighbour != s1) && (parentArr[currNeighbour][0] != 0) && (parentArr[currNeighbour][0] <= currDist))){
                    continue;
                }
                // currNeighbour not visited (i.e. distance(currNeighbour) = default = 0 & currNeighbour != s1) or (currNeighbour visited but distance(currNeighbour) > distance(currNode) + 1, which is impossible due to the nature of BFS)
                else {
                    if(((parentArr[currNeighbour][0] == 0) && (currNeighbour != s1)) || ((parentArr[currNeighbour][0] != 0) && (parentArr[currNeighbour][0] > currDist))){
                        parentArr[currNeighbour][0] = currDist;
                        parentArr[currNeighbour][1] = currNode;
                        parentArr[currNeighbour][2] = i;
                        queue[final_] = currNeighbour;
                        final_++;

                        // BFS already proceeds in shortest distance first. So, we have got a valid path to s2. Return immediately
                        if(currNeighbour == s2){
                            return parentArr;
                        }
                    }
                }
            }
        }
        return parentArr;
    }

    function sendAmount(uint user_id_1, uint user_id_2) public returns (bool){
        uint s1 = userIndex[user_id_1];
        uint s2 = userIndex[user_id_2];

        bool VALID_ACCOUNT = ((s1 == 0) || (s2 == 0) || (s1 == s2));
        if (VALID_ACCOUNT == true) {
            emit MyEvent(false);
            return false;
        }

        uint[3][] memory parentArr = shortestPath(s1, s2);
        
        // If distance(s2) = 0, then s2 is unreachable. But, the network is connected. This means that s1 does not have sufficient balance. Return false
        if(parentArr[s2][0] == 0 || parentArr[s2][1] == 0){
            emit MyEvent(false);
            return false;
        }

        uint node = parentArr[s2][1];
        uint index = parentArr[s2][2];
        uint numConnectedPeers = neighbours[node][index][2];
            
        
        // Add 1 to receiver and remove 1 from sender for each edge in the path
        while(s2!= s1){
            // Go to parent's edges = neighbours[parentArr[s2][1]]
            // Go to edge corresponding to s2 now = parentArr[s2][2]
            // Subtract 1 from balance
            neighbours[node][index][1]--;
            
            // Go to s2's edges
            // Go to edge corresponding to parent = neighbours[parent][edge idx of s2][2] = neighbours[parentArr[s2][1]][parentArr[s2][2]][2]
            // Increase balance by 1
            neighbours[s2][numConnectedPeers][1]++;
            
            // Go to parent
            s2 = node;
            if(s2 == s1){
                break;
            }
            else{
                node = parentArr[s2][1];
                index = parentArr[s2][2];
                numConnectedPeers = neighbours[node][index][2];
            } 
            
        }

        emit MyEvent(true);
        return true;
    }

    function closeAccount(uint user_id_1, uint user_id_2) public returns (bool) {

        uint s1 = userIndex[user_id_1];
        uint s2 = userIndex[user_id_2];
        
        bool INVALID = ((s1 == 0) || (s2 == 0));
        if (INVALID) {
            emit MyEvent(false);
            return false;
        }
        
        uint idx = edgeIndex(user_id_1, user_id_2);

        // IF Edge does not exist
        bool NOT_EDGE_EXIST = (neighbours[user_id_1].length == 0 || idx == neighbours[user_id_1].length);
        if (NOT_EDGE_EXIST) {
            emit MyEvent(false);
            return false;
        }
        // Edge at the end - just delete last element
        else if (idx == (neighbours[user_id_1].length - 1)) {
            delete neighbours[user_id_1][neighbours[user_id_1].length - 1];
            neighbours[user_id_1].length--;
        }
        // Edge in middle somewhere : swap with last element. Change idx in the opposite direction edge of last edge - swap this edge and last edge and delete
        else {
            neighbours[neighbours[user_id_1][neighbours[user_id_1].length - 1][0]][neighbours[user_id_1][neighbours[user_id_1].length - 1][2]][2] = idx;
            for(uint i=0; i<3; i++) {
                neighbours[user_id_1][idx][i] = neighbours[user_id_1][neighbours[user_id_1].length - 1][i];
            }
            delete neighbours[user_id_1][neighbours[user_id_1].length - 1];
            neighbours[user_id_1].length--;
        }

        idx = edgeIndex(user_id_2, user_id_1);

        // IF Edge does not exist
        NOT_EDGE_EXIST = (neighbours[user_id_2].length == 0 || idx == neighbours[user_id_2].length);
        if (NOT_EDGE_EXIST) {
            emit MyEvent(false);
            return false;
        }
        // Edge at the end - just delete last element
        else if (idx == (neighbours[user_id_2].length - 1)) {
            delete neighbours[user_id_2][neighbours[user_id_2].length - 1];
            neighbours[user_id_2].length--;
        }
        // Edge in middle somewhere : swap with last element. Change idx in the opposite direction edge of last edge - swap this edge and last edge and delete
        else {
            neighbours[neighbours[user_id_2][neighbours[user_id_2].length - 1][0]][neighbours[user_id_2][neighbours[user_id_2].length - 1][2]][2] = idx;
            for(i=0; i<3; i++) {
                neighbours[user_id_2][idx][i] = neighbours[user_id_2][neighbours[user_id_2].length - 1][i];
            }
            delete neighbours[user_id_2][neighbours[user_id_2].length - 1];
            neighbours[user_id_2].length--;
        }

        emit MyEvent(true);
        return true;
    }
}