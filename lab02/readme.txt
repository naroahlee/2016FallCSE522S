CSE522 lab02 Linux Scheuduler Profiling
Haoran Li
lihaoran@email.wustl.edu

Introduction
This readme has two Sections as instructions said:
Section I is divided into several subsections as follow: 1. Architecture and Implementation, 2. Answer to Questions, 3. Observation and Explanatation, 4. Time I spent, 5. Unsolved Problems / Insights / Suggestions
Section II is a manual for this project.

Section I

========================Architecture and Implementation====================
This project is programed in C. The libpthread provides both threads (pthread) and synchronization components (pthread_mutex_t, pthread_spinlock_t, pthread_cond_t and pthread_barrier_t) for this concurrent related project. For me, I am familiar with leveraging libpthread by using C language. In this project, I separate the system into different modules
Main, Error_Dection and Parameter_Parsing 

The Paramter_Parsing module is pretty self-explanatory.  Although designing this module can be pretty straight forward, 
I try to keep Main module clean and tidy by separating all the paramter-related functions from the Main module.
The input parameters are parsed and then stored into a data structure. Besides, I also collect the PCPU core number and store it in the same data structure.
In Addtion, extra information for different experiments are needed, such as the number of threads per CPU, the priority of high level and of low level.
I stored these parameters in a configuration file named exp.cfg.  The Parameter_Parsing module is also responsible for extract data from this configuration file.


The Error Detect Function is very handy in debugging and for sanity-check (it is very easy to make mistakes when tying parameters into your commandline).
I code the different type of errors.  When expections are found at run-time, we can easily trace where the problem is. 

The Main module is the most important part for this project. The working function (thread body) and global data structures are instantialized in this module.
I leverage pthread library to achieve multi-thread feature for this project, 
so that it is easy to share the global variables to all the threads.  
I specify pthread attributions, such as sched-policy, priority and cpu affinity, before I tried to create any workload (threads). 
In other words, I used a static approach to create threads.
Although dynamically setting attribution by using pthread_setschedparam(), pthread_set_cpuaffinity_np() are possible techniques to achieve the same goal,
I highly recommend to use the configuration-then-create approach, whose real-time characteristics is much stable.  At least, the spawned threads don't need to migrate once created, because they are already on their dedicated cores when their lives start.
This approach is perfect when dealing with RT-priority issues.  However, the true priority affects SCHED_OTHER is the nice value.  In Linux, it is possible to set multiple threads, which spawned by the same process, with different nice values.  It cannot be assigned before the thread creation.  A work around is to leverage a dynamic approach to deal with the nice value when SCHED_OTHER is choosen.


The rough procedure of main can be explained as follow:
1. Parsing the commandline-input parameters (get sched_policy, round, iter and a integer list)
2. Parsing the configuration file (get prioritys and number of threads per core)
3. Setup Pthread Attributes; Initialize Mutexes and Barriers;
4. Spawn pthread;
5. Waiting for all pthread's termination (pthread_join()); That's pretty much like the behavior of a barrier.
6. Clean the memory (free() destroy())

The key point of design a working function has already discussed in lab02 instruction: a lock for avoid racing situation of setting index / pointer of the integer list; and a barrier for preventing some threads start their work in advance.

Rather than use a plain integer multiplication for cubing a integer number, I prefer to use pow() which is supported in libm.  Since the pow() is a much time consuming function (float computation), a 1000 iterations for that can yield a significate workload for our working function.

Three types of barriers can be used in this system: CUSTOM_SPIN_BARRIER, CUSTOM_SLEEP_BARRIER and pthread_barrier_t (which is a sleep barrier). I referred to the implementation from:https://github.com/angrave/SystemProgramming/wiki/Synchronization,-Part-6:-Implementing-a-barrier

=============================Answer to Questions=======================
Question 1: How can you tell that your barrier worked?
A kernelshark would visualize the tracing data. The thread creation part will form a ladder-pattern if the sleep barrier works.  Basically, the most time-consuming part won't start until all the threads reach the barrier.

Question 2: What happens?
Well, when setting scheduler with SCHED_FIFO policy, we are able to see an infinite loop waiting at barrier, if spin-barrier is used.  The idea is that a thread would never yield until it finishes the whole procedure.  However, by no means another thread on the same core can reach the barrier, since it has no chance to run!  As a result, the former thread is not able to pass the barrier and thus cause a DEADLOCK.
If a sleep barrier is used, no deadlock would be found.  And SCHED_RR goes well in both sleep barrier and spin barrier case.

Question 3: Why would we have expected this?
That's the motivation which drives us to try another barrier implementaion, say, sleep barrier. In fact, it is a sleep barrier that the barrier in libpthread (pthread_barrier_t). To tell the truth, I firstly leverage that built-in pthread_barrier_t for this project -- clearly, I won't suffer from this DEADLOCK issue --. The instruction has explictly indicates using a spin barrier, which implies the drawback of a spin barrier!

Question 4: Which of the above solved your problem and why did it?
The separate-barriers idea works.  Basically, high prioirity threads would firstly reach the high-barrier as group one. Thus, they are able to run to end.  At that time, they can yield the processor for the low priority threads. 

======================Observation and Behavior Explanation=================

Passing the Barrier, Spin/Sleep:
Setting: Round 1000, Iter 1000, 2 Threads per core, SCHED_OTHER, priority (nice): all 0
Measuring the latency from the first thread created to all the threads pass the barrier:
Sleep-barrier:   983us
Spin-barrier : 10315us
As we can see, when the first threads reaching the barrier, it will keep spinning until its budget exhausts.  It may be scheduled for several round before all the thread passing the barrier.
On the other hand, the sleep barrier is an energy saving approach: once a thread reaches the barrier, it sleeps and yield the core to other thread.
The sleep-barrier apporach has a shorter barrier passing time than spin barrier is.

Overall Timing:
Setting: Round 1000, Iter 1000, 2 Threads per core, SCHED_OTHER, priority (nice): all -15 / -10 / -5 / 0 / 5 / 10 / 15
Note: I closed the Xserver(desktop), use tty1 only for this test (no background workload). 
Here is the result:
nice   time(s)
15     0.998679
10     0.996016
5      0.948692
0      0.955176
-5     0.998825
-10    0.989249
-15    0.990536
Remember, the overall computational workload is deterministic and const regardless of the priority.  Although the overall time varies, when we don't get a huge background workload, we can still get the conclusion that a change in priority would not largely affect the overall execution time for this program.  However, if we tried to run background processes like previous stuido, the nice value would affect much.

Thread Execution Quantum:
Setting: Round 1000, Iter 1000, 2 Threads per core, SCHED_OTHER, priority (nice):  (-10, 0) (-5, 0) (0, 5) (0, 10)
   Prio         Time(ms)
High Low     High   Low    Ratio
(-10,  0)   6.976   1.010  7:1
( -5,  0)   6.260   2.010  3:1
(  0,  5)   0.301   0.100  3:1
(  0, 10)   7.001   0.998  7:1
That's pretty normal the thread with higher priority get more computational resouce at first and soon finishes.  While the lower prioirty one get less proprotion before the high one finishes.  After higher one finishes, the lower priority one can occupy the core fully and run to the end.

Spin_Barrier cause deadlock while Sleep_lock works OK
I have mentioned this observation and given explanation in Q2~4.

SCHED_FIFO behavior:
Setting: Round 1000, Iter 1000, 2 Threads per core, SCHED_FIFO, priority : all 1
The linux kernel will add these threads into a RT-priority-1 run-queue. And pick up them with a FIFO fashion, each thread not yield the core until it runs to end.
If we are lucky enough, we can see 4 threads pinned to 4 different cores and run to end almost simutanuously, and another 4 threads follows at before.
However, exception exists:
Unlucky example:
After passing the barrier, the sequence of threads in run queue can be arbitrary.
         
RT-1-run-queue  7  5  4  0  6  3  1  2 (Thread ID)
CPU Affinity    3  1  0  0  2  3  1  2 (CPU ID)
The SCHED_FIFO is able to pick threads 7, 5 and 4 and sched them onto core 3, 1 and 0, respectively.
However, when it tries to pick up thread 0, since the core 0 is already occupied by thread 4, we had to wait 4 finished before we tried to sched 0.
And because of the SCHED_FIFO policy, we are not able to pick up the next thread 6, even if the core 2 now is idle (gap).

Setting: Round 1000, Iter 1000, 2 Threads per core, SCHED_FIFO, priority : High 2 Low 1
We won't suffer from the same gap issue we found before. Now we will have two run queue, since we have two priorities.
RT-2-run-queue  7  5  4  6  (Thread ID)
CPU Affinity    3  1  0  2  (CPU ID)

RT-1-run-queue  0  3  1  2 (Thread ID)
CPU Affinity    0  3  1  2 (CPU ID)

In the first round, we are albe to sched 7, 5, 4, 6 onto core 3, 1, 0, 2, respectively, without a "traffic jam".
Then in the second round (after the higher level RT tasks finish), the SCHED_FIFO is also to do the same thing.  Thus, we won't face such a scenario.

setting: round 1000, iter 1000, 4 threads per core, sched_fifo, priority : high 2 low 1
Increasing two more threads per core can introduce some complexity.
For each core, we must first finishes the two threads with higher priority before we can start focusing at lower-priority threads.
And the real sched sequence will subject to the cpu affinity.  This time, we can hardly be luck, so gap can always exist.
Remember we cannot sched the threads in RT-1-run-queue before we finished all the threads in RT-2-run-queue.

The switch only occurs once -- after we finished a thread from start to end.  
The time is: 470ms.  We can find this time is consistent with the time we got from SCHED_OTHER experiment.
According to these observation, it is not very hard to conclude that the overall running time varies correspondent to a specific sched patten, since the gap would introduce extra overhead to our system. And the overall responsive time would be close to multiple of 470ms, say, N * 470 (4 threads:  4 <= N <= 10).
Example, A worst case sched patten: Thread0~7 RT-Prio-2 Thread8~15 Rt-Prio-1

Blocks: 1 2 3 4 5 6  7  8  9 10
core 0: 0 4       8 12
core 1:   1 5        9 13
core 2:     2 6        10 14
core 3:       3 7         11 15
That's how we get N = 10;

SCHED_RR behavior:
Setting: Round 1000, Iter 1000, 2 Threads per core, SCHED_RR, priority : all 1
SCHED_RR can be somewhat harder to analyse.
Firstly, the gap issue exists. However, we may quickly convergent to a perfect pattern and thus get fully utilization of our cores.        
Example, SCHED_RR
Init State (Passing the barrier)
RT-1-run-queue  2 6 7 0 5 3 4 1 (Thread ID)
CPU Affinity    2 2 3 0 1 3 0 1 (CPU ID)
Blocks: 1 2 3 4 5 6 7 8
core 0:   0 0?
core 1:   5 5?
core 2: 2 6 2
core 3:   7 3

 
Block 1:
	Firstly, SCHED_RR would pick up a thread 2 and sched it onto core 2.  SCHED_RR tried to pick up another thread 6, but failed to pin it onto core 2, so it stops.
Updated:
RT-1-run-queue  6 7 0 5 3 4 1 (Thread ID)
CPU Affinity    2 3 0 1 3 0 1 (CPU ID)

Block 2:
	Thread 2 expires (after a jiffy -- 10ms), and it will be push at the tail of the run queue.  Now, it's time to do round robin, we are very luckily to pick up 6, 7, 0 and 5 onto 2, 3, 0 and 1, since they are in different core.
RT-1-run-queue  3 4 1 2 (Thread ID)
CPU Affinity    3 0 1 2 (CPU ID)

Block 3:
However, according to the tracing file I don't get why core 3 and core 2 would switch to thread 3 and 2, while core 1 and core 3 keep on running thread 5 and 7, respectively.
(Problem Unsolved)

Setting: Round 1000, Iter 1000, 2 Threads per core, SCHED_RR, priority : High 2 Low 1
In this setting, since RR only switches among the threads with same priority. So, the first 4 threads with higher priority would be executed and run to end firstly.  The remained 4 threads then follows.  It is very similar to the SCHED_FIFO with 2 threads per core and different priority.

setting: round 1000, iter 1000, 4 threads per core, SCHED_FIFO, priority : high 2 low 1
Increasing two more threads per core can introduce some complexity.
Although I can understand the two groups of threads are clusted by priority and how the SCHED_RR switches among the threads with same priority, I still don't get the detail about how we establish a stable sched pattern and why we may suffered from some gap according to our tracing file.

The switch occurs every jiffy -- 10ms.  
The overall time is around: 2s.
According to these observation, we can know that the overall running time varies correspondent to a specific sched patten. But the variance is not so much, because we can establish a stable sched pattern very fast.


============================ Time I spent on this assignment ======================
Well, this assignment did give me a lot of troble.
I tried to dynamically setting the thread properties in main() fxn just after I created pthreads.  
This strategy fails, since it is very likely that all the pthread passed the barrier before I have sucessfully got the properties set.  
Besides, implemention of a customized sleep barrier gave me extrodinary trouble: I need to find the trick of using pthread_cond_t. I finally referred a piece of code from website.
The most difficult problem I met is the SCHED_FIFO policy assoicated with cpu affinity.  I didn't realize the cpu affinities would block the SCHED_FIFO procedure sometime for aspecific SCHED_FIFO run queue sequence, until I tried tons of method to "solve" this problem and finally, of course, failed.
All in all, I spent more than 25 hours on this project. 

Developing Journal:

161014: 
	Build up project framework
	Error Code Feedback Mechanism
	Parsing Input Parameter from command line;
TBD: Thread Frameworks

161016:
	Create Threads and Interface the parameter list
	Set CPU Affinity and Sched Policy
	No synchronization Idea yet.
	Using pthread_barrier_wait(), for barrier sync
	Attention: I used O2 for compiling. Potential Bug exists (optimatization of temp = a^3)
	Mutex: Protect gs32cabinidx
		Once a thread tried to pick up a number from the list,
		it will firstly tried to lock gs32idx.
		It can get the number only if it holds the lock.
		A global variable gs32cabinidx to be protected by a mutex_cabinidx.
TBD:
	Different Priorites, Update my code.
	Starting writing a report

161017:
	Fix the barrier bug: put set_parameter_thread into work_fxn(), rather than leave them in main().
	Sched_Other: only support static priority 0. In order to change nice value for some threads in a process, set_priority() may be a solution.
   
161018:
	Use pthread_attr_t to initilized before creating the threads.
	Barrier: Issues find, sometimes some cores will be idle.
	Write a customized barrier. I still cannot solve this problem.

161019:
    I believe that gap / idle issue is not a bug.  Since we only have one global SCHED_FIFO queue for a specific priority, we may stuck due to the requirement of cpu affinity.
	I reimplemented a spin barrier to reevaluate the Question 1~3.

============================== Others ==========================
Unsolved Problems:
I still find some strange "gap / cpu idle" when perform a SCHED_RR with 2 identical-priority threads per core.  I think a partitioned run queue would solve this problem.

Insights / Lesson Learned
Setting the pthread properties staitcally before the thread created by using a pthread_attr_t is really handy, this is the first time I tried to leverage this feature.  You know, examples from Internet are usually showing a non-attribution / default thread creating method.
The SCHED_FIFO policy associated with cpu affinity, again, evinces how a global run queue works.
Implement a customized sleep barrier with a pthread_cond_t and mutex is a wonderful experience.  I leart the condition variables from CSE422S, but this is the first time I know why we really need this.

Suggestions:
Number of threads per core and High-level priority / Low-level Prioiry are three important parameters in this project.  However, have these parameters hard-coded is a bad idea. I'd like to pass these parameters from commandline, but I had to follow the specification / instruction of this project, in order to ensure the compatibility. The workaround I got for this project is that I take advantage of a configuration file "exp.cfg" to store these three parameters.
Here is my suggestion, add "-n #Thread/PCPU", "-h High-Priority" and "-l Low-Priority" opts in commandline. 

I don't like the spin-barrier / sleep-barrier comparison, say, the design / logic of Question 2, 3, 4 at all.  It is so apprent that spin-barrier would introduce large overhead, deadlock, starvation as well as prority inversion.  The most straight forward idea for us to evaluate a barrier is that we can inherit built-in barrier features in some common libraries.  In my case, pthread_barrier_t is the built-in one in libpthread.
My suggestion: Firstly we can tried to inherit the advanced version / verified version of barrier to evaluate our system.  Secondly, we can tried to implement a customized sleep barrier.  Thirdly, let's focus on why the common library discard such a spin barrier in their design -- what the drawback of spin barrier is.
 

Section II
1. unzip:
tar -xzv -f lab02_haoran.tar.gz

2. build program:
On Raspberry Pi III:
make clean
make all
I write a built-in useage in the program: lab02_main.

3.
I proivde several script for running the experiments:
run_single.sh shows how we can run this programi with different parameters.
run_q5.sh would generate data we need to evalutate SCHED_Other
run_q6.sh would generate data we need to evalutate SCHED_FIFO and SCHED_RR

