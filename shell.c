#include "shell.h"

char *builtin_cmds[] = {"cd", "clr", "dir", "environ", "echo", "help","pause", "quit"};
int (*builtin_funcs[])(char **) = {&cmd_cd, &cmd_clr, &cmd_dir, &cmd_environ, &cmd_echo, &cmd_help, &cmd_pause, &cmd_quit};

extern char **environ;

int main(int argc, char *argv[]){
    shell();
    EXIT_SUCCESS;
}

void shell(){
    char cwd[300];
    int loop = 1;
    while(loop != 0){
        getcwd(cwd, sizeof(cwd)); //Get the current directory
        printf("%s> ", cwd); //Print prompt
        char* input1 = get_input(); //Get the shell input
        char** input2 = read_input(input1); //Parse and tokenize the shell input
        loop = exec(input2); //Execute the command that was input
    }
}

char *get_input(void){
    char *input = NULL;
    size_t buffer = 64;
    getline(&input, &buffer, stdin); //Read the shell input and stores it in a buffer
    return input;
}

char **read_input(char *param){ //Read input from buffer and stores the command and its arguments into tokens
    int i = 0;
    int buf = T_BUFSIZE;
    char *token1; //Singular token
    char **token2 = malloc(buf * sizeof(char*)); //Allocate memory for multiple tokens

    if (token2 == NULL){
        printf("An error has occurred\n");
        }

    token1 = strtok(param, DELIM); //Splits up input entered into shell based on delimiters
    while(token1 != NULL){
        token2[i] = token1; //Adds tokens and
        i++;                //reads until entire input is read

        if(i >= buf){ //If more space is needed
            buf += T_BUFSIZE;
            token2 = realloc(token2, buf * sizeof(char*)); //Allocate more memory based on buffer size
            if (token2 == NULL){
                printf("An error has occurred\n");
                }
            }
        token1 = strtok(NULL, DELIM);
        }

    token2[i] = NULL;
    return token2; //Gets the final input after it has been divided into command and its arguments
}

int exec(char **param){ //Execute built-in functions
    int i, s;
    pid_t pid;

    if (param[0] == NULL){ //If no command was entered
        return 1; //Return nothing and keep looping shell
        }

    for (i = 0; i < BUILTINS; i++){
        if (strcmp(param[0], builtin_cmds[i]) == 0){ //If input is within built-in functions
            return (*builtin_funcs[i])(param); //Return given command
        }
    }

    //Forking
    pid = fork();
    if (pid < 0){ //Forking error
        printf("An error has occurred\n");
        }

    else if (pid == 0){ // Child process
        if(execvp(param[0], param) == -1){ //If input command isn't in args/Cannot be found
            printf("An error has occurred\n");
        }
        exit(0);
    }
    else { //Else, parent process
        do{
        waitpid(pid, &s, WUNTRACED); //Wait for child process
        }
        while(!WIFEXITED(s) && !WIFSIGNALED(s));
        }
    return 1;
}

/****************************************************************************************************/
/***************************************** BUILT-IN COMMANDS ****************************************/
/****************************************************************************************************/

int cmd_cd(char **param){ //Built-in function for "cd" command
    if(param[1] == NULL){ //If nothing is input after "cd"
        chdir(getenv("HOME")); //Change from current directory to home/root directory
        }
    else{
        chdir(param[1]); //Else change to the directory that is input after "cd"
    }
    return 1;
}

int cmd_clr(){ //Built-in function for "clr" command
    printf("\033[H\033[2J");
    return 1;
    }

int cmd_dir(char **param){ //Built-in function for "dir" command
    DIR *directory;
    struct dirent *dir;
    if(param[1] == NULL){ //If no argument after "dir"
        directory = opendir("."); //Get the directory contents of the current directory
        }
    else{
        if((directory = opendir(param[1])) == NULL){ //Check to see if directory exists. If it doesn't exist
            printf("Directory does not exist: %s.\n", param[1]); //Then print error message
            return 1;
            }
        }

    while((dir = readdir(directory)) != NULL){ //While directory isn't empty
        printf("%s \n", dir->d_name); //Gets name of each file in directory
    }
    printf("\n");
    closedir(directory);
    return 1;
    }

int cmd_echo(char **param){ //Built-in function for "echo" command
    int i = 1;
    while(param[i] != NULL){ //If there is an input after "echo"
        printf("%s ", param[i]); //Print the input word by word
        i++; //Increment until end of line/phrase/sentence
        }
        printf("\n"); //New line to ensure it works like the UNIX "echo" command
    return 1;
    }

int cmd_environ(){ //Built-in function for "environ" command
    for(int i = 0; environ[i] != NULL; i++) //Accesses extern char **environ and reads through it
        printf("%s\n", environ[i]); //Prints environment variables to screen
    return 1;
    }

int cmd_help(){ //Built-in function for "help" command
    FILE *readme;
    char filename[6];
    char ch;

    readme = fopen("readme.txt", "r"); //Open "readme" file for reading
    if (readme == NULL){ // If readme file doesn't exist or can't be found
        printf("File not found\n"); //Print error message
        return 1;
        }
    else{
        while ((ch = fgetc(readme)) != EOF){ //While end of file is not reached
            printf("%c", ch); //Print contents of readme file
            }
        }
    fclose(readme); //Close readme file
    return 1;
    }


int cmd_pause(){ //Built-in function for "pause" command
    printf("The shell has been paused. Press the 'ENTER' key to continue using the shell.");
    while (getchar() != '\n'); //Message stays on screen until 'ENTER' key is pressed
    return 1;
    }

int cmd_quit(char **param){ //Built-in function for "quit" command
    if(param[1] != NULL){ //If there's an argument
        printf("An error has occured\n");
        return 1;
    }
    else{
    printf("Closing shell...\n");
    return 0; //Close shell and exit loop by returning 0
    }
    return 0;
}
