#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vicNl.h>
#include <global.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
static char vcid[] = "$Id: vicNl.c,v 5.14.2.20 2012/02/05 00:15:44 vicadmin Exp $";

/** Main Program **/
//import router program writting by Fortran from rout.so
//void routmesos_(char *outputdir,int *str_len);

int vicmain(int nrank,int nvars,double* vars,char *outputdir,int nStart,int nEnd)
{
  //fprintf(stderr,"in vicmain:Commencing routmesos_()\n");
  //routmpi_(&rank);  
  //routmesos_(outputdir,&str_len);
  //exit(0);
  
fprintf(stderr,"New vars detected: %f,%f,%f,%f,%f,%f\n",vars[0],vars[1],vars[2],vars[3],vars[4],vars[5]);
char cwd[1024];
getcwd(cwd, sizeof(cwd));
fprintf(stderr,"current directory is : %s\n",cwd);
fprintf(stderr,"In Slave %d : grid start to endmake :--%d-- to --%d--\n",nrank,nStart,nEnd);
fprintf(stderr,"outputdir: %s\n",outputdir);
//return 0;
/* 
 while (*argv != NULL)
    {
            fprintf(stderr,"%s\n", *argv);
            argv++;
    }
  */
 //fprintf(stderr,"in vicmain after rout_() is called\n");
 // return 0;

  extern veg_lib_struct *veg_lib;
  extern option_struct options;
  extern Error_struct Error;
  extern global_param_struct global_param;

  /** Variable Declarations **/

  char                     NEWCELL;
  char                     LASTREC;
  char                     MODEL_DONE;
  char                     RUN_MODEL;
  char                    *init_STILL_STORM;
  char                     ErrStr[MAXSTRING];
  int                      rec, i, j;
  int                      veg;
  int                      dist;
  int                      band;
  int                      Ndist;
  int                      Nveg_type;
  int                      cellnum;
  int                      index;
  int                     *init_DRY_TIME;
  int                      Ncells;
  int                      cell_cnt;
  int                      startrec;
  int                      ErrorFlag;
  float                    mu;
  double                   storage;
  double                   veg_fract;
  double                   band_fract;
  double                   Clake;
  dmy_struct              *dmy;
  atmos_data_struct       *atmos;
  veg_con_struct          *veg_con;
  soil_con_struct          soil_con;
  dist_prcp_struct         prcp; /* stores information about distributed 
				    precipitation */
  filenames_struct         filenames;
  filep_struct             filep;
  lake_con_struct          lake_con;
  out_data_file_struct     *out_data_files;
  out_data_struct          *out_data;
  save_data_struct         save_data;
  
  /** Read Model Options **/
  initialize_global();
  strcpy(filenames.global,outputdir);
  //strcat(filenames.global, "chanliu_input.txt");
  //filenames = cmd_proc(argc, argv);
  //fprintf(stderr,"%s\n", filenames.global);
  //fprintf(stderr,"soil %s\n", filenames.soil);
  //fprintf(stderr,"result dir %s\n", filenames.result_dir);
  //fprintf(stderr,"soil %s\n", filenames.soil);
  //fprintf(stderr,"veg %s\n", filenames.veg);
  //fprintf(stderr,"veglib %s\n", filenames.veglib);

#if VERBOSE
  display_current_settings(DISP_VERSION,(filenames_struct*)NULL,(global_param_struct*)NULL);
#endif

  /** Read Global Control File **/
  filep.globalparam = open_file(filenames.global,"r");
  global_param = get_global_param(&filenames, filep.globalparam);

  /** Set up output data structures **/
  out_data = create_output_list();
  out_data_files = set_output_defaults(out_data);
  fclose(filep.globalparam);
  filep.globalparam=NULL;
  filep.globalparam = open_file(filenames.global,"r");
  parse_output_info(&filenames, filep.globalparam, &out_data_files, out_data);
  //获取旧的result存储路径
  //fprintf(stderr,"Adjusting old result_dir : %s\n",filenames.result_dir);
  //char *buf=(char*)malloc(sizeof(char)*1024);
  //strcpy(buf,filenames.result_dir);
  //char s[8]={0};
  //sprintf(s,"%d",rank);
  //strcat(filenames.result_dir,s);
  //创建新的产流路径:默认result_dir+rank(chanliu_result1,chanliu_result2...chanliu_resultrank)
  //mkdir(filenames.result_dir,0755);
  //strcpy(filenames.result_dir,outputdir);
  //strcat(filenames.result_dir,"output");

  //strcpy(filenames.forcing[0],outputdir);
  //strcat(filenames.forcing[0],"forcing/forcing_");
  
  //strcpy(filenames.soil,outputdir);
  //strcat(filenames.soil,"output_area_soil.txt");
  
  //strcpy(filenames.veglib,outputdir);
  //strcat(filenames.veglib,"veglib.LDAS");
  
  //strcpy(filenames.veg,outputdir);
  //strcat(filenames.veg,"output_area_veg.txt");
  
  //fprintf(stderr,"Adjusted new result_dir complete : %s\n",filenames.result_dir);
  /** Check and Open Files **/
  check_files(&filep, &filenames);

#if !OUTPUT_FORCE

  /** Read Vegetation Library File **/
  veg_lib = read_veglib(filep.veglib,&Nveg_type);

#endif // !OUTPUT_FORCE

  /** Initialize Parameters **/
  if(options.DIST_PRCP) Ndist = 2;
  else Ndist = 1;
  cellnum = -1;

  /** Make Date Data Structure **/
  dmy      = make_dmy(&global_param);

  /** allocate memory for the atmos_data_struct **/
  alloc_atmos(global_param.nrecs, &atmos);

  /** Initial state **/
  startrec = 0;
#if !OUTPUT_FORCE
  if ( options.INIT_STATE ) 
    filep.init_state = check_state_file(filenames.init_state, dmy, 
					 &global_param, options.Nlayer, 
					 options.Nnode, &startrec);

  /** open state file if model state is to be saved **/
  if ( options.SAVE_STATE && strcmp( filenames.statefile, "NONE" ) != 0 )
    filep.statefile = open_state_file(&global_param, filenames, options.Nlayer,
                                         options.Nnode);
  else filep.statefile = NULL;

#endif // !OUTPUT_FORCE

  /************************************
    Run Model for all Active Grid Cells
    ************************************/
  MODEL_DONE = FALSE;
  cell_cnt=0;
  while(!MODEL_DONE) {
      cellnum++;
    soil_con = read_soilparam(filep.soilparam, filenames.soil_dir, &cell_cnt, &RUN_MODEL, &MODEL_DONE);
    if(cellnum<nStart || cellnum>nEnd)
    {
        RUN_MODEL=FALSE;
    }
    if(RUN_MODEL) {
	//BEGIN UPDATE SOIL_CON FROM VARS
	//fprintf(stderr,"before update:%f,%f,%f,%f,%f,%f,%f\n",soil_con.b_infilt,soil_con.Ds,soil_con.Dsmax,soil_con.Ws,soil_con.c,soil_con.depth[1],soil_con.depth[2]);
	soil_con.b_infilt=vars[0];
	soil_con.Ds=vars[1];
	soil_con.Dsmax=vars[2];
	soil_con.Ws=vars[3];
	//soil_con.c=vars[4];
	soil_con.depth[1]=vars[4];
	soil_con.depth[2]=vars[5];
	//after update:0.910654,0.436423,24.482024,0.502105,3.778949,0.988413,0.413380
	//0.910654,0.436423,24.482024,0.502105,0.988413,0.413380
	//soil_con.b_infilt=0.910654;
	//soil_con.Ds=0.436423;
	//soil_con.Dsmax=24.482024;
	//soil_con.Ws=0.502105;
	//soil_con.c=3.778949;
	//soil_con.depth[1]=0.988413;
	//soil_con.depth[2]=0.413380;
	//fprintf(stderr,"7 PARAMETERS OF SOIL_CON HAVE BEEN MODIFIED\n");
	//fprintf(stderr,"after update:%f,%f,%f,%f,%f,%f,%f\n",soil_con.b_infilt,soil_con.Ds,soil_con.Dsmax,soil_con.Ws,soil_con.c,soil_con.depth[1],soil_con.depth[2]);
	//END OF UPDATE SOIL_CON

#if QUICK_FS
      /** Allocate Unfrozen Water Content Table **/
      if(options.FROZEN_SOIL) {
	for(i=0;i<MAX_LAYERS;i++) {
	  soil_con.ufwc_table_layer[i] = (double **)malloc((QUICK_FS_TEMPS+1)*sizeof(double *));
	  for(j=0;j<QUICK_FS_TEMPS+1;j++) 
	    soil_con.ufwc_table_layer[i][j] = (double *)malloc(2*sizeof(double));
	}
	for(i=0;i<MAX_NODES;i++) {
	  soil_con.ufwc_table_node[i] = (double **)malloc((QUICK_FS_TEMPS+1)*sizeof(double *));

	  for(j=0;j<QUICK_FS_TEMPS+1;j++) 
	    soil_con.ufwc_table_node[i][j] = (double *)malloc(2*sizeof(double));
	}
      }
#endif /* QUICK_FS */

      NEWCELL=TRUE;

	  //fprintf(stderr,"                         \n");
	  fprintf(stderr,"current cell number is %d\n",cellnum);
      //makefprintf(stderr,"                         \n");
#if !OUTPUT_FORCE

      /** Read Grid Cell Vegetation Parameters **/
      veg_con = read_vegparam(filep.vegparam, soil_con.gridcel,
                              Nveg_type);
	  //fprintf(stderr,"veg_con is over\n");
      calc_root_fractions(veg_con, &soil_con);
      //fprintf(stderr,"calc_root_fractions is over\n");
      if ( options.LAKES ) 
	lake_con = read_lakeparam(filep.lakeparam, soil_con, veg_con);

#endif // !OUTPUT_FORCE

      /** Build Gridded Filenames, and Open **/
      make_in_and_outfiles(&filep, &filenames, &soil_con, out_data_files);

      if (options.PRT_HEADER) {
        /** Write output file headers **/
        write_header(out_data_files, out_data, dmy, global_param);
      }

#if !OUTPUT_FORCE

      /** Read Elevation Band Data if Used **/
      read_snowband(filep.snowband, &soil_con);

      /** Make Precipitation Distribution Control Structure **/
      prcp     = make_dist_prcp(veg_con[0].vegetat_type_num);

#endif // !OUTPUT_FORCE

      /**************************************************
         Initialize Meteological Forcing Values That
         Have not Been Specifically Set
       **************************************************/

#if VERBOSE
      fprintf(stderr,"Initializing Forcing Data\n");
#endif /* VERBOSE */

      initialize_atmos(atmos, dmy, filep.forcing,
#if OUTPUT_FORCE
		       &soil_con, out_data_files, out_data); 
#else /* OUTPUT_FORCE */
                       &soil_con); 
#endif /* OUTPUT_FORCE */

#if !OUTPUT_FORCE

      /**************************************************
        Initialize Energy Balance and Snow Variables 
      **************************************************/

#if VERBOSE
      fprintf(stderr,"Model State Initialization\n");
#endif /* VERBOSE */
      ErrorFlag = initialize_model_state(&prcp, dmy[0], &global_param, filep, 
			     soil_con.gridcel, veg_con[0].vegetat_type_num,
			     options.Nnode, Ndist, 
			     atmos[0].air_temp[NR],
			     &soil_con, veg_con, lake_con,
			     &init_STILL_STORM, &init_DRY_TIME);
      if ( ErrorFlag == ERROR ) {
	if ( options.CONTINUEONERROR == TRUE ) {
	  // Handle grid cell solution error
	  fprintf(stderr, "ERROR: Grid cell %i failed in record %i so the simulation has not finished.  An incomplete output file has been generated, check your inputs before rerunning the simulation.\n", soil_con.gridcel, rec);
	  break;
	} else {
	  // Else exit program on cell solution error as in previous versions
	  sprintf(ErrStr, "ERROR: Grid cell %i failed in record %i so the simulation has ended. Check your inputs before rerunning the simulation.\n", soil_con.gridcel, rec);
	  vicerror(ErrStr);
	}
      }
      
#if VERBOSE
      fprintf(stderr,"Running Model\n");
#endif /* VERBOSE */

      /** Update Error Handling Structure **/
      Error.filep = filep;
      Error.out_data_files = out_data_files;

      /** Initialize the storage terms in the water and energy balances **/
      /** Sending a negative record number (-global_param.nrecs) to dist_prec() will accomplish this **/
      ErrorFlag = dist_prec(&atmos[0], &prcp, &soil_con, veg_con,
		  &lake_con, dmy, &global_param, &filep, out_data_files,
		  out_data, &save_data, -global_param.nrecs, cellnum,
                  NEWCELL, LASTREC, init_STILL_STORM, init_DRY_TIME);

      /******************************************
	Run Model in Grid Cell for all Time Steps
	******************************************/

      for ( rec = startrec ; rec < global_param.nrecs; rec++ ) {

        if ( rec == global_param.nrecs - 1 ) LASTREC = TRUE;
        else LASTREC = FALSE;

        ErrorFlag = dist_prec(&atmos[rec], &prcp, &soil_con, veg_con,
		  &lake_con, dmy, &global_param, &filep,
		  out_data_files, out_data, &save_data, rec, cellnum,
                  NEWCELL, LASTREC, init_STILL_STORM, init_DRY_TIME);

        if ( ErrorFlag == ERROR ) {
          if ( options.CONTINUEONERROR == TRUE ) {
            // Handle grid cell solution error
            fprintf(stderr, "ERROR: Grid cell %i failed in record %i so the simulation has not finished.  An incomplete output file has been generated, check your inputs before rerunning the simulation.\n", soil_con.gridcel, rec);
            break;
          } else {
	    // Else exit program on cell solution error as in previous versions
            sprintf(ErrStr, "ERROR: Grid cell %i failed in record %i so the simulation has ended. Check your inputs before rerunning the simulation.\n", soil_con.gridcel, rec);
            vicerror(ErrStr);
	  }
        }

        NEWCELL=FALSE;
	for ( veg = 0; veg <= veg_con[0].vegetat_type_num; veg++ )
	  init_DRY_TIME[veg] = -999;

      }	/* End Rec Loop */

#endif /* !OUTPUT_FORCE */

      close_files(&filep,out_data_files,&filenames); 

#if !OUTPUT_FORCE

#if QUICK_FS
      if(options.FROZEN_SOIL) {
	for(i=0;i<MAX_LAYERS;i++) {
	  for(j=0;j<6;j++) 
	    free((char *)soil_con.ufwc_table_layer[i][j]);
	  free((char *)soil_con.ufwc_table_layer[i]);
	}
	for(i=0;i<MAX_NODES;i++) {
	  for(j=0;j<6;j++) 
	    free((char *)soil_con.ufwc_table_node[i][j]);
	  free((char *)soil_con.ufwc_table_node[i]);
	}
      }
#endif /* QUICK_FS */
      free_dist_prcp(&prcp,veg_con[0].vegetat_type_num);
      free_vegcon(&veg_con);
      free((char *)soil_con.AreaFract);
      free((char *)soil_con.BandElev);
      free((char *)soil_con.Tfactor);
      free((char *)soil_con.Pfactor);
      free((char *)soil_con.AboveTreeLine);
      free((char*)init_STILL_STORM);
      free((char*)init_DRY_TIME);
#endif /* !OUTPUT_FORCE */
    }	/* End Run Model Condition */
  } 	/* End Grid Loop */

  /** cleanup **/
  free_atmos(global_param.nrecs, &atmos);
  free_dmy(&dmy);
  free_out_data_files(&out_data_files);
  free_out_data(&out_data);
#if !OUTPUT_FORCE
  free_veglib(&veg_lib);
#endif /* !OUTPUT_FORCE */
  fclose(filep.soilparam);
  filep.soilparam=NULL;
#if !OUTPUT_FORCE
  fclose(filep.vegparam);
  filep.vegparam=NULL;
  fclose(filep.veglib);
  filep.veglib=NULL;
  if (options.SNOW_BAND>1)
  {
    fclose(filep.snowband);
    filep.snowband=NULL;	  
  }

  if (options.LAKES)
  {
    fclose(filep.lakeparam);
    filep.lakeparam=NULL;	  
  }

  if ( options.INIT_STATE )
  {
    fclose(filep.init_state);
    filep.init_state=NULL;	  
  }

  if ( options.SAVE_STATE && strcmp( filenames.statefile, "NONE" ) != 0 )
  {
	fclose(filep.statefile);  
	filep.statefile=NULL;
  }

    

#endif /* !OUTPUT_FORCE */
  //fprintf(stderr,"VIC model water yielding complete\n");
  //fprintf(stderr,"VIC model water routing operational\n");
  //fprintf(stderr,"................routmesos_() commencing...................\n");
  //routmesos_(outputdir,&str_len);
  //fprintf(stderr,"................routmesos_() complete..................\n");
 // fprintf(stderr,"VIC model water routing complete\n");
  //fprintf(stderr,"Awaiting vicmain() for calculating objectives\n");
  //return buf;
  return EXIT_SUCCESS;
}	/* End Main Program */
