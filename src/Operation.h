#ifndef OPERATION_H
#define OPERATION_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fstream>
#include <fcntl.h>
#include "Base.h"

using namespace std;

const string BASH_COMMANDS[] = {"cat", "tr", "tee", "test", "[", "git","echo", "ls", "mkdir", "touch"}; // array that stores the commands
const string FLAGS[] = {"-e", "-f","-d"}; // array that stores the flags

class Operation : public Base
{
    public:
        Operation();
        Operation(string command) : Base(command) { }
        virtual ~Operation();

        void run();
        int getSize(string command, string word);
        string removeComment(string input);
        void passInput(string input);
        void testCommand(string flag, string dir);
        void runOperators(string command, int andLoc, int orLoc);
        void runRedirection(string command, int inputSymbol, int sOutputSymbol, int dOutputSymbol, int pipeSymbol);
        void andSymbol(string input);
        void orSymbol(string input);
        bool testFlag(string flag);
        bool testInput(string input);
        void runCmdRedirection(string leftCommand, string rightCmd);
        void runCommand(string input);
        void execute(string command, string word, int arraySize);

    private:
        bool hasRan;
        bool hasSymbol;

};

#endif // OPERATION_H
