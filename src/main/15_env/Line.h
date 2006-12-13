#ifndef SYNTHESE_ENV_LINE_H
#define SYNTHESE_ENV_LINE_H


#include <vector>
#include <string>

#include "01_util/RGBColor.h"
#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "Path.h"



namespace synthese
{
	namespace env
	{

		class Axis;
		class LineStop;
		class RollingStock;
		class Service;
		class TransportNetwork;


		/** Technical line.
			@ingroup m15

			Une ligne technique est un regroupement de services, constitu� dans un but d�am�lioration des performances de la recherche d'itin�raires. En effet, plusieurs services group�s dans une ligne ne comptent que pour un dans leur participation au temps de calcul, ce qui rend tout son int�r�t au regroupement des services en lignes.
			Le regroupement en lignes techniques s'effectue cependant, par convention, uniquement � l'int�rieur des \ref defLigneCommerciale "lignes commerciales". Ainsi deux services pouvant �tre group�s selon les crit�res ci-dessous, mais n'appartenant pas � la m�me ligne commerciale, ne seront pas group�s dans une ligne technique. De ce fait, les lignes techniques sont consid�r�s comme des subdivisions de lignes commerciales.

			Cependant, pour assurer la justesse des calculs, les regroupements en ligne doivent respecter des conditions strictes, constituant ainsi la th�orie des lignes&nbsp;:
				- Les services d�une m�me ligne doivent desservir exactement les m�mes points d�arr�t, dans le m�me ordre
				- Les conditions de desserte de chaque point d�arr�t (d�part/passage/arriv�e et horaires saisis/non saisis) sont les m�mes pour tous les services d�une ligne
				- Les services d�une m�me ligne sont assur�s par le m�me mat�riel roulant, observent les m�mes conditions de r�servation, appartiennent au m�me r�seau de transport, et suivent la m�me tarification
				- Un service d�une ligne ne doit pas desservir un point d�arr�t � la m�me heure qu�un autre service de la m�me ligne
				- Un service d�une ligne desservant un point d�arr�t avant un autre de la m�me ligne ne doit pas desservir un autre point d�arr�t apr�s ce dernier&nbsp;: deux services ne doivent pas se �&nbsp;doubler&nbsp;�

			NB : la correspondance entre deux services d'une m�me ligne est interdite, sauf dans les axes libres.

		*/
		class Line : 
			public synthese::util::Registrable<uid,Line>,
			public Path
		{
		private:
			TransportNetwork* _network; 
			const Axis* _axis; 
		    
			uid _rollingStockId;
		    
			std::string _name;  //!< Name (id)
			std::string _shortName;  //!< Name (cartouche)
			std::string _longName; //!< Name for schedule card

			synthese::util::RGBColor _color;  //!< Line color
			std::string _style;    //!< CSS style (cartouche)
			std::string _image;    //!< Display image (cartouche)

			std::string _timetableName; //!< Name for timetable
			std::string _direction;   //!< Direction (shown on vehicles)

			bool _isWalkingLine;
		    
			bool _useInDepartureBoards; 
			bool _useInTimetables; 
			bool _useInRoutePlanning; 


		public:

			Line (const uid& id,
			const std::string& name, 
			const Axis* axis);

			virtual ~Line();



			//! @name Getters/Setters
			//@{
			const uid& getId () const;

			const std::string& getName () const;
			void setName (const std::string& name);

			bool getUseInDepartureBoards () const;
			void setUseInDepartureBoards (bool useInDepartureBoards);

			bool getUseInTimetables () const;
			void setUseInTimetables (bool useInTimetables);

			bool getUseInRoutePlanning () const;
			void setUseInRoutePlanning (bool useInRoutePlanning);

			const Axis* getAxis () const;

			const synthese::util::RGBColor& getColor () const;
			void setColor (const synthese::util::RGBColor& color);

			const std::string& getDirection () const;
			void setDirection (const std::string& direction);

			const std::string& getImage () const;
			void setImage (const std::string& image);

			const std::string& getShortName () const;
			void setShortName (const std::string& shortName);

			const std::string& getLongName () const;
			void setLongName (const std::string& longName);

			const std::string& getTimetableName () const;
			void setTimetableName (const std::string& timetableName);

			const TransportNetwork* getNetwork () const;
			void setNetwork (TransportNetwork* network);
		    
			const std::string& getStyle () const;
			void setStyle (const std::string& style);
		    
			const uid& getRollingStockId () const;
			void setRollingStockId (const uid& id);

			bool getWalkingLine () const;
			void setWalkingLine (bool isWalkingLine);
		    
			//@}




			//! @name Update methods
			//@{
		    
			//! @name Query methods
			//@{

			bool isRoad () const;
			bool isLine () const;

			bool isReservable () const;

			//@}
		    
		    
		};


	}
}


#endif

