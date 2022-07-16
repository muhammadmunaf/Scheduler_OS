#pragma once

struct Command
{
    int CPU;
    int timeslice;
    char type;
    string inputFile;
    string outputFile;

    Command()
    {
        inputFile = "x";
    }
};

Command readCommand(string str)
{
    cout << str << "\nREADING COMMAND NOW\n";
    int i = 0;
    Command obj;
    // os-kernel <Input file> <# CPUs> r <timeslice> <Output file>

    string temp = "os-kernel", inputFile = "", outputFile = "", timeslice = "1000";
    int type = 0;

    for (i = 0; i < 9; i++)
    {
        if (temp[i] != str[i])
            return obj;
    }
    i++;

    while (str[i] != ' ')
    {
        inputFile += str[i];
        i++;
    }
    i++;

    int noOfCPU = str[i] - '0';

    i += 2;

    if (str[i] == 'f')
    {
        type = 'f';
        i += 2;

        while (str[i] != '\0')
        {
            outputFile += str[i];
            i++;
        }
    }
    else if (str[i] == 'p')
    {
        type = 'p';
        i += 2;
        while (str[i] != '\0')
        {
            outputFile += str[i];
            i++;
        }
    }
    else if (str[i] == 'r')
    {
        type = 'r';
        i += 2;
        timeslice = "";
        while (str[i] != ' ')
        {
            timeslice += str[i];
            i++;
        }
        i++;

        while (str[i] != '\0')
        {
            outputFile += str[i];
            i++;
        }
    }

    int timesliceInt = stoi(timeslice);

    cout << "No of CPUs: " << noOfCPU << "\nOutput file: " << outputFile << "\nInput file: " << inputFile << "\nType: " << type << "\nTimeslice: " << timesliceInt << endl;

    obj.CPU = noOfCPU;
    obj.inputFile = inputFile;
    obj.outputFile = outputFile;
    obj.timeslice = timesliceInt;
    obj.type = type;

    TIMESLICE = (double)obj.timeslice / 1000;
    CPU_COUNT = obj.CPU;
    SCHEDULE_TYPE = obj.type;

    OUTPUTFILE = obj.outputFile;
    INPUTFILE = obj.inputFile;

    cout << "No of CPUs: " << noOfCPU << "\nOutput file: " << outputFile << "\nInput file: " << inputFile << "\nType: " << type << "\nTimeslice: " << timesliceInt << endl;

    return obj;
}
