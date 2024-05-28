
#include <ql/quantlib.hpp>
#include <Eigen/dense>
#include "ALMIncludes.h"

using namespace QuantLib;
int main() {

    //Setting the valuation date to Jan 31, 2024.
    Settings::instance().evaluationDate() = Date(31, Jan, 2024);

    //Initializing a template to create yield curves at the valuation date.
    ALM::DynamicFlatForwardTemplate flat_forward_template(
        Rate(0.04),
        ActualActual(ActualActual::Actual365),
        Compounded,
        Semiannual);

    //Initializing a yield curve using the template. Time zero will be the valuation date.
    RelinkableHandle<YieldTermStructure> yield_curve(flat_forward_template.create());

    //Initializing a template to create 10-year fixed bond coupon schedules at the valuation date.
    ALM::DynamicScheduleTemplate schedule_10Y_template(
        Period(10, Years),
        Period(6, Months),
        TARGET(),
        Unadjusted,
        Unadjusted,
        DateGeneration::Backward,
        false);

    ALM::DynamicFixedRateBondTemplate fixed_bond_template(
        Natural(0),
        Real(100.0),
        schedule_10Y_template,
        { Rate(0.05) },
        ActualActual(ActualActual::Actual365)
    );

    ALM::Portfolio portfolio;
    
    auto sample_bond = boost::make_shared<ALM::CashflowInstrument<FixedRateBond>>(boost::dynamic_pointer_cast<FixedRateBond>(fixed_bond_template.create()), Real(1));
    sample_bond->instrument()->setPricingEngine(boost::make_shared<DiscountingBondEngine>(yield_curve));
    //portfolio.addInstrument(sample_bond);

    auto ptr_test = boost::make_shared<ALM::DynamicFixedRateBondTemplate>(fixed_bond_template);
    std::vector<std::pair<boost::shared_ptr<ALM::InstrumentTemplate>, Real>> instrument_templates;
    instrument_templates.emplace_back(std::make_pair(ptr_test, Real(1)));

    ALM::FixedRateBondReinvestment strategy(instrument_templates, yield_curve);
    
    strategy.applyTo(portfolio, 1000);
    

    Settings::instance().evaluationDate() = Settings::instance().evaluationDate().value() + Period(1, Years);
    yield_curve.linkTo(flat_forward_template.create());

    Settings::instance().evaluationDate() = Settings::instance().evaluationDate().value() + Period(1, Years);
    yield_curve.linkTo(flat_forward_template.create());

    std::cout << portfolio.NPV();
    return 0;
};