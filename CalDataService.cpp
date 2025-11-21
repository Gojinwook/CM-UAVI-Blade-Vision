//////////////////////////////////////////////////////////////////////
//
//			CalDataService( for XPi) Implementation
//
//////////////////////////////////////////////////////////////////////

#include"stdafx.h"
#include"uScan.h"
#include"CalDataService.h"

CCalDataService::CCalDataService()
{
	mdOriginXp = 0;
	mdOriginYp = 0;
	mdAngle = 0;

	mdFOVXm = 0;
	mdFOVYm = 0;
	mdPixelSizeXm = 0;
	mdPixelSizeYm = 0;
	mdPixelSizem = 0;
	mdPitchm = 0;
}

CCalDataService::~CCalDataService()
{
}

void CCalDataService::Load(CString FileName)
{
	FILE* fp = fopen((LPCTSTR) FileName, "rb");
	if (!fp) {
		CString message;
		message.Format("No such file %s", (LPCTSTR)FileName);
		AfxMessageBox((LPCTSTR) message);
		return;
	}
	camera_parameters cp_temp;
	calibration_data cd_temp;
	calibration_constants cc_temp;

try {
	CString Header = PCP_CALDATA_FILEHEADER;
	char caHeader[256];
	fread(caHeader, sizeof(char), Header.GetLength() + 1, fp);
	if (Header.Compare(caHeader)) {
		fclose(fp);
		AfxMessageBox("Wrong type of file");
		return;
	}

	fread(&mdVersion, sizeof(double), 1, fp);

	if (mdVersion < PCP_FILEVERSION_LEGACY || mdVersion > PCP_FILEVERSION_RECENT) {
		fclose(fp);
		AfxMessageBox("Wrong version of cal file");
		return;
	}

	fread(&mdFOVXm, sizeof(double), 1, fp);
	fread(&mdFOVYm, sizeof(double), 1, fp);
	fread(&mdPixelSizeXm, sizeof(double), 1, fp);
	fread(&mdPixelSizeYm, sizeof(double), 1, fp);
	fread(&mdPixelSizem, sizeof(double), 1, fp);
	fread(&mdPitchm, sizeof(double), 1, fp);
	fread(&mdOriginXp, sizeof(double), 1, fp);
	fread(&mdOriginYp, sizeof(double), 1, fp);
	fread(&mdAngle, sizeof(double), 1, fp);

	fread(&m_dA, sizeof(double), 1, fp);
	fread(&m_dB, sizeof(double), 1, fp);
	fread(&m_dC, sizeof(double), 1, fp);
	fread(&m_dD, sizeof(double), 1, fp);
	fread(&m_dA_inverse, sizeof(double), 1, fp);
	fread(&m_dB_inverse, sizeof(double), 1, fp);
	fread(&m_dC_inverse, sizeof(double), 1, fp);
	fread(&m_dD_inverse, sizeof(double), 1, fp);

	mdPixelSizeXm = PIXEL_SIZE_PMODE;
	mdPixelSizeYm = PIXEL_SIZE_PMODE;
	mdPixelSizeMm = PIXEL_SIZE_PMODE;

	load_cp_cc_data(fp, &cp_temp, &cc_temp);
	load_cd_data(fp, &cd_temp);
}
catch (CFileException* e)
{
	fclose(fp);
	char caErrorStr[255];
	e->GetErrorMessage(caErrorStr, 255);
	CString message;
	message = _T("Fail to load file: ");
	message += caErrorStr;
	AfxMessageBox(message);
	return;
}
	cp = cp_temp;
	cc = cc_temp;
	cd = cd_temp;

	fclose(fp);
	return;
}

void CCalDataService::Save(CString FileName)
{
	FILE* fp = fopen((LPCTSTR) FileName, "wb");
	if (!fp) {
		CString message;
		message.Format("No such file %s", (LPCTSTR)FileName);
		AfxMessageBox((LPCTSTR) message);
		return;
	}

	char caHeader[256];
	sprintf(caHeader, "%s", PCP_CALDATA_FILEHEADER);
	fwrite(caHeader, sizeof(char), strlen(caHeader) + 1, fp);

	mdVersion = PCP_FILEVERSION_RECENT;
	fwrite(&mdVersion, sizeof(double), 1, fp);
	
	fwrite(&mdFOVXm, sizeof(double), 1, fp);
	fwrite(&mdFOVYm, sizeof(double), 1, fp);
	fwrite(&mdPixelSizeXm, sizeof(double), 1, fp);
	fwrite(&mdPixelSizeYm, sizeof(double), 1, fp);
	fwrite(&mdPixelSizem, sizeof(double), 1, fp);
	fwrite(&mdPitchm, sizeof(double), 1, fp);
	fwrite(&mdOriginXp, sizeof(double), 1, fp);
	fwrite(&mdOriginYp, sizeof(double), 1, fp);
	fwrite(&mdAngle, sizeof(double), 1, fp);

	fwrite(&m_dA, sizeof(double), 1, fp);
	fwrite(&m_dB, sizeof(double), 1, fp);
	fwrite(&m_dC, sizeof(double), 1, fp);
	fwrite(&m_dD, sizeof(double), 1, fp);
	fwrite(&m_dA_inverse, sizeof(double), 1, fp);
	fwrite(&m_dB_inverse, sizeof(double), 1, fp);
	fwrite(&m_dC_inverse, sizeof(double), 1, fp);
	fwrite(&m_dD_inverse, sizeof(double), 1, fp);

	dump_cp_cc_data(fp, &cp, &cc);
	dump_cd_data(fp, &cd);

	return;
}

void CCalDataService::IPtoRP(double dXp, double dYp, double* dpXm, double* dpYm)
{
//	image_coord_to_world_coord_LFM(dXp, dYp, dpXm, dpYm);

	if (!dpXm || !dpYm) return;

// 1. 기존
// 	double dAlpha = (double)((int)(dXp / CAMERA_PARAM_RESOLUTION));
// 	dXp -= dAlpha * CAMERA_PARAM_RESOLUTION;
// 	
// 	*dpXm = m_dA * dXp * dXp * dXp + m_dB * dXp * dXp + m_dC * dXp + m_dD;
// 	(*dpXm) += dAlpha * mdFOVXm;
// 
// 	*dpYm = dYp * mdPixelSizeYm + mdPixelSizeYm / 2.;

// 2. cskim 수정
	*dpXm = dXp * mdPixelSizeXm;
	*dpYm = dYp * mdPixelSizeYm;
}
	
void CCalDataService::ArbIPtoRP(double dXp, double dYp, double* dpXm, double* dpYm)
{
	double dFovX;
	
	IPtoRL(-0.5, 0, CAMERA_PARAM_RESOLUTION - 0.5, 0, &dFovX);	// mdFOVXm
	
	int iLx = (int)(dXp / (double)CAMERA_PARAM_RESOLUTION);
	double dMx = dXp - (double)iLx * (double)CAMERA_PARAM_RESOLUTION;
	
	IPtoRP(dMx, dYp, dpXm, dpYm);
	
	*dpXm = iLx * dFovX + *dpXm;
}

void CCalDataService::RPtoIP(double dXm, double dYm, double* dpXp, double* dpYp)
{
	// world_coord_to_image_coord_LFM(dXm, dYm, dpXp, dpYp);

	if (!dpXp || !dpYp) return;
	
// 1. 기존
// 	double dAlpha = (double)((int)(dXm / mdFOVXm));	//?
// 	dXm -= dAlpha * mdFOVXm;
// 	
// 	*dpXp = m_dA_inverse * dXm * dXm * dXm + m_dB_inverse * dXm * dXm + m_dC_inverse * dXm + m_dD_inverse;
// 	(*dpXp) += dAlpha * CAMERA_PARAM_RESOLUTION;
// 
// 	*dpYp = (dYm - mdPixelSizeYm / 2.) / mdPixelSizeYm;

// 2. cskim 수정
	*dpXp = dXm / mdPixelSizeXm;
	*dpYp = dYm / mdPixelSizeYm;
}

void CCalDataService::ArbRPtoIP(double dXm, double dYm, double* dpXp, double* dpYp)
{
	double dFovX;
	
	IPtoRL(-0.5, 0, CAMERA_PARAM_RESOLUTION - 0.5, 0, &dFovX);

	int iLx = (int)(dXm / dFovX);
	double dMx = dXm - (double)iLx * dFovX;

	RPtoIP(dMx, dYm, dpXp, dpYp);

	*dpXp = iLx * CAMERA_PARAM_RESOLUTION + *dpXp;
}

void CCalDataService::ILtoRL(double dLp, double* dLm)
{
	*dLm = dLp * mdPixelSizem;
}

void CCalDataService::RLtoIL(double dLm, double* dLp)
{
	*dLp = dLm / mdPixelSizem;
}

void CCalDataService::IPtoRL(double dXp1, double dYp1, double dXp2, double dYp2, double* dpLm)
{
	double dXm1, dYm1, dXm2, dYm2;
	IPtoRP(dXp1, dYp1, &dXm1, &dYm1);
	IPtoRP(dXp2, dYp2, &dXm2, &dYm2);
	*dpLm = sqrt((dXm1-dXm2)*(dXm1-dXm2) + (dYm1-dYm2)*(dYm1-dYm2));
}
	
void CCalDataService::RPtoIL(double dXm1, double dYm1, double dXm2, double dYm2, double* dpLp)
{
	double dXp1, dYp1, dXp2, dYp2;
	RPtoIP(dXm1, dYm1, &dXp1, &dYp1);
	RPtoIP(dXm2, dYm2, &dXp2, &dYp2);
	*dpLp = sqrt((dXp1-dXp2)*(dXp1-dXp2) + (dYp1-dYp2)*(dYp1-dYp2));
}

void CCalDataService::GetPixelSize(double* pdPixelSizeXm, double* pdPixelSizeYm, double* pdAvgSizem)
{
	*pdPixelSizeXm = mdPixelSizeXm;
	*pdPixelSizeYm = mdPixelSizeYm;
	*pdAvgSizem = mdPixelSizem;
}

double CCalDataService::GetPixelSize() 
{ 
	return mdPixelSizeMm; 
}

double CCalDataService::GetPixelArea()
{ 
	return (mdPixelSizeMm * mdPixelSizeMm); 
}
	
void CCalDataService::GetFOVSize(double* pdFOVXm, double* pdFOVYm)
{
	*pdFOVXm = mdFOVXm;
	*pdFOVYm = mdFOVYm;
}

void CCalDataService::GetResData(int *piCameraPixelSize, double *pdMmPerPixel)
{
	*piCameraPixelSize = CAMERA_PARAM_RESOLUTION;
	*pdMmPerPixel = mdPixelSizeYm;
}

void CCalDataService::Reset()
{
	mdOriginXp = 0;
	mdOriginYp = 0;
	mdAngle = 0;

	mdFOVXm = 0;
	mdFOVYm = 0;
	mdPixelSizeXm = 0;
	mdPixelSizeYm = 0;
	mdPixelSizem = 0;
	mdPitchm = 0;
}

void CCalDataService::image_coord_to_world_coord_LFM( double dImageX, double dImageY, double *dpMachineX, double *dpMachineY)
{
	if (!dpMachineX || !dpMachineY) return;

	double dAlpha = (double)((int)(dImageX / CAMERA_PARAM_RESOLUTION));
	dImageX -= dAlpha * CAMERA_PARAM_RESOLUTION;

	*dpMachineX = m_dA * dImageX * dImageX * dImageX + m_dB * dImageX * dImageX + m_dC * dImageX + m_dD;
	*dpMachineY = dImageY * mdPixelSizeYm + mdPixelSizeYm / 2.;

	(*dpMachineX) += dAlpha * mdFOVXm;
}

void CCalDataService::world_coord_to_image_coord_LFM( double dMachineX, double dMachineY, double *dpImageX, double *dpImageY)
{
	if (!dpImageX || !dpImageY) return;

	double dAlpha = (double)((int)(dMachineX / mdFOVXm));	//?
	dMachineX -= dAlpha * mdFOVXm;

	*dpImageX = m_dA_inverse * dMachineX * dMachineX * dMachineX + m_dB_inverse * dMachineX * dMachineX + m_dC_inverse*dMachineX + m_dD_inverse;
	*dpImageY = (dMachineY-mdPixelSizeYm / 2.) / mdPixelSizeYm;

	(*dpImageX) += dAlpha * CAMERA_PARAM_RESOLUTION;
}

CCalDataService& CCalDataService::operator=(CCalDataService& data)
{
	if (this == &data) return *this;

	mdVersion = data.mdVersion;
	
	mdFOVXm = data.mdFOVXm;
	mdFOVYm = data.mdFOVYm;
	
	mdPixelSizeXm = data.mdPixelSizeXm;
	mdPixelSizeYm = data.mdPixelSizeYm;
	mdPixelSizem = data.mdPixelSizem;
	
	mdPitchm = data.mdPitchm;
	
	mdOriginXp = data.mdOriginXp;
	mdOriginYp = data.mdOriginYp;
	mdAngle = data.mdAngle;
	
	m_dA = data.m_dA;
	m_dB = data.m_dB;
	m_dC = data.m_dC;
	m_dD = data.m_dD;
	m_dA_inverse = data.m_dA_inverse;
	m_dB_inverse = data.m_dB_inverse;
	m_dC_inverse = data.m_dC_inverse;
	m_dD_inverse = data.m_dD_inverse;

	return *this;
}

