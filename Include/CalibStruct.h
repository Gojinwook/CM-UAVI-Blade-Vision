#ifndef _CALIB_STRUCT_H
#define _CALIB_STRUCT_H

/* Maximum number of data points allowed */
#define MAX_POINTS	2000


/*******************************************************************************\
*										*
* Camera parameters are usually the fixed parameters of the given camera 	*
* system, typically obtained from manufacturers specifications.			*
*										*
* Cy and Cy (the center of radial lens distortion), may be calibrated		*
* separately or as part of the coplanar/noncoplanar calibration.		*
* The same with sx (x scale uncertainty factor).				*
*										*
\*******************************************************************************/
struct camera_parameters {
    double    Ncx;		/* [sel]     Number of sensor elements in camera's x direction */
    double    Nfx;		/* [pix]     Number of pixels in frame grabber's x direction   */
    double    dx;		/* [mm/sel]  X dimension of camera's sensor element (in mm)    */
    double    dy;		/* [mm/sel]  Y dimension of camera's sensor element (in mm)    */
    double    dpx;		/* [mm/pix]  Effective X dimension of pixel in frame grabber   */
    double    dpy;		/* [mm/pix]  Effective Y dimension of pixel in frame grabber   */
    double    Cx;		/* [pix]     Z axis intercept of camera coordinate system      */
    double    Cy;		/* [pix]     Z axis intercept of camera coordinate system      */
    double    sx;		/* []        Scale factor to compensate for any error in dpx   */
};

/*******************************************************************************\
*										*
* Calibration data consists of the (x,y,z) world coordinates of a feature	*
* point	(in mm) and the corresponding coordinates (Xf,Yf) (in pixels) of the	*
* feature point in the image.  Point count is the number of points in the data	*
* set.										*
*										*
*										*
* Coplanar calibration:								*
*										*
* For coplanar calibration the z world coordinate of the data must be zero.	*
* In addition, for numerical stability the coordinates of the feature points	*
* should be placed at some arbitrary distance from the origin (0,0,0) of the	*
* world coordinate system.  Finally, the plane containing the feature points	*
* should not be parallel to the imaging plane.  A relative angle of 30 degrees	*
* is recommended.								*
*										*
*										*
* Noncoplanar calibration:							*
*										*
* For noncoplanar calibration the data must not lie in a single plane.		*
*										*
\*******************************************************************************/
struct calibration_data {
    int       point_count;	/* [points] 	 */
    double    xw[MAX_POINTS];	/* [mm]          */
    double    yw[MAX_POINTS];	/* [mm]          */
    double    zw[MAX_POINTS];	/* [mm]          */
    double    Xf[MAX_POINTS];	/* [pix]         */
    double    Yf[MAX_POINTS];	/* [pix]         */
};


/*******************************************************************************\
*										*
* Calibration constants are the model constants that are determined from the 	*
* calibration data.								*
*										*
\*******************************************************************************/
struct calibration_constants {
    double    f;		/* [mm]          */
    double    kappa1;		/* [1/mm^2]      */
    double    p1;		/* [1/mm]        */
    double    p2;		/* [1/mm]        */
    double    Tx;		/* [mm]          */
    double    Ty;		/* [mm]          */
    double    Tz;		/* [mm]          */
    double    Rx;		/* [rad]	 */
    double    Ry;		/* [rad]	 */
    double    Rz;		/* [rad]	 */
    double    r1;		/* []            */
    double    r2;		/* []            */
    double    r3;		/* []            */
    double    r4;		/* []            */
    double    r5;		/* []            */
    double    r6;		/* []            */
    double    r7;		/* []            */
    double    r8;		/* []            */
    double    r9;		/* []            */
};


#endif 