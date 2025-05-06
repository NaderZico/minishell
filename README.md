<pre markdown="1">

                          ┌────────────────────┐
                          │   minishell REPL   │
                          └────────────────────┘
                                     │
                                     ▼
                          ┌────────────────────┐
                          │ readline("…> ")    │
                          └────────────────────┘
                                     │
                      ┌──────────────┴──────────────┐
                      │                             │
                  EOF?│                         non-empty?
                      │ Yes                         │ Yes
                      ▼                             ▼
                 ┌──────────┐                  ┌──────────────┐
                 │  exit    │                  │ add_history  │
                 └──────────┘                  └──────────────┘
                                                    │
                                                    ▼
                                             ┌──────────────┐
                                             │ tokenize_    │
                                             │ input()      │
                                             └──────────────┘
                                                    │
                                                    ▼
                                             ┌──────────────┐
                                             │ validate_    │
                                             │ syntax()     │
                                             └──────────────┘
                                                    │
                                             ┌──────┴──────┐
                                             │             │
                                       syntax OK? No      Yes
                                             │             │
                                             ▼             ▼
                                       ┌──────────┐   ┌──────────────┐
                                       │ free_all │   │ expand_      │
                                       └──────────┘   │ tokens()     │
                                                      └──────────────┘
                                                             │
                                                             ▼
                                                      ┌──────────────┐
                                                      │ parse_       │
                                                      │ tokens()     │
                                                      └──────────────┘
                                                             │
                                                      ┌──────┴──────┐
                                                      │             │
                                                  done cmds?    loop
                                                      │             │
                                                      ▼             ▼
                                                ┌──────────┐    back to
                                                │ free_all │    readline
                                                └──────────┘
------------------------------------------------------------------------------------

Flow diagram visualizing the relationship between the parsing and execution

            1. struct contains parsed data required for the execution
                       ┌──────────────────────────────┐
                       │           t_data             │
                       ├──────────────────────────────┤
                       │ tokens:    t_token[MAX_TOKENS] │
                       │ token_count: int             │
                       │                              │
                       │ commands:  t_command[MAX_CMD]│◀─── populated by parser()
                       │ cmd_count:  int              │
                       │                              │
                       │ env:       char **           │
                       │ last_status: int             │
                       └──────────────────────────────┘
                                      │
                                      ▼
                              (pass pointer to)
                                      │
            2. *prototype* struct to handle execution of the parsed commands
                       ┌──────────────────────────────┐
                       │         t_executor           │
                       ├──────────────────────────────┤
                       │ • commands: t_command *      │◄── from t_data.commands
                       │ • cmd_count:  int            │◄── from t_data.cmd_count
                       │ • env:        char **        │◄── from t_data.env
                       │ • last_status: int           │◄── from t_data.last_status
                       │ • pipes:      int (*)[2]     │
                       │ • pids:       pid_t *        │
                       └──────────────────────────────┘
                                      │
                                      ▼
                       executor() loops over commands[]
                       and uses each t_command.redirs[] internally

Detailed Struct Relationships:
1. t_data is the root container:
   - Contains ALL tokens from tokenization phase
   - Contains ALL commands from parsing phase
   - Maintains environment and exit status

2. t_token details:
   - WORD type stores expanded value string
   - REDIR/PIPE types have NULL value
   - quote field guides expansion rules

3. t_command contains execution context:
   - args array is NULL-terminated for execve()
   - redirs array stores all I/O redirections
   - pipe_in/out manage inter-process communication

4. t_redir specifies I/O manipulation:
   - type determines operation (>, >>, <, <<)
   - file stores target filename/delimiter
-------------------------------------------------
Key Execution Steps Using Structs:
1. Pipe Creation:
   - Create N-1 pipes for N commands using pipe()
   - Store read/write ends in commands[i].pipe_out 
     and commands[i+1].pipe_in

2. Redirection Handling:
   for each t_redir in command:
   - REDIR_IN:  open(file, O_RDONLY)
   - REDIR_OUT: open(file, O_WRONLY|O_CREAT...)
   - dup2(fd, STDIN/STDOUT)

3. Process Execution:
   execve(command.args[0], command.args, data.env)
   - args[0] must be full path from PATH lookup
   - args array includes all command parameters

4. Builtins Handling:
   if args[0] is cd/echo/exit etc.:
   - Execute directly in parent process
   - Modify data.env or data.last_status

5. Signal Propagation:
   - Parent process handles SIGINT/SIGQUIT
   - Child processes reset to default signals

------------------------------------------------------------------------

Example 1

1. Tokenization
┌───────────────────────────────────────────────────────┐
│                Input: "ls -l | grep c > out.txt"      │
└───────────────────────────────────────────────────────┘
                               │
                               ▼
┌───────────────────────────────────────────────────────┐
│             struct   t_data.tokens[]                  │
│ ┌───────┬───────────┬───────────────┬───────────────┐ │
│ │ Index │  value    │     type      │    quote      │ │
│ ├───────┼───────────┼───────────────┼───────────────┤ │
│ │   0   │  "ls"     │     WORD      │   NO_QUOTE    │ │
│ │   1   │  "-l"     │     WORD      │   NO_QUOTE    │ │
│ │   2   │   NULL    │     PIPE      │   NO_QUOTE    │ │
│ │   3   │  "grep"   │     WORD      │   NO_QUOTE    │ │
│ │   4   │   "c"     │     WORD      │   NO_QUOTE    │ │
│ │   5   │   NULL    │  REDIR_OUT    │   NO_QUOTE    │ │
│ │   6   │ "out.txt" │     WORD      │   NO_QUOTE    │ │
└───────────────────────────────────────────────────────┘

2. Parsing
┌───────────────────────────────────────────────────────┐
│            struct       t_data.commands[]             │
├───────────────────────────────────────────────────────┤
│  Command 0: "ls -l"                                   │
│  ┌─────────────────────────────────────────────────┐  │
│  │ args: ["ls", "-l", NULL]                        │  │
│  │ redirs: []                                      │  │
│  │ redir_count: 0                                  │  │
│  │ pipe_in: 0 (stdin)                              │  │
│  │ pipe_out: pipe_fd[1] (write end of pipe)        │  │
│  └─────────────────────────────────────────────────┘  │
│                                                       │
│  Command 1: "grep c > out.txt"                        │
│  ┌─────────────────────────────────────────────────┐  │
│  │ args: ["grep", "c", NULL]                       │  │
│  │ redirs: { file: "out.txt", type: REDIR_OUT }    │  │
│  │ redir_count: 1                                  │  │
│  │ pipe_in: pipe_fd[0] (read end of pipe)          │  │
│  │ pipe_out: 1 (stdout)                            │  │
│  └─────────────────────────────────────────────────┘  │
└───────────────────────────────────────────────────────┘

3. Execution (later)
        ┌───────────────────┐               ┌───────────────────┐
        │    Command 0      │               │    Command 1      │
        │ (ls -l)           │               │ (grep c > out.txt)│
        ├───────────────────┤               ├───────────────────┤
        │ fork()            │               │ fork()            │
        │ ┌───────────────┐ │               │ ┌───────────────┐ │
        │ │ Child Process │ │               │ │ Child Process │ │
        │ │ - dup2(pipe_out, STDOUT)        │ │ - dup2(pipe_in, STDIN) │
        │ │ - close unused pipe ends        │ │ - open("out.txt", O_WRONLY) │
        │ │ - execve("/bin/ls", args)       │ │ - dup2(fd, STDOUT) │
        │ └───────────────┘ │               │ │ - execve("/bin/grep", args) │
        └───────────────────┘               └───────────────────┘
<<<<<<< HEAD

---------------------------------------------------------------

Example 2

1. Tokenization
┌───────────────────────────────────────────────────────┐
│   Input: "echo \"Hello $USER\" | cat > greeting.txt"  │
└───────────────────────────────────────────────────────┘
                               │
                               ▼
┌───────────────────────────────────────────────────────┐
│                t_data.tokens[]                        │
│ ┌───────┬──────────────────┬──────────────┬──────────┐ │
│ │ Index │      value       │    type      │  quote   │ │
│ ├───────┼──────────────────┼──────────────┼──────────┤ │
│ │   0   │     "echo"       │    WORD      │ NO_QUOTE │ │
│ │   1   │   "Hello $USER"  │    WORD      │ DBL_QUOTE│ │
│ │   2   │      NULL        │    PIPE      │ NO_QUOTE │ │
│ │   3   │     "cat"        │    WORD      │ NO_QUOTE │ │
│ │   4   │      NULL        │  REDIR_OUT   │ NO_QUOTE │ │
│ │   5   │  "greeting.txt"  │    WORD      │ NO_QUOTE │ │
└───────────────────────────────────────────────────────┘

2. Expansion
┌───────────────────────────────────────────────────────┐
│          After expanding $USER (e.g., "nakhalil")     │
│ ┌───────┬──────────────────┬──────────────┬──────────┐ │
│ │   1   │ "Hello nakhalil" │    WORD      │ DBL_QUOTE│ │
└───────────────────────────────────────────────────────┘

3. Parsing
┌───────────────────────────────────────────────────────┐
│                   t_data.commands[]                   │
├───────────────────────────────────────────────────────┤
│  Command 0: "echo \"Hello $USER\""                    │
│  ┌─────────────────────────────────────────────────┐  │
│  │ args: ["echo", "Hello nakhalil", NULL]          │  │
│  │ redirs: []                                      │  │
│  │ redir_count: 0                                  │  │
│  │ pipe_in: 0 (stdin)                              │  │
│  │ pipe_out: pipe_fd[1] (write end)                │  │
│  └─────────────────────────────────────────────────┘  │
│                                                       │
│  Command 1: "cat > greeting.txt"                      │
│  ┌─────────────────────────────────────────────────┐  │
│  │ args: ["cat", NULL]                             │  │
│  │ redirs: { file: "greeting.txt", type: REDIR_OUT }│  │
│  │ redir_count: 1                                  │  │
│  │ pipe_in: pipe_fd[0] (read end)                  │  │
│  │ pipe_out: 1 (stdout)                            │  │
│  └─────────────────────────────────────────────────┘  │
└───────────────────────────────────────────────────────┘

</pre>
</pre>
>>>>>>> 67c3b0989c652b5a84dddb0957810acdc1d38ae6
