#ifndef TEMPLATES_H
#define TEMPLATES_H
#include <ql/quantlib.hpp>
using namespace QuantLib;
namespace ALM {

	/* Schedule templates */
	class ScheduleTemplate {
	public:
		virtual ~ScheduleTemplate() = default;
		virtual boost::shared_ptr<QuantLib::Schedule> create() const = 0;
	};

	class StaticScheduleTemplate : public ScheduleTemplate {
	private:
		QuantLib::Date effective_date_;
		QuantLib::Date termination_date_;
		QuantLib::Period tenor_;
		QuantLib::Calendar calendar_;
		QuantLib::BusinessDayConvention convention_;
		QuantLib::BusinessDayConvention termination_date_convention_;
		QuantLib::DateGeneration::Rule rule_;
		bool end_of_month_;

	public:
		StaticScheduleTemplate(const QuantLib::Date& effective_date,
			const QuantLib::Date& termination_date,
			const QuantLib::Period& tenor,
			const QuantLib::Calendar& calendar,
			QuantLib::BusinessDayConvention convention,
			QuantLib::BusinessDayConvention termination_date_convention,
			QuantLib::DateGeneration::Rule rule,
			bool end_of_month)
			: effective_date_(effective_date),
			termination_date_(termination_date),
			tenor_(tenor),
			calendar_(calendar),
			convention_(convention),
			termination_date_convention_(termination_date_convention),
			rule_(rule),
			end_of_month_(end_of_month) {};

		boost::shared_ptr<QuantLib::Schedule> create() const {
			return boost::make_shared<QuantLib::Schedule>(
				effective_date_, termination_date_, tenor_, calendar_,
				convention_, termination_date_convention_, rule_, end_of_month_);
		}
	};

	class DynamicScheduleTemplate : public ScheduleTemplate {
	private:
		QuantLib::Period duration_;
		QuantLib::Period tenor_;
		QuantLib::Calendar calendar_;
		QuantLib::BusinessDayConvention convention_;
		QuantLib::BusinessDayConvention termination_date_convention_;
		QuantLib::DateGeneration::Rule rule_;
		bool end_of_month_;

	public:
		DynamicScheduleTemplate(const QuantLib::Period& duration,
			const QuantLib::Period& tenor,
			const QuantLib::Calendar& calendar,
			QuantLib::BusinessDayConvention convention,
			QuantLib::BusinessDayConvention termination_date_convention,
			QuantLib::DateGeneration::Rule rule,
			bool end_of_month)
			: duration_(duration),
			tenor_(tenor),
			calendar_(calendar),
			convention_(convention),
			termination_date_convention_(termination_date_convention),
			rule_(rule),
			end_of_month_(end_of_month) {};

		boost::shared_ptr<QuantLib::Schedule> create() const {
			return boost::make_shared<QuantLib::Schedule>(
				Settings::instance().evaluationDate().value(), Settings::instance().evaluationDate().value() + duration_,
				tenor_, calendar_, convention_, termination_date_convention_, rule_, end_of_month_);
		}
	};

	/* Yield curve templates */

	class YieldTermStructureTemplate {
		virtual ~YieldTermStructureTemplate() = default;
		virtual boost::shared_ptr<YieldTermStructure> create() const = 0;
	};
	
	class StaticFlatForwardTemplate {
	private:
		Date reference_date_;
		Rate forward_;
		DayCounter day_counter_;
		Compounding compounding_;
		Frequency frequency_;
	public:
		StaticFlatForwardTemplate(const QuantLib::Date& reference_date,
			QuantLib::Rate forward,
			const QuantLib::DayCounter& day_counter,
			QuantLib::Compounding compounding,
			QuantLib::Frequency frequency)
			: reference_date_(reference_date),
			forward_(forward),
			day_counter_(day_counter),
			compounding_(compounding),
			frequency_(frequency) {};

		boost::shared_ptr<YieldTermStructure> create() {
			return boost::make_shared<FlatForward>(reference_date_, forward_, day_counter_, compounding_, frequency_);
		};
	};

	class DynamicFlatForwardTemplate {
	private:
		Rate forward_;
		DayCounter day_counter_;
		Compounding compounding_;
		Frequency frequency_;
	public:
		DynamicFlatForwardTemplate(QuantLib::Rate forward,
			const QuantLib::DayCounter& day_counter,
			QuantLib::Compounding compounding,
			QuantLib::Frequency frequency)
			: forward_(forward),
			day_counter_(day_counter),
			compounding_(compounding),
			frequency_(frequency) {};

		boost::shared_ptr<YieldTermStructure> create() {
			return boost::make_shared<FlatForward>(Settings::instance().evaluationDate().value(), forward_, day_counter_, compounding_, frequency_);
		};
	};

	/* Instrument Templates */
	class InstrumentTemplate {
	public:
		virtual ~InstrumentTemplate() = default;
		virtual boost::shared_ptr<QuantLib::Instrument> create() const = 0;
	};

	/* Fixed Rate Bonds */

	class StaticFixedRateBondTemplate : public InstrumentTemplate {
	private:
		Natural settlement_days_;
		Real face_amount_;
		Schedule schedule_;
		std::vector<Rate> coupons_;
		DayCounter accrual_day_counter_;
	public:
		StaticFixedRateBondTemplate(QuantLib::Natural settlement_days,
			QuantLib::Real face_amount,
			const QuantLib::Schedule& schedule,
			const std::vector<QuantLib::Rate>& coupons,
			const QuantLib::DayCounter& accrual_day_counter)
			: settlement_days_(settlement_days),
			face_amount_(face_amount),
			schedule_(schedule),
			coupons_(coupons),
			accrual_day_counter_(accrual_day_counter) {};

		boost::shared_ptr<QuantLib::Instrument> create() const {
			return boost::make_shared<FixedRateBond>(settlement_days_,face_amount_,	schedule_,	coupons_,accrual_day_counter_);
		};
	};
	
	class DynamicFixedRateBondTemplate : public InstrumentTemplate {
	private:
		Natural settlement_days_;
		Real face_amount_;
		DynamicScheduleTemplate schedule_template_;
		std::vector<Rate> coupons_;
		DayCounter accrual_day_counter_;
	public:
		DynamicFixedRateBondTemplate(QuantLib::Natural settlement_days,
			QuantLib::Real face_amount,
			const DynamicScheduleTemplate& schedule_template,
			const std::vector<QuantLib::Rate>& coupons,
			const QuantLib::DayCounter& accrual_day_counter)
			: settlement_days_(settlement_days),
			face_amount_(face_amount),
			schedule_template_(schedule_template),
			coupons_(coupons),
			accrual_day_counter_(accrual_day_counter) {};

		boost::shared_ptr<QuantLib::Instrument> create() const {
			return boost::make_shared<FixedRateBond>(settlement_days_, face_amount_, *schedule_template_.create(), coupons_, accrual_day_counter_);
		};
	};

	/* OTHER CLASSES ?? */

};

#endif