#ifndef _OPTION_HPP
#define _OPTION_HPP

#include "pnl/pnl_matrix.h"

/// \brief Classe Option (classe abstraite)
class Option
{
public:
	double T_; /*! maturit� de l'option */
	int TimeSteps_; /*! nombre de pas de temps de discr�tisation */
	int size_; /*! nombre d'actifs sous-jacents de l'option (dimension du mod�le), redondant avec BS::size_ */
	char *optionType_; /*! type de l'option */

	/*!
	* Destructeur de la classe Option
	*/
	virtual ~Option()
	{
	}

	/*!
	* Calcul de la valeur du payoff sur la trajectoire
	*
	* @param[in] path est une matrice de taille D x (N + 1) contenant une trajectoire du mod�le telle que cr��e par la fonction asset
	* @return phi(trajectoire)
	*/
	virtual double payoff(const PnlMat *path) = 0;

	/*!
	* Affichage des caract�ristiques de l'option
	*/
	virtual void print() = 0;
};

#endif /* _OPTION_HPP */
