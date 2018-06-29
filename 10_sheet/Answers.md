# Practical Course on Parallel Computing
## Assignment sheet 10
Jonas Hügel & Robin Hundt

### Assignmnent 1

#### a) Shortly sketch and explain the three different phases of the MapReduce workflow!

1. Map  
Data is split into chunks that are distributed on the mappers. Each mapper applies the specified `map` function to its chunk. Map functions take an input that consists of `<key, value>` pairs and output again `<key, value>` pairs (possibly of different types). 
2. Shuffle and sort  
During the shuffle and sort phase the output of the mappers is sorted and grouped by their keys and distributed to the reducers. 
3. Reduce  
The output of the Shuffle and sort phase is passed to the reducers which execute the specified reduce function. The signature of the reduction is `<key, Iterable<value>, key, value>`. Example: A reducer could take all the occurrences of a single word and sum up their respective counts in splits of a file in order to get the total number of occurrences within that file.

#### b) What is the combiner phase and why is it optional?  

The combiner phase can be used to to decrease the size of the data which is transmitted to the reducers over the network. The function specified as the combiner is executed on the Node where the original data is and where the mapping occurred. The signature of the method is the same as the reducer.  
It is optional because the reducer doesn't require that the output of the mappers has been combined. If there is no combine step simply more data will have to be transmitted.

#### c) Explain the purpose of the NameNode, DataNode, ResourceManager, and NodeManager daemons in HDFS and YARN!

**NameNode**:  
The NameNode is the central part of the HDFS where the record of all files in the system is kept (metadata) and the file locations on the cluster are tracked. Block locations are not stored persistently and are reconstructed from the data nodes when the system starts.
**DataNode**:  
DataNodes provide the actual storage and are responsible for serving read and write requests from the clients.
**ResourceManager**:
The ResourceManager is part of the Yarn system and the ultimate authority that divides the cluster resources among the applications. It's main responsibility is scheduling the resources of the cluster. 
**NodeManager**:
The NodeManager within the Yarn system is responsible for managing the resources on the individual compute nodes in a cluster. It's tasks are communicating with the ResourceManager, managing containers, monitoring node resources (e.g. CPU/Memory).

#### d) Explain the meaning of the replication factor and the blocksize in the Hadoop Distributed File System (HDFS). What is controlled by the number of slots?
Files within in the HDFS are partitioned into blocks with a maximal size of `blocksize`. Those blocks are replicated up to `replication factor` many times and distributed on the DataNodes, such that identical blocks are on different nodes.  
The number of slots specifies the maximum amount of map/reduce tasks that can be run in parallel.


### Assignment 2
#### c,d)
Results:
http://hadoop-master:8088/proxy/application_1528982385706_0292/
or in the file: JobResult-Assignment2

Occurence Master: 43 +1 =44

$ hdfs dfs -cat output/part-r-00000 |grep Master
Master  43
Master! 1
Mastering   1
Masterly    1
Masterpiece 1
Masterpiece. 1
Masters 1
Masterstroke    1
Masterstrokes.) 1
#### e)

For an analysis of the character frequency see CharacterCount/char-frequency-visualization.pdf .

### Assignment 3
The number of map and reduce jobs was 250/1 respectively. This is due to the input consisting of 250 individual files, each with a size lower than the blocksize. Only 1 reduce job was started because the default value (1) was not modified.

 Anagrams of respect: spectre scepter sceptre respect