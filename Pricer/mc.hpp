#ifndef _MC_HPP
#define _MC_HPP

#include "option.hpp"
#include "structured_product.hpp"
#include "bs.hpp"
#include <cmath>
#include <ctime>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#include <cassert>
#include <iomanip> 

class MonteCarlo
{
public:
	BS *mod_; /*! pointeur vers le mod�le */
	Option *opt_; /*! pointeur sur l'option */
	PnlRng *rng; /*! pointeur sur le g�n�rateur */
	double h_; /*! pas de diff�rence finie */
	int samples_; /*! nombre de tirages Monte Carlo */

	/*!
	* Constructeur de la classe MonteCarlo
	*
	* @param[in] size_ est le nombre d'actifs sous-jacents de l'option (dimension du mod�le)
	* @param[in] r_ est le taux d'int�r�t
	* @param[in] rho_ est la matrice des coefficients de corr�lation entre les actifs
	* @param[in] sigma_ est le vecteur de volatilit�s des diff�rents actifs
	* @param[in] spot_ est le vecteur des valeurs initiales des diff�rents actifs
	* @param[in] trend_ est le vecteur des tendances des diff�rents actifs
	* @param[in] h_ est le pas de diff�rence finie
	* @param[in] samples_ est le nombre de tirages Monte Carlo
	* @param[in] T_ est la maturit� de l'option
	* @param[in] Time_Steps_ est le nomre de pas de temps de discr�tisation
	* @param[in] optionType_ est le type de l'option
	* @param[in] F0_ est le capital investi
	*/
	MonteCarlo(int size_, double r_, PnlMat *rho_, PnlVect *sigma_, PnlVect *spot_, PnlVect *trend_, double h_, int samples_, double T_, int TimeSteps_, char *optionType_, double F0_);


	/*!
	* Destructeur de la classe MonteCarlo
	*/
	~MonteCarlo();

	/*!
	* Calcul du prix de l'option � la date 0
	*
	* @param[out] prix valeur de l'estimateur Monte Carlo
	* @param[out] ic largeur de l'intervalle de confiance
	*/
	void price(double &prix, double &ic);

	/*!
	* Calcul du prix de l'option � la date t
	*
	* @param[in]  past contient la trajectoire du sous-jacent
	* jusqu'� l'instant t
	* @param[in] t date � laquelle le calcul est fait
	* @param[out] prix contient le prix
	* @param[out] ic contient la largeur de l'intervalle
	* de confiance sur le calcul du prix
	*/
	void price(const PnlMat *past, double t, double &prix, double &ic);

	/*!
	* Calcul du delta de l'option � la date t
	*
	* @param[in] past contient la trajectoire du sous-jacent
	* jusqu'� l'instant t
	* @param[in] t date � laquelle le calcul est fait
	* @param[out] delta contient le vecteur de delta
	*/
	void delta(const PnlMat *past, double t, PnlVect *delta);

	/*!
	* Construction du portefeuille de couverture et calcul du P&L
	*
	* @param[out] V matrice des valeurs du portefeuille de couverture
	* de dimension (H + 1) * 3 (1�re colonne : portefeuille de couverture ; 2�me colonne : portefeuille "risqu�" ; 3�me colonne : portefeuille "sans riqsue")
	* @param[in]  H nombre de date de constatation
	* @param[in]  marketPath matrice de taille D x (H + 1) qui contient
	* une simulation du march�

	* @param[out] PL Profit and Loss
	*/
	void hedge(PnlMat *V, double &PL, int H, const PnlMat *marketPath, bool affichage, double &prix, PnlMat *delta);

	/*!
	* Construction du portefeuille de couverture et calcul du P&L
	*
	* @param[in]  H nombre de date de constatation
	* @param[out] V matrice des valeurs du portefeuille de couverture
	* de dimension (H + 1) * 3 (1�re colonne : portefeuille de couverture ; 2�me colonne : portefeuille "risqu�" ; 3�me colonne : portefeuille "sans riqsue")

	* @param[out] PL Profit and Loss
	*/
	void hedge(PnlMat *V, double &PL, int H, bool affichage, double &prix, PnlMat *delta);

	/*!
	* Cr�ation d'une matrice du pass� sur la grille de discr�tisation � partir d'une matrice de pass� journali�re � une date t > 0
	*/
	PnlMat* processing(PnlMat *past, double t);
};

#endif /* _MC_H */
