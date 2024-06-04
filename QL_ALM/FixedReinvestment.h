#ifndef FIXED_REINVESTMENT_H
#define FIXED_REINVESTMENT_H
#include "InvestmentStrategy.h"
#include "InstrumentEngine.h"
namespace ALM {
	class FixedReinvestment : public InvestmentStrategy {
	private:
		std::vector<std::pair<QuantLib::Real, boost::shared_ptr<InstrumentEngine>>> strategy_;
	public:
		//FixedReinvestment(const std::vector<std::pair<QuantLib::Real, boost::shared_ptr<InstrumentEngine>>>& strategy) :
		//	strategy_(strategy) {};

		void add(const std::pair<QuantLib::Real, boost::shared_ptr<InstrumentEngine>> strategy) {
			strategy_.push_back(strategy);
		}

		void apply(const boost::shared_ptr<Portfolio>& portfolio, QuantLib::Real& amount) const {
			
			for (const auto& item : strategy_) {
				QuantLib::Real amount_to_invest = item.first * amount;
				auto instrument = boost::make_shared<ScaledInstrument>(item.second->create(amount_to_invest));
				portfolio->addInstrument(instrument);
			}

			amount = 0;
		
		};
	};
};

#endif
