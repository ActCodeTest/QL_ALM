#ifndef PRICE_SOLVER_H
#define PRICE_SOLVER_H
#include <ql/quantlib.hpp>
using namespace QuantLib;
namespace ALM {

	template<typename _instrument>
	struct Portfolio {
		std::vector<std::pair<boost::shared_ptr<_instrument>, Real>> instruments_;
	};

	template<>
	struct Portfolio<FixedRateBond> {
		std::vector<std::pair<boost::shared_ptr<FixedRateBond>, Real>> instruments_;
	};

	typedef Portfolio<FixedRateBond> FixedRateBondPortfolio;

	/* Any instrument calculations */

	//Scale all assets by a factor
	template<typename _instrument>
	inline void scale(Portfolio<_instrument>& portfolio, Real scalar) {
		for (auto& instrument : portfolio.instruments_) {
			instrument.second *= scalar;
		}
	};

	//Calculate the NPV
	template<typename _instrument>
	inline Real NPV(const Portfolio<_instrument>& portfolio) {
		Real total_npv = 0;
		for (const auto& instrument  : portfolio.instruments_) {
			Real npv = instrument.first->NPV();
			Real scalar = instrument.second;
			total_npv += npv * scalar;
		}
		return total_npv;
	};

	//Sell some amount, returning the amount sold
	template<typename _instrument>
	inline Real sellProRata(Portfolio<_instrument>& portfolio, Real amount) {
		Real npv = NPV(portfolio);
		Real scalar = 0;
		if (amount > npv) { //Sell the entire portfolio
			scale(portfolio, scalar);
			return npv;
		}
		else { //Sell a portion of the portfolio
			scalar = Real(1) - amount / npv;
			scale(portfolio, scalar);
			return amount;
		}
	};

	//Buy some amount, returning the amount bought
	template<typename _instrument>
	inline Real buyProRata(Portfolio<_instrument>& portfolio, Real amount) {
		Real npv = NPV(portfolio);
		Real scalar = Real(1) + amount / npv;
		scale(portfolio, scalar);
		return amount;
	};

	/* Bond calculations */

	//Return cashflows
	template<typename _instrument>
	inline Leg cashflows(const Portfolio<_instrument>& bond_portfolio) {
		Leg total_cashflows;
		for (const auto& bond : bond_portfolio.instruments_) {
			Leg cashflows = bond.first->cashflows();
			Real scalar = bond.second;
			for (const auto& cashflow : cashflows) {
				Date date = cashflow->date();
				Real amount = cashflow->amount() * scalar;
				total_cashflows.push_back(boost::make_shared<SimpleCashFlow>(amount, date));
			}
		}
		return total_cashflows;
	};

	//Return the dirty price
	template<typename _instrument>
	inline Real dirtyPrice(const Portfolio<_instrument>& bond_portfolio) {
		Real total_dirty_price = 0;
		for (const auto& bond : bond_portfolio.instruments_) {
			Real dirty_price = bond.first->dirtyPrice();
			Real scalar = bond.second;
			total_dirty_price += dirty_price * scalar;
		}
		return total_dirty_price;
	};

	//Return the clean price
	template<typename _instrument>
	inline Real cleanPrice(const Portfolio<_instrument>& bond_portfolio) {
		Real total_clean_price = 0;
		for (const auto& bond : bond_portfolio.instruments_) {
			Real clean_price = bond.first->cleanPrice();
			Real scalar = bond.second;
			total_clean_price += clean_price * scalar;
		}
		return total_clean_price;
	};

	//Return cashflows between two dates
	template<typename _instrument>
	inline Real cashflowBetween(const Portfolio<_instrument>& portfolio, const Date& start_date, const Date& end_date) {
		Real total_cashflow = 0;
		for (const auto& bond : portfolio.instruments_) {
			Leg cashflows = bond.first->cashflows();
			Real scalar = bond.second;
			for (const auto& cashflow : cashflows) {
				Date date = cashflow->date();
				if (date >= start_date && date <= end_date) {
					Real amount = cashflow->amount() * scalar;
					total_cashflow += amount;
				}
			}
		}
		return total_cashflow;
	};

};
#endif