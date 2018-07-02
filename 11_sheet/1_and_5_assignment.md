# Practical Course on Parallel Computing · SoSe 2018
## Assignment Sheet 11
### Assignment 1 – Scheduling in YARN
#### a) Which schedulers are implemented in the YARN framework? Use the ResoureManager Web Interface to determine which of them is used in our cluster!
* kinds of schedulers
    * fair scheduler
    * capacity scheduler
* The fair scheduler is used in our cluster.
#### b) Explain the basic functionality of the FairScheduler!
The FairScheduler is a scheduler that equaly diverts the available ressources to all application in such a way that in the end, on average over time, all application haven an equal share 
of the ressources. In the basic configuration it only considers the memory consumption, but it could be extended. When extended the CPU consumption will also be considered. A single app will use the complete cluster. Additional apps will be assigned unused (free) ressources. The apps are organized in queues. By default there is only one queue for all user. Queues can be limited in Memory and CPU. But the queues are also assigned a minimum share (can be assigned to other if unused). This allows smaller apps to finish fast, while big apps can work in parallel. In the queue the ressources are assigned to the apps based on memory consumption, FIFO or multiple ressources with Dominant Resource Fairness.
It is possible to order the queues hierarchly to split the cluster in given proportions. Moreover, the number of running apps per user/queue can be limited.

### Assignment 5 – Benefits and Limitations of MapReduce

#### a) Why do you think MapReduce and Hadoop were such a great success?

#### b) What are the limitations of MapReduce? Are there problem classes that can not be implemented with help of this computation model?
