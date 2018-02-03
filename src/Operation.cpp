#include "Operation.h"

Operation::Operation()
{
    //ctor
    this->hasRan = false;
    this->hasSymbol = false;
}


Operation::~Operation()
{
    //dtor
}

void Operation::run()
{
    passInput(getString());
}


string Operation::removeComment(string input)
{
	/* checks for a comment, anything after '#' is considered a comment */
	string::size_type pos = input.find('#');
    if (pos != string::npos)
    {
        return input.substr(0, pos);
    }

    return input;
}

void Operation::passInput(string input)
{
	string commentlessInput = removeComment(input);

	vector<string> wantedCommands;

	stringstream ss(commentlessInput);
	string com;

	/* 
	Precondition: needs to be commentless, has no '#' symbol
	check for semicolons 
	*/
	while(getline(ss, com, ';')) 
	{
		/* pulls the first set of commands */
		stringstream ss_cpy(com);
		string command, word;
		ss_cpy >> command >> word;

		/* checks for an empty command */
		if(command == "")
		{
			continue;
		}

		/* if multiple commands seperated by "; " removes the space */
		while(com[0] == ' ')
		{
			com = com.substr(1);
		}

		/* if word isn't empty, make sure the word contains the entire string, including spaces */
		if(word != "")
		{
			word = com.substr(com.find_first_of(" \t")+1);
		}

		/* forms a single command */
		stringstream cmdTest;
		cmdTest << command << " " << word;

		wantedCommands.push_back(cmdTest.str()); // stores all the commands inside a vector
	}

	/* Loops through the vector to run the commands */
	for(unsigned int i = 0; i < wantedCommands.size(); i++)
	{
		/* finds location of the "&&" and "||" connectors */
		int andLoc = wantedCommands[i].find("&&");
		int orLoc = wantedCommands[i].find("||");

		int inputSymbol = wantedCommands[i].find("<");
		int sOutputSymbol = wantedCommands[i].find(">");
		int dOutputSymbol = wantedCommands[i].find(">>");
		int pipeSymbol = wantedCommands[i].find("|");

		if(andLoc > 0 || orLoc > 0) // there is an operator in the command
		{
			runOperators(wantedCommands[i], andLoc, orLoc);
			continue;
		}

		if(inputSymbol > 0 || sOutputSymbol > 0 || dOutputSymbol > 0 || pipeSymbol > 0)
		{
			stringstream redirection(wantedCommands[i]);
			string cat, symbol, filename;
			redirection >> cat >> symbol >> filename;
			if(cat == "cat")
			{
				runRedirection(wantedCommands[i], inputSymbol, sOutputSymbol, dOutputSymbol, pipeSymbol);
				return;
			}
			else
			{
				cout << "Error: invalid command" << endl;
				return;
			}
		}

		/* tests for parathesis */
		if(wantedCommands[i][0] == '(')
		{
			int fPara = 0;
			int bPara = 0;
			for(unsigned int j = 0; j < wantedCommands[i].size(); j++)
			{
				if(wantedCommands[i][j] == '(')
					fPara++;
				if(wantedCommands[i][j] == ')')
					bPara++;
			}
			string command = wantedCommands[i];
			while(command[0] == '(')
				command = command.substr(1); // removes "("
			if(fPara == bPara) // there is a closing ')'
			{
				/* Removes the ')' */
				string::size_type pos = command.find(')');
			    if (pos != string::npos)
			    {
			        command = command.substr(0, pos);
			    }
			    if(testInput(command))
			    {
					runCommand(command);
			    	continue;
			    }
			}
			else
			{
				cout << "Error: mismatch '(' and ')'" << endl;
				return;
			}
		}

		if(testInput(wantedCommands[i]))
			runCommand(wantedCommands[i]);
		else
			cout << wantedCommands[i] << ": command not found\n";
	}

}

void Operation::testCommand(string flag, string dir)
{	
	struct stat st;
	stat(dir.c_str(), &st);

	if(flag == "-e")
	{
		if(S_ISDIR(st.st_mode) || S_ISREG(st.st_mode))
		{
			cout << "(true)" << endl;
			return;
		}
		else
		{
			cout << "(false)" << endl;
			return;
		}
	}
	else if(flag == "-f")
	{
		if(S_ISREG(st.st_mode))
		{
			cout << "(true)" << endl;
			return;
		}
		else
		{
			cout << "(false)" << endl;
			return;
		}
	}
	else if(flag == "-d")
	{
		if(S_ISDIR(st.st_mode))
		{
			cout << "(true)" << endl;
			return;
		}
		else
		{
			cout << "(false)" << endl;
			return;
		}
	}
	return;
}

void Operation::runRedirection(string command, int inputSymbol, int sOutputSymbol, int dOutputSymbol, int pipeSymbol)
{
    stringstream ss(command);
    string cat, symbol, filename;

    ss >> cat >> symbol >> filename; // left command should be in the form "cat < filename"

    if(inputSymbol >= 0 && sOutputSymbol < 0 && dOutputSymbol < 0 && pipeSymbol < 0)
    {
      	passInput(cat + " " + filename);
    	return;
    }
    if(symbol == "<")
    {
    	command = "cat" + command.substr(inputSymbol + 1);
    }
    else
    {
    	filename = symbol;
    }

	if(pipeSymbol >= 0)
	{
    	int pSymbol = command.find("|");
		hasSymbol = true;
		string rightCommand = command.substr(pSymbol + 1);
		
		// removes any spaces if right command starts with spaces
		while(rightCommand[0] == ' ')
			rightCommand = rightCommand.substr(1);

		string leftCommand = command.substr(0, pSymbol);
		
		runCmdRedirection(leftCommand, rightCommand);
		
		
	}
	//it should work only leftcommand but not rightCmd 
	//	runCmdRedirection(leftCommand, rightCmd);
	if(pipeSymbol < 0 && (sOutputSymbol >= 0 || dOutputSymbol >= 0))
	{
		int pid, out;
		int pipefd[2];
		string file;
		char * cmd[3];
		cmd[0] = (char*) cat.c_str();
		cmd[1] = (char*) filename.c_str();
		cmd[2] = NULL;

		if(dOutputSymbol >= 0)
		{
			dOutputSymbol = command.find(">>");
			file = command.substr(dOutputSymbol + 2);
		}
		else if(sOutputSymbol >= 0)
		{
			sOutputSymbol = command.find("> ");
			file = command.substr(sOutputSymbol + 1);
		}
				
		
		while(file[0] == ' ')
			file = file.substr(1);

		// fork
		pid = fork();
		pipe(pipefd);
		if (pid == 0) 
		{
			// give stdout
			if(dOutputSymbol >= 0) // ">>"
			{
				out = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND);
    			dup2(out, 1);

			}
			else if(sOutputSymbol >= 0) // ">"
			{
				out = open(file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
				dup2(out,1);
			}
			
			execvp(cmd[0], cmd); // should be "cat < filename"
			exit(0);
		}
		else 
		{
			wait(0);
			return;
		}
		
	}
	return;
}


void Operation::runOperators(string command, int andLoc, int orLoc)
{
	int startParaLoc = command.find('(');
	if(startParaLoc >= 0 && ((startParaLoc < andLoc && andLoc > 0) || (startParaLoc < orLoc && orLoc > 0)))
	{
		/* tests for parathesis */
		if(command[0] == '(')
		{
			int endParaLoc = command.find(")");
			int fPara = 0;
			int bPara = 0;
			for(unsigned int j = 0; j < command.size(); j++)
			{
				if(command[j] == '(')
					fPara++;
				if(command[j] == ')')
					bPara++;
			}
			if(fPara == bPara && endParaLoc > 0) // there is a closing ')'
			{
				string cmd = command; // removes "("
				int paraCount = 0;
				while(cmd[0] == '(')
				{
					cmd = cmd.substr(1);
					paraCount++;
				}

				cmd = cmd.substr(0, endParaLoc - paraCount);
				cout << cmd << endl;
			    passInput(cmd);
			    if(command.substr(endParaLoc + 1) != "") // if there are more things after the "( )"
			    {
			    	string temp = command.substr(endParaLoc + 1);
			    	int tempAnd = temp.find("&&");
			    	int tempOr = temp.find("||");
			    	if(tempAnd >= 0 && tempOr < 0)
			    	{
			    		passInput(temp.substr(tempAnd + 2));
			    	}
			    	if(tempAnd >= 0 && (tempOr >= 0 && tempOr > tempAnd)) // there is an and symbol
			    	{
			    		passInput(temp.substr(tempAnd + 2));
			    	}
			    	if(!hasRan && tempOr >= 0 && tempOr < tempAnd) 
			    	{
			    		passInput(temp.substr(tempOr + 2));
			    	}
		   		}
		    
		    	return;
			}
		}
	}
	/** Contains multiple operators **/ 
	if(andLoc != -1 && andLoc < orLoc) // and is the first operator 
	{
		andSymbol(command);
		return;
	}
	if(orLoc != -1 && andLoc > orLoc) // or is the first operator 
	{
		orSymbol(command);
		return;
	}

	/** Only one operator **/
	if(andLoc > 0) // and symbol
	{
		andSymbol(command);
		return;
	}
	if(orLoc > 0) // or symbol
	{
		orSymbol(command);
		return;
	}
}

void Operation::andSymbol(string input)
{
	cout << "AND: " << input << endl;

	string test = input; // stores the command
	string cmdLeft, cmdRight; //two strings to store both arguments
	/* get the left side command before the && */
	string::size_type andPos = test.find('&');
    if (andPos != string::npos)
    {
        cmdLeft = test.substr(0, andPos);
    }

    /* get the right side command after the && */
    cmdRight = test.substr(test.find('&') + 2);
    /* removes an empty space if there is a space after the && */
    while(cmdRight[0] == ' ')
	{
		cmdRight = cmdRight.substr(1);
	}

	/* removes the ( ) if cmdRight has them */
	int startParaLoc = cmdRight.find('(');
	if(startParaLoc != -1)
	{
		/* tests for parathesis */
		if(cmdRight[0] == '(')
		{
			int endParaLoc = cmdRight.find(")");
			if(endParaLoc > 0) // there is a closing ')'
			{
				cmdRight = cmdRight.substr(1); // removes "("
				/* Removes the ')' */
				string::size_type pos = cmdRight.find(')');
			    if (pos != string::npos)
			    {
			        cmdRight = cmdRight.substr(0, pos);
			    }
			}
			else
			{
				cout << "Error: no closing ')'" << endl;
				return;
			}
		}
	}

	int andLLoc = cmdLeft.find("&&");
	int orLLoc = cmdLeft.find("||");
	int andRLoc = cmdRight.find("&&");
	int orRLoc = cmdRight.find("||");

	/** For deubgging purposes 
	cout << "\nAND COMMANDS\n";
	cout << "CMD RIGHT: " << cmdRight << endl;
	cout << "AND LOC: RIGHT: " << andRLoc << " LEFT: "<< andLLoc << endl;
	cout << "CMD LEFT: " << cmdLeft << endl;
	cout << "OR LOC: RIGHT: " << orRLoc << " LEFT: "<< orLLoc << endl;
	cout << endl;
	**/

	if(testInput(cmdLeft) || testInput(cmdRight)) // at least one statement is true
	{
		/* left statement is true */
		if(testInput(cmdLeft) && orLLoc < 0 && andLLoc < 0)
		{
			runCommand(cmdLeft);
		}

		if(!testInput(cmdLeft))
		{
			cout << cmdLeft << ": command not found"<< endl;
			return;
		}
		string temp = cmdRight;
		if (andPos != string::npos)
    	{
        	cmdRight = temp.substr(0, andPos);
    	}
    	string::size_type orPos = test.find('|');
		if (orPos != string::npos)
    	{
        	cmdRight = temp.substr(0, orPos);
    	}
		if(!testInput(cmdRight) && orRLoc < 0 && andRLoc < 0)
		{
			cout << cmdRight << ": command not found"<< endl;
		}
		cmdRight = temp;
		/* right statement is true */
		if(testInput(cmdRight) && orRLoc < 0 && andRLoc < 0)
		{
			runCommand(cmdRight);
		}
		if(andLLoc == -1 && andRLoc == -1 && orLLoc == -1 && orRLoc == -1)
			return;
	}

	

	/** Contains multiple operators **/ 
	// and is the first operator 
	if(andRLoc >= 0 || orRLoc >= 0)
	{
		runOperators(cmdRight, andRLoc, orRLoc);
	}

	return;
}

void Operation::orSymbol(string input)
{
	cout << "OR: " << input << endl;

	string test = input; // stores the command
	string cmdLeft, cmdRight; //two strings to store both arguments

	/* get the left side command before the && */
	string::size_type pos = input.find('|');
    if (pos != string::npos)
    {
        cmdLeft = test.substr(0, pos);
    }

    /* get the right side command after the && */
    cmdRight = test.substr(test.find('|') + 2);
    /* removes an empty space if there is a space after the && */
    while(cmdRight[0] == ' ')
	{
		cmdRight = cmdRight.substr(1);
	}

	int startParaLoc = cmdRight.find('(');
	if(startParaLoc >= 0)
	{
		/* tests for parathesis */
		if(cmdRight[0] == '(')
		{
			int endParaLoc = cmdRight.find(")");
			if(endParaLoc > 0) // there is a closing ')'
			{
				cmdRight = cmdRight.substr(1); // removes "("
				/* Removes the ')' */
				string::size_type pos = cmdRight.find(')');
			    if (pos != string::npos)
			    {
			        cmdRight = cmdRight.substr(0, pos);
			    }
			}
			else
			{
				cout << "Error: no closing ')'" << endl;
				return;
			}
		}
	}

	int andLLoc = cmdLeft.find("&&");
	int orLLoc = cmdLeft.find("||");
	int andRLoc = cmdRight.find("&&");
	int orRLoc = cmdRight.find("||");

	/** For debugging purposes 
	cout << "\nOR COMMANDS\n";
	cout << "CMD RIGHT: " << cmdRight << endl;
	cout << "AND LOC: RIGHT: " << andRLoc << " LEFT: "<< andLLoc << endl;
	cout << "CMD LEFT: " << cmdLeft << endl;
	cout << "OR LOC: RIGHT: " << orRLoc << " LEFT: "<< orLLoc << endl;
	cout << endl;
	**/

	cout << "cmdLeft " << cmdLeft << endl;
	cout << "cmdRight " << cmdRight << endl;

	if(testInput(cmdLeft) || testInput(cmdRight)) // at least one statement is true
	{
		cout << "orLoc " << orLLoc << endl;
		cout << "andLoc " << andLLoc << endl;

		/* left statement is true */
		if(testInput(cmdLeft) && orLLoc < 0 && andLLoc < 0)
		{
			hasRan = true;
			runCommand(cmdLeft);
			
			if(andRLoc == -1)
				return;

			/** Contains multiple operators **/ 
			// and is the first operator 
			if(andRLoc != -1 && (andRLoc < orRLoc || orRLoc == -1))
			{
				/* get the right side command after the && */
			    cmdRight = cmdRight.substr(cmdRight.find('&') + 2);
			    /* removes an empty space if there is a space after the && */
			    while(cmdRight[0] == ' ')
				{
					cmdRight = cmdRight.substr(1);
				}
				if(!hasRan && testInput(cmdRight))
				{
					hasRan = true;
					runCommand(cmdRight);
				}
			}

		}
		/* right statement is true */
		if(!hasRan && testInput(cmdRight) && orRLoc < 0 && andRLoc < 0)
		{
			hasRan = true;
			runCommand(cmdRight);
			return;
		}
	}

	/** Contains multiple operators **/ 
	// and is the first operator 
	if(!hasRan && (andRLoc != -1 || orRLoc != -1) && (andRLoc < orRLoc || orRLoc == -1))
	{
		runOperators(cmdRight, andRLoc, orRLoc);
	}

	return;
}

bool Operation::testFlag(string flag)
{
	int commandSize = sizeof(FLAGS) / 8;
	for(int i = 0; i < commandSize; i++)
	{
		if(flag == FLAGS[i])
			return true;
	}
	return false;
}

bool Operation::testInput(string input)
{
	int commandSize = sizeof(BASH_COMMANDS) / 8; // stores the number of commands there are

	stringstream ss(input);
	string com;
	/* looks to see if the first word was a valid command */
	if(getline(ss, com, ' '))
	{
		/* searches if the first input for a valid command */
		for(int i = 0; i < commandSize; i++)
		{
			if(com == BASH_COMMANDS[i]) // valid command
			{
				return true;
			}
		}
	}

	return false;
}

void Operation::runCmdRedirection(string left, string right)
{
	int pSymbol = right.find("|");
	string rightCmd = right.substr(0, pSymbol);


	int c = count(right.begin(), right.end(), '|');
	c++;
	char* token;
	vector<char*> leftcmd;
	vector<char*> rightcmd;

	token = strtok((char*)left.c_str(), " \t");
	while (token != 0) 
	{
		leftcmd.push_back(token);
		token = strtok(NULL, " ");
	}
	leftcmd.push_back(0);

	token = strtok((char*)right.c_str(), " \t");
	while (token != 0) 
	{
		rightcmd.push_back(token);
		token = strtok(NULL, " ");
	}
	rightcmd.push_back(0);

	for(int i = 0; i < c; i++)
	{

		int pipefd[2];
		pipe(pipefd);
	
		if(c >= 1)
		{
			c--;
			pid_t pid = fork();
			if (pid == 0) 
			{
				close(pipefd[0]);
				dup2(pipefd[1], 1);
				execvp(leftcmd[0], leftcmd.data());	
				perror(left.c_str());
				exit(EXIT_FAILURE);
			}
			else 
			{
				waitpid(pid, 0, 0);
				close(pipefd[1]);
			}
		
			pid = fork();
			if (pid == 0) 
			{
				close(pipefd[1]);
				dup2(pipefd[0], 0);
				execvp(rightcmd[0], rightcmd.data());	
				perror(right.c_str());
				exit(EXIT_FAILURE);
			}
			else 
			{
				waitpid(pid, 0, 0);
				close(pipefd[0]);
			}
		}
		c--;
		if(c == 0 || c < 0)
			break;
		
		pSymbol = right.find("|");
		rightCmd = right.substr(0, pSymbol);
		token = strtok((char*)rightCmd.c_str(), " \t");
		while (token != 0) 
		{
			leftcmd.push_back(token);
			token = strtok(NULL, " ");
		}
		leftcmd.push_back(0);

		pSymbol = right.find("|");
		rightCmd = right.substr(0, pSymbol);
		int r = right.find(rightCmd);
		right = right.substr(r + 2 + strlen(rightCmd.c_str()));	
			
		if(right.find("|")!=string::npos)
		{
			pSymbol = right.find("|");
			rightCmd = right.substr(0, pSymbol);
		}
		else
		{
		    break;	
		}

		c = count(rightCmd.begin(), rightCmd.end(), '|');
	}
}

void Operation::runCommand(string input)
{
	stringstream ss(input);
	string command, word, flag, dir;
	ss >> command >> word;

	/* if word isn't empty, make sure the word contains the entire string, including spaces */
	if(word != "")
	{
		word = input.substr(input.find_first_of(" \t")+1);
	}

	if(command == "test" || command == "[")
	{
		stringstream ss_temp(word);
		ss_temp >> flag >> dir;

		if(dir == "" || dir == "]") // if user did not input a flag
		{
			dir = flag;
			flag = "-e";
		}

		int bracketLoc = input.find("]");
		if(command == "[" && bracketLoc < 0)
		{
			cout << "Error missing ']'" << endl;
			return;
		}

		if(testFlag(flag)) // see if flag is valid
		{
			testCommand(flag, dir);
			return;
		}
		else // not a valid flag
		{
			cout << flag << " is not a valid flag\n";  
			return;
		} 
	}
	hasRan = true;

	int size = getSize(command, word); // gets the size of the array
	execute(command, word, size);      // executes the bash code
}

int Operation::getSize(string command, string word)
{
	if(word == "")
		return 2;

	if(command != "" && word != "")
		return 3;

	return 0;
}

void Operation::execute(string command, string word, int arraySize)
{
	char * args[3];

	if(arraySize == 2)
	{
		args[0] = (char*)command.c_str();
		args[1] = NULL;
	}
	if(arraySize == 3)
	{
		args[0] = (char*)command.c_str();
		args[1] = (char*)word.c_str();
		args[2] = NULL;
	}

	pid_t pid = fork();

    if(pid == -1) // failed child
    {
        perror("fork error");
        exit(1);
    }
    else if(pid == 0) // child
    {
        if(execvp(args[0], args) == -1) // tries to run the bash command
        {
        	perror("exec");
        }
    }
    else // parent
    {
        if(wait(0) == -1)
        {
            perror("wait error");
        }
    }
}

