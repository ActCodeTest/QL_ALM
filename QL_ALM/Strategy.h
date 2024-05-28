#ifndef STRATEGY_H
#define STRATEGY_H
#include "Portfolio.h"
#include "Templates.h"
namespace ALM {
	
	class Strategy {
	public:
		virtual Real applyTo(Portfolio& portfolio, const Real& amount) { return Real(0); };
	};

	class ProRataDisinvestment : public Strategy {
	public:
		Real applyTo(Portfolio& portfolio, const Real& amount) {
			Real total_npv = portfolio.NPV();
			if (amount < total_npv) {
				Real scalar = Real(1) - amount / total_npv;
				for (auto& instrument : portfolio.instruments()) {
					instrument->scale(scalar);
				}
				return amount;
			}
			for (auto& instrument : portfolio.instruments()) {
				instrument->scale(0); //Sell everything
			}
			return total_npv;
		};
	};

	class FixedRateBondReinvestment : public Strategy {
	private:
		std::vector<std::pair<boost::shared_ptr<InstrumentTemplate>, Real>> instrument_templates_;
		RelinkableHandle<YieldTermStructure> discount_curve_;
	public:
		FixedRateBondReinvestment(
			const std::vector<std::pair<boost::shared_ptr<InstrumentTemplate>, Real>>& instrument_templates,
			const RelinkableHandle<YieldTermStructure>& discount_curve
		) : instrument_templates_(instrument_templates), discount_curve_(discount_curve) {};

		Real applyTo(Portfolio& portfolio, const Real& amount) {
			for (const auto& instrument_template : instrument_templates_) {
				Real amount_to_invest = instrument_template.second * amount;

				boost::shared_ptr<FixedRateBond> instrument = boost::dynamic_pointer_cast<FixedRateBond>(instrument_template.first->create());
				instrument->setPricingEngine(boost::make_shared<DiscountingBondEngine>(discount_curve_));
				Real npv = instrument->NPV();

				Real scalar = amount_to_invest / npv;

				auto portfolio_instrument = boost::make_shared<CashflowInstrument<FixedRateBond>>(instrument, scalar);
				portfolio.addInstrument(portfolio_instrument);

			};

			return amount;
		};
	};



};
#endif
