#ifndef PORTFOLIO_PROJECTION_H
#define PORTFOLIO_PROJECTION_H
#include "Portfolio.h"
#include "InvestmentStrategy.h"
#include "HelperFunctions.h"
namespace ALM {
	class PortfolioProjection {
	protected:
		boost::shared_ptr<Portfolio> assets_;
		boost::shared_ptr<Portfolio> liabilities_;
		QuantLib::Real cash_;
		boost::shared_ptr<InvestmentStrategy> reinvestment_strategy_;
		boost::shared_ptr<InvestmentStrategy> disinvestment_strategy_;

		std::vector<std::pair<QuantLib::Date, QuantLib::Real>> asset_cashflow_data_;
		std::vector<std::pair<QuantLib::Date, QuantLib::Real>> liability_cashflow_data_;
		std::vector<std::pair<QuantLib::Date, QuantLib::Real>> asset_market_value_data_;
		std::vector<std::pair<QuantLib::Date, QuantLib::Real>> liability_market_value_data_;
		std::vector<std::pair<QuantLib::Date, QuantLib::Real>> cash_position_data_;

		void clearData() {
			asset_cashflow_data_.clear();
			liability_cashflow_data_.clear();
			asset_market_value_data_.clear();
			liability_market_value_data_.clear();
			cash_position_data_.clear();
		};

		void storeData(
			const QuantLib::Date& date,
			const QuantLib::Real& asset_cashflow,
			const QuantLib::Real& liability_cashflow,
			const QuantLib::Real& asset_market_value,
			const QuantLib::Real& liability_market_value,
			const QuantLib::Real& cash_position) {

			asset_cashflow_data_.push_back(std::make_pair(date, asset_cashflow));
			liability_cashflow_data_.push_back(std::make_pair(date, liability_cashflow));
			asset_market_value_data_.push_back(std::make_pair(date, asset_market_value));
			liability_market_value_data_.push_back(std::make_pair(date, liability_market_value));
			cash_position_data_.push_back(std::make_pair(date, cash_position));
		};

	public:
		enum dataType {
			CASH,
			ASSET_CF,
			LIABILITY_CF,
			ASSET_MV,
			LIABILITY_MV
		};

		std::vector<std::pair<QuantLib::Date, QuantLib::Real>> getData(const dataType& type) {
			switch (type) {
			case CASH:
				return cash_position_data_;
			case ASSET_CF:
				return asset_cashflow_data_;
			case LIABILITY_CF:
				return liability_cashflow_data_;
			case ASSET_MV:
				return asset_market_value_data_;
			case LIABILITY_MV:
				return liability_market_value_data_;
			default:
				return std::vector<std::pair<QuantLib::Date, QuantLib::Real>>();
			}
		};

		PortfolioProjection(
			const boost::shared_ptr<Portfolio>& assets,
			const boost::shared_ptr<Portfolio>& liabilities,
			const QuantLib::Real cash,
			const boost::shared_ptr<InvestmentStrategy>& reinvestment_strategy,
			const boost::shared_ptr<InvestmentStrategy>& disinvestment_strategy) :
			assets_(assets), liabilities_(liabilities), cash_(cash),
			reinvestment_strategy_(reinvestment_strategy), 
			disinvestment_strategy_(disinvestment_strategy) {};
	
		

		void project(const QuantLib::Date& start, const QuantLib::Date& end, const QuantLib::Period& timestep) {

			clearData();

			QuantLib::Date date = start;
			QuantLib::Date prior_date = date;

			QuantLib::Real cash_position = cash_;
			while (date < end) {
				QuantLib::Settings::instance().evaluationDate() = date;

				QuantLib::Real asset_cf = totalCashflowBetween(assets_->cashflows(), prior_date, date);
				QuantLib::Real liability_cf = totalCashflowBetween(liabilities_->cashflows(), prior_date, date);

				cash_position += (asset_cf - liability_cf);

				QuantLib::Real asset_mv = assets_->NPV();
				QuantLib::Real liability_mv = liabilities_->NPV();

				storeData(date, asset_cf, liability_cf, asset_mv, liability_mv, cash_position);

				if (cash_position > 0) {
					reinvestment_strategy_->apply(assets_, cash_position);
				}
				else if (cash_position < 0) {
					disinvestment_strategy_->apply(assets_, cash_position);
				};

				//std::cout << assets_->NPV() << "\n";

				prior_date = date + QuantLib::Period(1, QuantLib::Days);
				date += timestep;
			}
		};
	};
};

#endif
