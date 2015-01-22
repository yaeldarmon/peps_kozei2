#include "estimation.hpp"

using namespace std;
void Estimation::mat_histo(PnlMat *past, PnlMat *Mat_Histo)
{
	for (int i = 0; i < past->n; i++)
	{
		for (int j = 0; j < past->m - 1; j++)
		{
			MLET(Mat_Histo, j, i) = log(MGET(past, j + 1, i) / MGET(past, j, i));
		}
	}
}

void Estimation::moy_histo(PnlMat *Mat_Histo, PnlVect *Moy_Histo)
{
	double tmp;
	for (int i = 0; i < Mat_Histo->n; i++)
	{
		tmp = 0;
		for (int j = 0; j < Mat_Histo->m; j++)
		{
			tmp += MGET(Mat_Histo, j, i);
		}
		LET(Moy_Histo, i) = tmp;
		LET(Moy_Histo, i) /= Mat_Histo->m;
	}
}

void Estimation::sigma_histo(PnlMat *Mat_Histo, PnlVect *Moy_Histo, PnlVect *Sigma_Histo)
{
	double tmp;
	for (int i = 0; i < Mat_Histo->n; i++)
	{
		tmp = 0;
		for (int j = 0; j < Mat_Histo->m; j++)
		{
			tmp += pow(MGET(Mat_Histo, j, i), 2);
		}
		tmp /= Mat_Histo->m;
		tmp -= pow(GET(Moy_Histo, i), 2);
		LET(Sigma_Histo, i) = sqrt(tmp);
	}
}

void Estimation::sigma_emv_histo(int size, PnlVect *Sigma_Histo, PnlVect *Sigma_EMV_Histo)
{
	for (int i = 0; i < Sigma_Histo->size; i++)
	{
		LET(Sigma_EMV_Histo, i) = GET(Sigma_Histo, i) * sqrt((double)size / (size - 1));
	}

}

void Estimation::cov_histo(PnlMat *Mat_Histo, PnlVect *Moy_Histo, PnlMat *Cov_Histo)
{
	double tmp;
	for (int i = 0; i < Mat_Histo->n; i++)
	{
		for (int j = 0; j < Mat_Histo->n; j++)
		{
			tmp = 0;
			for (int k = 0; k < Mat_Histo->m; k++)
			{
				tmp += (MGET(Mat_Histo, k, i) - GET(Moy_Histo, i)) * (MGET(Mat_Histo, k, j) - GET(Moy_Histo, j));
			}
			MLET(Cov_Histo, i, j) = tmp;
			MLET(Cov_Histo, i, j) /= Mat_Histo->m;
		}
	}
}

void Estimation::rho_histo(PnlMat *Cov_Histo, PnlVect *Sigma_Histo, PnlMat *Rho_Histo)
{
	for (int i = 0; i < Cov_Histo->m; i++)
	{
		for (int j = 0; j < Cov_Histo->n; j++)
		{
			MLET(Rho_Histo, i, j) = MGET(Cov_Histo, i, j) / (GET(Sigma_Histo, i) * GET(Sigma_Histo, j));
		}
	}
}

void Estimation::My_EMV_histo(PnlMat *past, PnlVect *My_Trend_Histo, PnlVect *My_Vol_Histo, PnlMat *My_Rho_Histo)
{
	PnlMat *My_Mat_Histo = pnl_mat_create(past->m - 1, past->n);
	mat_histo(past, My_Mat_Histo);
	PnlVect *My_Moy_Histo = pnl_vect_create(My_Mat_Histo->n);
	moy_histo(My_Mat_Histo, My_Moy_Histo);
	PnlVect *My_Sigma_Histo = pnl_vect_create(My_Mat_Histo->n);
	sigma_histo(My_Mat_Histo, My_Moy_Histo, My_Sigma_Histo);
	PnlVect *My_Sigma_EMV_Histo = pnl_vect_create(My_Mat_Histo->n);
	sigma_emv_histo(My_Mat_Histo->m, My_Sigma_Histo, My_Sigma_EMV_Histo);
	PnlMat *My_Cov_Histo = pnl_mat_create(past->n, past->n);
	cov_histo(My_Mat_Histo, My_Moy_Histo, My_Cov_Histo);
	for (int i = 0; i < My_Vol_Histo->size; i++)
	{
		LET(My_Vol_Histo, i) = GET(My_Sigma_EMV_Histo, i) * sqrt(252);
	}
	for (int i = 0; i < My_Trend_Histo->size; i++)
	{
		LET(My_Trend_Histo, i) = GET(My_Moy_Histo, i) * 252 + pow(GET(My_Sigma_Histo, i), 2) / 2;
	}
	rho_histo(My_Cov_Histo, My_Sigma_Histo, My_Rho_Histo);
	pnl_mat_free(&My_Mat_Histo);
	pnl_vect_free(&My_Moy_Histo);
	pnl_vect_free(&My_Sigma_Histo);
	pnl_vect_free(&My_Sigma_EMV_Histo);
	pnl_mat_free(&My_Cov_Histo);
}
