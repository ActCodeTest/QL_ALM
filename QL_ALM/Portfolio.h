#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "ScaledInstrument.h"

namespace ALM {
	class Portfolio {
	private:

		std::vector<boost::shared_ptr<ScaledInstrument>> instruments_;

	public:
		//Add an instrument to the portfolio
		void addInstrument(const boost::shared_ptr<ScaledInstrument>& instrument) {
			instruments_.push_back(instrument);
		};

		//Scale all instrument
		void scale(const QuantLib::Real& scalar) const {
			for (const auto& instrument : instruments_) {
				instrument->setScalar(scalar * instrument->getScalar());
			}
		};

		//Calculate the NPV
		QuantLib::Real NPV() const {
			QuantLib::Real total_npv = 0;
			for (const auto& instrument : instruments_) {
				total_npv += instrument->NPV();
			}
			return total_npv;
		};
		
		//Access the cashflows
		QuantLib::Leg cashflows() const {
			QuantLib::Leg total_cashflows;
			for (const auto& instrument : instruments_) {
				QuantLib::Leg instrument_cashflows = instrument->cashflows();
				total_cashflows.insert(total_cashflows.end(), instrument_cashflows.begin(), instrument_cashflows.end());
			}
			return total_cashflows;
		};

	};
};

#endif