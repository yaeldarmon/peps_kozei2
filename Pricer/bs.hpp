#ifndef _BS_HPP
#define _BS_HPP

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <cmath>
#include <ctime>
#include <cassert>

/// \brief Modèle de Black Scholes
class BS
{
public:
	int size_; /*! nombre d'actifs du modèle */
	double r_; /*! taux d'intérêt */
	PnlMat *rho_; /*! matrice des coefficients de corrélation entre les actifs */
	PnlVect *sigma_; /*! vecteur de volatilités des différents actifs */
	PnlVect *spot_; /*! vecteur des valeurs initiales des différents actifs */
	PnlVect *trend_; /*! vecteur des tendances des différents actifs */
	PnlMat *L; /*! factorisation de Cholesky de la matrice de corrélation */


	/*!
	* Constructeur de la classe BS
	*
	* @param[in] size_ est le nombre d'actifs sous-jacents de l'option (dimension du modèle)
	* @param[in] r_ est le taux d'intérêt
	* @param[in] rho_ est la matrice des coefficients de corrélation entre les actifs
	* @param[in] sigma_ est le vecteur de volatilités des différents actifs
	* @param[in] spot_ est le vecteur des valeurs initiales des différents actifs
	* @param[in] trend_ est le vecteur des tendances des différents actifs
	*/
	BS(int size_, double r_, PnlMat *rho_, PnlVect *sigma_, PnlVect *spot_, PnlVect *trend_);

	/*!
	* Destructeur de la classe Structured_Product
	*/
	~BS();

	/*!
	* Génération d'une trajectoire du modèle et stockage dans path
	*
	* @param[out] path contient une trajectoire du modèle (matrice de taille D x (N + 1))
	* @param[in] T est la maturité
	* @param[in] N est le nombre de dates de constatation
	* @param[in] rng est le générateur de nombres aléatoires
	*/
	void asset(PnlMat *path, double T, int N, PnlRng *rng);

	/*!
	* Calcul d'une trajectoire du sous-jacent connaissant le
	* passé jusqu'à la date t
	*
	* @param[out] path contient une trajectoire du sous-jacent donnée jusqu'à l'instant t par la matrice past
	* @param[in] t est la date jusqu'à laquelle on connait la trajectoire (t n'est pas forcément une date de discrétisation)
	* @param[in] N est le nombre de pas de constatation
	* @param[in] T est la date jusqu'à laquelle on simule la trajectoire
	* @param[in] past est la trajectoire réalisée jusqu'a la date t
	* @param[in] rng est le générateur de nombres aléatoires
	*/
	void asset(PnlMat *path, double t, int N, double T, PnlRng *rng, const PnlMat *past);

	/*!
	* Shift d'une trajectoire du sous-jacent
	*
	* @param[in] path contient en input la trajectoire du sous-jacent
	* @param[out] shift_path contient la trajectoire path dont la composante d a été shiftée par (1 + h) à partir de la date t
	* @param[in] t est la date à partir de laquelle on shift
	* @param[in] h est le pas de différences finies
	* @param[in] d est l'indice du sous-jacent à shifter
	* @param[in] timestep est le pas de constatation du sous-jacent
	* @param[in] T est la date jusqu'à laquelle on simule la trajectoire
	*/
	void shift_asset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double T, double timestep);

	/*!
	* Génération d'une simulation du marché et stockage dans path
	*
	* @param[out] path contient une simulation du marché (matrice de taille (H + 1) x D)
	* @param[in] T est la maturité
	* @param[in] H est le nombre de dates de rebalancement
	* @param[in] rng est le générateur de nombres aléatoires
	*/
	void simul_market(PnlMat *path, double T, int H, PnlRng *rng);
};

#endif /* _BS_HPP */
