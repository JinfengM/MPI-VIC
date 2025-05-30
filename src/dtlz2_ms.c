/* Copyright 2012-2014 The Pennsylvania State University
 *
 * This software was written by David Hadka and others.
 * 
 * The use, modification and distribution of this software is governed by the
 * The Pennsylvania State University Research and Educational Use License.
 * You should have received a copy of this license along with this program.
 * If not, contact <dmh309@psu.edu>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <math.h>
#include <mpi.h>
#include "borgms.h"
#include "vicNl.h"
int nvars = 6;
int nobjs = 3;
float NSE(int nlen, const float* sim, const float* obs)
 {
	 float avObs = 0;
	 float tmp1, tmp2;
	 tmp1 = 0;
	 tmp2 = 0;

	 int i, k;

	 for (k = 0; k < nlen; k++) {
		 avObs = avObs + obs[k];

	 }
	 avObs = avObs / nlen;

	 for (i = 0; i < nlen; i++)
	 {
		 tmp1 = tmp1+ (obs[i] - sim[i]) * (obs[i] - sim[i]);
		 tmp2 = tmp2 + (obs[i] - avObs) * (obs[i] - avObs);
	 }
	 tmp1 = 1 - (tmp1) / (tmp2);
	 if(tmp1<0)
	 {
		tmp1=0;
	 }
	 //极小值
	 return (1-tmp1);
 }
 float logNSE(int nlen, const float* sim, const float* obs)
 {
	 float avObs = 0;
	 float tmp1, tmp2;
	 tmp1 = 0;
	 tmp2 = 0;

	 int i, k;

	 for (k = 0; k < nlen; k++) {
		 avObs = avObs + log(obs[k]);

	 }
	 avObs = avObs / nlen;

	 for (i = 0; i < nlen; i++)
	 {
		 tmp1 = tmp1+ (log(obs[i]) - log(sim[i])) * (log(obs[i]) - log(sim[i]));
		 tmp2 = tmp2 + (log(obs[i]) - avObs) * (log(obs[i]) - avObs);
	 }
	 tmp1 = 1 - (tmp1) / (tmp2);
	 if(tmp1<0)
	 {
		tmp1=0;
	 }
	 //极小值
	 return (1-tmp1);
 }
float R2(int nlen, const float* sim, const float* obs) {

		float avSim = 0;
		float avObs = 0;
		float tmp1, tmp2, tmp3;
		tmp1 = 0;
		tmp2 = 0;
		tmp3 = 0;
		int i, k;
		for (i = 0; i < nlen; i++) {
			avSim = avSim + sim[i];

		}
		avSim = avSim / nlen;

		for (k = 0; k < nlen; k++) {
			avObs = avObs + obs[k];

		}
		avObs = avObs / nlen;

		for (i = 0; i < nlen; i++) {

			tmp1 = tmp1 + (obs[i] - avObs)
					* (sim[i] - avSim);

			tmp2 = (tmp2 + (obs[i] - avObs)
					* (obs[i]- avObs));

			tmp3 = tmp3 + (sim[i] - avSim)
					* (sim[i] - avSim);
		}

		tmp1 = (tmp1 * tmp1) / (tmp2 * tmp3);
		//极小值
		return (1-tmp1);
	}
float RMSE(int nlen, const float* sim, const float* obs) {
		float tmp1;
		tmp1 = 0;
		int i, k;
		for (i = 0; i < nlen; i++) {
			tmp1 = tmp1 + (obs[i]-sim[i])*(obs[i]-sim[i]);
		}
		tmp1 = tmp1 / nlen;
		tmp1=sqrt(tmp1);
		//极小值
		return tmp1;
	}
float PBIS(int nlen, const float* sim, const float* obs) {
		float tmp1,tmp2;
		tmp1 = 0;
		tmp2 = 0;
		int i=0;
		for (i = 0; i < nlen; i++) {
			tmp1 = tmp1 + (sim[i]-obs[i]);
			tmp2 = tmp2 + obs[i];
		}
		//极小值
		return (tmp1/tmp2);
	}
void getObjectiveFuns(double *vars,double* objs,int rank,char* result_dir)
{
//result_dir+rank为产流结果文件，确保不重复
//get observations from external files
//get simulations  from external files
fprintf(stderr,"getObjectiveFuns() functional \n");
//set objs,note objs are maintained by Borg algorithms,all you need to do is to set specfic values. 
    int i;
	int icount = 0;
	int readint1 = 0;
	int readint2 = 0;

	//float fltObs1[72] = {0};
	//float fltObs2[72] = { 0 };
	//float fltObs3[72] = { 0 };
	//float fltObs4[72] = { 0 };
	//float fltObs5[72] = { 0 };
	float fltObs6[72] = { 0 };
	
	//float fltSim1[72] = { 0 };
	//float fltSim2[72] = { 0 };
	//float fltSim3[72] = { 0 };
	//float fltSim4[72] = { 0 };
	//float fltSim5[72] = { 0 };
	float fltSim6[72] = { 0 };


	char signore[128] = {0};

	float readfloat1 = 0.0;
	float readfloat2 = 0.0;
	float readfloat3 = 0.0;
	float readfloat4 = 0.0;
	float readfloat5 = 0.0;
	float readfloat6 = 0.0;
	
	char srank[8]={0};
	sprintf(srank,"%d",rank);
	
	char result_dir_rank[72]={0};
	strcpy(result_dir_rank,result_dir);
	strcat(result_dir_rank,srank);

	//Observation文件存放在模板文件夹中
	//模拟结果位于result_dir+rank目录中
	/*
	const char* obsfileName = "/root/mjf/VIC/run_lh/huiliu_result/observation.csv";	
	const char* simfileName1 = "/root/mjf/VIC/run_lh/huiliu_result/sanda.month";//三道河子
	const char* simfileName2 = "/root/mjf/VIC/run_lh/huiliu_result/boluo.month";
	const char* simfileName3 = "/root/mjf/VIC/run_lh/huiliu_result/wulon.month";
	const char* simfileName4 = "/root/mjf/VIC/run_lh/huiliu_result/gouta.month";
	const char* simfileName5 = "/root/mjf/VIC/run_lh/huiliu_result/weich.month";
	const char* simfileName6 = "/root/mjf/VIC/run_lh/huiliu_result/luanx.month";
    */
	char obsfileName[72]={0};
	strcpy(obsfileName,result_dir);
	strcat(obsfileName,"/observation.csv");
	fprintf(stderr,"obsfilename opening %s\n",obsfileName);
	FILE* obsFile = fopen(obsfileName, "r");
	//First line of observation is header, ignore
	fscanf(obsFile, "%s\n", signore);

	//FILE* simFile1 = fopen(simfileName1, "r");
	//FILE* simFile2 = fopen(simfileName2, "r");
	//FILE* simFile3 = fopen(simfileName3, "r");
	//FILE* simFile4 = fopen(simfileName4, "r");
	//FILE* simFile5 = fopen(simfileName5, "r");
	char simfileName6[72]={0};
	strcpy(simfileName6,result_dir_rank);
	strcat(simfileName6,"/luanx.month");
	fprintf(stderr,"simfileName6 opening %s\n",simfileName6);
	FILE* simFile6 = fopen(simfileName6, "r");

	if (simFile6 && obsFile)
	{
		while (fscanf(simFile6,"%d %d %f\n",&readint1,&readint2,&readfloat6) != EOF)
		{
			//fprintf(stderr, "1:%d,%d,%f\n", readint1, readint2, readfloat1);
			//fltSim1[icount]= readfloat1;
			//逐个得到6个站点模拟径流量
			//fscanf(simFile2, "%d %d %f\n", &readint1, &readint2, &readfloat2);
			//fprintf(stderr, "2:%d,%d,%f\n", readint1, readint2, readfloat2);
			//fltSim2[icount] = readfloat2;
			//fscanf(simFile3, "%d %d %f\n", &readint1, &readint2, &readfloat3);
			//fprintf(stderr, "3:%d,%d,%f\n", readint1, readint2, readfloat3);
			//fltSim3[icount] = readfloat3;
			//fscanf(simFile4, "%d %d %f\n", &readint1, &readint2, &readfloat4);
			//fprintf(stderr, "4:%d,%d,%f\n", readint1, readint2, readfloat4);
			//fltSim4[icount] = readfloat4;
			//fscanf(simFile5, "%d %d %f\n", &readint1, &readint2, &readfloat5);
			//fprintf(stderr, "5:%d,%d,%f\n", readint1, readint2, readfloat5);
			//fltSim5[icount] = readfloat5;
			//fscanf(simFile6, "%d %d %f\n", &readint1, &readint2, &readfloat6);
			//fprintf(stderr, "6:%d,%d,%f\n", readint1, readint2, readfloat6);
			fltSim6[icount] = readfloat6;
			//得到6个站点实测径流量
			fscanf(obsFile, "%f,%f,%f,%f,%f,%f\n", &readfloat1, &readfloat2, &readfloat3, &readfloat4, &readfloat5, &readfloat6);
			//fprintf(stderr, "7: %f,%f,%f,%f,%f,%f\n", readfloat1, readfloat2, readfloat3, readfloat4, readfloat5, readfloat6);
			//fltObs1[icount] = readfloat1;
			//fltObs2[icount] = readfloat2;
			//fltObs3[icount] = readfloat3;
			//fltObs4[icount] = readfloat4;
			//fltObs5[icount] = readfloat5;
			fltObs6[icount] = readfloat6;
			icount++;
		}
		fprintf(stderr,"Objective functions operational \n");
		//计算NSE
		//objs[0]=NSE(72, fltObs1, fltSim1);
		//objs[1]=NSE(72, fltObs2, fltSim2);
		//objs[2]=NSE(72, fltObs3, fltSim3);
		//objs[3]=NSE(72, fltObs4, fltSim4);
		//objs[4]=NSE(72, fltObs5, fltSim5);
		//objs[5]=NSE(72, fltObs6, fltSim6);
		objs[0]=NSE(72, fltObs6, fltSim6);//lognse
		objs[1]=R2(72, fltObs6, fltSim6);//PBIS
		objs[2]=PBIS(72, fltObs6, fltSim6);//rmse
		fprintf(stderr, "NSE,R2,RMSE SUCCESSFUL : %f,%f,%f\n", objs[0], objs[1], objs[2]);
		//fprintf(stderr, "NES2 IS: %f\n", objs[1]);
		//fprintf(stderr, "NES3 IS: %f\n", objs[2]);
		//fprintf(stderr, "NES4 IS: %f\n", objs[3]);
		//fprintf(stderr, "NES5 IS: %f\n", objs[4]);
		//fprintf(stderr, "NES6 IS: %f\n", objs[5]);

		FILE* outputFile = NULL;
		//const char* outputFilename="/root/mjf/VIC/run_lh/huiliu_result/vic-borg.set";
		char archfile[72]={0};
		strcpy(archfile,result_dir);
		strcat(archfile,"/vic-borg-mpi.set");
		fprintf(stderr,"Ready for writing archive file: %s\n",archfile);
		outputFile = fopen(archfile, "a+");
		//fprintf(outputFile, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",vars[0],vars[1],vars[2],vars[3],vars[4],vars[5],objs[0], objs[1], objs[2], objs[3], objs[4], objs[5]);
		fprintf(outputFile, "%f,%f,%f,%f,%f,%f,%f,%f,%f\n",vars[0],vars[1],vars[2],vars[3],vars[4],vars[5],objs[0],objs[1],objs[2]);
		fclose(outputFile);
		outputFile=NULL;
		//fprintf(stderr,"test1\n");
		
		//fclose(simFile1);
		//fclose(simFile2);
		//fclose(simFile3);
		//fclose(simFile4);
		//fclose(simFile5);
		fclose(simFile6);
		fclose(obsFile);

		obsFile  = NULL;
		//simFile1 = NULL;
		//simFile2 = NULL;
		//simFile3 = NULL;
		//simFile4 = NULL;
		//simFile5 = NULL;
		simFile6 = NULL;
		fprintf(stderr,"Free memory confirmed\n");
		free(result_dir);
		result_dir=NULL;
	}
}
void VIC_OBJS(double* vars, double* objs, double* consts,int argc, char* argv[],int rank) {
	fprintf(stderr,"VIC_OBJS commencing: %f,%f,%f,%f,%f,%f\n",vars[0],vars[1],vars[2],vars[3],vars[4],vars[5]);
	fprintf(stderr,"***************************************************\n");
    fprintf(stderr,"                                                   \n");
	fprintf(stderr,"OBJECTIVE FUNCTIONS OPERATIONAL\n");
    fprintf(stderr,"                                                   \n");
	//result_dir 由malloc而来，必须free.
    char* result_dir=vicmain(nvars,vars,argc,argv,rank);
	//char* result_dir="/root/mjf/VIC/run_lh/chanliu_result";
	fprintf(stderr,"vicmain successful with result_dir: %s\n",result_dir);
	fprintf(stderr,"***************************************************\n");
    fprintf(stderr,"RECEIVING TRANSMISSION\n");
    fprintf(stderr,"***************************************************\n");
    getObjectiveFuns(vars,objs,rank,result_dir);
	//exit(0);
	/*
    int i;
	int j;
	int k = nvars - nobjs + 1;
	double g = 0.0;

	for (i=nvars-k; i<nvars; i++) {
		g += pow(vars[i] - 0.5, 2.0);
	}

	for (i=0; i<nobjs; i++) {
		objs[i] = 1.0 + g;

		for (j=0; j<nobjs-i-1; j++) {
			objs[i] *= cos(0.5*PI*vars[j]);
		}

		if (i != 0) {
			objs[i] *= sin(0.5*PI*vars[nobjs-i-1]);
		}
	}
	*/
}

int main(int argc, char* argv[]) {
	int i, j;
	int rank;
	char runtime[256]={0};

	// All master-slave runs need to call startup and set the runtime
	// limits.
	BORG_Algorithm_ms_startup(&argc, &argv);
	BORG_Algorithm_ms_max_evaluations(100000);

	// Define the problem.  Problems are defined the same way as the
	// serial example (see dtlz2_serial.c).
	BORG_Problem problem = BORG_Problem_create(nvars, nobjs, 0, VIC_OBJS);

	BORG_Problem_set_bounds(problem, 0, 0.01, 0.5);//b_infilt :(0-1)
	BORG_Problem_set_bounds(problem, 1, 0.01, 1);//Ds       :(0.001-1)
	BORG_Problem_set_bounds(problem, 2, 0.1, 30);//Dsmax       :(0.1-50)
	BORG_Problem_set_bounds(problem, 3, 0.01, 1.0);//Ws       :(0-1.0)
	//BORG_Problem_set_bounds(problem, 4, 0.0, 5.0);//c          :(0-5.0)
	BORG_Problem_set_bounds(problem, 4, 0.1, 1.5);//depth[1]   :(0.1-3.0)
	BORG_Problem_set_bounds(problem, 5, 0.1, 1.5);//depth[2]   :(0.1-3.0)

	for (j=0; j<nobjs; j++) {
		BORG_Problem_set_epsilon(problem, j, 0.01);
	}

	// Get the rank of this process.  The rank is used to ensure each
	// parallel process uses a different random seed.
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// When running experiments, we want to run the algorithm multiple
	// times and average the results.

		// Save runtime dynamics to a file.  Only the master node
		// will write to this file.  Note how we create separate
		// files for each run.
		BORG_Algorithm_output_runtime(runtime);

		// Seed the random number generator.
		BORG_Random_seed(37*(rank+1));

		// Run the master-slave Borg MOEA on the problem.
		BORG_Archive result = BORG_Algorithm_ms_run(problem,argc,argv,rank);

		// Only the master process will return a non-NULL result.
		// Print the Pareto optimal solutions to the screen.
		if (result != NULL) {
			fprintf(stderr,"Congratulation ! VIC-Borg-MPI with 100000 iterations accomplished\n");
			const char* outputFile = "/root/mjf/VIC/run_lh/chanliu_result/Pareto_output.txt";
			FILE* output = fopen(outputFile, "w");
			BORG_Validate_file(output);
			BORG_Archive_print(result, output);
			fclose(output);
			output=NULL;
			BORG_Archive_destroy(result);
		}


	// Shutdown the parallel processes and exit.
	BORG_Algorithm_ms_shutdown();
	BORG_Problem_destroy(problem);
	return EXIT_SUCCESS;
}

