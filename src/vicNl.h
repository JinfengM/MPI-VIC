#include <math.h>
#include <vicNl_def.h>
#include <LAKE.h>

/*** SubRoutine Prototypes ***/
int vicmain(int nrank,int nvars,double* vars,char *outputdir,int nStart,int nEnd);
double advected_sensible_heat(double, double, double, double, double);
void alloc_atmos(int, atmos_data_struct **);
double arno_evap(layer_data_struct *, layer_data_struct *, double, double, 
		 double, double, double, double, double, double, double, double, 
#if SPATIAL_FROST
		 double, double *);
#else
		 double);
#endif // SPATIAL_FROST
unsigned char average_moisture_for_storm(double *, double *, double, double);

int   CalcAerodynamic(char, double, double, double, double, double,
	  	       double *, double *, double *, double *, double *);
void   calc_cloud_cover_fraction(atmos_data_struct *, dmy_struct *, int,
				 int, int, double *);
double calc_energy_balance_error(int, double, double, double, double, double);
#if OUTPUT_FORCE_STATS
void   calc_forcing_stats(int, atmos_data_struct *);
#endif // OUTPUT_FORCE_STATS
void   calc_longwave(double *, double, double, double);
void   calc_netlongwave(double *, double, double, double);
double calc_netshort(double, int, double, double *);
double calc_rainonly(double,double,double,double,double);
double calc_rc(double,double,float,double,double,double,double,char);
void   calc_root_fractions(veg_con_struct *, soil_con_struct *);
double calc_snow_coverage(int *, double, double, double, double, double, 
                          double, double, double *, double *, double *, 
                          double *, double *);
double calc_snow_ground_flux(int, int, int, int, double, double, double, 
			     double, double, double *, double *, double *, 
			     double *, energy_bal_struct *, 
			     snow_data_struct *, layer_data_struct *,
                             layer_data_struct *, soil_con_struct *, char *);
#if QUICK_FS
int    calc_soil_thermal_fluxes(int, double *, double *, char *, int *, double *, double *, 
				double *, double *, double *,double *, 
				double *, double *, double *, 
				double *, double *, double *, double ***, int, int, int, int);
#else
int    calc_soil_thermal_fluxes(int, double *, double *, char *, int *, double *, double *, 
				double *, double *, double *,double *, 
				double *, double *, double *, 
				double *, double *, double *, 
#if EXCESS_ICE
				double *, double *,
#endif // EXCESS_ICE
				int, int, int, int);
#endif // QUICK_FS
double CalcSnowPackEnergyBalance(double Tsurf, ...);
double CalcBlowingSnow(double, double, int, double, double, double, double, 
                       double, double, double, double, double, float, 
                       float, double, int, int, float, double, double, double *); 
double calc_atmos_energy_bal(double, double, double, double, double, double, 
                             double, double, double, double, double, double, 
                             double, double, double, double, 
                             double *, double *, double *, double *, 
                             double *, double *, double *, double *, char *, int *);
int    calc_layer_average_thermal_props(energy_bal_struct *, layer_data_struct *,
					layer_data_struct *, layer_data_struct *,
					soil_con_struct *, int, int, double *);
double calc_surf_energy_bal(double, double, double, double, double, double,
                            double, double, double, double, double, double,
                            double, double, double, double, double, double,
                            double, double, double, double, double, double,
                            double, double, double,
                            double *, double *, double *, double *, double *,
                            double *, double *, double *, double *, double *,
                            float *, int, int,
                            int, int, int, int, int, int, int, int, int, int,
                            atmos_data_struct *, dmy_struct *,
                            energy_bal_struct *, layer_data_struct *,
                            layer_data_struct *,
                            snow_data_struct *, soil_con_struct *,
                            veg_var_struct *, veg_var_struct *, int);
double calc_trans(double, double);
double calc_veg_displacement(double);
double calc_veg_height(double);
double calc_veg_roughness(double);
double calc_water_balance_error(int, double, double, double);
double canopy_evap(layer_data_struct *, layer_data_struct *,
		   veg_var_struct *, veg_var_struct *, char, int, int, 
		   double, double *, double, double, double, double, 
		   double, double, double, double, double, double, 
		   double *, double *, double *, double *, 
#if SPATIAL_FROST
                   double *, float *);
#else
                   float *);
#endif
void   check_files(filep_struct *, filenames_struct *);
FILE  *check_state_file(char *, dmy_struct *, global_param_struct *, int, int, 
                        int *);
void   close_files(filep_struct *, out_data_file_struct *, filenames_struct *);
filenames_struct cmd_proc(int argc, char *argv[]);
void   collect_eb_terms(energy_bal_struct, snow_data_struct, cell_data_struct,
                        int *, int *, int *, int *, int *, double, double, double,
                        int, int, double, int, int, double *, double *,
#if SPATIAL_FROST
                        double *, double,
#endif
                        out_data_struct *);
void   collect_wb_terms(cell_data_struct, veg_var_struct, snow_data_struct, lake_var_struct,
                        double, double, double, double, int, int, double, int, double *,
#if SPATIAL_FROST
                        double *,
#endif
                        out_data_struct *);
void   compress_files(char string[]);
void   compute_dz(double *, double *, int, double);
void   correct_precip(double *, double, double, double, double);
void   compute_pot_evap(int, dmy_struct *, int, int, double, double , double, double, double, double **, double *);
void   compute_runoff_and_asat(soil_con_struct *, double *, double, double *, double *);
void   compute_soil_layer_thermal_properties(layer_data_struct *, double *,
					     double *, double *, double *, 
					     double *, double *, double *, 
#if SPATIAL_FROST
                                             double *,
#endif
					     int);
void   compute_treeline(atmos_data_struct *, dmy_struct *, double, double *, char *);
double compute_zwt(soil_con_struct *, int, double);
out_data_struct *create_output_list();

void   display_current_settings(int, filenames_struct *, global_param_struct *);
int    dist_prec(atmos_data_struct *,dist_prcp_struct *,soil_con_struct *,
		 veg_con_struct *, lake_con_struct *,
		 dmy_struct *,global_param_struct *,
		 filep_struct *, out_data_file_struct *,
		 out_data_struct *, save_data_struct *,
		 int, int, char, char, char *, int *);
#if QUICK_FS
int  distribute_node_moisture_properties(double *, double *, double *, double *,
					 double *, double *, double *, double ***, 
					 double *, double *, double *, double *, double *,
					 double *, double *, double *, int, int, char);
#else
#if EXCESS_ICE
int  distribute_node_moisture_properties(double *, double *, double *, double *,
					 double *, double *, double *, double *, 
					 double *, double *, double *,
					 double *, double *, double *, double *, double *,
					 double *, double *, double *, int, int, char);
#else
int  distribute_node_moisture_properties(double *, double *, double *, 
					 double *, double *, double *,
					 double *, double *, double *,
					 double *, double *, double *, double *, double *,
					 double *, double *, double *, int, int, char);
#endif
#endif
void   distribute_soil_property(double *,double,double,
				double **l_param,
				int, int, double *, double *);

double error_calc_atmos_energy_bal(double Tcanopy, ...);
double error_calc_atmos_moist_bal(double , ...);
double error_calc_canopy_energy_bal(double Tsurf, ...);
double error_calc_snow_ground_flux(double Tsurf, ...);
double error_calc_surf_energy_bal(double Tsurf, ...);
double ErrorSnowPackEnergyBalance(double Tsurf, ...);
double error_print_atmos_energy_bal(double, va_list);
double error_print_atmos_moist_bal(double, va_list);
double error_print_canopy_energy_bal(double, va_list);
double error_print_snow_ground_flux(double, va_list);
double ErrorPrintSnowPackEnergyBalance(double, va_list);
double error_print_solve_T_profile(double, va_list);
double error_print_surf_energy_bal(double, va_list);
double error_solve_T_profile(double Tsurf, ...);
double estimate_dew_point(double, double, double, double, double);
#if QUICK_FS
int estimate_layer_ice_content(layer_data_struct *, double *, double *,
			       double *, double ***, double *,
			       double *, double ***, 
#if SPATIAL_FROST
			       double *, double,
#endif // SPATIAL_FROST
			       int, int, char);
#else
int estimate_layer_ice_content(layer_data_struct *, double *, double *,
			       double *, double *, double *, double *,
			       double *, double *, double *, 
#if SPATIAL_FROST
			       double *, double, 
#endif // SPATIAL_FROST
#if EXCESS_ICE
			       double *, double *,
#endif // EXCESS_ICE
			       int, int, char);
#endif
int estimate_layer_ice_content_quick_flux(layer_data_struct *, double *,
					  double, double, double, double,
					  double *,
#if QUICK_FS
					  double ***,
#else
					  double *, double *,
#endif // QUICK_FS
#if SPATIAL_FROST
					  double *, double,
#endif // SPATIAL_FROST
#if EXCESS_ICE
					  double *, double *,
#endif // EXCESS_ICE
					  char);
double estimate_T1(double, double, double, double, double, double, double, 
		   double, double, double, double);
double exp_interp(double,double,double,double,double);

double f(double, double, double, double, double, double, double, double,
         double, double, int, double *, double, double, double, double *,
         double *, double *, double *, double *, double *);
void   fda_heat_eqn(double *, double *, int, int, ...);
void   fdjac3(double *, double *, double *, double *, double *,
            void (*vecfunc)(double *, double *, int, int, ...), 
            int);
void   find_0_degree_fronts(energy_bal_struct *, double *, double *, int);
layer_data_struct find_average_layer(layer_data_struct *, layer_data_struct *,
				     double, double);
void   find_sublayer_temperatures(layer_data_struct *, double *, double *,
				  double *, double, double, int, int);
void   free_atmos(int nrecs, atmos_data_struct **atmos);
void   free_dist_prcp(dist_prcp_struct *, int);
void   free_dmy(dmy_struct **dmy);
void   free_vegcon(veg_con_struct **);
void   free_veglib(veg_lib_struct **);
void   free_out_data_files(out_data_file_struct **);
void   free_out_data(out_data_struct **);
int    full_energy(char, int, int, atmos_data_struct *, dist_prcp_struct *,
		   dmy_struct *, global_param_struct *, lake_con_struct *,
                   soil_con_struct *, veg_con_struct *);
double func_aero_resist(double,double,double,double,double);
double func_atmos_energy_bal(double, va_list);
double func_atmos_moist_bal(double, va_list);
double func_canopy_energy_bal(double, va_list);
double func_snow_ground_flux(double, va_list);
double func_surf_energy_bal(double, va_list);

double get_avg_temp(double, double, double *, double *, int);
double get_dist(double, double, double, double);
void   get_force_type(char *, int, int *);
global_param_struct get_global_param(filenames_struct *, FILE *);
void   get_next_time_step(int *, int *, int *, int *, int *, int);

double hermint(double, int, double *, double *, double *, double *, double *);
void   hermite(int, double *, double *, double *, double *, double *);
void   HourlyT(int, int, int *, double *, int *, double *, double *);

void   init_output_list(out_data_struct *, int, char *, int, float);
void   initialize_atmos(atmos_data_struct *, dmy_struct *, FILE **,
#if OUTPUT_FORCE
			soil_con_struct *, out_data_file_struct *, out_data_struct *);
#else
			soil_con_struct *);
#endif
void   initialize_global();
int   initialize_model_state(dist_prcp_struct *, dmy_struct,
			      global_param_struct *, filep_struct, 
			      int, int, int, int, 
			      double, soil_con_struct *,
                              veg_con_struct *, lake_con_struct,
			      char **, int **);
int    initialize_new_storm(cell_data_struct ***, veg_var_struct ***,
			    int, int, int, double, double);
void   initialize_snow(snow_data_struct **, int, int);
void   initialize_soil(cell_data_struct **, soil_con_struct *, veg_con_struct *, int);
void   initialize_veg( veg_var_struct **, veg_con_struct *,
		       global_param_struct *, int);

void   latent_heat_from_snow(double, double, double, double, double, 
                             double, double, double *, double *, 
                             double *, double *, double *);
double linear_interp(double,double,double,double,double);

cell_data_struct **make_cell_data(int, int);
dist_prcp_struct make_dist_prcp(int);
dmy_struct *make_dmy(global_param_struct *);
energy_bal_struct **make_energy_bal(int);
void make_in_and_outfiles(filep_struct *, filenames_struct *, 
			  soil_con_struct *, out_data_file_struct *);
out_data_struct *make_out_data(int);
snow_data_struct **make_snow_data(int);
veg_var_struct **make_veg_var(int);
void   MassRelease(double *,double *,double *,double *);
#if EXCESS_ICE
double maximum_unfrozen_water(double, double, double, double, double, double);
#else
double maximum_unfrozen_water(double, double, double, double);
#endif
#if QUICK_FS
double maximum_unfrozen_water_quick(double, double, double **);
#endif
double modify_Ksat(double);
void mtclim_wrapper(int, int, double, double, double, double,
                      double, double, double, double,
                      int, dmy_struct *, double *,
                      double *, double *, double *, double *, double *);

double new_snow_density(double);
int    newt_raph(void (*vecfunc)(double *, double *, int, int, ...), 
               double *, int);
void   nrerror(char *);

FILE  *open_file(char string[], char type[]);
FILE  *open_state_file(global_param_struct *, filenames_struct, int, int);

void parse_output_info(filenames_struct *, FILE *, out_data_file_struct **, out_data_struct *);
double penman(double, double, double, double, double, double, double);
void   prepare_full_energy(int, int, int, dist_prcp_struct *, 
			   soil_con_struct *, double *, double *); 
double priestley(double, double);
int    put_data(dist_prcp_struct *, atmos_data_struct *,
		soil_con_struct *, veg_con_struct *,
                lake_con_struct *, out_data_file_struct *,
		out_data_struct *, save_data_struct *,
 	        dmy_struct *, int); 

double read_arcinfo_value(char *, double, double);
int    read_arcinfo_info(char *, double **, double **, int **);
void   read_atmos_data(FILE *, global_param_struct, int, int, double **);
double **read_forcing_data(FILE **, global_param_struct);
void   read_initial_model_state(FILE *, dist_prcp_struct *, 
				global_param_struct *, int, int, int, 
				soil_con_struct *, int, char *,
				int *, lake_con_struct);
void   read_snowband(FILE *, soil_con_struct *);
void   read_snowmodel(atmos_data_struct *, FILE *, int, int, int, int);
soil_con_struct read_soilparam(FILE *, char *, int *, char *, char *);
soil_con_struct read_soilparam_arc(FILE *, char *, int *, char *, int);
veg_lib_struct *read_veglib(FILE *, int *);
veg_con_struct *read_vegparam(FILE *, int, int);
int    redistribute_during_storm(cell_data_struct ***, veg_var_struct ***,
				 int, int, int, double, double, double, 
				 double *);
void   redistribute_moisture(layer_data_struct *, double *, double *,
			     double *, double *, double *, int);
unsigned char redistribute_moisture_for_storm(double *, double *, double, 
					      double, double);
double root_brent(double, double, char *, double (*Function)(double, va_list), ...);
int    runoff(cell_data_struct *, cell_data_struct *,
              energy_bal_struct *, soil_con_struct *, double *,
#if EXCESS_ICE
	      int,
#endif
#if SPATIAL_FROST
              double *, 
#endif
              double, int, int, int, int, int);

void set_max_min_hour(double *, int, int *, int *);
void set_node_parameters(double *, double *, double *, double *, double *, double *,
			 double *, double *, double *, double *, double *,
			 double *, double *,
#if QUICK_FS
			 double ***,
#endif
#if EXCESS_ICE
			 double *, double *, double *, double *,
#endif
			 int, int, char);
out_data_file_struct *set_output_defaults(out_data_struct *);
int set_output_var(out_data_file_struct *, int, int, out_data_struct *, char *, int, char *, int, float);
double snow_albedo(double, double, double, double, double, double, int, char);
double snow_density(snow_data_struct *, double, double, double, double, double);
int    snow_intercept(double, double, double, double, double, double,
                      double, double, double, double, double, double, 
                      double *, double *, double *, double *, double *, 
                      double *, double *, double *, double *, double *, 
                      double *, double *, double *, double *, double *, 
                      double *, char *, int *, double *, double *, double *, 
                      double *, double *, double *, float *,
                      int, int, int, int, int, int, int, int,
                      atmos_data_struct *, layer_data_struct *, 
                      layer_data_struct *, soil_con_struct *, 
                      veg_var_struct *, veg_var_struct *);
int    snow_melt(double, double, double, double, double *, double, double *, double, 
		 double, double, double, double, double, double, double, 
                 double, double, double, double, double, double, 
                 double *, double *, double *, double *, double *, double *, 
                 double *, double *, double *, double *, double *, double *, 
                 int, int, int, int, snow_data_struct *, soil_con_struct *);
double SnowPackEnergyBalance(double, va_list);
double soil_conductivity(double, double, double, double, double, double, double, double);
void   soil_thermal_calc(soil_con_struct *, layer_data_struct *,
			 energy_bal_struct, double *, double *, double *,
			 int, int);
double soil_thermal_eqn(double, va_list);
double solve_snow(char, double, double, double, double, double, double,
                  double, double, double, double, double, double,
                  double *, double *, double *, double *, double *,
                  double *, double *, double *, double *, double *,
                  double *, double *, double *, double *, double *, double *,
                  double *, double *, double *, double *, double *, double *,
                  double *, double *, double *, double *, double *, double *,
                  float *, int, int, int, int, int, int, int, int, int, int *,
                  dmy_struct *, atmos_data_struct *, energy_bal_struct *,
                  layer_data_struct *, layer_data_struct *,
                  snow_data_struct *, soil_con_struct *,
                  veg_var_struct *, veg_var_struct *);
double solve_atmos_energy_bal(double Tcanopy, ...);
double solve_atmos_moist_bal(double , ...);
double solve_canopy_energy_bal(double Tfoliage, ...);
double solve_snow_ground_flux(double Tsurf, ...);
double solve_surf_energy_bal(double Tsurf, ...);
#if QUICK_FS
int    solve_T_profile(double *, double *, char *, int *, double *, double *,double *, 
		       double *, double, double *, double *, double *,
		       double *, double *, double *, double *, double, double *, double ***,
		       int, int *, int, int, int, int);
#else
int    solve_T_profile(double *, double *, char *, int *, double *, double *,double *, 
		       double *, double, double *, double *, double *,
		       double *, double *, double *, double *, double, double *,
#if EXCESS_ICE
		       double *, double *,
#endif
		       int, int *, int, int, int, int);

#endif
int   solve_T_profile_implicit(double *, double *, double *, double *, double *,
			       double *, double, double *, double *, double *,
#if EXCESS_ICE
			       double *, double *,
#endif
			       double *, double *, double *, double *, double, int, int *,
			       int, int, int, int, 
			       double *, double *, double *, double *, double *, double *, double *);
double StabilityCorrection(double, double, double, double, double, double);
int    surface_fluxes(char, double, double, double, double, 
#if EXCESS_ICE
		      int, double *, double *,
#endif
		      double, double, double *, double *, double **,
                      double *, double *, double *, double *, 
                      double *, double *, double *, double *, double *,
		      float *, int, int, int, int, int, 
                      int, int, int, int, atmos_data_struct *, dmy_struct *, 
                      energy_bal_struct *, global_param_struct *, 
                      cell_data_struct *, cell_data_struct *, 
                      snow_data_struct *, soil_con_struct *, 
                      veg_var_struct *, veg_var_struct *, float, float, float);
double svp(double);
double svp_slope(double);

void transpiration(layer_data_struct *, int, int, double, double, double, 
		   double, double, double, double, double, double, double, 
		   double *, double *, double *, double *, double *, double *,
#if SPATIAL_FROST
                   double *,
#endif
                   float *);
void tridag(double *,double *,double *,double *,double *,int);
void tridiag(double *, double *, double *, double *, unsigned);
int update_thermal_nodes(dist_prcp_struct *, 
			  int, int, int, soil_con_struct *, veg_con_struct *);
void usage(char *);

void   vicerror(char *);
double volumetric_heat_capacity(double,double,double,double);

void wrap_compute_zwt(soil_con_struct *, cell_data_struct *);
void write_data(out_data_file_struct *, out_data_struct *, dmy_struct *, int);
void write_dist_prcp(dist_prcp_struct *);
#if OUTPUT_FORCE
void write_forcing_file(atmos_data_struct *, int, out_data_file_struct *, out_data_struct *);
#endif
void write_header(out_data_file_struct *, out_data_struct *, dmy_struct *, global_param_struct);
void write_layer(layer_data_struct *, int, int, 
#if SPATIAL_FROST
                 double *,
#endif
                 double *);
void write_model_state(dist_prcp_struct *, global_param_struct *, int, 
		       int, filep_struct *, soil_con_struct *, char *,
		       int *, lake_con_struct);
void write_snow_data(snow_data_struct, int, int);
void write_soilparam(soil_con_struct *);
void write_vegparam(veg_con_struct *);
void write_vegvar(veg_var_struct *, int);

void zero_output_list(out_data_struct *);
