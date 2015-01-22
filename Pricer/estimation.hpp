#ifndef _ESTIMATION_HPP
#define _ESTIMATION_HPP

#include <cmath>
#include <ctime>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class Estimation
{
public:
	static void mat_histo(PnlMat *past, PnlMat *Mat_Histo);

	static void moy_histo(PnlMat *Mat_Histo, PnlVect *Moy_Histo);

	static void sigma_histo(PnlMat *Mat_Histo, PnlVect *Moy_Histo, PnlVect *Sigma_Histo);

	static void sigma_emv_histo(int size, PnlVect *Sigma_Histo, PnlVect *Sigma_EMV_Histo);

	static void cov_histo(PnlMat *Mat_Histo, PnlVect *Moy_Histo, PnlMat *Cov_Histo);

	static void rho_histo(PnlMat *Cov_Histo, PnlVect *Sigma_Histo, PnlMat *Rho_Histo);

	static void My_EMV_histo(PnlMat *past, PnlVect *My_Trend_Histo, PnlVect *My_Vol_Histo, PnlMat *My_Rho_Histo);
};

#endif /* _ESTIMATION_HPP */
