/*!	\file cInterface.h
	\brief En-t�te classes d'interface
*/

#ifndef SYNTHESE_CINTERFACE_H
#define SYNTHESE_CINTERFACE_H

class cInterface;
#include "cInterface_Objet_AEvaluer_PageEcran.h"
#include "cPeriodeJournee.h"


// Inclusions
#include "cTexte.h"

#include <string>
#include <vector>

/** Impl�mentation de la notion d'Interface
	\author Hugues Romain
	\date 2001-2005
	\todo Lancer une d�marche globale de d�finition du format de sortie en collaboration avec cTexte et ses d�riv�s
	@ingroup m11
	
Cette classe d�finit les diff�rentes interfaces pour l'affichage de r�sultats issus de calculs SYNTHESE. Une interface est constitu�e de mod�les d'affichage, permettant la d�finition du graphisme en sortie, fournis � SYNTHESE en tant que donn�es. Des mod�les de fichiers de sortie doivent �tre fournis au chargement d'apr�s un format de codage d�fini pour l'occasion.</p>

La liste des \ref InterfaceObjetsStandard d�finit les objets qui peuvent �tre d�crits par un mod�le. La fourniture d'un mod�le pour chacun de ces objets est facultative. En cas de demande d'affichage d'un objet pour lequel aucun mod�le n'a �t� fourni, une page vide sera retourn�e.

Les mod�les sont d�finis par la classe cElementInterface et ses d�riv�s, et sont ind�x�s dans le tableau de pointeurs vElement, selon une indexation par num�ro d'objet standard, selon la nomenclature des \ref InterfaceObjetsStandard .

Des constantes permettent de d�signer les index du tableau vElement et sont d�crits dans le fichier Parametres.h.

Les p�riodes de la journ�e correspondent � des masques permettant de n'effectuer des calculs qu'entre deux heures de la journ�e (ex: fiche horaire de 10:00 � 12:00)

@attention Par convention, la p�riode portant l'index 0 d�crit la journ�e enti�re. Elle doit donc �tre d�crite obligatoirement par les donn�es pour que l'interface soit valide.
*/
class cInterface
{
public:
	/** Période journée */
	static const size_t ALL_DAY_PERIOD;

private:
	const size_t _id;									//!< Index de l'interface dans la base de donn�es SYNTHESE
	cInterface_Objet_AEvaluer_PageEcran*			_Element;								//!< Tableau des �l�ments standard d�finis
	vector<cPeriodeJournee*>	_Periode;								//!< Tableau des p�riodes de la journ�e
	cTexte											_LibelleJourSemaine[JOURS_PAR_SEMAINE];	//!< Tableau des libell�s des jours de semaine
	cTexte											_LibelleMois[MOIS_PAR_AN + 1];			//!< Tableau des libell�s des mois
	cTableauDynamiqueObjets<cTexte>					_PrefixesAlerte;						//!< Pr�fixes de messages d'alerte


public:
	//!	\name M�thode d'enregistrement
	//@{
//	bool					Enregistre(cObjetInterfaceStandard&, tIndex);
	//@}
	
	//!	\name Accesseurs
	//@{
	cInterface_Objet_AEvaluer_PageEcran&		Element(tIndex);
	const cInterface_Objet_AEvaluer_PageEcran&	operator[](tIndex)									const;
	const size_t&										Index()												const;
	const cPeriodeJournee*						GetPeriode(size_t __Index = ALL_DAY_PERIOD)	const;
	const cTexte&								getPrefixeAlerte(tIndex __NiveauAlerte)				const;
// 	const cTexte&								LibelleJourSemaine(tIndex)							const;
	//@}
	
	//!	\name Calculateurs
	//@{
	bool	OK()																							const;
	
	/** Affichage d'une date selon les param�tres de l'interface
		@param __Date Date � afficher
		@param __Textuel true pour obtenir une date en texte, false pour l'obtenir au format num�rique (d�faut = true)
		@param __JourDeSemaine true pour afficher le jour de la semaine (d�faut = true)
		@param __Annee true pour afficher l'ann�e (d�faut = true)
	*/
	void AfficheDate(std::ostream& __Flux, const cDate& __Date, bool __Textuel=true, bool __JourDeSemaine = true, bool __Annee=true)	const
	{
		if (__JourDeSemaine && __Textuel)
			__Flux << _LibelleJourSemaine[__Date.JourSemaine()] << " ";
		__Flux << __Date.Jour();
		if (__Textuel)
			__Flux << " " << _LibelleMois[__Date.Mois()] << " ";
		else
			__Flux << "/" << __Date.Mois();
		if (__Annee)
		{
			if (__Textuel)
				__Flux << " ";
			else
				__Flux << "/";
			__Flux << __Date.Annee();
		}
	}
	//@}
	
	//!	\name Modificateurs
	//@{
	void					AddPeriode(cPeriodeJournee*);
	bool					SetLibelleJour(tIndex, const cTexte&);
	bool					SetLibelleMois(tIndex, const cTexte&);
	bool					SetPrefixeAlerte(tIndex, const cTexte&);
	//@}
	
	cInterface(const size_t&);
	~cInterface();
};


/** @} */


#endif
