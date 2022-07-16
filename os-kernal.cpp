#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <iomanip>
#include <unistd.h>
#include <chrono>

using namespace std;

int PID_COUNT = 0;
int EXEC_COUNT = 0;
int CPU_COUNT = 4;
double TIMESLICE = 1;
int CONTACT_SWITCH_COUNT = 0;

ofstream outfile;

string INPUTFILE = "Processes1.txt", OUTPUTFILE = "output.txt";

char SCHEDULE_TYPE = 'f';

double TIME_SECONDS = 0;

bool counter; // this will prevent the threads from freely executing without time increment
bool doPreempt = false;

mutex mutexPCBLock;
mutex mutexScheduleLock;
mutex mutexWakeUpLock;
mutex mutexTerminateLock;

#include "command.h"
#include "process.h"
#include "LL.h"

Process idle(-1, "(idle)");

Process tempProcess; // just for basic testing and conditions

// queue<Process> readyQueue;      // this queue will contain all the processes that are ready to be executed
queue<Process> *runningQueue;   // the processes currently executing will be in this queue
queue<Process> terminatedQueue; // processes that have finished their execution comes here
queue<Process> readyQueueRR;

struct PriorityQueueComp
{
    bool operator()(Process p1, Process p2)
    {
        if (SCHEDULE_TYPE == 'f') // || SCHEDULE_TYPE == 'r')
            return p1.arrivalTime > p2.arrivalTime;
        else if (SCHEDULE_TYPE == 'p')
            return p1.priority < p2.priority;
        else
            return false;
    }
};

priority_queue<Process, vector<Process>, PriorityQueueComp> readyQueue;

void force_preempt(int i)
{
    // cout << "Force Preempt\n";
    mutexScheduleLock.lock();
    readyQueue.push(runningQueue[i].front());
    runningQueue[i].pop();
    CONTACT_SWITCH_COUNT++;
    mutexScheduleLock.unlock();
    doPreempt = false;
}

void wakeup()
{
    if (SCHEDULE_TYPE == 'p')
    {
        if (!newQueue.empty())
        {
            Process temp = newQueue.top();

            if ((int)temp.arrivalTime >= TIME_SECONDS) // if current time is greater than or equal to arrival time
            {
                // lock the mutex and push arrived process into readyQueue
                mutexWakeUpLock.lock();
                readyQueue.push(temp);
                newQueue.pop();
                mutexWakeUpLock.unlock();

                if (temp.priority > tempProcess.priority)
                {
                    doPreempt = true;
                }
            }
        }
    }
    else if (SCHEDULE_TYPE == 'f')
        if (!newQueue.empty())
        {
            Process temp = newQueue.top();

            if ((int)temp.arrivalTime >= TIME_SECONDS) // if current time is greater than or equal to arrival time
            {
                // lock the mutex and push arrived process into readyQueue
                mutexWakeUpLock.lock();
                readyQueue.push(temp);
                newQueue.pop();
                mutexWakeUpLock.unlock();
            }
        }
}

void _terminate(int i = 0)
{
    mutexTerminateLock.lock();
    runningQueue[i].front().endingTime = TIME_SECONDS;
    terminatedQueue.push(runningQueue[i].front());

    if (runningQueue[i].front().procName != "(idle)")
        cout << "Terminating " << runningQueue[i].front().procName << endl;

    if (runningQueue[i].front().procName != "(idle)")
    {
        EXEC_COUNT++;
        cout << runningQueue[i].front().procName << " EXEC ++\n";
    }

    runningQueue[i].pop();

    mutexTerminateLock.unlock();
}

#include "schedule.h"

void preempt(int i = 0)
{
    mutexScheduleLock.lock();
    readyQueue.push(runningQueue[i].front());
    mutexScheduleLock.unlock();
}

void contactSwitch(int i = 0, int timeslice = TIMESLICE)
{
    if (SCHEDULE_TYPE == 'f') // execute with FCFS Algo
    {
        Process curr = runningQueue[i].front();
        while (1)
        {

            if (curr.executionTime >= curr.cpuTime)
                break;

            curr.executionTime++;

            counter = true;
            while (counter) // waits for main to increment
            {
            }
            cout << "Time: " << TIME_SECONDS << endl;
        }
        // runningQueue[i].front().executionTime = curr.executionTime
        if (runningQueue[i].front().procName != "(idle)")
            CONTACT_SWITCH_COUNT++;

        _terminate(i);

        // runningQueue[i].pop();
    }
    else if (SCHEDULE_TYPE == 'p') // execute with Priority Algo
    {

        Process curr = runningQueue[i].front();
        while (1)
        {
            if (curr.executionTime >= curr.cpuTime)
                break;

            curr.executionTime++;

            counter = true;
            while (counter) // waits for main to increment
            {
            }

            if (doPreempt)
            {
                force_preempt(i);
                break;
            }
            doPreempt = false;

            cout << "Time: " << TIME_SECONDS << endl;
        }
        if (!runningQueue[i].empty())
            if (runningQueue[i].front().procName != "(idle)")
                CONTACT_SWITCH_COUNT++;
        if (!runningQueue[i].empty())
            _terminate(i);
    }
}

void schedule(int i = 0)
{
    // cout << i << " Executed Processes: " << EXEC_COUNT <<endl;
    mutexScheduleLock.lock(); // locks when checking the readyqueue
    if (!readyQueue.empty())  // if readyQueue is not empty, move front process into runningQueue of current thread
    {
        // runningQueue[i].push(readyQueue.front());
        runningQueue[i].push(readyQueue.top());
        readyQueue.pop();
    }
    else // if no process in readyQueue then push idle process in running queue
    {
        runningQueue[i].push(idle);
    }
    mutexScheduleLock.unlock();

    // cout << "Loading: " << runningQueue[i].front().procName << endl;

    contactSwitch(i);
}

void startOS(int i = 0)
{
    while (PID_COUNT != EXEC_COUNT)
    {
        schedule(i);
    }

    cout << "\n\nThread " << i << " Terminating at " << EXEC_COUNT << "\n\n";
}

class PCB
{
public:
    double time;
    string *cpus;
    string io;

    PCB()
    {
        time = TIME_SECONDS;
        cpus = new string[CPU_COUNT];

        for (int i = 0; i < CPU_COUNT; i++)
            cpus[i] = "(idle)";
    }

    void addEntry();

    void displayPCB()
    {
        cout << left << setw(10) << time; // << setw(16) << processNameCPU << setw(16) << processNameIO << endl;
        outfile << left << setw(10) << time;
        for (int i = 0; i < CPU_COUNT; i++)
        {
            cout << setw(16) << cpus[i];
            outfile << setw(16) << cpus[i];
        }
        cout << endl;
        outfile << endl;
    }
};

LinkedList<PCB> PCB_list; // maintain full list of every cpu at every second

void PCB::addEntry() // locks mutex and then checks all their queues to maintain PCB entries
{
    mutexPCBLock.lock();
    cout << "\n\nPCB Entry IN\n";

    PCB *temp = new PCB;
    cout << TIME_SECONDS << " ";
    for (int i = 0; i < CPU_COUNT; i++) // pcb entry for all cpus
    {
        // cout << "123\n";
        if (!runningQueue[i].empty())
        {
            if (runningQueue[i].front().procName == "")
                temp->cpus[i] = "TEMP";
            else
                temp->cpus[i] = runningQueue[i].front().procName; // writes names of processes in each queue

            cout << temp->cpus[i] << " ";
        }
    }
    cout << endl;

    PCB_list.insert(*temp);

    cout << "PCB Entry OUT\n\n";
    mutexPCBLock.unlock();
}

void displayPCB()
{
    cout << left << setw(10) << "TIME";
    for (int i = 0; i < CPU_COUNT; i++)
        cout << setw(4) << "CPU#" << left << setw(12) << i;

    cout << endl;

    outfile.open(OUTPUTFILE);

    outfile << left << setw(10) << "TIME";
    for (int i = 0; i < CPU_COUNT; i++)
        outfile << setw(4) << "CPU#" << left << setw(12) << i;
    outfile << endl;

    while (!PCB_list.isEmpty())
    {
        PCB_list.head->data.displayPCB();

        PCB_list.pop();
    }

    outfile.close();
}

double getTotalReadyTime()
{
    double total = 0;
    while (!terminatedQueue.empty())
    {
        Process temp = terminatedQueue.front();
        if (temp.procName == "(idle)")
        {
            terminatedQueue.pop();
            continue;
        }
        // cout << temp.procName << " " << temp.cpuTime << " " << temp.endingTime << " " << temp.arrivalTime << endl;

        double t = temp.endingTime - temp.arrivalTime - temp.cpuTime;
        total += t;

        terminatedQueue.pop();
    }

    return total;
}

int main()
{
    string str; // = "os-kernel Processes1.txt 1 p output.txt";

    cout << "> ";
    getline(cin, str);

    readCommand(str);

    _start(INPUTFILE);

    PCB *ptr_PCB;

    thread *CPUS[CPU_COUNT];

    runningQueue = new queue<Process>[CPU_COUNT];

    cout << "\n\n";

    if (SCHEDULE_TYPE != 'r')
    {
        for (int i = 0; i < CPU_COUNT; i++)
            CPUS[i] = new thread(startOS, i);

        while (PID_COUNT != EXEC_COUNT) // loop till all processes are executed
        {
            cout << "Cycle\n";
            wakeup();
            ptr_PCB->addEntry();

            this_thread::sleep_for(200ms);

            if (SCHEDULE_TYPE == 'r')
                TIME_SECONDS += TIMESLICE;
            else
                TIME_SECONDS++;

            counter = false;
            cout << "Cycled\n";
        }
    }
    else if (SCHEDULE_TYPE == 'r')
    {
        for (int i = 0; i < CPU_COUNT; i++)
            CPUS[i] = new thread(startOS1, i);

        while (PID_COUNT != EXEC_COUNT) // loop till all processes are executed
        {
            cout << "Cycle\n";
            wakeup1();
            ptr_PCB->addEntry();

            this_thread::sleep_for(200ms);

            if (SCHEDULE_TYPE == 'r')
                TIME_SECONDS += TIMESLICE;
            else
                TIME_SECONDS++;

            counter = false;
            cout << "Cycled\n";
        }
    }

    cout << "\n\nMain Counting Ended at " << EXEC_COUNT << endl;

    for (int i = 0; i < CPU_COUNT; i++)
        CPUS[i]->join();

    cout << "All Threads Killed\n";

    displayPCB();

    cout << "Total time in ready state: " << getTotalReadyTime() << endl;
    cout << "Total contact switches: " << CONTACT_SWITCH_COUNT << endl;
    cout << "Timeslice: " << TIMESLICE * 1000 << "ms\n";
    cout << "Type: " << SCHEDULE_TYPE << endl;

    return 0;
}
