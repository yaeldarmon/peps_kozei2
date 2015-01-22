#include <iostream>
#include "structured_product.hpp"

using namespace std;

Structured_Product::Structured_Product(double T_, int TimeSteps_, int size_, char *optionType_, double F0_)
{
	this->T_ = T_;
	this->TimeSteps_ = TimeSteps_;
	this->size_ = size_;
	this->optionType_ = optionType_;
	this->F0_ = F0_;
}

Structured_Product::~Structured_Product()
{
#ifdef _DEBUG
	cout << "~Structured_Product()" << endl;
#endif
}

double Structured_Product::payoff(const PnlMat *path)
{
	double payoff = 0.0;
	double payoff_temp;
	for (int i = 1; i < this->TimeSteps_ + 1; i++)
	{
		payoff_temp = 0.0;
		for (int j = 0; j < this->size_; j++)
		{
			payoff_temp += (MGET(path, i, j) - MGET(path, 0, j)) / MGET(path, 0, j);
		}
		payoff_temp = fmax(payoff_temp / this->size_, 0);
		payoff += payoff_temp;
	}
	payoff = F0_ * (1.0 + 0.6 * (1.0 / this->TimeSteps_) * payoff);
	return payoff;
}

void Structured_Product::print()
{
	cout << "option type " << this->optionType_ << endl;
	cout << "maturity " << this->T_ << endl;
	cout << "timestep number " << this->TimeSteps_ << endl;
	cout << "option size " << this->size_ << endl;
	cout << "invested capital " << this->F0_ << endl;
}
