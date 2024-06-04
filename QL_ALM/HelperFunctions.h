#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include <ql/quantlib.hpp>
namespace ALM {
	inline QuantLib::Real totalCashflowBetween(const QuantLib::Leg& leg, const QuantLib::Date& start, const QuantLib::Date& end) {
		QuantLib::Real total = 0;
		for (const auto& cf : leg) {
			if (cf->date() <= end && cf->date() >= start) {
				total += cf->amount();
			}
		}
		return total;
	};

};

#endif
