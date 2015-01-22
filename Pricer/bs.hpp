#ifndef _BS_HPP
#define _BS_HPP

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <cmath>
#include <ctime>
#include <cassert>

/// \brief Mod�le de Black Scholes
class BS
{
public:
	int size_; /*! nombre d'actifs du mod�le */
	double r_; /*! taux d'int�r�t */
	PnlMat *rho_; /*! matrice des coefficients de corr�lation entre les actifs */
	PnlVect *sigma_; /*! vecteur de volatilit�s des diff�rents actifs */
	PnlVect *spot_; /*! vecteur des valeurs initiales des diff�rents actifs */
	PnlVect *trend_; /*! vecteur des tendances des diff�rents actifs */
	PnlMat *L; /*! factorisation de Cholesky de la matrice de corr�lation */


	/*!
	* Constructeur de la classe BS
	*
	* @param[in] size_ est le nombre d'actifs sous-jacents de l'option (dimension du mod�le)
	* @param[in] r_ est le taux d'int�r�t
	* @param[in] rho_ est la matrice des coefficients de corr�lation entre les actifs
	* @param[in] sigma_ est le vecteur de volatilit�s des diff�rents actifs
	* @param[in] spot_ est le vecteur des valeurs initiales des diff�rents actifs
	* @param[in] trend_ est le vecteur des tendances des diff�rents actifs
	*/
	BS(int size_, double r_, PnlMat *rho_, PnlVect *sigma_, PnlVect *spot_, PnlVect *trend_);

	/*!
	* Destructeur de la classe Structured_Product
	*/
	~BS();

	/*!
	* G�n�ration d'une trajectoire du mod�le et stockage dans path
	*
	* @param[out] path contient une trajectoire du mod�le (matrice de taille D x (N + 1))
	* @param[in] T est la maturit�
	* @param[in] N est le nombre de dates de constatation
	* @param[in] rng est le g�n�rateur de nombres al�atoires
	*/
	void asset(PnlMat *path, double T, int N, PnlRng *rng);

	/*!
	* Calcul d'une trajectoire du sous-jacent connaissant le
	* pass� jusqu'� la date t
	*
	* @param[out] path contient une trajectoire du sous-jacent donn�e jusqu'� l'instant t par la matrice past
	* @param[in] t est la date jusqu'� laquelle on connait la trajectoire (t n'est pas forc�ment une date de discr�tisation)
	* @param[in] N est le nombre de pas de constatation
	* @param[in] T est la date jusqu'� laquelle on simule la trajectoire
	* @param[in] past est la trajectoire r�alis�e jusqu'a la date t
	* @param[in] rng est le g�n�rateur de nombres al�atoires
	*/
	void asset(PnlMat *path, double t, int N, double T, PnlRng *rng, const PnlMat *past);

	/*!
	* Shift d'une trajectoire du sous-jacent
	*
	* @param[in] path contient en input la trajectoire du sous-jacent
	* @param[out] shift_path contient la trajectoire path dont la composante d a �t� shift�e par (1 + h) � partir de la date t
	* @param[in] t est la date � partir de laquelle on shift
	* @param[in] h est le pas de diff�rences finies
	* @param[in] d est l'indice du sous-jacent � shifter
	* @param[in] timestep est le pas de constatation du sous-jacent
	* @param[in] T est la date jusqu'� laquelle on simule la trajectoire
	*/
	void shift_asset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double T, double timestep);

	/*!
	* G�n�ration d'une simulation du march� et stockage dans path
	*
	* @param[out] path contient une simulation du march� (matrice de taille (H + 1) x D)
	* @param[in] T est la maturit�
	* @param[in] H est le nombre de dates de rebalancement
	* @param[in] rng est le g�n�rateur de nombres al�atoires
	*/
	void simul_market(PnlMat *path, double T, int H, PnlRng *rng);
};

#endif /* _BS_HPP */
