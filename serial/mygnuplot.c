#include "mygnuplot.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>


void open_gnuplot(const int nchannels){
    pid_t pid = fork();
    if(pid < 0){
        perror("Error in creating a new child process");
        exit(EXIT_FAILURE);
    }
    FILE* file;
    char name[20];
    int result;
    
    const char *gnuplot_command = 
            "gnuplot -persist -e \""
            "plot [0:100] [0:6] '< tail -n100 datafile0' title 'Grafico Datafile0' with lines; "
            "while (1) { sleep(0.4); replot; }"
            "\"";
    if(pid == 0){
        //waits 5 seconds before starting to plot so that I've something to see
        usleep(500000);
        printf("[CHILD] creating (gnu)plots %d...\n", getpid());
        for(int i = 0; i < nchannels; i++){
            sprintf(name, "datafile%d", i);
            file = fopen(name, "w");
            if(file == NULL){
                perror("Error in creating a new child process");
                exit(EXIT_FAILURE);
            }
            fputc('0', file);
            fclose(file);
            result = system(gnuplot_command);
            if (result == -1) {
                perror("Error while executing gnuplot");
                exit(EXIT_FAILURE);
            }
        }
        _exit(EXIT_SUCCESS);
    }
}
