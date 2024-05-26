
#include <ql/quantlib.hpp>
#include <Eigen/dense>
#include "ALMIncludes.h"

using namespace QuantLib;
int main() {

    //Setting the valuation date to Jan 31, 2024.
    Settings::instance().evaluationDate() = Date(31, Jan, 2024);

    //Initializing a template to create yield curves at the valuation date.
    ALM::DynamicFlatForwardTemplate flat_forward_template;
    flat_forward_template.day_counter_ = ActualActual(ActualActual::Actual365);
    flat_forward_template.compounding_ = Compounded;
    flat_forward_template.frequency_ = Semiannual;
    flat_forward_template.forward_ = Rate(0.04);

    //Initializing a yield curve using the template. Time zero will be the valuation date.
    RelinkableHandle<YieldTermStructure> yield_curve(ALM::createFrom(flat_forward_template));

    //Initializing a template to create 3-year fixed bond coupon schedules at the valuation date.
    ALM::DynamicScheduleTemplate schedule_3Y_template;
    schedule_3Y_template.duration_ = Period(3, Years);
    schedule_3Y_template.tenor_ = Period(6, Months);
    schedule_3Y_template.calendar_ = TARGET();
    schedule_3Y_template.termination_date_convention_ = Unadjusted;
    schedule_3Y_template.convention_ = Unadjusted;
    schedule_3Y_template.rule_ = DateGeneration::Backward;
    schedule_3Y_template.end_of_month_ = false;

    //Initializing a template to create 10-year fixed bond coupon schedules at the valuation date.
    ALM::DynamicScheduleTemplate schedule_10Y_template;
    schedule_10Y_template.duration_ = Period(10, Years);
    schedule_10Y_template.tenor_ = Period(6, Months);
    schedule_10Y_template.calendar_ = TARGET();
    schedule_10Y_template.termination_date_convention_ = Unadjusted;
    schedule_10Y_template.convention_ = Unadjusted;
    schedule_10Y_template.rule_ = DateGeneration::Backward;
    schedule_10Y_template.end_of_month_ = false;

    //Initializing a template to create fixed coupon bonds at the valuation date.
    ALM::DynamicFixedRateBondTemplate fixed_bond_3Y_template;
    fixed_bond_3Y_template.face_amount_ = 100.0;
    fixed_bond_3Y_template.coupons_ = { Rate(0.05) };
    fixed_bond_3Y_template.schedule_ = schedule_3Y_template;
    fixed_bond_3Y_template.accrual_day_counter_ = Thirty360(Thirty360::BondBasis);
    fixed_bond_3Y_template.settlement_days_ = 0;

    //Initializing a template to create fixed coupon bonds at the valuation date.
    ALM::DynamicFixedRateBondTemplate fixed_bond_10Y_template;
    fixed_bond_10Y_template.face_amount_ = 100.0;
    fixed_bond_10Y_template.coupons_ = { Rate(0.06) };
    fixed_bond_10Y_template.schedule_ = schedule_10Y_template;
    fixed_bond_10Y_template.accrual_day_counter_ = Thirty360(Thirty360::BondBasis);
    fixed_bond_10Y_template.settlement_days_ = 0;
    
    //Initializing a portfolio of fixed coupon bonds.
    //This is simply a vector of bonds and scalar adjustments (that reduce or increase cashflows, market values).
    ALM::Portfolio<FixedRateBond> fixed_bond_portfolio;

    //Initializing a reinvestment strategy.
    //The strategy will invest 50% of some cashflow into a 3-year bond and the valuation date, and 50% into a 10-year bond at the valuation date.
    //The yield curve will be used to price the bonds when determining the amount to invest
    //NOTE: risk free is assumed; use RiskyFixedRateBondPurchase for risky bonds.
    ALM::FixedRateBondPurchase reinvestment_strategy(
        { std::make_pair(fixed_bond_3Y_template, Real(0.5)), std::make_pair(fixed_bond_10Y_template, Real(0.5))},
        yield_curve
    );

    //Initializing a disinvestment strategy.
    //The strategy will divest from every asset in the portfolio equally.
    //NOTE: negative assets will not occur.
    ALM::ProRataDisinvestment<FixedRateBond> disinvestment_strategy;


    //Sample portfolio projection process...
    
    //Start with 100,000 in cash.
    Real cash_holdings = 100000;
    std::cout << "Cash held (before reinvestment): " << cash_holdings << "\n\n";

    //Invest cash into the portfolio.
    std::cout << "Reinvesting " << cash_holdings / 2.0 << " into bonds...\n";
    cash_holdings -= reinvestment_strategy.applyTo(fixed_bond_portfolio, cash_holdings / 2.0);
    std::cout << "Cash held (after reinvestment): " << cash_holdings << "\n";
    
    //Show that the portfolio value equals the amount invested.
    std::cout << "Portfolio value (excluding cash): " << ALM::NPV(fixed_bond_portfolio) << "\n\n";
    
    //Disinvest from the portfolio.
    Real disinvestment_volume = 10000;
    std::cout << "Disinvesting " << disinvestment_volume << " on a pro-rata basis...\n";
    cash_holdings += disinvestment_strategy.applyTo(fixed_bond_portfolio, disinvestment_volume);

    //Show that the portfolio value decreased by the disinvestment volume and cash holdings increased.
    std::cout << "Cash held (after disinvestment): " << cash_holdings << "\n";
    std::cout << "Portfolio value (excluding cash): " << ALM::NPV(fixed_bond_portfolio) << "\n\n";

    //Try incrementing the valuation date by one year.
    std::cout << "Incrementing the valuation date by one year...\n";
    Settings::instance().evaluationDate() = Settings::instance().evaluationDate().value() + Period(1, Years);

    //Need to rebuild the yield curve for pricing to work properly.
    yield_curve.linkTo(ALM::createFrom(flat_forward_template));

    //Show that portfolio value changed and coupon income occured.
    cash_holdings += ALM::cashflowBetween(fixed_bond_portfolio, Settings::instance().evaluationDate().value() - Period(1, Years), Settings::instance().evaluationDate().value());
    std::cout << "Cash held (one year later): " << cash_holdings << "\n";
    std::cout << "Portfolio value (one year later): " << ALM::NPV(fixed_bond_portfolio) << "\n\n";
  
    return 0;
};