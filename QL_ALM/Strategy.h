#ifndef STRATEGY_H
#define STRATEGY_H
#include "Portfolio.h"
#include "Templates.h"
namespace ALM {
	template<typename _instrument>
	class Strategy {
	public:
		virtual Real applyTo(Portfolio<_instrument>& portfolio, const Real& amount) { return Real(0); };
	};

	template<typename _instrument>
	class ProRataDisinvestment : public Strategy<_instrument> {
	public:
		Real applyTo(Portfolio<_instrument>& portfolio, const Real& amount) {
			return sellProRata(portfolio, amount);
		};
	};

	class FixedRateBondPurchase : public Strategy<FixedRateBond> {
	private:
		std::vector<std::pair<DynamicTemplate<FixedRateBond>, Real>> instrument_templates_;
		Handle<YieldTermStructure> discount_curve_;
	public:
		FixedRateBondPurchase(
			const std::vector<std::pair<DynamicTemplate<FixedRateBond>, Real>>& instrument_templates,
			const Handle<YieldTermStructure>& discount_curve
		) : instrument_templates_(instrument_templates), discount_curve_(discount_curve) {};

		Real applyTo(Portfolio<FixedRateBond>& portfolio, const Real& amount) {
			for (const auto& instrument_template : instrument_templates_) {
				Real amount_to_invest = instrument_template.second * amount;

				boost::shared_ptr<FixedRateBond> instrument = createFrom(instrument_template.first);
				instrument->setPricingEngine(boost::make_shared<DiscountingBondEngine>(discount_curve_));
				Real npv = instrument->NPV();

				Real scalar = amount_to_invest / npv;

				portfolio.instruments_.push_back(std::make_pair(instrument, scalar));

			};

			return amount;
		};
	};

	class RiskyFixedRateBondPurchase : public Strategy<FixedRateBond> {
	private:
		std::vector<std::pair<DynamicTemplate<FixedRateBond>, Real>> instrument_templates_;
		Handle<DefaultProbabilityTermStructure> default_curve_;
		Real recovery_rate_;
		Handle<YieldTermStructure> discount_curve_;
	public:
		RiskyFixedRateBondPurchase(
			const std::vector<std::pair<DynamicTemplate<FixedRateBond>, Real>>& instrument_templates,
			const Handle<DefaultProbabilityTermStructure>& default_curve,
			const Real& recovery_rate,
			const Handle<YieldTermStructure>& discount_curve
		) : instrument_templates_(instrument_templates), default_curve_(default_curve), recovery_rate_(recovery_rate), discount_curve_(discount_curve) {};

		Real applyTo(Portfolio<FixedRateBond>& portfolio, const Real& amount) {
			for (const auto& instrument_template : instrument_templates_) {
				Real amount_to_invest = instrument_template.second * amount;

				boost::shared_ptr<FixedRateBond> instrument = createFrom(instrument_template.first);
				instrument->setPricingEngine(boost::make_shared<RiskyBondEngine>(default_curve_, recovery_rate_, discount_curve_));
				Real npv = instrument->NPV();

				Real scalar = amount_to_invest / npv;

				portfolio.instruments_.push_back(std::make_pair(instrument, scalar));

			};

			return amount;
		};
	};

};
#endif
