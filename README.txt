+------------------------------------------------------------------------------+
|                               My Shell Program                               |
+------------------------------------------------------------------------------+

About:
This is a shell program written by Patrick Bauer that replaces the command shell
in a Unix environment. After started, it prints out a prompt "#" and reads a
command line terminated by newline. The line is parsed out into a command and
all its arguments.

Note:
Special characters are not handled. This shell is unable to support arguments
with spaces in them. For example, this shell doesn't support file paths with
spaces in them.

How To Run:
1. Open terminal.
2. Navigate to directory where mysh.cpp is stored.
3. run: "g++ mysh.cpp" to compile the program.
4. run: "./a.out" to run the compiled code.

+------------------------------------------------------------------------------+
|                                   Commands                                   |
+------------------------------------------------------------------------------+

# movetodir directory
This command is used to change the current directory (which is stored as an
internal variable) to the specified one. If the specified directory does not
exist, the program will keep the old directory and write an error message.

# movetodir /Home/Documents
/Home/Documents
#

changes current directory to /Home/Documents

# movetodir ..
/Home
#

changes current directory to parent folder

# movetodir Documents
/Home/Documents
#

changes current directory to /Home/Documents

+------------------------------------------------------------------------------+

# whereami
This commands prints the current directory.
Example:

# whereami
/Home/Documents
#

+------------------------------------------------------------------------------+

# history [-c]
This command (without a parameter) prints out the recently typed commands (with
their arguments). If "-c" is passed in as a parameter, it clears the list of
recently typed commands.
Example:

# test
# test 2
# test 3 a
# history
Command History
test
test 2
test 3 a

#

The previous 3 commands and their arguments that were typed in were saved when
they were executed, and displayed when the history command was executed.

# history -c
# history
Command History

#

With the "-c" parameter passed in, the history command clears the list of
recently typed commands.

+------------------------------------------------------------------------------+

# byebye
This command exits the shell program.

+------------------------------------------------------------------------------+

# run program [parameters]
This command executes the program specified in "program". If the argument starts
with a "/", the shell interprets it as a full path. Otherwise, the program will
be interpreted as a relative path starting from the current directory. The
program will be executed with the optional "parameters. The shell uses fork()
and exec() to start the program with the corresponding parameters, and waits
until the program terminates before retruning to the prompt.
Example:

# run /usr/bin/xterm -bg green

This would bring up a terminal with a green background. The prompt would not
return until the terminal is closed.

+------------------------------------------------------------------------------+

# background program [parameters]
This command executes the program specified in "program" similarly to the run
command but it immediately prints the PID of the program it started, and returns
to the prompt.
Example:

# background /usr/bin/xterm
#

This would bring up a terminal. The prompt is returned without needing to close
the terminal.

+------------------------------------------------------------------------------+

# exterminate PID
This command immediately terminates the program with the specified PID
(presumably started from this command line interpreter). It uses the kill()
function call to send SIGKILL signal to the program.
Example with 1 process running with PID of 12345:

#exterminate 12345
#

This would terminate the process with PID of 12345.

+------------------------------------------------------------------------------+

# exterminateall
This command immediately terminates all the programs previously started by the
mysh shell which had not been previosly terminated by it, or by "# exterminate"
Before returning to the prompt, the shell will display the PIDs of the processes
it terminated.
Example with 3 processes running with PIDs of 16000, 31012, and 31013:

# exterminateall
Murdering 3 processes: 16000 31012 31013
#

This would terminate all processes.

+------------------------------------------------------------------------------+

# Repeat n command [...]
This command interprets n as the number of times the command must be run,
command as the path to the program to be executed, and [...] as other
parameters that the command will take in. The command will start the specified
number of program instances, print the PIDs of the created processes and then
return to the prompt, without waiting for the process to terminate.
Example:

# repeat 4 /usr/bin/xterm -bg green
PIDs: 16000 16001 16002 16003
#

This would bring up 4 terminals with their backgrounds set to green.

+------------------------------------------------------------------------------+