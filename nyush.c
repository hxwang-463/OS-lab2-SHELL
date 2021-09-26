#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct {
    char **command;
    int input_fd;
    char* input_file;
    int output_fd;
    char* output_file;
    struct vari* next;
}Link;
Link* head = NULL;



void sig_int(int x){
    printf("\n");
    return;
}

int my_system(char **argv, int io_redirect, char* redirect_path){
    
    int pid;
    if (pid=fork() == 0) {
        if(io_redirect){
            int fp;
            if(io_redirect==1){
                fp = open(redirect_path, O_RDWR);
                if(fp==-1){
                    fprintf(stderr, "Error: invalid file\n");
                    exit(-1);}
                dup2(fp, STDIN_FILENO);}
            else{
                if(io_redirect==2){fp = open(redirect_path, O_WRONLY|O_CREAT|O_TRUNC, 00600);}
                else if(io_redirect==3){fp = open(redirect_path, O_RDWR|O_CREAT|O_APPEND, 00600);}
                else{fprintf(stderr, "Unknown Error 1\n");exit(-1);}
                if(fp==-1){
                    fprintf(stderr, "Error: invalid file\n");
                    exit(-1);}
                dup2(fp, STDOUT_FILENO);}
        }
        

        char* prog_name = (char*)malloc(1001*sizeof(char));
        if(argv[0][0]==0x2f||argv[0][0]==0x2e){
            execv(argv[0], argv);
            fprintf(stderr, "Error: invalid progarm\n");
            exit(-1);
        }
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

void command_parser(char* command_line, int is_first_command, int is_last_command, int input_fd, int output_fd){ //no pipe in this function

    char command_line_new[1001];
    memcpy(command_line_new,command_line, 1001);
    char* command;
    char* rest_command = command_line_new;
    command = strtok_r(rest_command, " ", &rest_command);
    if(!strcmp(command, "exit")){
        if(!is_first_command||!is_last_command){
            fprintf(stderr, "Error: invalid command\n");
            return;
        }
        if(strtok_r(rest_command, " ", &rest_command)){
            fprintf(stderr, "Error: invalid command\n");
            return;
        }
        /* 检查是否有暂停程序

        */
        printf("exit\n");
        int pid = getpid();
        kill(pid, SIGKILL);
    }
    else if(!strcmp(command, "cd")){
        if(!is_first_command||!is_last_command){
            fprintf(stderr, "Error: invalid command\n");
            return;
        }
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
        if(!is_first_command||!is_last_command){
            fprintf(stderr, "Error: invalid command\n");
            return;
        }



        return;
    }
    else if(!strcmp(command, "fg")){
        if(!is_first_command||!is_last_command){
            fprintf(stderr, "Error: invalid command\n");
            return;
        }



        return;
    }

    Link *one_command=(Link*)malloc(sizeof(Link));


    char** argv;
    argv = (char**)malloc(sizeof(char*)*1001);
    
    int io_redirect = 0; // 0:default 1:< 2:> 3:>>
    char redirect_path[1001] = {0};
    int i = 0;
    while (command){
        if(!strcmp(command, "<")||!strcmp(command, ">")||!strcmp(command, ">>")){
            if(!strcmp(command, "<")){
                io_redirect = 1;
                if(!is_first_command){
                    fprintf(stderr, "Error: invalid command\n");
                    return;}}
            else if(!strcmp(command, ">")){
                io_redirect = 2;
                if(!is_last_command){
                    fprintf(stderr, "Error: invalid command\n");
                    return;}}
            else{
                io_redirect = 3;    
                if(!is_last_command){
                    fprintf(stderr, "Error: invalid command\n");
                    return;}}
            command = strtok_r(rest_command, " ", &rest_command);
            if(command == NULL){
                fprintf(stderr, "Error: invalid command\n");
                return;}
            strcpy(redirect_path, command);
            command = strtok_r(rest_command, " ", &rest_command);
            if(command != NULL){ //
                fprintf(stderr, "Error: invalid command\n");
                return;}
            argv[i+1] = NULL;
            my_system(argv, io_redirect, redirect_path);
            return;
        }
        argv[i]  = (char*)malloc(1001*sizeof(char));
        strcpy(argv[i], command);
        i=i+1;
        command = strtok_r(rest_command, " ", &rest_command);
    }
    argv[i] = NULL;

    one_command->command = (char**)malloc(sizeof(argv));
    memcpy(one_command->command, argv, sizeof(argv));

    my_system(argv, 0, NULL);
}


void command_line_parser(char* command_line){  //deal with pipe
    char command_line_new[1001];
    memcpy(command_line_new, command_line, 1001);
    char* command_prev = NULL; 
    char* command; 
    char* command_next; 
    char* rest_command = command_line_new;
    int is_first_command, is_last_command;
    command = strtok_r(rest_command, "|", &rest_command);
    int pipefd[2];
    int input_fd, output_fd;
    while (command){
        command_next = strtok_r(rest_command,  "|", &rest_command);
        if(command_prev==NULL) is_first_command = 1; else is_first_command = 0;
        if(command_next==NULL) is_last_command = 1; else is_last_command = 0;
        if(command_prev){ //input from previous
            input_fd = pipefd[0];
        }
        else input_fd = -1;
        if(command_next){ //output to next
            pipe(pipefd);
            output_fd = pipefd[1];
        }
        else output_fd = -1;
        
        if(command[0]==0x20)command = command+1;
        command_parser(command, is_first_command, is_last_command, input_fd, output_fd);
        command_prev=command;
        command=command_next;
    }


}


int main(){
    signal(SIGINT, sig_int);
    signal(SIGQUIT, sig_int);
    signal(SIGTERM, sig_int);
    signal(SIGSTOP, sig_int);
    signal(SIGTSTP, sig_int);

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