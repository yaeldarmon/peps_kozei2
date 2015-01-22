#ifndef _STRUCTURED_PRODUCT_HPP
#define _STRUCTURED_PRODUCT_HPP

#include "option.hpp"
#include "pnl/pnl_matrix.h"
#include <cmath>
#include <cassert>

/// \brief Classe Structured_Product pour l'option constituant le produit structur�
class Structured_Product : public Option
{
public:
	double F0_;
	/*!
	* Constructeur de la classe Structured_Product
	*
	* @param[in] T_ est la maturit� de l'option
	* @param[in] Time_Steps_ est le nomre de pas de temps de discr�tisation
	* @param[in] size est le nombre d'actifs sous-jacents de l'option (dimension du mod�le)
	* @param[in] optionType_ est le type de l'option
	* @param[in] F0_ est le capital investi
	*/
	Structured_Product(double T_, int TimeSteps_, int size, char *optionType_, double F0_);

	/*!
	* Destructeur de la classe Structured_Product
	*/
	virtual ~Structured_Product();

	/*!
	* Calcul de la valeur du payoff sur la trajectoire
	*
	* @param[in] path est une matrice de taille D x (N + 1) contenant une trajectoire du mod�le telle que cr��e par la fonction asset
	* @return phi(trajectoire) = payoff(trajectoire)
	*/
	double payoff(const PnlMat *path);

	/*!
	* Affichage des caract�ristiques de l'option
	*/
	void print();
};

#endif /* _STRUCTURED_PRODUCT_H */
