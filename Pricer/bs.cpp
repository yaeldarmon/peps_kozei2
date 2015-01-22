#include <iostream>
#include "bs.hpp"

using namespace std;


BS::BS(int size_, double r_, PnlMat *rho_, PnlVect *sigma_, PnlVect *spot_, PnlVect *trend_)
{
	this->size_ = size_;
	this->r_ = r_;
	this->rho_ = pnl_mat_copy(rho_);
	this->sigma_ = pnl_vect_copy(sigma_);
	this->spot_ = pnl_vect_copy(spot_);
	this->trend_ = pnl_vect_copy(trend_);
	if (this->size_ == 1)
	{
		assert(MGET(this->rho_, 0, 0) == 1.0);
	}
	else
	{
		assert((MGET(this->rho_, 1, 0) < 1.0) && (MGET(this->rho_, 1, 0) > -1.0 / ((double) this->size_ - 1)));
	}
	L = pnl_mat_copy(this->rho_);
	pnl_mat_chol(L);
}

BS::~BS()
{
#ifdef _DEBUG
	cout << "~BS() : Ready to call pnl_vect_free on sigma, spot and trend  " << endl;
#endif
	pnl_vect_free(&this->sigma_);
	pnl_mat_free(&this->rho_);
	pnl_vect_free(&this->spot_);
	pnl_vect_free(&this->trend_);
	pnl_mat_free(&L);
#ifdef _DEBUG
	cout << "~BS() : Successfull call of pnl_rng_free" << endl;
#endif
}

void BS::asset(PnlMat *path, double T, int N, PnlRng *rng)
{
	double step = T / N;
	double prodScal = 0;
	double sigma_d = 0;

	// The Gaussian vector
	PnlVect *G = pnl_vect_create_from_zero(this->size_);
	PnlVect *Ld = pnl_vect_create_from_zero(this->size_);
	// First col of path contains spot
	for (int d = 0; d < this->size_; d++)
		MLET(path, 0, d) = GET(this->spot_, d);

	// Generation from 2 to N+1(th) column
	for (int ti = 1; ti < N + 1; ti++)
	{
		// Gaussian Dimension size_ generation
		pnl_vect_rng_normal(G, this->size_, rng);
		for (int d = 0; d < this->size_; d++)
		{
			pnl_mat_get_row(Ld, L, d);
			prodScal = pnl_vect_scalar_prod(Ld, G);
			sigma_d = GET(this->sigma_, d);
			MLET(path, ti, d) = MGET(path, ti - 1, d)*exp((this->r_ - pow(sigma_d, 2) / 2)*step + sigma_d*sqrt(step)*prodScal);
		}
	}

	// Memory free
	pnl_vect_free(&G);
	pnl_vect_free(&Ld);
}

void BS::asset(PnlMat *path, double t, int N, double T, PnlRng *rng, const PnlMat *past)
{
	if (t == 0.0)
	{
		this->asset(path, T, N, rng);
		return;
	}
	double step = T / N;
	double dt = t / step;
	double Error = abs(dt - round(dt)) / (dt);
	bool copySt = false;
	int lastIndexOfPast = 0;
	if (Error <= 0.05)
	{
		lastIndexOfPast = round(dt);
		copySt = true;
	}
	else{
		lastIndexOfPast = floor(dt);
		copySt = false;
	}
	double prodScal = 0;
	double sigma_d = 0;
	// Copy of the past on the generated path
	for (int ti = 0; ti <= lastIndexOfPast; ti++)
	{
		for (int d = 0; d < this->size_; d++)
		{
			MLET(path, ti, d) = MGET(past, ti, d);
		}
	}
	// The vector St
	PnlVect *St = pnl_vect_create_from_zero(this->size_);
	if (!copySt)
	{
		for (int d = 0; d < this->size_; d++)
			pnl_vect_set(St, d, MGET(past, lastIndexOfPast + 1, d));
		//pnl_vect_set(St,d,MGET(past,past->m-1,d));
	}
	// The Gaussian vector
	PnlVect *G = pnl_vect_create_from_zero(this->size_);
	PnlVect *Ld = pnl_vect_create_from_zero(this->size_);
	// Generation from 2 to N+1(th) column
	for (int ti = lastIndexOfPast + 1; ti < N + 1; ti++)
	{
		// Gaussian Dimension size_ generation
		pnl_vect_rng_normal(G, this->size_, rng);
		for (int d = 0; d < this->size_; d++)
		{
			pnl_mat_get_row(Ld, L, d);
			prodScal = pnl_vect_scalar_prod(Ld, G);
			sigma_d = GET(this->sigma_, d);
			if ((ti == lastIndexOfPast + 1) && (!copySt))
				MLET(path, ti, d) = GET(St, d)*exp((this->r_ - pow(sigma_d, 2) / 2)*(ti*step - t) + sigma_d*sqrt(ti*step - t)*prodScal);
			else
				MLET(path, ti, d) = MGET(path, ti - 1, d)*exp((this->r_ - pow(sigma_d, 2) / 2)*step + sigma_d*sqrt(step)*prodScal);
		}
	}
	// Memory free
	pnl_vect_free(&G);
	pnl_vect_free(&Ld);
	pnl_vect_free(&St);
}

void BS::shift_asset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double T, double timestep)
{
	int pastSize = floor((timestep / T)*t);
	for (int ti = 0; ti < timestep + 1; ti++)
	{
		for (int j = 0; j < this->size_; j++)
		{
			if ((ti > pastSize) && (j == d))
				MLET(shift_path, ti, j) = (1 + h)*MGET(path, ti, j);
			else
				MLET(shift_path, ti, j) = MGET(path, ti, j);
		}
	}
}

void BS::simul_market(PnlMat *path, double T, int H, PnlRng *rng)
{
	double step = T / H;
	double prodScal = 0;
	double sigma_d = 0;
	double mu_d = 0;

	// The Gaussian vector
	PnlVect *G = pnl_vect_create_from_zero(this->size_);
	PnlVect *Ld = pnl_vect_create_from_zero(this->size_);
	// First col of path contains spot
	for (int d = 0; d < this->size_; d++)
		MLET(path, 0, d) = GET(this->spot_, d);

	// Generation from 2 to H+1(th) column
	for (int ti = 1; ti < H + 1; ti++)
	{
		// Gaussian Dimension size_ generation
		pnl_vect_rng_normal(G, this->size_, rng);
		for (int d = 0; d < this->size_; d++)
		{
			pnl_mat_get_row(Ld, L, d);
			prodScal = pnl_vect_scalar_prod(Ld, G);
			sigma_d = GET(this->sigma_, d);
			mu_d = GET(this->trend_, d);
			MLET(path, ti, d) = MGET(path, ti - 1, d)*exp((mu_d - pow(sigma_d, 2) / 2)*step + sigma_d*sqrt(step)*prodScal);
		}
	}
	// Memory free
	pnl_vect_free(&G);
	pnl_vect_free(&Ld);
}
