#ifndef PORTFOLIO_H
#define PORTFOLIO_H
#include <ql/quantlib.hpp>
//#include <complex.h>
using namespace QuantLib;
namespace ALM {

	class PortfolioInstrument {
	protected:
		Real scalar_;
	public:
		PortfolioInstrument() : scalar_(1) {};
		PortfolioInstrument(const Real& scalar = 1) : scalar_(scalar) {};
		virtual ~PortfolioInstrument() = default;
		virtual Real NPV() const = 0;
		virtual Leg cashflows() const { return Leg(); }
		void scale(const Real& scalar) { scalar_ *= scalar; };
	};

	template<typename _instrument_type>
	class CashflowInstrument : public PortfolioInstrument {
	private:
		boost::shared_ptr<_instrument_type> instrument_;

	public:
		boost::shared_ptr<_instrument_type> instrument() const { return instrument_; };

		CashflowInstrument(const boost::shared_ptr<_instrument_type>& instrument, const Real& scalar = 1)
			: instrument_(instrument), PortfolioInstrument(scalar) {};

		Real NPV() const {
			return instrument_->NPV() * scalar_;
		};

		Leg cashflows() const {
			Leg scaled_cashflows;
			for (const auto& cashflow : instrument_->cashflows()) {
				Date date = cashflow->date();
				Real amount = cashflow->amount() * scalar_;
				scaled_cashflows.push_back(boost::make_shared<SimpleCashFlow>(amount, date));
			}
			return scaled_cashflows;
		};
	};

	template<typename _instrument_type>
	class NonCashflowInstrument : public PortfolioInstrument {
	private:

		boost::shared_ptr<_instrument_type> instrument_;
	public:
		boost::shared_ptr<_instrument_type> instrument() const { return instrument_; };
		NonCashflowInstrument(const boost::shared_ptr<_instrument_type>& instrument, const Real& scalar = 1)
			: instrument_(instrument), PortfolioInstrument(scalar) {};

		Real NPV() const {
			return instrument_->NPV() * scalar_;
		};
	};

	class Portfolio {
	private:
		std::vector<boost::shared_ptr<PortfolioInstrument>> instruments_;
	public:
		Portfolio() {};
		std::vector<boost::shared_ptr<PortfolioInstrument>> instruments() const {return instruments_; };
		void addInstrument(const boost::shared_ptr<PortfolioInstrument>& instrument) {
			instruments_.push_back(instrument);
		};
		void scale(const Real& scalar) {
			for (auto& instrument : instruments_) {
				instrument->scale(scalar);
			};
		};
		Real NPV() const {
			Real total_npv = 0;
			for (auto& instrument : instruments_) {
				total_npv += instrument->NPV();
			}
			return total_npv;
		};

		Leg cashflows() const {
			Leg total_cashflows;
			for (const auto& instrument : instruments_) {
				Leg cashflows = instrument->cashflows();
				total_cashflows.insert(total_cashflows.end(), cashflows.begin(), cashflows.end());
			}
			return total_cashflows;
		};

		Real cashflowBetween(const Date& start, const Date& end) const {
			Real total_cashflow = 0;
			Leg cashflows = this->cashflows();
			for (const auto& cashflow : cashflows) {
				if (cashflow->date() >= start && cashflow->date() <= end) {
					total_cashflow += cashflow->amount();
				}
			}
			return total_cashflow;
		};

	};
};
#endif