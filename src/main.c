#include <stdio.h>
#include <string.h>

void routmesos_(char *outputdir,int *str_len);
int main(int argc, char *argv[])
{
fprintf(stderr,"here in main.c\n");
fprintf(stderr,"before routmesos_()\n");
char outputdir[]="/root/mjf/VIC/run_mesos/";
int str_len=strlen(outputdir);
routmesos_(outputdir,&str_len);
//int nvars=6;
//double *vars = (double*)malloc(nvars * sizeof(double));
//0.910654,0.436423,24.482024,0.502105,0.988413,0.413380
//vars[0]=0.910654;
//vars[1]=0.436423;
//vars[2]=24.482024;
//vars[3]=0.502105;
//vars[4]=0.988413;
//vars[5]=0.413380;
//vicmain(nvars,vars,argc,argv,outputdir,str_len);
fprintf(stderr,"after routmesos_()\n");
//free(vars);
//vars=NULL;
return 0;
}
