#ifndef FIXED_RATE_BOND_ENGINE_H
#define FIXED_RATE_BOND_ENGINE_H
#include "InstrumentEngine.h"
namespace ALM {

    class FixedRateBondEngine : public InstrumentEngine {
    private:
        //Parameters to construct the bond
        QuantLib::Period tenor_;
        QuantLib::Real coupon_rate_;
        QuantLib::Calendar calendar_;
        QuantLib::DayCounter day_counter_;
        QuantLib::BusinessDayConvention convention_;
        QuantLib::Handle<QuantLib::YieldTermStructure> discount_curve_;

        QuantLib::Real target_price_ = 0;

    public:
        QuantLib::Real operator()(const QuantLib::Real face_amount) const {

            QuantLib::Schedule schedule(
                QuantLib::Settings::instance().evaluationDate().value(),
                QuantLib::Settings::instance().evaluationDate().value() + tenor_,
                QuantLib::Period(QuantLib::Semiannual),
                calendar_,
                convention_,
                convention_,
                QuantLib::DateGeneration::Backward,
                false);

            auto bond = boost::make_shared<QuantLib::FixedRateBond>(
                0,
                face_amount,
                schedule,
                std::vector<QuantLib::Rate>(1, coupon_rate_),
                day_counter_);

            bond->setPricingEngine(boost::make_shared<QuantLib::DiscountingBondEngine>(discount_curve_));

            return bond->NPV() - target_price_;
        };

        FixedRateBondEngine(
            const QuantLib::Period& tenor,
            const QuantLib::Real& coupon_rate,
            const QuantLib::Calendar& calendar,
            const QuantLib::DayCounter& day_counter,
            const QuantLib::BusinessDayConvention& convention,
            const QuantLib::Handle<QuantLib::YieldTermStructure>& discount_curve) :
            tenor_(tenor), coupon_rate_(coupon_rate), calendar_(calendar),
            day_counter_(day_counter), convention_(convention), discount_curve_(discount_curve) {};


        //Create a fixed coupon bond at the evaluation date according to saved parameters
        //with a present value equal to some target price
        boost::shared_ptr<QuantLib::Instrument> create(
            const QuantLib::Real& target_price,
            const QuantLib::Real& initial_guess = 1000.0,
            const QuantLib::Real& accuracy = 1e-8) {

            target_price_ = target_price;

            QuantLib::Brent solver;
            QuantLib::Real face_amount = solver.solve(*this, accuracy, initial_guess, accuracy);

            QuantLib::Schedule schedule(
                QuantLib::Settings::instance().evaluationDate().value(),
                QuantLib::Settings::instance().evaluationDate().value() + tenor_,
                QuantLib::Period(QuantLib::Semiannual),
                calendar_,
                convention_,
                convention_,
                QuantLib::DateGeneration::Backward,
                false);

            auto bond = boost::make_shared<QuantLib::FixedRateBond>(
                0,
                face_amount,
                schedule,
                std::vector<QuantLib::Rate>(1, coupon_rate_),
                day_counter_);

            bond->setPricingEngine(boost::make_shared<QuantLib::DiscountingBondEngine>(discount_curve_));

            return boost::dynamic_pointer_cast<QuantLib::Instrument>(bond);
        };

    };
};
#endif