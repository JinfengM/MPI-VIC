#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <vicNl.h>
#include <string.h>
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int i,rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Status status;
    MPI_Request send_request, recv_request[size-1];
    if(rank ==0)
    {
        printf("I am master\n");
        for (i = 1; i < size; i++) {
            MPI_Irecv(&rank, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &recv_request[i-1]);
        }
        // 等待所有接收完成
        printf("等待接收消息\n");

        MPI_Waitall(size-1, recv_request, MPI_STATUSES_IGNORE);
        printf("收到信息，完成计算\n");
    }
    if (rank != 0) {
        //get grid number
        FILE *file=NULL;
        int count = 0;  // Line counter (result)
        char c;  // To store a character read from file
        double vars[6]={0.910654,0.436423,24.482024,0.502105,0.988413,0.413380};
        char* outputdir="/root/mjf/VIC/run_lh/divide/";
        char* filename="/root/mjf/VIC/run_lh/divide/output_area_soil.txt";
        int str_len=strlen(outputdir);

        // Open the file
        file = fopen(filename, "r");
        // Check if file exists
        if (file == NULL) {
            printf("Could not open file %s", filename);
            return 0;
        }
        // Extract characters from file and store in variable c
        for (c = getc(file); c != EOF; c = getc(file)) {
            if (c == '\n') { // Increment count if this character is newline
                count = count + 1;
            }
        }
        // Check if last character is not a newline
        if (c == EOF && count > 0 && c != '\n') {
            count = count + 1;
        }
        // Close the file
        fclose(file);
        // Print the number of lines
        printf("The soil grid file %s has %d lines\n", filename, count);

        int nTotal =4; // Set the total number of lines
        int linesPerNode = nTotal / (size - 1); // Exclude the master node from allocation
        int startLine, endLine;

        startLine = (rank - 1) * linesPerNode;
        endLine = startLine + linesPerNode - 1;
        // Handling remainder to ensure each slave node has the same number of lines
        if (rank <= nTotal % (size - 1)) {
            startLine += rank - 1;
            endLine += rank;
        } else {
            startLine += nTotal % (size - 1);
            endLine += nTotal % (size - 1);
        }

        //Obtaining the identifiers of the starting and ending grids.
        printf("Slave %d: Processing lines from %d to %d\n",rank, startLine, endLine);

        vicmain(6,vars,outputdir,str_len,startLine,endLine);
        fprintf(stderr,"vicmain in rank %d is completed\n",rank);
        MPI_Isend(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &send_request);
        // 等待发送完成
        MPI_Wait(&send_request, &status);
        printf("Slave %d: Processing lines from %d to %d completed !\n",rank, startLine, endLine);
    }
    MPI_Finalize();
    return 0;
}
