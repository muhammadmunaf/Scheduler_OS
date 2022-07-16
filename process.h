#pragma once

class Process
{
public:
    int PID;
    int priority;
    double arrivalTime;
    char procType;
    double cpuTime;
    double ioTime;
    string procName;

    double waitingTime;
    double executionTime;
    double endingTime;

    Process()
    {
        PID = -1;
        procName = "";
        waitingTime = 0;
        executionTime = 0;
        cpuTime = 1;
        ioTime = 1;
    }
    Process(int id, string name)
    {
        PID = id;
        procName = name;
    }

    void fetchProcessDetails(string str)
    {
        // cout << "\nInside Details\n";
        int i = 0;
        string tempNum;

        while (str[i] != '\t')
        {
            procName += str[i];
            i++;
        }

        while (str[i] == '\t' || str[i] == ' ')
            i++;

        while (str[i] != '\t')
        {
            tempNum += str[i];
            i++;
        }
        priority = stoi(tempNum);

        i++;

        tempNum = "";
        while (str[i] != '\t')
        {
            tempNum += str[i];
            i++;
        }
        arrivalTime = stof(tempNum);

        i++;

        procType = str[i];

        i += 2;
        tempNum = "";
        while (str[i] != '\t')
        {
            tempNum += str[i];
            i++;
        }
        cpuTime = stof(tempNum);

        i++;
        tempNum = "";
        while (1)
        {
            tempNum += str[i];
            if (i >= str.length())
                break;
            i++;
        }
        ioTime = stof(tempNum);

        PID = PID_COUNT++;
    }

    void fetchProcessDetailsEmpty(string str)
    {
        // cout << "\nInside Details -> " << str << endl;
        int i = 0;
        string tempNum;

        while (str[i] != '\t')
        {
            procName += str[i];
            i++;
        }

        // cout << "1\n";

        while (str[i] == '\t' || str[i] == ' ')
            i++;

        // cout << "2\n";

        while (str[i] != '\t')
        {
            tempNum += str[i];
            i++;
        }
        priority = stoi(tempNum);

        // cout << "3\n";

        i++;

        tempNum = "";
        while (str[i] != '\t')
        {
            tempNum += str[i];
            i++;
        }
        arrivalTime = stof(tempNum);

        // cout << "4\n";

        i++;

        procType = str[i];

        /* i += 2;
        tempNum = "";
        while (str[i] != '\t')
        {
            tempNum += str[i];
            i++;
        } */
        // cout << "12345\n";
        cpuTime = rand() % 15;

        ioTime = 0;

        cout << "CPU TIME: " << cpuTime << endl;

        PID = PID_COUNT++;
    }

    void displayProcesses(Process *list)
    {
        for (int i = 0; i < PID_COUNT; i++)
        {
            cout << list[i].PID << " " << list[i].procName << " " << list[i].priority << " ";
            cout << list[i].arrivalTime << " " << list[i].cpuTime << " " << list[i].procType << " " << list[i].ioTime << endl;
        }
    }

    void displayProcess()
    {
        cout << PID << " " << procName << " " << priority << " ";
        cout << arrivalTime << " " << cpuTime << " " << procType << " " << ioTime << endl;
    }

    bool operator<(const Process &rhs) const
    {
        return arrivalTime > rhs.arrivalTime;
    }
};

priority_queue<Process> newQueue;

void _start(string processesFile)
{
    Process *processList; // to initially contain the processes that are created

    ifstream infile(processesFile);

    string str;
    int count = -1;

    while (getline(infile, str))
    {
        count++;
    }
    infile.close();

    int i = 0;
    processList = new Process[count]; // creates obj for each process

    infile.open(INPUTFILE);

    getline(infile, str);

    cout << str << endl;
    int spaceCount = 0;
    while (i < str.length())
    {
        if (str[i] == '\t')
            spaceCount++;

        i++;
    }

    // cout << "SpaceCount: " << spaceCount << endl;
    i = 0;
    while (getline(infile, str))
    {
        cout << str << endl;
        if (spaceCount > 4)
            processList[i].fetchProcessDetails(str);
        else if (spaceCount < 5)
            processList[i].fetchProcessDetailsEmpty(str);

        newQueue.push(processList[i]);

        i++;
    }
    processList->displayProcesses(processList);

    cout << "done\n";

    infile.close();
}