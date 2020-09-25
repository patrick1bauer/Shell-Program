#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>
using namespace std;

/*  Patrick Bauer
 *  Professor Bölöni
 *  COP 4600 - Operating Systems
 *  9/24/2020
 */

// =========================================================================================================
// Class that stores variables for the instance of the shell program.
// =========================================================================================================

class MyShell
{
    // Shell variables.
    private:
    string currentDir;
    vector<string> commandHistory;
    vector<pid_t> pidRunningProcesses;
    
    public:
    // Constructor.
    MyShell(string dir)
    {
        currentDir = dir;
        commandHistory.clear();
        pidRunningProcesses.clear();
    };
    
    // Gets current saved directory.
    string getMyShellDir()
    {
        return currentDir;
    }

    // Sets current directory.
    void setMyShellDir(string dir)
    {
        currentDir = dir;
    }

    // Adds command & arguments to history vector.
    void addCommandToHistory(string toRecord)
    {
        commandHistory.push_back(toRecord);
    }

    // Print all entries in command history vector.
    void printCommandHistory () 
    {
        int size = commandHistory.size();
        
        if(size == 0)
        {
            return;
        }       
                
        int i = 0;
        
        cout << "Command History\n";
        for(; i < size - 1; i++)
        {
            cout << commandHistory.at(i) << "\n";
        }
    }
    
    // Delete all entries in command history vector.
    void eraseHistory()
    {
        commandHistory.clear();
    }

    // Adds PID to PID list vector.
    void savePID(pid_t toRecord)
    {
        pidRunningProcesses.push_back(toRecord);
    }

    // Get PID list vector size.
    int getPIDSize()
    {
        return pidRunningProcesses.size();
    }

    // Get PID value at index.
    pid_t getPIDAt(int i)
    {
        return pidRunningProcesses.at(i);
    }

    // Delete PID at index.
    void deletePIDAt(int i)
    {
        pidRunningProcesses.erase(pidRunningProcesses.begin() + i);
    }

    // Delete all PIDs from PID list vector.
    void erasePIDHistory()
    {
        pidRunningProcesses.clear();
    }
};

// =========================================================================================================
// Helper functions.
// =========================================================================================================

int convertStringToInt(string inputString)
{
    // Convert inputString to val.
    double exp = 0;
    int val = 0;
    
    for (int size = inputString.size() - 1; size >= 0; size--)
    {
        val += ((int)inputString[size] - 48) * pow(10, exp);
        exp++;
    }
            
    return val;
}

char ** tokenizeString(string inputString)
{
    // Take inputString & Tokenize it.
    stringstream ss(inputString);
    vector<string> tokens;
    tokens.clear();
    
    // Add tokens into tokens vector.
    while(ss >> inputString)
    {
        tokens.push_back(inputString);
    }

    // Create an array of args.
    int size = tokens.size();
    int newSize = 0;        
    char **args = new char*[size + 1];
    args[0] = new char[tokens.at(0).size()];
    strcpy(args[0], tokens.at(0).c_str());
    int j = 0;
    for(int i = 1; i < size; i++)
    {
        j++;
        args[i] = new char[tokens.at(i).size()];
        strcpy(args[i], tokens.at(i).c_str() );
    }

    // Add null character to end of array.
    args[j + 1] = (char*) 0;

    return args;
}

// =========================================================================================================
// Command functions.
// =========================================================================================================

MyShell movetodirCommand(MyShell shellInstance, stringstream* ss, string argument)
{
    // Get target directory.
    *ss >> argument;
    char* directory = strcpy(new char[argument.length() + 1], argument.c_str());

    // Attempt to move to directory.
    if (chdir(directory) != 0)
    {
        cout << "[Error]: Directory does not exist, please enter a valid directory\n";
    }

    // Get & update shellInstance's directory to new directory.
    char *buf;
    char *newDirectory;
    long size = pathconf(".", _PC_PATH_MAX);

    if ((buf = (char*)malloc((size_t)size)) != NULL)
    {
        // Get current directory.
        newDirectory = getcwd(buf, (size_t)size);
    }
    
    //Update shellInstance directory.
    string newDir(newDirectory);
    shellInstance.setMyShellDir(newDir);

    // Print current directory.
    cout << shellInstance.getMyShellDir();

    return shellInstance;
}

MyShell whereamiCommand(MyShell shellInstance)
{
    // Get Directory from shell & display it.
    cout << shellInstance.getMyShellDir();
    return shellInstance;
}

MyShell historyCommand(MyShell shellInstance, stringstream* ss, string argument)
{
    // Get args
    *ss >> argument;
    
    // Check if "-c" is present
    if(argument.compare("-c") == 0)
    {
        // Erase history if it is present.
        shellInstance.eraseHistory();
        cout << "History Erased";
    }
    else if(argument == "")
    {
        // Display history if it is not present.
        shellInstance.printCommandHistory();
    }
    else
    {
        // Invalid argument?
        cout << "[Error]: Invalid argument for history command.\n";
    }
    return shellInstance;
}

MyShell runCommand(MyShell shellInstance, stringstream* ss, string argument, string parameters)
{
    // Get command.
    *ss >> argument;
    string executeString = "";
    int childEscapeStatus;
    
    // Determine whether command is relative or absolute.
    if(argument[0] == '/')
    {
        // Absolute
        executeString = argument;
    }   
    else
    {
        // Relative
        executeString = shellInstance.getMyShellDir() + '/' + argument;
    }
    
    // Check if program exists.
    const char * executeChar = executeString.c_str();
    if(access(executeChar, X_OK) != 0)
    {
        cout << "File does not exist\n";
        return shellInstance;
    }
    
    // Fork! Knife! Spoon!
    pid_t PID = fork();

    // Run command.
    if(PID == 0)
    {
        // Get parameters.
        while(*ss >> parameters)
        {
            argument.append(" ");
            argument.append(parameters);
        }

        // Some tokenizing.
        char **args = tokenizeString(argument);

        // Execute Command with parameters.
        execv(executeChar, args);
    }
    // Check for Errors.
    else if(PID < 0)
    {
        cout << "[Error]: Failed to fork\n";
        return shellInstance;
    }

    // Wait for program to terminate before returning prompt.
    else
    {
        waitpid(PID, &childEscapeStatus, WUNTRACED);
    }
                    
    return shellInstance;
}

MyShell backgroundCommand(MyShell shellInstance, stringstream* ss, string argument, string parameters)
{
    // Get command
    *ss >> argument;
    string executeString = "";
    int childEscapeStatus;
    
    // Determine if command is relative or absolute.
    if(argument[0] == '/')
    {
        // Absolute
        executeString = argument;
    }
    else
    {
        // Relative
        executeString = shellInstance.getMyShellDir() + '/' + argument;
    }
    
    // Check whether program exists.
    const char * executeChar = executeString.c_str();
    if(access(executeChar, X_OK) != 0)
    {
        cout << "Program does not exist\n";
        return shellInstance;
    }

    // Fork!
    pid_t PID = fork();
            
    // Run command
    if(PID == 0)
    {
        // Add parameters from rest of input
        while(*ss >> parameters)
        {
            argument.append(" ");
            argument.append(parameters);
        }

        // Some tokenizing
        char **args = tokenizeString(argument);

        // Execute command with parameters
        execv(executeChar, args);
    }

    // Check for Errors
    else if(PID < 0)
    {
        cout << "[Error]: Failed to fork\n";
        return shellInstance;
    }

    // Save PID in vector list & display it
    else
    {
        cout << "Process PID: " << PID ;
        shellInstance.savePID(PID);
        waitpid(PID, &childEscapeStatus, WNOHANG);
    }
    
    return shellInstance;
}

MyShell exterminateCommand(MyShell shellInstance, stringstream* ss, string argument)
{
    // Get process info
    *ss >> argument;
    pid_t arg = (pid_t)convertStringToInt(argument);

    // Search for process to kill
    for (int i = 0; i < shellInstance.getPIDSize(); i++)
    {
        if (shellInstance.getPIDAt(i) == arg)
        {
            // Kill selected process.
            shellInstance.deletePIDAt(i);
        }
    }
    int exterminated = kill(arg, SIGKILL);
    
    // Failure
    if(exterminated == -1)
    {
        cout << "Failed to exterminate\n";
    }
    
    return shellInstance;
}

MyShell exterminateallCommand(MyShell shellInstance)
{
    // Get processes info
    int size = shellInstance.getPIDSize();
    pid_t currentPID;
    cout << "Murdering " << size << " processes: ";

    // Kill all running processes
    for (int i = 0; i < size; i++)
    {
        // Kill current process
        currentPID = shellInstance.getPIDAt(i);
        kill(currentPID, SIGKILL);
        cout << currentPID << " ";
    }

    // Erase PID history
    shellInstance.erasePIDHistory();

    return shellInstance;
}

MyShell repeatCommand(MyShell shellInstance, stringstream* ss, string argument, string parameters)
{
    // Get the number of times to repeat the command.
    string numberOfTimesToRepeat;
    *ss >> numberOfTimesToRepeat;
    stringstream timesRepeated(numberOfTimesToRepeat);
    int repeatTimes = 0;
    timesRepeated >> repeatTimes;

    // Get the command
    *ss >> argument;
    string executeString = "";
    int childEscapeStatus;
    
    // Check if argument is relative or absolute.
    if(argument[0] == '/')
    {
        // Absolute
        executeString = argument;
    }
    else
    {
        // Relative
        executeString = shellInstance.getMyShellDir() + '/' + argument;
    }
    
    // Check if program exists
    const char * executeChar = executeString.c_str();
    if(access(executeChar, X_OK) != 0)
    {
        cout << "[Error]: Program does not exist!\n";
        return shellInstance;
    }

    // Append the parameters to the argument being run.
    while(*ss >> parameters)
    {
        argument.append(" ");
        argument.append(parameters);
    }

    vector<pid_t> currentPIDs;

    for (int i = 0; i < repeatTimes; i++)
    {
        pid_t PID = fork();

        currentPIDs.push_back(PID);

        if(PID == 0)
        {
            char **args = tokenizeString(argument);
            execv(executeChar, args);
        }
        // If its an error, return
        else if(PID < 0)
        {
            cout << "Failed to fork, returning \n";
            return shellInstance;
        }
        // If its a parent process, wait for the child process to finish 
        else
        {
            shellInstance.savePID(PID);
            waitpid(PID, &childEscapeStatus, WNOHANG);
        }
    }

    cout << "PIDs: ";
    for (int i = 0; i < currentPIDs.size(); i++)
    {
        cout << currentPIDs.at(i) << " ";
    }
    
    return shellInstance;
}

MyShell unknownCommand(MyShell shellInstance)
{
    cout << "[Error]: Unrecognized command.";
    return shellInstance;
}

// =========================================================================================================
// Command Parser
// =========================================================================================================

MyShell executeCommands(MyShell shellInstance, string inputString)
{
    // Overhead
    stringstream ss(inputString);
    string command;
    string argument= "";
    string parameters;
    
    // Save command with arguments to shellInstance's history.
    shellInstance.addCommandToHistory(inputString);
    
    // Parse command.
    ss >> command;

    // Change Directory.
    if(command.compare("movetodir") == 0)
    {
        return movetodirCommand(shellInstance, &ss, argument);
    }
    // Print Current Directory.
    else if(command.compare("whereami") == 0)
    {
        return whereamiCommand(shellInstance);
    }
    //Print Command History.
    else if(command.compare("history") == 0)
    {
        return historyCommand(shellInstance, &ss, argument);
    } 
    // Runs a single program.
    else if(command.compare("run") == 0)
    {
        return runCommand(shellInstance, &ss, argument, parameters);
    }
    // Runs a single program in the background and returns prompt.
    else if(command.compare("background") == 0)
    {
        return backgroundCommand(shellInstance, &ss, argument, parameters);
    }
    // Immediately terminates the program desired.
    else if(command.compare("exterminate") == 0)
    {
        return exterminateCommand(shellInstance, &ss, argument);
    }
    // Immediately terminates all programs.
    else if (command.compare("exterminateall") == 0)
    {
        return exterminateallCommand(shellInstance);               
    }
    // Repeats the input command a certain number of times and returns prompt.
    else if (command.compare("repeat") == 0)
    {
        return repeatCommand(shellInstance, &ss, argument, parameters);
    }
    // Unknown command.
    else
    {
        return unknownCommand(shellInstance);
    }
}

// =========================================================================================================
// Main function
// =========================================================================================================

int main()
{
    // Overhead
    std::string inputString;
    char currentDir[500];
    string workingDir = getcwd(currentDir, sizeof(currentDir));
    MyShell shellInstance(workingDir);

    // Start executing commands.
    cout << "# ";
    getline(cin, inputString);
    while(inputString.compare("byebye") != 0)
    {
        shellInstance = executeCommands(shellInstance, inputString);
        cout << "\n# ";
        getline(cin, inputString);
    }

    // Exit program
}