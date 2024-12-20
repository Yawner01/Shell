# Project 1

The purpose of this project is to design and develop a comprehensive shell interface that enhances process control, user interaction, and error handling mechanisms. By constructing this shell, you will gain valuable insights into the intricacies of operating system processes, command-line parsing, and the robustness required for error-free execution.

## Group Members
- **Wesley Yawn**: way23@fsu.edu
- **Jose Rodriguez**: jmr21a@fsu.edu
- **Eli Butters**: ebb21g@fsu.edu
## Division of Labor

### Part 1: Prompt
- **Responsibilities**: The user will be greeted with a prompt that should indicate the absolute working directory, the user name, and the machine name.
- **Assigned to**: Eli Butters

### Part 2: Environment Variables
- **Responsibilities**: In the context of program execution within its own environment, you will undertake the task of replacing tokens prefixed with the dollar sign character ($) with their corresponding values.
- **Assigned to**: Eli Butters

### Part 3: Tilde Expansion
- **Responsibilities**: In Bash, tilde (~) may appear at the beginning of a path, and it expands to the environment variable $HOME.
- **Assigned to**: Eli Butters, Wesley Yawn

### Part 4: $PATH Search
- **Responsibilities**: In Bash, when a command like "ls" is entered, the shell is able to execute the corresponding program/executable located at "/usr/bin/ls" through a process called path search. This path search is not magical, but rather a straightforward search conducted within a predefined list of directories. The list of directories is specified in the environment variable $PATH.
- **Assigned to**: Eli Butters, Wesley Yawn

### Part 5: External Command Execution
- **Responsibilities**: Once you have obtained the path to the program you intend to execute, either because the command included a slash or through the $PATH search, the next step is to execute the external command. However, executing an external command requires more than just a single line of code using the execv() function. To accomplish this, a two-step process is involved. First, you need to fork() to create a child process. The child process will be responsible for executing the desired command using the execv() function. This separation between the parent and child processes ensures that the execution of the command does not interfere with the operation of the shell itself.
- **Assigned to**: Wesley Yawn

### Part 6: I/O Redirection
- **Responsibilities**: In this section, we will focus on implementing input/output (I/O) redirection from/to a file. By default, the shell receives input from the keyboard and outputs to the screen. However, with I/O redirection, we can replace the keyboard with input from a specified file and redirect output to a designated file.
- **Assigned to**: Eli Butters, Wesley Yawn

### Part 7: Piping
- **Responsibilities**: Beyond simple I/O redirection, we will explore the concept of piping, a more sophisticated form of I/O manipulation. Contrary to regular I/O redirection, piping involves the simultaneous execution of multiple commands, with the input and output of these commands interconnected. This setup allows the output generated by the initial command to be seamlessly passed as input to the subsequent command.
- **Assigned to**: Jose Rodriguez, Eli Butters

### Part 8: Background Processing
- **Responsibilities**: The final functionality we will incorporate is background processing. Thus far, the shell has been waiting to prompt for additional user input whenever there were external commands running. Background processing allows the shell to execute external commands without waiting for their completion. However, it is still essential for the shell to keep track of their completion status periodically.
- **Assigned to**: Eli Butters, Wesley Yawn

### Part 9: Internal Command Execution
- **Responsibilities**: Having completed external command execution, the next aspect to address is the implementation of internal commands, often referred to as built-in functions. These functions are natively supported by the shell and will be integrated into your implementation.
- **Assigned to**: Jose Rodriguez, Wesley Yawn

### Extra Credit
- **Responsibilities**: Support unlimited number of pipes
- **Assigned to**: Jose Rodriguez, Eli Butters

### Extra Credit
- **Responsibilities**: Support piping and I/O redirection in a single command
- **Assigned to**: Eli Butters

### Extra Credit
- **Responsibilities**: Shell-ception: Execute your shell from within a running shell process repeatedly
- **Assigned to**: Wesley Yawn

## File Listing
```
root/
└── bin/
    └── .gitkeep
└── include/
    └── commands.h
    └── env_utils.h
    └── exec_utils.h
    └── job_utils.h
    └── lexer.h
└── obj/
    └── .gitkeep
└── src/
    └── commands.c
    └── env_utils.c
    └── exec_utils.c
    └── job_utils.c
    └── lexer.c
└── .gitignore
└── Makefile
└── README.md

```
## How to Compile & Execute

### Requirements
- **Compiler**: `gcc`
- **Dependencies**: 

### Compilation
```bash
make
```
This will build the executable in root/bin/
### Execution
```bash
make run
```
This will run the program immediately after compiling

## Bugs
- **Bug 1**: No current bugs

## Extra Credit
- **Extra Credit 1:**: Support unlimited number of pipes 
- **Extra Credit 2:**: Support piping and I/O redirection in a single command
- **Extra Credit 3:**: Shell-ception: Execute your shell from within a running shell process repeatedly

## Considerations
This operating system is designed for linprog running Linux, so interaction with environment variables on different systems and kernels might lead to small errors such as $MACHINE being stored in another environment variable.
