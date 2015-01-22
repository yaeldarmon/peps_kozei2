#include <iostream>
#include "mc.hpp"

using namespace std;

MonteCarlo::MonteCarlo(int size_, double r_, PnlMat *rho_, PnlVect *sigma_, PnlVect *spot_, PnlVect *trend_, double h_, int samples_, double T_, int TimeSteps_, char *optionType_, double F0_)
{
	assert(strcmp(optionType_, "structured_product") == 0);
	this->opt_ = new Structured_Product(T_, TimeSteps_, size_, optionType_, F0_);
	this->mod_ = new BS(size_, r_, rho_, sigma_, spot_, trend_);
	/// Initialisation du generateur a MERSENNE : type 7 page 63
	rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));
	this->h_ = h_;
	this->samples_ = samples_;
}


MonteCarlo::~MonteCarlo()
{
#ifdef _DEBUG
	cout << "~MonteCarlo() : Ready to call pnl_rng_free " << endl;
#endif
	pnl_rng_free(&rng);
	delete this->mod_;
	delete this->opt_;
#ifdef _DEBUG
	cout << "~MonteCarlo() : Successfull call of pnl_rng_free" << endl;
#endif
}

void MonteCarlo::price(double &prix, double &ic)
{
	PnlMat *generatedPath = pnl_mat_create(this->opt_->TimeSteps_ + 1, this->opt_->size_);
	double payoff = 0;
	double sumPayoff = 0;
	double sumPayoffSquare = 0;
	prix = 0;
	ic = 0;
	for (int i = 0; i < this->samples_; i++)
	{
		this->mod_->asset(generatedPath, this->opt_->T_, this->opt_->TimeSteps_, this->rng);
		payoff = this->opt_->payoff(generatedPath);
		sumPayoff += payoff;
		sumPayoffSquare += payoff*payoff;
	}
	prix = exp(-this->mod_->r_*this->opt_->T_)*(sumPayoff / this->samples_);
	double x = exp(-2 * this->mod_->r_*this->opt_->T_)*((sumPayoffSquare / this->samples_) - (sumPayoff / this->samples_)*(sumPayoff / this->samples_));
	ic = 2 * 1.96*sqrt(x) / sqrt(this->samples_);
	pnl_mat_free(&generatedPath);
}

void MonteCarlo::price(const PnlMat *past, double t, double &prix, double &ic)
{
	PnlMat *generatedPath = pnl_mat_create(this->opt_->TimeSteps_ + 1, this->opt_->size_);
	double payoff = 0;
	double sumPayoff = 0;
	double sumPayoffSquare = 0;
	double precision = this->samples_*(1.0e-16);
	prix = 0;
	ic = 0;
	for (int i = 0; i < this->samples_; i++)
	{
		this->mod_->asset(generatedPath, t, this->opt_->TimeSteps_, this->opt_->T_, this->rng, past);
		//pnl_mat_print(generatedPath);
		payoff = this->opt_->payoff(generatedPath);
		sumPayoff += payoff;
		sumPayoffSquare += payoff*payoff;
	}
	prix = exp(-this->mod_->r_*(this->opt_->T_ - t))*(sumPayoff / this->samples_);
	double x = exp(-2 * this->mod_->r_*(this->opt_->T_ - t))*(sumPayoffSquare / this->samples_) - prix*prix;
	if (abs(x) < precision)
		x = 0;
	ic = 2 * 1.96*sqrt(x) / sqrt(this->samples_);
	pnl_mat_free(&generatedPath);
}

void MonteCarlo::delta(const PnlMat *past, double t, PnlVect *delta)
{
	pnl_vect_set_all(delta, 0);
	PnlMat *generatedPath = pnl_mat_create(this->opt_->TimeSteps_ + 1, this->opt_->size_);
	PnlMat *shiftPath_Right = pnl_mat_create(this->opt_->TimeSteps_ + 1, this->opt_->size_);
	PnlMat *shiftPath_Left = pnl_mat_create(this->opt_->TimeSteps_ + 1, this->opt_->size_);
	double tmp = 0;
	// The vector St
	PnlVect *St = pnl_vect_create_from_zero(this->opt_->size_);
	int lastIndexOfPast = 0;
	if (t == 0.0)
		pnl_vect_clone(St, this->mod_->spot_);
	else
	{
		double step = this->opt_->T_ / this->opt_->TimeSteps_;
		double dt = t / step;
		double Error = abs(dt - round(dt)) / (dt);
		if (Error <= 0.05)
			lastIndexOfPast = round(dt);
		else
			lastIndexOfPast = floor(dt) + 1;
	}

	for (int j = 0; j<this->samples_; j++)
	{
		//Generation d'une trajectoire
		this->mod_->asset(generatedPath, t, this->opt_->TimeSteps_, this->opt_->T_, this->rng, past);
		for (int d = 0; d<this->mod_->size_; d++)
		{
			// Shift à droite
			this->mod_->shift_asset(shiftPath_Right, generatedPath, d, this->h_, t, this->opt_->T_, this->opt_->TimeSteps_);
			// Shift à droite
			this->mod_->shift_asset(shiftPath_Left, generatedPath, d, -this->h_, t, this->opt_->T_, this->opt_->TimeSteps_);
			// Delta payoff Right Left
			tmp = this->opt_->payoff(shiftPath_Right) - this->opt_->payoff(shiftPath_Left);
			pnl_vect_set(delta, d, GET(delta, d) + tmp);
		}
	}
	for (int d = 0; d<this->opt_->size_; d++)
	{
		pnl_vect_set(St, d, MGET(past, lastIndexOfPast, d));
		pnl_vect_set(delta, d, GET(delta, d)*exp(-this->mod_->r_*(this->opt_->T_ - t)) / (this->samples_*this->h_ * 2 * GET(St, d)));
	}
	pnl_mat_free(&generatedPath);
	pnl_mat_free(&shiftPath_Right);
	pnl_mat_free(&shiftPath_Left);
	pnl_vect_free(&St);
}

void MonteCarlo::hedge(PnlMat *V, double &PL, int H, const PnlMat *marketPath, bool affichage, double &prix, PnlMat *delta)
{
	PnlVect *delta_cour = pnl_vect_create_from_zero(this->opt_->size_);
	PnlVect *delta_prec = pnl_vect_create_from_zero(this->opt_->size_);
	PnlVect *St = pnl_vect_copy(this->mod_->spot_);
	PnlMat *past = pnl_mat_create(this->opt_->TimeSteps_ + 1, this->opt_->size_);

	pnl_mat_set_row(past, St, 0);

	int M = (H / this->opt_->TimeSteps_);
	int ti = 1;
	double p = 0;
	double ic = 0;
	double value = 0;

	// Affichage des valeurs du portefeuille	
	///cout << " ---- \t Date n° \t PortFolio Value " << endl;
	if (affichage)
		cout << " ---- Date n° \t Valeur Portefeuille \t Actif Risqué \t\t Actif Sans Risque " << endl;
	// Initialisation du portefeuille
	this->price(p, ic);
	this->delta(past, 0, delta_cour);
	pnl_mat_set_row(delta, delta_cour, 0);
	//LET(V1, 0) = p;
	//LET(V2, 0) = pnl_vect_scalar_prod(delta_cour, St);
	//LET(V3, 0) = GET(V1, 0) - GET(V2, 0);
	MLET(V, 0, 0) = p;
	MLET(V, 0, 1) = pnl_vect_scalar_prod(delta_cour, St);
	MLET(V, 0, 2) = MGET(V, 0, 0) - MGET(V, 0, 1);
	if (affichage)
		cout << " ---- " << 0 << " \t " << MGET(V, 0, 0) << " \t\t " << MGET(V, 0, 1) << " \t\t " << MGET(V, 0, 2) << " \t " << endl;
	/// value = p-pnl_vect_scalar_prod(delta_cour,St);
	/// pnl_vect_set(V,0,value);
	/// cout << " ---- \t " << 0 << " \t \t" << GET(V,0) << endl; 
	// Calcul des Vi
	for (int i = 1; i<H; i++)
	{
		pnl_mat_get_row(St, marketPath, i);
		pnl_mat_set_row(past, St, ti);
		if ((i % M) == 0)
			ti++;
		pnl_vect_clone(delta_prec, delta_cour);
		this->delta(past, i*this->opt_->T_ / H, delta_cour);
		pnl_mat_set_row(delta, delta_cour, i);
		pnl_vect_minus_vect(delta_prec, delta_cour);

		//LET(V2, i) = pnl_vect_scalar_prod(delta_cour, St);
		//LET(V3, i) = GET(V3, i - 1)*exp(this->mod_->r_*this->opt_->T_ / H) + pnl_vect_scalar_prod(delta_prec, St);
		//LET(V1, i) = GET(V2, i) + GET(V3, i);
		MLET(V, i, 1) = pnl_vect_scalar_prod(delta_cour, St);
		MLET(V, i, 2) = MGET(V, i - 1, 2)*exp(this->mod_->r_*this->opt_->T_ / H) + pnl_vect_scalar_prod(delta_prec, St); /// t/H
		MLET(V, i, 0) = MGET(V, i, 1) + MGET(V, i, 2);
		if (affichage)
			cout << " ---- " << i << " \t " << MGET(V, i, 0) << " \t\t " << MGET(V, i, 1) << " \t\t " << MGET(V, i, 2) << " \t " << endl;
		///value = GET(V,i-1)*exp(this->mod_->r_*this->opt_->T_/H) + pnl_vect_scalar_prod(delta_prec,St);
		///pnl_vect_set(V,i,value);
		///cout << " ---- \t " << i << " \t \t" << GET(V,i) << endl;
	}
	// Calcul de VH
	pnl_mat_get_row(St, marketPath, H);
	for (int i = 0; i < this->opt_->size_; i++)
	{
		MLET(delta, H, i) = 0.0;
	}

	//LET(V3, H) = GET(V3, H - 1)*exp(this->mod_->r_*this->opt_->T_ / H);
	//LET(V2, H) = 0; /// !!!
	//LET(V1, H) = GET(V3, H);
	MLET(V, H, 2) = MGET(V, H - 1, 2)*exp(this->mod_->r_*this->opt_->T_ / H) + pnl_vect_scalar_prod(delta_cour, St); /// t/H
	MLET(V, H, 1) = 0;
	MLET(V, H, 0) = MGET(V, H, 2);
	if (affichage)
		cout << " ---- " << H << " \t " << MGET(V, H, 0) << " \t\t " << MGET(V, H, 1) << " \t\t " << MGET(V, H, 2) << " \t " << endl;
	double payoff = this->opt_->payoff(past);
	PL = MGET(V, H, 2) - payoff;
	cout << endl;
	prix = p;
	cout << "  ---- Erreur de Couverture Finale Relative en % du Prix en t = 0 : " << (PL / p) * 100 << endl;
	cout << "  ---- Erreur de Couverture Finale : " << PL << endl;
	cout << "  ---- Valeur Finale du Portefeuille : " << MGET(V, H, 0) << endl;
	cout << "  ---- Payoff  : " << payoff << endl;
	cout << "  ---- Prix  du produit structuré en t = 0 : " << p << endl;
	///value = GET(V,H-1)*exp(this->mod_->r_*this->opt_->T_/H) ;
	///pnl_vect_set(V,H,value);
	///cout << " ---- \t " << H << " \t \t" << GET(V,H) << "\t Valeur avant livraison" << endl;
	// calcul P&L
	//double payoff = this->opt_->payoff(past);

	//PL = GET(V3, H) + pnl_vect_scalar_prod(delta_cour, St) - payoff;
	//cout << "  ---- Erreur de Couverture Relative en % : " << (PL / p) * 100 << endl;

	///PL = GET(V,H) + pnl_vect_scalar_prod(delta_cour,St) - payoff;
	///cout << " " << endl;
	///cout << "  ---- Prix de l'option en 0 = " << p << endl;
	///cout << "  ---- Erreur de couverture relative en % = " << (PL/p)*100 << endl;
	// memory free
	pnl_vect_free(&St);
	pnl_vect_free(&delta_cour);
	pnl_vect_free(&delta_prec);
	pnl_mat_free(&past);
}

void MonteCarlo::hedge(PnlMat *V, double &PL, int H, bool affichage, double &prix, PnlMat *delta)
{
	PnlMat *marketPath = pnl_mat_create_from_zero(H + 1, this->opt_->size_);
	this->mod_->simul_market(marketPath, this->opt_->T_, H, this->rng);
	this->hedge(V, PL, H, marketPath, affichage, prix, delta);
	pnl_mat_free(&marketPath);
}

PnlMat* MonteCarlo::processing(PnlMat *past, double t)
{
	PnlMat *processedpast;
	int annualTradingDaysNumber = 252;
	int factor = annualTradingDaysNumber * this->opt_->T_ / this->opt_->TimeSteps_;
	int size = (past->m - 1) / factor + 1;
	int condition = t * this->opt_->TimeSteps_ / this->opt_->T_;
	double onStatementDate = t * this->opt_->TimeSteps_ / this->opt_->T_;
	if (((double)condition) == onStatementDate)
	{
		processedpast = pnl_mat_create(size, past->n);
		for (int i = 0; i < past->n; i++)
		{
			for (int j = 0; j < size; j++)
			{
				MLET(processedpast, j, i) = MGET(past, j * factor, i);
			}
		}
	}
	else
	{
		size++;
		processedpast = pnl_mat_create(size, past->n);
		for (int i = 0; i < past->n; i++)
		{
			for (int j = 0; j < size - 1; j++)
			{
				MLET(processedpast, j, i) = MGET(past, j * factor, i);
			}
			MLET(processedpast, size - 1, i) = MGET(past, past->m - 1, i);
		}
	}
	return processedpast;
}