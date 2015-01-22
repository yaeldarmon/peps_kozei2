#pragma once
#include "pricer.hpp"
using namespace System;

namespace PricerWrapper {

	public ref class WrapperClass
	{
	private:
		Pricer *pricer;
		double price;
		double confidenceInterval;
		double pnl;


	public:
		WrapperClass();
		WrapperClass(int size, double r, double* rho, double*sigma, double*spot, double*trend, double h, int samples, double T, int TimeSteps, char*optionType, double InvestissementInit);
		~WrapperClass();
		
		double getPrice();
		double getConfidence();
		double getPnL();

		void getPrice_0();
		void getPrice_t(double t);
		void hedge_T(int rebal, int n, array<double>^ portfolio, double PnL, double px, array<double>^ delta);
	};
}
