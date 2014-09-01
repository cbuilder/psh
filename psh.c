#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define PRINTFD	stdout
#define READFD  stdin
#define PROMPT_GREETING "Fxxxxxx's GXXXXXX/GXX Xeference Xtation"
#define PROMPT_PREFIX "pipe"
#define CMD_PATH "/home/dima/workspace/6.828/work/shell/"
#define CMD_FILENAME_MAXLEN 64
#define CMD_MAX_ARGS  8

typedef struct cmd
{
    char *args[CMD_MAX_ARGS];
} cmd_t;

void print_help()
{
    DIR *cmd_dir;
    struct dirent *cmd_dir_entry;
    fprintf(PRINTFD, "Available commands:\n");
    fprintf(PRINTFD, "help\n");
    cmd_dir = opendir(CMD_PATH);
    while((cmd_dir_entry = readdir(cmd_dir)) != NULL)
        fprintf(PRINTFD, "%s\n", cmd_dir_entry->d_name);
}

int parse(cmd_t *cmd, char *usrinput)
{
    char * pch = NULL;
    int i = 0;
    if (usrinput != NULL) {
    memset(cmd, 0, sizeof(cmd_t));
    for (i = strlen(usrinput) - 1; i >= 0; i--) {
        if (isspace(usrinput[i]))
            usrinput[i] = 0;
        else
            break;
    }
    for (i = strlen(usrinput) - 1; i >= 0; i--) {
        if (!isprint(usrinput[i]))
            return 1;
    }
    pch = strtok(usrinput," ");
    cmd->args[0] = pch;
    i = 1;
    while (pch != NULL && i <= CMD_MAX_ARGS) {
            pch = strtok(NULL, " ");
            cmd->args[i++] = pch;
        }
    }
    return 0;
}

int execute(cmd_t *cmd)
{
    pid_t pid;
    struct stat exists;
    int ret = 0;
    char filename[CMD_FILENAME_MAXLEN];
    char filename_len_count = 0;
    char *cmd_name = cmd->args[0];
    memset(filename, 0, CMD_FILENAME_MAXLEN);
    if (cmd_name == NULL)
        return ret;
    if (!strcmp(cmd_name, "bash"))
        exit(EXIT_SUCCESS);
    else if (!strncmp(cmd_name, "help", 4))
        print_help();
    else if (strlen(CMD_PATH) + strlen(cmd_name) >= CMD_FILENAME_MAXLEN)
        fprintf(PRINTFD, "Unknown command\n");
    else {
        strcat(filename, CMD_PATH);
        strcat(filename, cmd_name);
        if (stat(filename, &exists) < 0) {
            fprintf(PRINTFD, "Unknown command\n");
            return 1;
        }
        pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            ret = execv(filename, cmd->args);
            if (ret == -1)
                perror("execve");
            else
                ret = 0;
        }
        wait(&ret);
    }
    return ret;
}

int main()
{
    char incmd[64];
    cmd_t cmd;
    fprintf(PRINTFD, "*********************************************\n");
    fprintf(PRINTFD, "*  %s  *\n", PROMPT_GREETING);
    fprintf(PRINTFD, "*********************************************\n");
    fprintf(PRINTFD, "CLI at %s\n", ttyname(fileno(PRINTFD))); //debug
    while(1) {
        fprintf(PRINTFD, "%s>", PROMPT_PREFIX);
        memset(incmd, 0, sizeof(incmd));
        fgets(incmd, 64, READFD);
        if (parse(&cmd, incmd) == 0)
	    execute(&cmd);
        else
            fprintf(PRINTFD, "Bad command\n");
    }
    return 0;
}
