# RShell

## Author
Jonathan Ho
Wentao Xu

## Project Details
This program consists of writing a command shell in C++. The command shell, titled rshell,
should be able to receive basic bash inputs from a user and execute them. The command shell prints a command
prompt. The rshell reads in a command on one line in the format: 
```
cmd = executable [argumentList] [connector cmd]
connector = || or && or ;
```
Users have no limit to the number of commands that can be chained in the rshell. Commands
can be chained together by the use of three connectors, ||, && and ;. The connectors
execute commands based on the specific rules of each connector.

Users can now use the "test" command to see if the file or directory is there. This command also works with brackets. If there are no flags set, it will automatically be set to -e.
```
cmd = test [flag] [file/directory]
or
[ [flag] [file/directory] ]
```
Users can input commands with parathensis to emphasize which commands should run together. Users are able to nest multiple parenthesis among each other.

The user is able to pipe simple commands and use redirection symbols. Order of operations do matter, can't have ">" or ">>" before any piping symbols and can't have piping symbols after those as well. Also, "<" can only be at the beginning before all the other redirection symbols.
```
cat [filename]
cat < [filename]
cat < [filename] | [command] [args]
cat < [filename] > [newfile]
cat < [filename] >> [appendfile]
```

## Known Bugs
1. Can't run multiple pipes together. Only can run single piped commands. When multi pipe commands are ran, it makes multiple files because it thinks every entry after that is a new file name

2. Can't run piping symbol '|' with make new file operator '>'

3. Can't run piping symbol '|' with append ">>"


