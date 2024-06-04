#ifndef INSTRUMENT_ENGINE_H
#define INSTRUMENT_ENGINE_H
#include <ql/quantlib.hpp>

namespace ALM {
	
    class InstrumentEngine {
    public:
        virtual boost::shared_ptr<QuantLib::Instrument> create(
            const QuantLib::Real& target_price,
            const QuantLib::Real& initial_guess = 1000.0,
            const QuantLib::Real& accuracy = 1e-8) = 0;
    };

};
#endif