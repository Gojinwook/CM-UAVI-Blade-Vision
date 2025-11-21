#ifndef _TSAICALIBRATOR_H_
#define _TSAICALIBRATOR_H_

#include "calibstruct.h"


//////////////////////////////////////////////////////////////////////
// C type functions called by cpp functions 
//////////////////////////////////////////////////////////////////////


extern "C" {
	void initialize_photometrics_parms ();
	void print_cp_cc_data(FILE *fp, struct camera_parameters *cp, struct calibration_constants *cc);
	void coplanar_calibration_with_full_optimization ();

	void image_coord_to_world_coord (double Xfd, double Yfd, double zw, double *xw, double *yw);
	void world_coord_to_image_coord (double xw, double yw, double zw, double *Xf, double *Yf);

	void  load_cd_data(FILE *fp, calibration_data *cd);
	void  dump_cd_data(FILE *fp, calibration_data *cd);
	void  load_cp_cc_data(FILE *fp, camera_parameters *cp, calibration_constants *cc);
	void  dump_cp_cc_data(FILE *fp, camera_parameters *cp, calibration_constants *cc);

	//global to communicate with C-Module
	extern camera_parameters cp;
	extern calibration_data cd;
	extern calibration_constants cc;
	char camera_type[];
}

#endif 