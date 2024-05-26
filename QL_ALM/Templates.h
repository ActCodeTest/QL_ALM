#ifndef TEMPLATES_H
#define TEMPLATES_H
#include <ql/quantlib.hpp>
using namespace QuantLib;
namespace ALM {

	template<typename _type> 
	struct BaseTemplate {};

	//Base template to construct an instrument
	template<typename _type>
	struct StaticTemplate : public BaseTemplate<_type> {};

	//Dynamic templates use valuation date to construct an instrument
	template<typename _type>
	struct DynamicTemplate : public BaseTemplate<_type> {};

	//Base method to convert template into an object
	template<typename _type>
	inline boost::shared_ptr<_type> createFrom(const BaseTemplate<_type>& base_template) {
		return boost::make_shared<_type>();
	};

	/* Schedules */

	//Static Schedules
	template<>
	struct StaticTemplate<Schedule> {
		Date effective_date_;
		Date termination_date_;
		Period duration_;
		Period tenor_;
		Calendar calendar_;
		BusinessDayConvention convention_;
		BusinessDayConvention termination_date_convention_;
		DateGeneration::Rule rule_;
		bool end_of_month_;
	};

	typedef StaticTemplate<Schedule> StaticScheduleTemplate;

	inline boost::shared_ptr<Schedule> createFrom(const StaticScheduleTemplate& schedule_template) {
		return boost::make_shared<Schedule>(
			schedule_template.effective_date_,
			schedule_template.termination_date_,
			schedule_template.tenor_,
			schedule_template.calendar_,
			schedule_template.convention_,
			schedule_template.termination_date_convention_,
			schedule_template.rule_,
			schedule_template.end_of_month_);
	};

	//Dynamic Schedules
	template<>
	struct DynamicTemplate<Schedule> {
		Period duration_;
		Period tenor_;
		Calendar calendar_;
		BusinessDayConvention convention_;
		BusinessDayConvention termination_date_convention_;
		DateGeneration::Rule rule_;
		bool end_of_month_;
	};

	typedef DynamicTemplate<Schedule> DynamicScheduleTemplate;

	inline boost::shared_ptr<Schedule> createFrom(const DynamicScheduleTemplate& schedule_template) {
		return boost::make_shared<Schedule>(
			Settings::instance().evaluationDate().value(),
			Settings::instance().evaluationDate().value() + schedule_template.duration_,
			schedule_template.tenor_,
			schedule_template.calendar_,
			schedule_template.convention_,
			schedule_template.termination_date_convention_,
			schedule_template.rule_,
			schedule_template.end_of_month_);
	};

	/* Flat Forward */
	template<>
	struct DynamicTemplate<FlatForward> {
		Rate forward_;
		DayCounter day_counter_;
		Compounding compounding_;
		Frequency frequency_;
	};

	typedef DynamicTemplate<FlatForward> DynamicFlatForwardTemplate;

	inline boost::shared_ptr<FlatForward> createFrom(const DynamicFlatForwardTemplate& flat_forward_template) {
		return boost::make_shared<FlatForward>(
			Settings::instance().evaluationDate().value(),
			flat_forward_template.forward_,
			flat_forward_template.day_counter_,
			flat_forward_template.compounding_,
			flat_forward_template.frequency_
		);
	};


	/* Fixed Rate Bonds */

	//Static Fixed Rate Bonds
	
	template<>
	struct StaticTemplate<FixedRateBond> {
		Natural settlement_days_;
		Real face_amount_;
		Schedule schedule_;
		std::vector<Rate> coupons_;
		DayCounter accrual_day_counter_;
	};

	typedef StaticTemplate<FixedRateBond> StaticFixedRateBondTemplate;

	inline boost::shared_ptr<FixedRateBond> createFrom(const StaticFixedRateBondTemplate& fixed_rate_bond_template) {
		return boost::make_shared<FixedRateBond>(
			fixed_rate_bond_template.settlement_days_,
			fixed_rate_bond_template.face_amount_,
			fixed_rate_bond_template.schedule_,
			fixed_rate_bond_template.coupons_,
			fixed_rate_bond_template.accrual_day_counter_);
	};

	//Dynamic Fixed Rate Bonds
	
	template<>
	struct DynamicTemplate<FixedRateBond> {
		Natural settlement_days_;
		Real face_amount_;
		DynamicScheduleTemplate schedule_;
		std::vector<Rate> coupons_;
		DayCounter accrual_day_counter_;
	};
	
	typedef DynamicTemplate<FixedRateBond> DynamicFixedRateBondTemplate;
	
	inline boost::shared_ptr<FixedRateBond> createFrom(const DynamicFixedRateBondTemplate& fixed_rate_bond_template) {
		return boost::make_shared<FixedRateBond>(
			fixed_rate_bond_template.settlement_days_,
			fixed_rate_bond_template.face_amount_,
			(*createFrom(fixed_rate_bond_template.schedule_)),
			fixed_rate_bond_template.coupons_,
			fixed_rate_bond_template.accrual_day_counter_);
	};
	
	/* OTHER CLASSES ?? */

};

#endif