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
        }
    }



    while (command){
        // printf("%s\n", command);
        command = strtok_r(rest_command, " ", &rest_command);
    }
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
        command_line[strlen(command_line)-1] = 0;
        // printf("%s", command_line);
        command_line_parser(command_line);




    }
}