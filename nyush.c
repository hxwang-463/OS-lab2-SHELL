#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>

void sig_int(int x){}

int my_system(char **argv) {
    if (fork() == 0) {
        char* prog_name = (char*)malloc(1001*sizeof(char));
        if(argv[0][0]==0x2f|argv[0][0]==0x2e)execv(argv[0], argv);
        char prog_root[1001] = {0};
        strcpy(prog_root, "/bin/");
        strcat(prog_root, argv[0]);
        execv(prog_root, argv);
        strcpy(prog_root, "/usr/bin/");
        strcat(prog_root, argv[0]);
        execv(prog_name, argv);
        fprintf(stderr, "Error: invalid progarm\n");
        exit(-1);
    }
    else{
        wait(NULL);
    }
    return 0;
}

void command_line_parser(char* command_line){

    char command_line_new[1001];
    memcpy(command_line_new,command_line, 1001);
    char* command;
    char* rest_command = command_line_new;
    command = strtok_r(rest_command, " ", &rest_command);
    if(!strcmp(command, "exit")){
        if(strtok_r(rest_command, " ", &rest_command)){
            fprintf(stderr, "Error: invalid command\n");
            return;
        }
        /* 检查是否有暂停程序

        */
        int pid = getpid();
        kill(pid, SIGKILL);
    }
    else if(!strcmp(command, "cd")){
        char *path = strtok_r(rest_command, " ", &rest_command);
        if(strtok_r(rest_command, " ", &rest_command)){
            fprintf(stderr, "Error: invalid command\n");
            return;
        }
        char working_directory[1024] = {0};
        getcwd(working_directory, sizeof(working_directory));
        int check;
        check = chdir(path);
        if(check == -1){
            fprintf(stderr, "Error: invalid directory\n");
            return;
        return;
        }
    }
    else if(!strcmp(command, "jobs")){

    }
    else if(!strcmp(command, "fg")){

    }


    char** argv;
    argv = (char**)malloc(sizeof(char*)*1001);
    int i = 0;

    while (command){
        argv[i]  = (char*)malloc(1001*sizeof(char));
        strcpy(argv[i], command);
        i=i+1;
        command = strtok_r(rest_command, " ", &rest_command);
    }
    argv[i] = NULL;
    my_system(argv);
}

int main(){
    signal(SIGINT, sig_int);

    char working_directory[1024] = {0};
    char* current_folder;
    char slash = 0x2f;
    char* command_line = (char*)malloc(sizeof(char)*1002);
    while(1){
        getcwd(working_directory, sizeof(working_directory));
        current_folder = strrchr(working_directory, slash);
        printf("[nyush %s]$ ", current_folder+1);
        fgets(command_line, 1001, stdin);
        if(command_line[0] == 0x0A)continue;
        command_line[strlen(command_line)-1] = 0;
        
        command_line_parser(command_line);




    }
}