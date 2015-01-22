// Il s'agit du fichier DLL principal.
#include "stdafx.h"
#include "PricerWrapper.h"
#include "pricer.hpp"


namespace PricerWrapper {

	WrapperClass::WrapperClass() {
		// Création paramètres
		int size = 30;
		double r = 0.05;
		double* rho = new double[size * size];
		double* sigma = new double[size];
		double* spot = new double[size];
		double* trend = new double[size];
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
				rho[i * size + j] = (i == j) ? 1 : 0.2;
			sigma[i] = 0.2;
			spot[i] = 100;
			trend[i] = 0.07;
		}
		double h = 0.1;
		int samples = 1000;
		double T = 8.0;
		int TimeSteps = 16;
		char* optionType = "structured_product";
		double InvestissementInit = 1000000;

		this->pricer = new Pricer(size, r, rho, sigma, spot, trend, h, samples, T, TimeSteps, optionType, InvestissementInit);
		this->price = 0;
		this->confidenceInterval = 0;
	}

	WrapperClass::WrapperClass(int size, double r, double* rho, double*sigma, double*spot, double*trend, double h, int samples, double T, int TimeSteps, char*optionType, double InvestissementInit) {
		this->pricer = new Pricer(size,r,rho, sigma,spot,trend,  h,  samples,  T,  TimeSteps, optionType,  InvestissementInit);
		this->price = 0;
		this->confidenceInterval = 0;
	};

	WrapperClass::~WrapperClass() {
		delete pricer;
	}

	double WrapperClass::getPrice() {
		return this->price;

	}

	double WrapperClass::getConfidence() {
		return this->confidenceInterval;
	}

	double WrapperClass::getPnL() {
		return this->pnl;
	}

	void WrapperClass::getPrice_0() {
		double px = 0, ic = 0;

		this->pricer->price_0(px, ic);

		this->price = px;
		this->confidenceInterval = ic;
	}

	void WrapperClass::getPrice_t(double t) {
		double px = 0, ic = 0;
		int annualTradingDaysNumber = 252;
		int m = t * annualTradingDaysNumber + 1;
		int n = 30;
		double *past = new double[n*m];
		past = this->pricer->simule_past(t);
		this->pricer->price_t(px, ic, past, m, n, t);

		this->price = px;
		this->confidenceInterval = ic;
	}

	void WrapperClass::hedge_T(int rebal, int n, array<double>^ portfolio, double PnL, double px, array<double>^ delta) {
		pin_ptr<double> portPin = &portfolio[0];
		double* pportfolio = portPin;

		pin_ptr<double> PnLPin = &PnL;
		double* pPnL = PnLPin;

		pin_ptr<double> pxPin = &px;
		double* ppx = pxPin;

		pin_ptr<double> deltaPin = &delta[0];
		double* pdelta = deltaPin;

		this->pricer->hedge_T(rebal, n, pportfolio, pPnL, ppx, pdelta);

		this->pnl = PnL;
		this->price = px;
	}
	
}