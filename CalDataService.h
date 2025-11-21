//////////////////////////////////////////////////////////////////////
//
//			CalDataService( for XPi) Header
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALDATASERVICE_H__D03B2E75_70E7_4EFD_88B7_CD5ACEEF7F67__INCLUDED_)
#define AFX_CALDATASERVICE_H__D03B2E75_70E7_4EFD_88B7_CD5ACEEF7F67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TsaiCalibrator.h"


#define PCP_CALDATA_FILEHEADER			"Synapseimaging Calibration Data File"
#define PCP_CALWORKSPACE_FILEHEADER		"Synapseimaging Calibration Workspace File"

#define PCP_FILEVERSION_010000			1.0
#define PCP_FILEVERSION_LEGACY			PCP_FILEVERSION_010000
#define PCP_FILEVERSION_RECENT			PCP_FILEVERSION_010000

class CCalDataService  
{
public:
	CCalDataService();
	virtual ~CCalDataService();

	//load calibration data file
	void Load(CString FileName);
	void Save(CString FileName);

	//transform position information
	void IPtoRP(double dXp, double dYp, double* dpXm, double* dpYm);
	void ArbIPtoRP(double dXp, double dYp, double* dpXm, double* dpYm);
	
	void RPtoIP(double dXm, double dYm, double* dpXp, double* dpYp);
	void ArbRPtoIP(double dXm, double dYm, double* dpXp, double* dpYp);
	
	void ILtoRL(double dLp, double* dLm);
	void RLtoIL(double dLm, double* dLp);
	
	//position<->distance transformation
	void IPtoRL(double dXp1, double dYp1, double dXp2, double dYp2, double* dpLm);	//image position to real-world distance
	void RPtoIL(double dXm1, double dYm1, double dXm2, double dYm2, double* dpLp);	//real-world position to pixel distance
	
	//pixel size
	void GetPixelSize(double* pdPixelSizeXm, double* pdPixelSizeYm, double* pdAvgSizem);
	double GetPixelSize();
	double GetPixelArea();
	
	//FOV size
	void GetFOVSize(double* pdFOVXm, double* pdFOVYm);
	void GetResData(int *piCameraPixelSize, double *pdMmPerPixel);
	void Reset();

	void world_coord_to_image_coord_LFM( double dMachineX, double dMachineY, double *dpImageX, double *dpImageY);
	void image_coord_to_world_coord_LFM( double dImageX, double dImageY, double *dpMachineX, double *dpMachineY);
	
	double mdVersion;

	double mdFOVXm;
	double mdFOVYm;

	double mdPixelSizeXm;
	double mdPixelSizeYm;
	double mdPixelSizem;
	double mdPixelSizeMm;

	double mdPitchm;

	double mdOriginXp;
	double mdOriginYp;
	double mdAngle;

	double m_dA;
	double m_dB;
	double m_dC;
	double m_dD;
	double m_dA_inverse;
	double m_dB_inverse;
	double m_dC_inverse;
	double m_dD_inverse;

	CCalDataService& operator=(CCalDataService& data);
};

#endif // !defined(AFX_CALDATASERVICE_H__D03B2E75_70E7_4EFD_88B7_CD5ACEEF7F67__INCLUDED_)

