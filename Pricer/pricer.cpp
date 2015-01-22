#include "pricer.hpp"

using namespace std;

Pricer::Pricer(int size, double r, double* rho, double*sigma, double*spot, double*trend, double h, int samples, double T, int TimeSteps, char*optionType, double InvestissementInit) {
	PnlMat* rho_ = pnl_mat_create_from_ptr(size, size, rho);
	PnlVect* sigma_ = pnl_vect_create_from_ptr(size, sigma);
	PnlVect* spot_ = pnl_vect_create_from_ptr(size, spot);
	PnlVect* trend_ = pnl_vect_create_from_ptr(size, trend);
	this->mc = new MonteCarlo(size, r, rho_, sigma_, spot_, trend_, h, samples, T, TimeSteps, optionType, InvestissementInit);

	pnl_mat_free(&rho_);
	pnl_vect_free(&sigma_);
	pnl_vect_free(&spot_);
	pnl_vect_free(&trend_);

	int i = 0;
}

void Pricer::price_0(double &prix, double &ic) {
	double tmpprix = 0, tmpic = 0;
	mc->price(tmpprix, tmpic);
	prix = tmpprix;
	ic = tmpic;
}

void Pricer::price_t(double &prix, double &ic, double* past, int m, int n, double t) {
	double tmpprix = 0, tmpic = 0;
	PnlMat* pnlPast = pnl_mat_create_from_ptr(m, n, past);
	mc->price(pnlPast, t, tmpprix, tmpic);
	prix = tmpprix;
	ic = tmpic;
	pnl_mat_free(&pnlPast);
}

void Pricer::delta_t(double* delta, double* past, int m, int n, double t) {
	double tmpprix = 0, tmpic = 0;
	PnlMat* pnlPast = pnl_mat_create_from_ptr(m, n, past);
	PnlVect* pnlDelta = pnl_vect_create_from_ptr(n, delta);
	mc->delta(pnlPast, t, pnlDelta);
	for (int i = 0; i < n; i++)
		delta[i] = GET(pnlDelta, i);
	pnl_vect_free(&pnlDelta);
	pnl_mat_free(&pnlPast);
}

void Pricer::hedge_T(int rebal, int n, double* portfolio, double* PnL, double *px, double*delta) {
	double pnl = 0, prix = 0;
	PnlMat* pnlPort = pnl_mat_create_from_ptr(rebal + 1, 3, portfolio);
	PnlMat* pnlDelta = pnl_mat_create_from_ptr(rebal + 1, n, delta);
	mc->hedge(pnlPort, pnl, rebal, false, prix, pnlDelta);
	for (int i = 0; i < rebal + 1; i++) {
		for (int j = 0; j < n; j++)
			delta[i*n + j] = MGET(pnlDelta, i, j);
	}
	*PnL = pnl;
	*px = prix;
	pnl_mat_free(&pnlDelta);
	pnl_mat_free(&pnlPort);
}

void Pricer::performance(int rebal, double* portfolio, double* PnL, int nb_simul) {

}

Pricer::~Pricer()
{
	delete mc;
}

double* Pricer::simule_past(double t)
{

	int annualTradingDaysNumber = 252;
	PnlMat *pastMatrix;
	if (t == 0.0)
	{
		pastMatrix = pnl_mat_create(1, this->mc->mod_->size_);
		pnl_mat_set_row(pastMatrix, this->mc->mod_->spot_, 0);
	}
	else
	{
		pastMatrix = pnl_mat_create(t * annualTradingDaysNumber + 1, this->mc->mod_->size_);
		this->mc->mod_->simul_market(pastMatrix, t, t * annualTradingDaysNumber, this->mc->rng);
	}
	double* past = new double[pastMatrix->m*pastMatrix->n];
	for (int i = 0; i < pastMatrix->m*pastMatrix->n; i++)
		past[i] = pastMatrix->array[i];
	return past;
}

void Pricer::test(int rebal, double *port)
{
	for (int i = 0; i < (rebal + 1) * 3; i++)
		port[i] = i;
}