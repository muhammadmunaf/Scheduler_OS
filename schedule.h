#pragma once

void wakeup1()
{
    if (!newQueue.empty())
    {
        Process temp = newQueue.top();

        if ((int)temp.arrivalTime >= TIME_SECONDS) // if current time is greater than or equal to arrival time
        {
            // lock the mutex and push arrived process into readyQueueRR
            mutexWakeUpLock.lock();
            readyQueueRR.push(temp);
            newQueue.pop();
            mutexWakeUpLock.unlock();
        }
    }
}

void preempt1(int i = 0)
{
    mutexScheduleLock.lock();
    readyQueueRR.push(runningQueue[i].front());
    mutexScheduleLock.unlock();
}

void contactSwitch1(int i = 0, int timeslice = TIMESLICE)
{
    // for contact switch count we have to check whether the last process equals the
    // new process in running queue and if it is different then do an increment
    Process curr = runningQueue[i].front();
    {
        if (curr.procName != tempProcess.procName && curr.procName != "(idle)")
            CONTACT_SWITCH_COUNT++;

        if (runningQueue[i].front().executionTime >= runningQueue[i].front().cpuTime)
        {
            _terminate(i);
            return;
        }
        runningQueue[i].front().executionTime += TIMESLICE;

        counter = true;
        while (counter) // waits for main to increment
        {
        }
        cout << "Time: " << TIME_SECONDS << endl;

        preempt1(i);

        tempProcess = runningQueue[i].front();
        runningQueue[i].pop();
    }
}

void schedule1(int i = 0)
{
    // cout << i << " Executed Processes: " << EXEC_COUNT <<endl;
    mutexScheduleLock.lock();  // locks when checking the readyQueueRR
    if (!readyQueueRR.empty()) // if readyQueueRR is not empty, move front process into runningQueue of current thread
    {
        // runningQueue[i].push(readyQueueRR.front());
        runningQueue[i].push(readyQueueRR.front());
        readyQueueRR.pop();
    }
    else // if no process in readyQueueRR then push idle process in running queue
    {
        runningQueue[i].push(idle);
    }
    mutexScheduleLock.unlock();

    // cout << "Loading: " << runningQueue[i].front().procName << endl;

    contactSwitch1(i);
}

void startOS1(int i = 0)
{
    while (PID_COUNT != EXEC_COUNT)
    {
        schedule1(i);
    }

    cout << "\n\nThread " << i << " Terminating at " << EXEC_COUNT << " ROUND ROBIN\n\n";
}
