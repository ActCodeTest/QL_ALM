#ifndef PRO_RATA_DISINVESTMENT_H
#define PRO_RATA_DISINVESTMENT_H
#include "InvestmentStrategy.h"
namespace ALM {
	class ProRataDisinvestment : public InvestmentStrategy {
	public: 
		void apply(const boost::shared_ptr<Portfolio>& portfolio, QuantLib::Real& amount) const {
			QuantLib::Real npv = portfolio->NPV();
			if (-amount > npv) {
				portfolio->scale(0);
				amount += npv;
			}
			else {
				QuantLib::Real scalar = 1.0 + amount / npv;
				portfolio->scale(scalar);
				amount = 0;
			}
		};
	};
};
#endif