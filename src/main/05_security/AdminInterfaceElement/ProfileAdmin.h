
#ifndef SYNTHESE_SECURITY_PROFILE_H
#define SYNTHESE_SECURITY_PROFILE_H

namespace synthese
{
	namespace interfaces
	{
		/** Ecran d'�dition de profil.
			@ingroup m05
		
			@image html cap_admin_profile.png
			@image latex cap_admin_profile.png "Maquette d'�cran d'�dition de profil" width=14cm

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::HomeAdmin
				- Lien vers synthese::interfaces::ProfilesAdmin
				- Nom du profil

			<i>Zone de contenus</i> :
				-# Champ de <b>modification du nom</b>, accompagn� d'un bouton <tt>Supprimer</tt>. Champ obligatoire. La tentative d'enregistrement d'un nom vide produit le message d'erreur suivant :
					@code Un profil doit poss�der un nom @endcode
					Le nom doit �tre unique. En cas de doublon, la modification du nom est refus�e et le message d'erreur suivant s'affiche :
					@code Un profil portant le m�me nom existe d�j� @endcode
				-# <b>Tableau des habilitations</b> : repr�sente chaque habilitation d�finissant le profil, par les colonnes suivantes :
					-# <tt>Module</tt> : Module sur lequel porte l'habilitation (Tous = tous les modules � la fois).
					-# <tt>Op�ration</tt> : Op�ration du module sur laquelle porte l'habilitation (Tous = toutes les op�rations du modules, y compris administration)
					-# <tt>P�rim�tre</tt> : Texte repr�sentant le p�rim�tre sur lequel porte l'habilitation, dont le formalisme d�pend du module et de l'op�ration. Un caract�re * signifie "peut �tre remplac� par tout texte".
					-# <tt>Droit</tt> : Droit obtenu pour le module, l'op�ration, et le p�rim�tre de l'habilitation
					-# <tt>Actions</tt> : contient un bouton supprimer pour chaque habilitation, permettant de supprimer la ligne du tableau. L'appui sur le bouton entraine une confirmation par boite de dialogue standard. La suppression est ensuite d�finitive.
				-# <b>Formulaire d'ajout d'habilitation</b> :
					-# <tt>Module</tt> : Liste de choix contenant tous les modules de SYNTHESE, plus un choix <tt>Tous</tt> plac� en t�te et propos� par d�faut. Le changement de valeur sur la liste entraine le rafraichissement de la page pour mettre � jour la liste suivante.
					-# <tt>Op�ration</tt> : Liste des op�rations disponibles pour le module s�lectionn�, plus un choix <tt>Toutes</tt> plac� en t�te et propos� par d�faut. Le changement de valeur sur la liste entra�ne le rafraichissement de la page pour mettre � jour la liste suivante.
					-# <tt>P�rim�tre</tt> : Liste des p�rim�tres possibles pour l'op�ration s�lectionn�es, plus un choix <tt>Tout</tt> plac� en t�te et propos� par d�faut.
					-# <tt>Droit</tt> : Liste des niveaux de droit possibles :
						- Utilisation
						- Lecture
						- Modification
						- Ecriture
						- Suppression
					-# <tt>Ajouter</tt> : bouton d'ajout d'habilitation. L'ajout se fait directement sans confirmation.
			
			<i>S�curit�</i>
				- L'affichage de la fen�tre en consultation n�cessite une habilitation securit� de niveau lecture
				- L'affichage de la fen�tre en mode �dition, suppression d'habilitations comprises, requiert une habilitation s�curit� de niveau �criture

			<i>Journaux</i> : Les op�rations suivantes sont consign�es dans le journal de s�curit� :
				- Modification du nom du profil
				- Ajout d'habilitation au profil
				- Suppression d'habilitation au profil
		*/
		class ProfileAdmin : public AdminInterfaceElement
		{
		};
	}
}