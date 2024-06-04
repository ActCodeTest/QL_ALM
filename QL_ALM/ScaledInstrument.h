#ifndef SCALED_INSTRUMENT_H
#define SCALED_INSTRUMENT_H
#include <ql/quantlib.hpp>

namespace ALM {
	class ScaledInstrument {
	private:
		boost::shared_ptr<QuantLib::Instrument> instrument_;
		QuantLib::Real scalar_;
	public:
		//Constructor
		ScaledInstrument(const boost::shared_ptr<QuantLib::Instrument>& instrument, const QuantLib::Real& scalar = 1) :
			instrument_(instrument), scalar_(scalar) {};

		//Access to scalar multiple
		QuantLib::Real getScalar() const {
			return scalar_;
		};

		void setScalar(const QuantLib::Real& scalar) {
			scalar_ = scalar;
		};

		//Access to underlying instrument
		boost::shared_ptr<QuantLib::Instrument> get() const {
			return instrument_;
		};

		//Scaled NPV calculation
		QuantLib::Real NPV() const {
			return scalar_ * instrument_->NPV();
		};

		//Scaled CF calculation
		QuantLib::Leg cashflows() const {
			QuantLib::Leg nominal_cashflows;
			if (boost::shared_ptr<QuantLib::Bond> bond = boost::dynamic_pointer_cast<QuantLib::Bond>(instrument_)) {
				nominal_cashflows = bond->cashflows();
			}
			/*
				Handle other classes here . . .
			*/

			QuantLib::Leg scaled_cashflows;
			for (const auto& nominal_cashflow : nominal_cashflows) {
				QuantLib::Real amount = scalar_ * nominal_cashflow->amount();
				QuantLib::Date date = nominal_cashflow->date();
				scaled_cashflows.push_back(boost::make_shared<QuantLib::SimpleCashFlow>(amount, date));
			};

			return scaled_cashflows;

		};
	};
};

#endif