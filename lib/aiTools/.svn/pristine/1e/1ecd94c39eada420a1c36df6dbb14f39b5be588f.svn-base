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

#include <aiTools/Classification/ConfusionMatrix.h>
#include <aiTools/Util/Helper.h>

#include <iostream>

namespace aiTools
{
	namespace Classification
	{
		ConfusionMatrix::ConfusionMatrix(std::size_t numberClasses) :
				mMatrix(numberClasses, std::vector<unsigned int>(numberClasses))
		{
		}

		ConfusionMatrix::ConfusionMatrix(std::vector<std::vector<unsigned int>>& matrix) :
				mMatrix(ensureSquareMatrix(matrix))
		{
		}

		ConfusionMatrixAccessor ConfusionMatrix::registerValue(std::size_t actualClass)
		{
			ASSERT(actualClass < mMatrix.size(), "actualClass value of " << actualClass << " exceeds confusion matrix dimension: " << mMatrix.size());
			return ConfusionMatrixAccessor(mMatrix[actualClass]);
		}

		std::vector<std::vector<unsigned int>>& ConfusionMatrix::ensureSquareMatrix(std::vector<std::vector<unsigned int>>& matrix)
		{
			const std::size_t xDim = matrix.size();
			for(const auto& col : matrix)
				ASSERT(col.size() == xDim, "disparity in matrix detected, column with " << col.size() << " elements, but the number of columns is:" << xDim);

			return matrix;
		}

		std::vector<double> ConfusionMatrix::calculatePrecisions() const
		{
			std::vector<double> result(mMatrix.size());

			for(std::size_t y = 0; y < mMatrix.size(); ++y)
			{
				unsigned int correctlyClassified = mMatrix[y][y];//diagonal element;
				unsigned long long sumClassifiedAs = 0;
				for(std::size_t x = 0; x < mMatrix.size(); ++x)
					sumClassifiedAs += mMatrix[x][y];

				result[y] = static_cast<double>(
					static_cast<long double>(correctlyClassified) / static_cast<long double>(sumClassifiedAs));
			}
			return result;
		}

		std::vector<double> ConfusionMatrix::calculateRecalls() const
		{
			std::vector<double> result(mMatrix.size());

			for(std::size_t x = 0; x < mMatrix.size(); ++x)
			{
				unsigned int correctlyClassified = mMatrix[x][x];//diagonal element;
				unsigned long long sumMisclassified = 0;
				for(std::size_t y = 0; y < mMatrix.size(); ++y)
					sumMisclassified += mMatrix[x][y];

				result[x] = static_cast<double>(
					static_cast<long double>(correctlyClassified) / static_cast<long double>(sumMisclassified));
			}
			return result;
		}

		double ConfusionMatrix::calculateAccurracy() const
		{
			unsigned long long sumDiagonal = 0;
			unsigned long long totalSum = 0;

			for(std::size_t x = 0; x < mMatrix.size(); ++x)
			{
				for(std::size_t y = 0; y < mMatrix.size(); ++y)
				{
					totalSum += mMatrix[x][y];
					if(x == y)
						sumDiagonal += mMatrix[x][y];
				}
			}


			return static_cast<double>(
				static_cast<long double>(sumDiagonal) / static_cast<long double>(totalSum));
		}

		ConfusionMatrix& operator+=(ConfusionMatrix& lhs, const ConfusionMatrix& rhs)
		{
			ASSERT(lhs.mMatrix.size() == rhs.mMatrix.size(), "matrix dimensions do not fit, " << lhs.mMatrix.size() << " vs " << rhs.mMatrix.size());

			for(std::size_t x = 0; x < lhs.mMatrix.size(); ++x)
			{
				for(std::size_t y = 0; y < lhs.mMatrix.size(); ++y)
				{
					lhs.mMatrix[x][y] += rhs.mMatrix[x][y];
				}
			}

			return lhs;
		}

		std::ostream& operator<<(std::ostream& ostream, const ConfusionMatrix& matrix)
		{
			for(std::size_t y = 0; y < matrix.mMatrix.size(); ++y)
			{
				for(std::size_t x = 0; x < matrix.mMatrix.size(); ++x)
				{
					ostream << matrix.mMatrix[x][y];
					if(x != matrix.mMatrix.size()-1)
						ostream << ",";
				}
				ostream << std::endl;
			}
			return ostream;
		}


		ConfusionMatrixAccessor::ConfusionMatrixAccessor(std::vector<unsigned int>& data) : mData(data)
		{}

		void ConfusionMatrixAccessor::classifiedAs(std::size_t predictedClass)
		{
			ASSERT(predictedClass < mData.size(), "predictedClass value of " << predictedClass << " exceeds confusion matrix dimension: " << mData.size());
			++mData[predictedClass];
		}
	}
}
