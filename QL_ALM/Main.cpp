#include <Eigen/dense>
#include <ql/quantlib.hpp>
#include <iostream>
#include "FixedRateBondEngine.h"
#include "Portfolio.h"
#include "PortfolioProjection.h"
#include "FixedReinvestment.h"
#include "ProRataDisinvestment.h"
int main() {
    using namespace QuantLib;
    using namespace ALM;

    //Sample discount curve
    Settings::instance().evaluationDate() = Date(31, Dec, 2024);
    Handle<YieldTermStructure> discount_curve(boost::make_shared<FlatForward>(0, TARGET(), 0.03, Actual365Fixed()));

    //Sample reinvestment strategies
    auto bond_a = boost::make_shared<FixedRateBondEngine>(
        Period(3, Years),
        0.03,
        TARGET(),
        ActualActual(ActualActual::Bond),
        Unadjusted,
        discount_curve);

    auto re_strategy_a = boost::make_shared<FixedReinvestment>();
    re_strategy_a->add(std::make_pair(1, bond_a));

    auto bond_b = boost::make_shared<FixedRateBondEngine>(
        Period(5, Years),
        0.035,
        TARGET(),
        ActualActual(ActualActual::Bond),
        Unadjusted,
        discount_curve);

    auto re_strategy_b = boost::make_shared<FixedReinvestment>();
    re_strategy_b->add(std::make_pair(1, bond_b));

    auto bond_c = boost::make_shared< FixedRateBondEngine>(
        Period(10, Years),
        0.04,
        TARGET(),
        ActualActual(ActualActual::Bond),
        Unadjusted,
        discount_curve);

    auto liab = boost::make_shared<FixedRateBondEngine>(
        Period(15, Years),
        0.02,
        TARGET(),
        ActualActual(ActualActual::Bond),
        Unadjusted,
        discount_curve);

    auto re_strategy_c = boost::make_shared<FixedReinvestment>();
    re_strategy_c->add(std::make_pair(1, bond_c));

    //Sample disinvestment strategy
    auto dis_strategy = boost::make_shared<ProRataDisinvestment>();

    // Create a fixed rate bond with a target price
    auto asset_portfolio_a = boost::make_shared<Portfolio>();
    auto asset_portfolio_b = boost::make_shared<Portfolio>();
    auto asset_portfolio_c = boost::make_shared<Portfolio>();

    auto liability_portfolio = boost::make_shared<Portfolio>();
    liability_portfolio->addInstrument(boost::make_shared<ScaledInstrument>(liab->create(1000)));


    PortfolioProjection proj_a(
        asset_portfolio_a,
        liability_portfolio,
        1000,
        re_strategy_a,
        dis_strategy
    );

    PortfolioProjection proj_b(
        asset_portfolio_b,
        liability_portfolio,
        1000,
        re_strategy_b,
        dis_strategy
    );

    PortfolioProjection proj_c(
        asset_portfolio_c,
        liability_portfolio,
        1000,
        re_strategy_c,
        dis_strategy
    );


    proj_a.project(Date(31, Dec, 2024), Date(31, Dec, 2054), Period(1, Months));
    proj_b.project(Date(31, Dec, 2024), Date(31, Dec, 2054), Period(1, Months));
    proj_c.project(Date(31, Dec, 2024), Date(31, Dec, 2054), Period(1, Months));

    auto cash_a = proj_a.getData(PortfolioProjection::CASH);
    auto cash_b = proj_b.getData(PortfolioProjection::CASH);
    auto cash_c = proj_c.getData(PortfolioProjection::CASH);
    auto asset_cf_a = proj_a.getData(PortfolioProjection::ASSET_CF);
    auto asset_cf_b = proj_b.getData(PortfolioProjection::ASSET_CF);
    auto asset_cf_c = proj_c.getData(PortfolioProjection::ASSET_CF);
    auto liab_cf = proj_a.getData(PortfolioProjection::LIABILITY_CF);

    
    Eigen::Matrix<double, 360, 3> A;
    Eigen::Matrix<double, 360, 1> B;
    for (auto i = 0; i < asset_cf_a.size(); i++) {
        A(i, 0) = asset_cf_a[i].second + cash_a[i].second;
        A(i, 1) = asset_cf_b[i].second + cash_b[i].second;
        A(i, 2) = asset_cf_c[i].second + cash_c[i].second;
        B(i, 0) = liab_cf[i].second;
    }

    Eigen::Matrix<double, 3, 1> X = A.colPivHouseholderQr().solve(B);
    //std::cout << A << "\n\n";
    //std::cout << B << "\n\n";
    std::cout << X << "\n\n";
    std::cout << X / X.sum();

    return 0;


};
