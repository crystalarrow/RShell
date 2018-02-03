#include <iostream>
#include "Base.h"
#include "Operation.h"

using namespace std;


void checkToExit(string input);

int main()
{

	string s; // string for input

	while(true)
	{
        /* prints command prompt */
		cout << "$ " ;
		getline(cin, s);

        /* exits the command prompt */
		checkToExit(s);
        Operation* userInput = new Operation(s);
        userInput->run();
	}

    return 0;
}

void checkToExit(string input)
{
	if(input == "exit")
	{
	    exit(0);
	}
}
