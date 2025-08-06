//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCost.h
//
//	@doc:
//		Cost type
// edited
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_CCost_H
#define GPNAUCRATES_CCost_H

#include "gpos/base.h"
#include "gpos/common/CDouble.h"
#include "gpos/common/CDynamicPtrArray.h"
// TODO: remove reliance on <map> and use CHashMap - would need to route mp through to every CCost instantiation
#include <map>
#include "gpoptextender/EngineProperty/EEngineType.hpp"

namespace gpopt
{
using namespace gpos;

class CCost;
class CEngineSpec;
typedef CDynamicPtrArray<CCost, gpos::CleanupDelete> CCostArray;

// TODO make all req ops commutative with CDouble. Also impl all math ops for CCost (e.g.  -, /, <=, ...) - may want to lose dependence on CDouble

// typedef CHashMap<EEngineType, CDouble, EngineTypeHashValue,
// 					EngineTypeEquals, CleanupNULL<EEngineType>,
// 					CleanupDelete<CDouble>>
// 	EngineTypeToCostMap;

//---------------------------------------------------------------------------
//	@class:
//		CCost
//
//	@doc:
//		Cost for comparing two plans
//
//---------------------------------------------------------------------------
// returns the max of the engines now.
class CCost : public CDouble
{
public:
	std::map<EEngineType, CDouble> engineCostMap; 

	CCost() : CDouble(0.0) 
	{
		engineCostMap.insert(std::make_pair(EetAny, CDouble(0.0)));
	}

	// ctor. Maintain original ctors for compatibility with the original system.

	explicit CCost(DOUBLE d) : CDouble(d)
	{
		engineCostMap.insert(std::make_pair(EetAny, CDouble(d)));
	}

	explicit CCost(DOUBLE d, EEngineType t) : CDouble(d)
	{
		engineCostMap.insert(std::make_pair(t, CDouble(d)));
	}

	// ctor
	explicit CCost(CDouble d) : CDouble(d.Get())
	{
		engineCostMap.insert(std::make_pair(EetAny, CDouble(d.Get())));
	}

	explicit CCost(CDouble d, EEngineType t) : CDouble(d)
	{
		engineCostMap.insert(std::make_pair(t, CDouble(d.Get())));
	}

	// ctor
	CCost(const CCost &cost) : CDouble(cost.Get()), engineCostMap(cost.engineCostMap)
	{}

	inline CDouble DeriveCostFromMap(const std::map<EEngineType, CDouble>& costs)
	{
			if (gpopt::useMaxCosting) {
				CDouble maxCost(0.0);
				for (std::map<EEngineType, CDouble>::const_iterator it = costs.begin();
						it != costs.end();
						++it)
				{
						if (it->second.Get() > maxCost.Get()) {
								maxCost = it->second;
						}
				}
				return maxCost;
			}
				
			CDouble sumCost(0.0);
			for (std::map<EEngineType, CDouble>::const_iterator it = costs.begin();
					it != costs.end();
					++it)
			{
					sumCost = sumCost + it->second;
			}
			return sumCost;
	}

	CCost(std::map<EEngineType, CDouble>& costs) : CDouble(DeriveCostFromMap(costs)), engineCostMap(costs)
	{
	}

	// assignment
	CCost &
	operator=(const CCost &cost)
	{
		CDouble::operator=(cost);
		engineCostMap = cost.engineCostMap;
		return *this;
	}

	// addition operator
	CCost
	operator+(const CCost &cost) const
	{
			// copy our own costs
			std::map<EEngineType, CDouble> merged = engineCostMap;

			for (const auto& [key, val] : cost.engineCostMap) {
					auto it = merged.find(key);
					if (it == merged.end()) {
            auto p = merged.insert(std::make_pair(key, CDouble(0.0)));
            it = p.first;
					}
					it->second = it->second + val;
			}

			return CCost(merged);
	}

	CCost
	operator+(const CDouble &cost) const
	{
			// not well defined
			GPOS_ASSERT(false);
			return CCost(0);
	}

	// multiplication operator
	CCost
	operator*(const CCost &cost) const
	{
			std::map<EEngineType, CDouble> merged = engineCostMap;

			for (const auto& [key, val] : cost.engineCostMap) {
					auto it = merged.find(key);
					if (it == merged.end()) {
            auto p = merged.insert(std::make_pair(key, CDouble(1.0)));
            it = p.first;
					}
					it->second = it->second * val;
			}

			return CCost(merged);
	}

	CCost
	operator/(const CDouble &cost) const
	{
			// not well defined
			GPOS_ASSERT(false);
			return CCost(0);
	}

	CCost
	operator-(const CDouble &cost) const
	{
			// not well defined
			GPOS_ASSERT(false);
			return CCost(0);
	}

	CCost
	operator*(const CDouble &cost) const
	{
    std::map<EEngineType, CDouble> merged = engineCostMap;

    for (auto &kv : merged)
    {
        kv.second = kv.second * cost;
    }

    return CCost(merged);
	}



	BOOL
	operator<(const CCost &cost) const
	{
		if (useMaxCosting) {
			BOOL t = false;

			for (const auto & [key, lhsVal] : engineCostMap) {
				auto it = cost.engineCostMap.find(key);
				if (it == cost.engineCostMap.end()) {
					return false;
				}

				const CDouble & rhsVal = it->second;
				if (lhsVal < rhsVal) {
					t = true;
				} else if (!(lhsVal <= rhsVal)) {
					return false;
				}
			}

			return t;
		}

		return (CDouble)(*this) < (CDouble) cost;
	}


	BOOL
	operator<(const CDouble &cost) const
	{
		if (useMaxCosting) {
			BOOL t = false;

			for (const auto & [key, lhsVal] : engineCostMap) {
				if (lhsVal < cost) {
					t = true;
				} else if (!(lhsVal <= cost)) {
					return false;
				}
			}

			return t;
		}

		return (CDouble)(*this) < cost;
	}

	// comparison operator
	BOOL
	operator>(const CCost &cost) const
	{
		if (useMaxCosting) {
			BOOL t = false;

			for (const auto & [key, lhsVal] : engineCostMap) {
				auto it = cost.engineCostMap.find(key);
				if (it == cost.engineCostMap.end()) {
					continue;
				}

				const CDouble & rhsVal = it->second;
				if (lhsVal > rhsVal) {
					t = true;
				} else if (!(lhsVal >= rhsVal)) {
					return false;
				}
			}

			return t;
		}

		return (CDouble) cost < (CDouble)(*this);
	}


	BOOL
	operator>(const CDouble &cost) const
	{
		if (useMaxCosting) {
			BOOL t = false;
			for (const auto & [key, lhsVal] : engineCostMap) {
				if (lhsVal > cost) {
					t = true;
				} else if (!(lhsVal >= cost)) {
					return false;
				}
			}

			return t;
		}

		return cost < (CDouble)(*this); 
	}

	// d'tor
	virtual ~CCost()
	{
	}

};	// class CCost

}  // namespace gpopt

#endif	// !GPNAUCRATES_CCost_H

// EOF
