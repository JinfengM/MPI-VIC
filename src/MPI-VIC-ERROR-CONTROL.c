#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vicNl.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>

#define MASTER_RANK 0
#define TAG_START 100
#define TAG_END   200
#define MSG_LEN   16

void rout_();
//计算文件的行数，并分为npart块，返回每块的个数。比如10分成三块，最后返回[4,3,3]
int* DivideFilePart(char* filename,int npart) {
    FILE *file=NULL;
    int i,count = 0;  // Line counter (result)
    char c;  // To store a character read from file

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
    printf("The file %s has %d lines\n", filename, count);

    int* parts = (int*) malloc(npart * sizeof(int));
    int quotient = count / npart;
    int remainder = count % npart;

    for (i = 0; i < npart; i++) {
        parts[i] = quotient;
        if (remainder > 0) {
            parts[i]++;
            remainder--;
        }
    }

    return parts;//注意释放 free(parts);
}
/*
ExtractSoilData函数从配置文件中提取土壤数据文件路径。

功能:
1. 打开指定的配置文件
2. 查找以"SOIL"开头的行
3. 提取该行中"SOIL"后、"#"注释前的文件路径
4. 去除路径字符串两端的空格
5. 返回提取的文件路径

参数:
- filename: 配置文件路径
返回值:
- 成功时返回提取的文件路径字符串
- 失败时返回NULL

示例配置文件行:
SOIL        ./soil/soil.txt    # soil parameter path
*/

char* ExtractSoilData(char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        return NULL;
    }

    char* result = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&result, &len, file)) != -1) {
        if (strncmp(result, "SOIL", 4) == 0) {
            char* start = result + 4; // 跳过"SOIL"
            char* end = strchr(start, '#');
            if (end != NULL) {
                *end = '\0'; // 截断到'#'字符
            }
            // 去掉左右空格
            char* trimmed = strtok(start, " \t\n");
            fclose(file);
            return strdup(trimmed); // 返回路径部分
        }
    }

    fclose(file);
    free(result);
    return NULL; // 如果没有找到以"SOIL"开头的行
}

int main(int argc, char** argv) {
  //for (int i = 0; i < argc; i++) {
  //  printf("Argument %d: %s\n", i, argv[i]);
  //}
  char cwd[1024]={0};
  getcwd(cwd, sizeof(cwd));
  printf("Current directory is: %s\n", cwd);
  double vars[6]={0};
  vars[0]=atof(argv[3]);
  vars[1]=atof(argv[4]);
  vars[2]=atof(argv[5]);
  vars[3]=atof(argv[6]);
  vars[4]=atof(argv[7]);
  vars[5]=atof(argv[8]);
  char  outputdir[1024]={0};
  strcpy(outputdir,cwd);
  strcat(outputdir,"/");
  strcat(outputdir,argv[2]);
  char* filename = ExtractSoilData(argv[2]);
  //printf("Soil data file: %s\n",filename);
  // 复制 filename 的内容到一个新的字符串
  char* filename_copy = strdup(filename);
  char* dir = dirname(filename_copy); // 使用复制的字符串
  //printf("Directory extracted from filename: %s\n", dir);
  //printf("Soil data file: %s\n", filename);
  char obj_dir[1024]={0};
  //计算完成后汇流模拟结果存储文件夹
  strcpy(obj_dir,dir);
  strcat(obj_dir,"/chanliu_result/");
  //double vars[6]={0.910654,0.436423,24.482024,0.502105,0.988413,0.413380};
  MPI_Init(&argc, &argv);

  int world_size,i;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int NUM_SLAVES = world_size - 1;
  char start_msg[] = "START";
  char end_msg[] = "END";
  char error_msg[] = "ERROR";

  MPI_Request* requests = (MPI_Request*)malloc(2 * NUM_SLAVES * sizeof(MPI_Request));
  char** recv_end_msgs = (char**)malloc(NUM_SLAVES * sizeof(char*));

  if (requests == NULL || recv_end_msgs == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory\n");
    free(requests);
    free(recv_end_msgs);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  for (i = 0; i < NUM_SLAVES; i++) {
    recv_end_msgs[i] = (char*)malloc(MSG_LEN * sizeof(char));
    if (recv_end_msgs[i] == NULL) {
      fprintf(stderr, "Error: Failed to allocate memory\n");
      free(recv_end_msgs[i]);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
  }

  if (world_rank == MASTER_RANK) {
      clock_t start = clock();
    fprintf(stderr,"Master sending 'START' to --%d-- slave nodes.\n",NUM_SLAVES);

    for (i = 1; i <= NUM_SLAVES; i++) {
      MPI_Isend(start_msg, strlen(start_msg) + 1, MPI_CHAR, i, TAG_START, MPI_COMM_WORLD, &requests[i-1]);
    }

    for (i = 1; i <= NUM_SLAVES; i++) {
      MPI_Irecv(recv_end_msgs[i-1], MSG_LEN, MPI_CHAR, i, TAG_END, MPI_COMM_WORLD, &requests[NUM_SLAVES + i - 1]);
      }

    MPI_Waitall(2 * NUM_SLAVES, requests, MPI_STATUSES_IGNORE);
    
    // Check for errors from slave nodes
    int error_detected = 0;
    for ( i = 1; i <= NUM_SLAVES; i++) {
        fprintf(stderr,"Master received message: %s from --%d--.\n",recv_end_msgs[i-1],i);
        if (strcmp(recv_end_msgs[i-1], "ERROR") == 0) {
            fprintf(stderr, "Master: ERROR detected from rank %d, aborting all processes...\n", i);
            error_detected = 1;
            break;
        }
    }
    
    if (error_detected) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
      clock_t end = clock();
      double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
      fprintf(stderr,"%d Slaves time taken: %f seconds\n",NUM_SLAVES, time_spent);
      printf("Commencing Routing module\n");
      rout_();
      //汇流模拟结果存储文件读取
      printf("%s\n",obj_dir);
  } else
  {
    //Slave run here
    fprintf(stderr,"Slave --%d-- receiving 'START' to run VIC model.\n",world_rank);
    char recv_start_msg[MSG_LEN];
    MPI_Request request;
    MPI_Recv(recv_start_msg, MSG_LEN, MPI_CHAR, MASTER_RANK, TAG_START, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    fprintf(stderr,"Slave %d received message: %s.\n",world_rank,recv_start_msg);
    if (strcmp(recv_start_msg, "START") == 0) {
        fprintf(stderr,"Slave %d working.\n",world_rank);
        FILE *file=NULL;
        int count = 0;  // Line counter (result)
        char c;  // To store a character read from file
        //fprintf(stderr,"Soil data file: %s\n",filename);
        //int str_len=strlen(outputdir);

        // Open the file
        file = fopen(filename, "r");
        // Check if file exists
        if (file == NULL) {
            fprintf(stderr, "Rank %d: Could not open file %s\n", world_rank, filename);
            MPI_Send(error_msg, strlen(error_msg) + 1, MPI_CHAR, MASTER_RANK, TAG_END, MPI_COMM_WORLD);
            MPI_Finalize();
            return 1;
        }
        // Extract characters from file and store in variable c
        for (c = getc(file); c != EOF; c = getc(file)) {
            if (c == '\n') { // Increment count if this character is newline
                count = count + 1;
            }
        }
        // Check if last character is not a newline
        //if (c == EOF && count > 0 && c != '\n') {
        //    count = count + 1;
        //}
        // Close the file
        fclose(file);
        // Print the number of lines
        fprintf(stderr,"The soil grid file %s has --%d-- lines\n", filename, count);

        int nTotal = count; // Set the total number of lines
        int linesPerNode = nTotal / (world_size - 1); // Exclude the master node from allocation
        int startLine, endLine;

        startLine = (world_rank - 1) * linesPerNode;
        endLine = startLine + linesPerNode - 1;
        // Handling remainder to ensure each slave node has the same number of lines
        if (world_rank <= nTotal % (world_size - 1)) {
            startLine += world_rank - 1;
            endLine += world_rank;
        } else {
            startLine += nTotal % (world_size - 1);
            endLine += nTotal % (world_size - 1);
        }

        // Boundary checks to prevent startLine > endLine or invalid ranges
        if (startLine >= nTotal) {
            fprintf(stderr, "Rank %d: No task assigned (startLine=%d >= nTotal=%d), sending END.\n", 
                    world_rank, startLine, nTotal);
            MPI_Send(end_msg, strlen(end_msg) + 1, MPI_CHAR, MASTER_RANK, TAG_END, MPI_COMM_WORLD);
            MPI_Finalize();
            return 0;
        }
        
        if (startLine > endLine) {
            fprintf(stderr, "Rank %d: Invalid task range (startLine=%d > endLine=%d), sending ERROR.\n", 
                    world_rank, startLine, endLine);
            MPI_Send(error_msg, strlen(error_msg) + 1, MPI_CHAR, MASTER_RANK, TAG_END, MPI_COMM_WORLD);
            MPI_Finalize();
            return 1;
        }

        //Obtaining the identifiers of the starting and ending grids.
        fprintf(stderr,"Slave %d: Processing lines from --%d-- to --%d--\n",world_rank, startLine, endLine);

        // Call vicmain and check return value
        int vic_result = vicmain(world_rank,6,vars,outputdir,startLine,endLine);
        if (vic_result == 0) {
            fprintf(stderr,"vicmain in rank --%d-- is completed successfully\n",world_rank);
            MPI_Isend(end_msg, strlen(end_msg) + 1, MPI_CHAR, MASTER_RANK, TAG_END, MPI_COMM_WORLD, &request);
        } else {
            fprintf(stderr,"vicmain in rank --%d-- failed with return code %d\n",world_rank, vic_result);
            MPI_Isend(error_msg, strlen(error_msg) + 1, MPI_CHAR, MASTER_RANK, TAG_END, MPI_COMM_WORLD, &request);
        }
        MPI_Wait(&request, MPI_STATUS_IGNORE);
    } else {
        // Received invalid start message
        fprintf(stderr, "Rank %d: Invalid start message received: %s\n", world_rank, recv_start_msg);
        MPI_Send(error_msg, strlen(error_msg) + 1, MPI_CHAR, MASTER_RANK, TAG_END, MPI_COMM_WORLD);
    }
  }
  //free dynamic allocated memory
  free(requests);
  for (i = 0; i < NUM_SLAVES; i++) {
    free(recv_end_msgs[i]);
  }
  free(recv_end_msgs);

  // 释放动态分配的内存
  free(filename_copy);

  MPI_Finalize();
  return 0;
}
