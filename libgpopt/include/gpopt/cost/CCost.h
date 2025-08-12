//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCost.h
//
//	@doc:
//		Cost type
// 
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Modifications to file: store a mapping of engines to double
// Enable cost to be MAX([engine costs]) - can be toggled with original cost
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
class CCost
{
private:
	CDouble dCost;

public:
	std::map<EEngineType, CDouble> engineCostMap; 

	CCost() : dCost(0.0) 
	{
		engineCostMap.insert(std::make_pair(EetAny, CDouble(0.0)));
	}

	// ctor. Maintain original ctors for compatibility with the original system.

	explicit CCost(DOUBLE d) : dCost(d)
	{
		engineCostMap.insert(std::make_pair(EetAny, CDouble(d)));
	}

	explicit CCost(DOUBLE d, EEngineType t) : dCost(d)
	{
		engineCostMap.insert(std::make_pair(t, CDouble(d)));
	}

	// ctor
	explicit CCost(CDouble d) : dCost(d.Get())
	{
		engineCostMap.insert(std::make_pair(EetAny, CDouble(d.Get())));
	}

	explicit CCost(CDouble d, EEngineType t) : dCost(d.Get())
	{
		engineCostMap.insert(std::make_pair(t, CDouble(d.Get())));
	}

	// ctor
	CCost(const CCost &cost) : dCost(cost.Get()), engineCostMap(cost.engineCostMap)
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

	CCost(std::map<EEngineType, CDouble>& costs) : dCost(DeriveCostFromMap(costs).Get()), engineCostMap(costs)
	{
	}


	// value accessor
	inline DOUBLE
	Get() const
	{
		return dCost.Get();
	}

	inline DOUBLE
	CostSum() const
	{
		DOUBLE cost = 0.0;
		for (const auto& [_, val] : engineCostMap) {
			cost = cost + val.Get();
		}
		return cost;
	}


	// assignment
	CCost &
	operator=(const CCost &cost)
	{
		dCost = CDouble(cost.Get());
		engineCostMap = cost.engineCostMap;
		return *this;
	}

	// addition operator. Add each engine individually
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
	operator*(const CDouble &cost) const
	{
    std::map<EEngineType, CDouble> merged = engineCostMap;

    for (auto &kv : merged)
    {
        kv.second = kv.second * cost;
    }

    return CCost(merged);
	}

	// multiplication operator
	CCost
	operator*(const CCost &cost) const
	{
		return (*this) * CDouble(cost.Get());
	}

	CCost
	operator/(const CCost &cost) const
	{
		return (*this) * (1/CDouble(cost.Get()));
	}


	// we implement comparisons like this so branches don't get wrongly pruned in FSafeToPrune().
	// Note this is fine because FBetterThan() converts to doubles before doing the comparison.
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

		return dCost < CDouble(cost.Get());
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


		return dCost < cost;
	}

	BOOL 
	operator<=(const CCost &cost) const 
	{
		if (useMaxCosting) {
			for (const auto & [key, lhsVal] : engineCostMap) {
				auto it = cost.engineCostMap.find(key);
				if (it == cost.engineCostMap.end()) {
					return false;
				}

				const CDouble & rhsVal = it->second;
				if (!(lhsVal <= rhsVal)) {
					return false;
				}
			}

			return true;
		}

		return CDouble(cost.Get()) >= dCost;
	}


	// comparison operator
	BOOL
	operator>(const CCost &cost) const
	{
		return cost < (*this);	
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

		return cost < dCost;
	}

	BOOL 
	operator==(const CCost &cost) const 
	{
		if (useMaxCosting) {
			for (const auto & [key, lhsVal] : engineCostMap) {
				auto it = cost.engineCostMap.find(key);
				if (it == cost.engineCostMap.end()) {
					return false;
				}

				const CDouble & rhsVal = it->second;
				if (!(lhsVal == rhsVal)) {
					return false;
				}
			}

			return true;
		}
		
		return dCost == CDouble(cost.Get());
	}


	BOOL 
	operator!=(const CCost &cost) const 
	{
		return !(*this == cost);
	}

	// d'tor
	virtual ~CCost()
	{
	}


		// print to stream
	inline IOstream &
	OsPrint(IOstream &os) const
	{
		return os << dCost.Get();
	}

};	// class CCost

inline IOstream &
operator<<(IOstream &os, CCost d)
{
	return d.OsPrint(os);
}

}  // namespace gpopt

#endif	// !GPNAUCRATES_CCost_H

// EOF
