#include <stdio.h>
#include <string.h>
#include <vicNl.h>
#include <stdlib.h>
//void routmesos_(char *outputdir,int *str_len);
int main()
{
fprintf(stderr,"here in main.c\n");
fprintf(stderr,"before routmesos_()\n");

char outputdir[]="/root/mjf/VIC/run_mesos/";
int str_len=strlen(outputdir);
int nvars=6;
double *vars = (double*)malloc(nvars * sizeof(double));
vars[0]=0.910654;
vars[1]=0.436423;
vars[2]=24.482024;
vars[3]=0.502105;
vars[4]=0.988413;
vars[5]=0.413380;
vicmain(nvars,vars,outputdir,str_len);
fprintf(stderr,"after routmesos_()\n");
free(vars);
vars=NULL;
return 0;
}
