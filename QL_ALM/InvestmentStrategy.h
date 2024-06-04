#ifndef INVESTMENT_STRATEGY_H
#define INVESTMENT_STRATEGY_H
#include "Portfolio.h"
namespace ALM {
	class InvestmentStrategy {
	public:
		virtual void apply(const boost::shared_ptr<Portfolio>& portfolio, QuantLib::Real& amount) const = 0;
	};
};

#endif
