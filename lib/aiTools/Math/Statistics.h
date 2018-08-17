// aiTools is a free C++ library of AI tools.
// Copyright (C) 2013 - 2016  Benjamin Schnieders

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program, see file LICENCE.txt.
// Alternativley, see <http://www.gnu.org/licenses/>.

#ifndef __AITOOLS_STATISTICS_H_INCLUDED
#define __AITOOLS_STATISTICS_H_INCLUDED

#include <aiTools/Math/Vector.h>
#include <aiTools/Math/SparseVector.h>
#include <aiTools/Math/Distance.h>

#include <aiTools/Util/Histogram.h>
#include <aiTools/Util/RestrictedValue.h>
#include <aiTools/Util/Helper.h>

#include <functional>
#include <cstddef>

namespace aiTools
{
	namespace Math
	{
		template <typename FwdIterType, typename ResultType, template <typename> class Plus, template <typename> class Divides>
		ResultType average(FwdIterType begin, FwdIterType end)
		{
			ResultType result = ResultType(0);

			Plus<ResultType> plus;
			Divides<ResultType> divides;

			unsigned int count = 0;
			while(begin != end)
			{
				result = plus(result, *begin);

				++count;
				++begin;
			}

			return divides(result, count);
		}

		template <typename FwdIterType, typename ResultType = double>
		ResultType average(FwdIterType begin, FwdIterType end)
		{
			return average<FwdIterType, ResultType, std::plus, std::divides>(begin, end);
		}

				template <typename FwdIterType, typename ResultType = double>
		RestrictedValue<RangeRestriction<PositiveRange<ResultType>, Abs>> variance(FwdIterType begin, FwdIterType end)
		{
			ResultType avg = average<FwdIterType, ResultType>(begin, end);

			ResultType result = ResultType(0);

			unsigned int count = 0;
			while(begin != end)
			{
				ResultType difference = *begin - avg;
				result += difference*difference;

				++begin;
				++count;
			}

			return result / count;
		}

		template <typename FwdIterType, typename ResultType = double>
		RestrictedValue<RangeRestriction<PositiveRange<ResultType>, Abs>> std_deviation(FwdIterType begin, FwdIterType end)
		{
			using std::sqrt;

			return sqrt(variance(begin, end));
		}

		template <typename FwdIterType, typename OtherFwdIterType, typename ResultType = double>
		ResultType covariance(FwdIterType beginA, FwdIterType endA, OtherFwdIterType beginB, OtherFwdIterType endB)
		{
			ResultType avgA = average<FwdIterType, ResultType>(beginA, endA);
			ResultType avgB = average<OtherFwdIterType, ResultType>(beginB, endB);

			ResultType result = ResultType(0);

			unsigned int count = 0;
			while(beginA != endA && beginB != endB)
			{
				result += (*beginA - avgA) * (*beginB - avgB);
				++beginA;
				++beginB;
				++count;
			}

			return result / count;
		}

		template <typename FwdIterType, typename OtherFwdIterType, typename ResultType = double>
		RestrictedValue<RangeRestriction<MinusOneToOne<ResultType>, Clamp>> correlation(FwdIterType beginA, FwdIterType endA, OtherFwdIterType beginB, OtherFwdIterType endB)
		{
			ResultType cov = covariance<FwdIterType, OtherFwdIterType, ResultType>(beginA, endA, beginB, endB);

			ResultType devA = std_deviation<FwdIterType, ResultType>(beginA, endA);
			ResultType devB = std_deviation<OtherFwdIterType, ResultType>(beginB, endB);

			return cov / (devA * devB);
		}

		template <typename FwdIterType, typename ResultType = double>
		ResultType entropy(FwdIterType begin, FwdIterType end)
		{
			typedef std::map<typename FwdIterType::value_type, unsigned int> map_type;

			std::size_t noEntries = 0;
			map_type histogram;
			//makeHistogram(begin, end, histogram);
			//inlined here for additional entry counting:
			while(begin != end)
			{
				++histogram[*begin++];
				++noEntries;
			}

			const std::size_t noDifferentEntries = histogram.size();

			ResultType result = ResultType(0);

			using std::log;
			const ResultType logv = log(noDifferentEntries);
			const ResultType scale = (logv==0)?1:logv;

			for(typename map_type::const_iterator iter = histogram.begin(); iter != histogram.end(); ++iter)
			{
				ResultType probability = ResultType(iter->second) / noEntries;
				ResultType weighted = -probability * (log(probability) / scale);

				result += weighted;
			}

			return result;
		}

		template <typename FwdIterType, typename OtherFwdIterType, typename ResultType = double>
		typename std::enable_if<!is_pair<typename FwdIterType::value_type>::value, ResultType>::type
		jaccard_similarity(FwdIterType beginA, FwdIterType endA, OtherFwdIterType beginB, OtherFwdIterType endB)
		{
			unsigned int cAnd = 0;
			unsigned int cOr = 0;

			while(beginA != endA && beginB != endB) //TODO should take or here - and count all those as "or" fromt he larger set?
			{
				cAnd += *beginA && *beginB;
				cOr  += *beginA || *beginB;

				++beginA;
				++beginB;
			}
			return static_cast<ResultType>(cAnd) / static_cast<ResultType>(cOr);
		}

		template <typename FwdIterType, typename OtherFwdIterType, typename ResultType = double>
		typename std::enable_if<is_pair<typename FwdIterType::value_type>::value, ResultType>::type
		jaccard_similarity(FwdIterType beginA, FwdIterType endA, OtherFwdIterType beginB, OtherFwdIterType endB)
		{
			return sparse_jaccard_similarity<FwdIterType, OtherFwdIterType, ResultType>(beginA, endA, beginB, endB);
		}

		template <typename FwdIterType, typename OtherFwdIterType, typename ResultType = double>
		ResultType cosine_similarity(FwdIterType beginA, FwdIterType endA, OtherFwdIterType beginB, OtherFwdIterType endB)
		{
			ResultType dotProduct = dot_product<FwdIterType, OtherFwdIterType, ResultType>(beginA, endA, beginB, endB);
			ResultType lengthA = euclidean_length<FwdIterType, ResultType>(beginA, endA);
			ResultType lengthB = euclidean_length<OtherFwdIterType, ResultType>(beginB, endB);

			return dotProduct / (lengthA * lengthB);
		}
	}
}

#endif // __AITOOLS_STATISTICS_H_INCLUDED
