
#ifndef SYNTHESE_AddGuaranteeAction_H__
#define SYNTHESE_AddGuaranteeAction_H__

#include "04_time/DateTime.h"

#include "30_server/Action.h"

namespace synthese
{
	namespace accounts
	{
		class Account;
	}

	namespace vinci
	{
		class VinciContract;

		class VinciAddGuaranteeAction : public server::Action
		{
		public:
			static const std::string PARAMETER_AMOUNT;
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_CONTRACT_ID;
			static const std::string PARAMETER_ACCOUNT_ID;

		private:
			double				_amount;
			VinciContract*		_contract;
			accounts::Account*	_account;
			time::DateTime		_date;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:

			/** Action to run, defined by each subclass.
			*/
			void run();

			VinciAddGuaranteeAction();
			~VinciAddGuaranteeAction();
		};
	}
}

#endif // SYNTHESE_AddGuaranteeAction_H__

