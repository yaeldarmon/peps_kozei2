#pragma once
#define DLLEXP   __declspec( dllexport )

#include "mc.hpp"

class DLLEXP Pricer {
	private:
		MonteCarlo *mc;

	public:
		Pricer(int size, double r, double* rho, double*sigma, double*spot, double*trend, double h, int samples, double T, int TimeSteps, char*optionType, double InvestissementInit);
		void price_0(double &prix, double &ic); //prix à lorigine du contrat
		void price_t(double &prix, double &ic, double* past, int m, int n, double t); // prix mainenant
		void hedge_T(int rebal, int n, double* portfolio, double* PnL, double *px, double*delta); //evolution de la couverture sur les 8 ans
		void delta_t(double* delta, double* past, int m, int n, double t); //compostiion du pf de couv maintenant 
		void performance(int rebal, double* portfolio, double* PnL, int nb_simul);
	
		double* simule_past(double t);
		void test(int rebal, double *port);
		~Pricer();
};