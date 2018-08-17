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



//include all aiTools headers
#include <aiTools/Algorithm/AStar.h>
#include <aiTools/Algorithm/Dijkstra.h>
#include <aiTools/Classification/ConfusionMatrix.h>
#include <aiTools/Math/Angle.h>
#include <aiTools/Math/Distance.h>
#include <aiTools/Math/FixedPrecision.h>
#include <aiTools/Math/Geometry.h>
#include <aiTools/Math/LinearTransformation.h>
#include <aiTools/Math/LosslessConversion.h>
#include <aiTools/Math/MovingAverage.h>
#include <aiTools/Math/Numeric.h>
#include <aiTools/Math/SparseVector.h>
#include <aiTools/Math/Statistics.h>
#include <aiTools/Math/Vector.h>
#include <aiTools/Math/Vector2.h>
#include <aiTools/Math/Vector3.h>
#include <aiTools/Serialization/Endianness.h>
#include <aiTools/Serialization/IntrinsicDatatypes.h>
#include <aiTools/Serialization/Serialization.h>
#include <aiTools/Serialization/STLDataTypes.h>
#include <aiTools/Threading/ParallelForEach.h>
#include <aiTools/Threading/RAIIThread.h>
#include <aiTools/Threading/TaskQueue.h>
#include <aiTools/Time/StopWatch.h>
#include <aiTools/Time/Time.h>
#include <aiTools/Time/WallClock.h>
#include <aiTools/Util/ForEachPair.h>
#include <aiTools/Util/Functors.h>
#include <aiTools/Util/Grid.h>
#include <aiTools/Util/Helper.h>
#include <aiTools/Util/Histogram.h>
#include <aiTools/Util/IterateMultiple.h>
#include <aiTools/Util/MetaprogrammingHelpers.h>
#include <aiTools/Util/PlatformDetection.h>
#include <aiTools/Util/RandomizedForEach.h>
#include <aiTools/Util/Range.h>
#include <aiTools/Util/RestoreStreamState.h>
#include <aiTools/Util/RestrictedValue.h>
#include <aiTools/Util/Trie.h>

//include additional test code header
#include <aiTools/Test/Test.h>


#include <iostream>

namespace aiTests
{
	template <typename Container>
	void testForEachPair()
	{
		Container container;

		for(int i = 0; i<20;++i)
		{
			int count = 0;
			aiTools::for_each_pair(container.begin(), container.end(),
						  [&count](int a, int b)
							{
								int callNr = count++;
								//std::cout << callNr << ":" << a << "," << b << std::endl;
								int offset = callNr * 2;
								QASSERT(a == offset);
								QASSERT(b == offset+1);
							});
			QASSERT(count == i/2);//rounds down, =works for odd numbers

			container.insert(container.end(), i);
		}
	}

	template <typename Container>
	void testForEachOverlappingPair()
	{
		Container container;

		for(int i = 0; i<20;++i)
		{
			int count = 0;
			aiTools::for_each_overlapping_pair(container.begin(), container.end(),
						  [&count](int a, int b)
							{
								int callNr = count++;
								QASSERT(a == callNr);
								QASSERT(b == callNr+1);
							});
			if(i == 0)
				QASSERT(count==0)
			else
				QASSERT(count == i-1)//one less, for the 1-item thing we miss in duplets

			container.insert(container.end(), i);
		}
	}

	void testForEachPairs()
	{
		testForEachPair<std::vector<int>>();
		testForEachPair<std::set<int>>();
		testForEachOverlappingPair<std::vector<int>>();
		testForEachOverlappingPair<std::set<int>>();
	}


	void testRandomizedForEach()
	{
		const auto size = 30;
		const std::vector<int> firstDefaultShuffled =  {4, 28, 11, 15, 24, 16, 17, 29, 26, 9, 3, 22, 19, 2, 0, 23, 25, 18, 13, 8, 5, 21, 7, 12, 14, 20, 27, 6, 10, 1};
		const std::vector<int> secondDefaultShuffled = {5, 12, 20, 6, 25, 0, 21, 15, 26, 27, 11, 13, 8, 17, 9, 7, 24, 4, 2, 3, 29, 14, 28, 19, 10, 22, 1, 18, 23, 16};
		const std::vector<int> seed0shuffled =         {18, 20, 27, 5, 11, 3, 12, 19, 0, 21, 10, 13, 29, 1, 26, 22, 8, 9, 28, 2, 6, 16, 25, 24, 7, 14, 23, 4, 15, 17};


		std::vector<int> test1;
		std::set<int> test2;

		for(auto value = 0; value < size; ++value)
		{
			test1.push_back(value);
			test2.insert(value);
		}

		//random shuffle analogue
		int index = 0;
		aiTools::randomized_for_each(test1.begin(), test1.end(),
										[&firstDefaultShuffled, &index](int val)
										{
											QASSERT(val == firstDefaultShuffled[index++]);
										}
									);
		index = 0;
		aiTools::randomized_for_each(test2.begin(), test2.end(),
										[&secondDefaultShuffled, &index](int val)
										{
											//std::cout << val << ", ";
											QASSERT(val == secondDefaultShuffled[index++]);
										}
									);

		//random shuffle analogue with custom rand func
		auto badrandfunc = [](int i){return i-1;};

		index = 0;
		aiTools::randomized_for_each(test1.begin(), test1.end(),
										[&index](int val)
										{
											QASSERT(val == index++);
										},
										badrandfunc
									);
		index = 0;
		aiTools::randomized_for_each(test2.begin(), test2.end(),
										[&index](int val)
										{
											//std::cout << val << ", ";
											QASSERT(val == index++);
										},
										badrandfunc
									);


		//std::shuffle analogue
		const unsigned seed = 0;
		index = 0;
		aiTools::shuffled_for_each(test1.begin(), test1.end(),
										[&seed0shuffled, &index](int val)
										{
											//std::cout << val << ", ";
											QASSERT(val == seed0shuffled[index++]);
										},
										std::default_random_engine(seed)
									);
		index = 0;
		std::cout << std::endl;
		aiTools::shuffled_for_each(test2.begin(), test2.end(),
										[&seed0shuffled, &index](int val)
										{
											//std::cout << val << ", ";
											QASSERT(val == seed0shuffled[index++]);
										},
										std::default_random_engine(seed)
									);
	}

	template <typename T>
	void testRangeForType()
	{
		const T lower = 0;
		const T upper = 10;
		aiTools::Range<T> range01(lower, upper);
		aiTools::Range<T> range02(upper, lower);

		QASSERT(!range01.isInside(lower-1));
		QASSERT(range02.isInside(lower-1));

		for(T i = lower;i<upper;++i)
		{
			QASSERT(range01.isInside(i));
			QASSERT(!range02.isInside(i));
		}

		QASSERT(!range01.isInside(upper));
		QASSERT(!range01.isInside(upper+1));
		QASSERT(range02.isInside(upper));
		QASSERT(range02.isInside(upper+1));
	}

	void testRange()
	{
		testRangeForType<int>();
		testRangeForType<float>();




		//TODO: test different restrictions and so on. something like this: (w/ dynamic range here)

//		using EnforcerType = aiTools::WrapAround<aiTools::Range<int>>;
//		EnforcerType enforce{{0, 10}};
//		for(int i = -10;i<=20;++i)
//		{
//			std::cout << i << " mapped to " << enforce(i) << std::endl;
//		}
	}

	void testRestoreStreamState()
	{
		std::stringstream stream;

		stream << aiTools::Math::PI;
		const std::string defaultPiStr("3.14159");
		QASSERT(stream.str() == defaultPiStr);//less stream test, more of a locale test, not needed
		stream.str("");

		{
			auto restorer = aiTools::RestoreStreamState(stream);

			stream.precision(3);
			stream << std::fixed;
			stream << aiTools::Math::PI;
			const std::string defaultPi3Str("3.142");
			QASSERT(stream.str() == defaultPi3Str);
			stream.str("");
		}

		stream << aiTools::Math::PI;
		QASSERT(stream.str() == defaultPiStr);//could also test against "value before".

		//TODO in theory one could test more here.
	}

	void testTrie()
	{
		aiTools::Trie<int> trie;

		std::vector<std::string> strings = {"nom", "a", "b", "ab", "test"};
		int nr = 0;

		for(const auto& str : strings)
		{
			trie.insert(str, nr++);
		}

		nr = 0;
		for(const auto& str : strings)
		{
			for(std::size_t iter = 0; iter != str.length(); ++iter)
			{
				auto shortstr = str.substr(0, iter);
				QASSERT(trie.contains(shortstr));
			}
			QASSERT(trie.contains(str));
			int currentVal = nr++;

			QASSERT(trie.retrieve(str) == currentVal);
			QASSERT(trie.hasValue(str));
			QASSERT(trie.tryRetrieve(str));
			QASSERT(*trie.tryRetrieve(str) == currentVal);
		}

		//with current implementation, can not test a lot more
		QASSERT(! trie.contains("common sense"));
		QASSERT(trie.tryRetrieve("common sense") == nullptr);
		QASSERT(! trie.hasValue("common sense"));

		trie.clear();
		nr = 0;
		QASSERT(trie.contains(""));
		for(const auto& str : strings)
		{
			QASSERT(!trie.contains(str));
			QASSERT(!trie.hasValue(str));
			QASSERT(!trie.tryRetrieve(str));
		}

		std::string lstr("long string");
		trie.insert(lstr);
		QASSERT(trie.contains(lstr));
		QASSERT(trie.hasValue(lstr));

		for(std::size_t iter = 0; iter != lstr.length(); ++iter)
		{
			auto shortstr = lstr.substr(0, iter);
			QASSERT(trie.contains(shortstr));
			QASSERT(!trie.hasValue(shortstr));
		}
	}

	template <typename T>
	void testGridIndexWrapper()
	{
		const int w = 5;
		const int h = 4;
		aiTools::GridIndexWrapper<std::size_t, T> gridIndices(w, h);

		unsigned int i = 0;
		for(int y = 0;y<h;++y)
		{
			for(int x = 0;x<w;++x)
			{
				//std::cout << "mapped (" << x << "," << y << ") to index " << gridIndices(x, y) << std::endl;
				QASSERT(gridIndices(x, y) == i);
				int tx, ty;
				std::tie(tx, ty) = gridIndices(i);
				//std::cout << "reverse-mapped index " << i << " to (" << x << "," << y << ")" << std::endl;
				QASSERT(tx == x);
				QASSERT(ty == y);
				++i;
			}
		}
	}

	void testGridIndexWrapper()
	{
		testGridIndexWrapper<aiTools::WrapIndices>();
		testGridIndexWrapper<aiTools::ClampIndices>();
		testGridIndexWrapper<aiTools::AssertIndices>();
	}

	void testGrid()
	{
		for(int h = 1;h<4;++h)
		{
			for(int w = 1;w<5;++w)
			{
				aiTools::TorusGrid<int> grid(w, h);//TODO - write a proper test, also one that differentiates torus and normal grid

				int i=0;
				for(int y = 0;y<h;++y)
				{
					for(int x = 0;x<w;++x)
					{
						auto node = grid.at(x, y);
						node.mData = i++;
					}
				}

				for(int y = 0;y<h;++y)
				{
					for(int x = 0;x<w;++x)
					{
						//std::cout << "neighbors for (" << x << "," << y << ") are: {";
						auto node = grid.at(x, y);
						//grid.forAll4NeighborsDo
						//		(x, y, [](const GridNode<int>& node){std::cout << "(" << node.x << "," << node.y << ") ";});
						//grid.forAll8NeighborsDo
						//		(x, y, [](const GridNode<int>& node){std::cout << "(" << node.x << "," << node.y << ") ";});

						for(auto dir : aiTools::Direction::AllDirections)
						{
							if(node.hasNeighbor(dir))
							{
								//auto neighbor = node.getNeighbor(dir);
								//std::cout << "(" << neighbor.x << "," << neighbor.y << ") " << std::flush;
							}
						}


						//std::cout << "}" << std::endl;
					}
				}

				std::cout << std::endl;
			}
		}
	}

	void testVector2()
	{
		using Vec = aiTools::Math::Vector2<float>;

		const float a=1.f, b=0.f;

		//construction:
		Vec test001;
		QASSERT(test001.x == 0 && test001.y == 0);
		QASSERT(test001.lengthSquared() == 0);

		Vec test002(a);
		QASSERT(test002.x == a && test002.y == 0);
		QASSERT(test002.lengthSquared() == a);

		Vec test003(a, b);
		QASSERT(test002 == test003);

		Vec test004(b, a);
		QASSERT(test003 != test004);

		float vals[2] = {1, 0};
		Vec test005(vals);
		QASSERT(test003 == test005);

		std::vector<float> vec = {1, 0};
		Vec test006(vec);
		QASSERT(test005 == test006);

		std::vector<double> vecd = {1., 0.};
		Vec test007(vecd);
		QASSERT(test007 == test006);

		//TODO
		//Vec test008(vec.begin(), vec.end());
		//QASSERT(test007 == test008);

		using iVec = aiTools::Math::Vector2<int>;

		iVec t001 = {2, 2};
		QASSERT(t001.lengthSquared() == 8);
		QASSERT(t001.length() == 2);
		t001.setLength(4);
		QASSERT(t001.lengthSquared() == 32);
		QASSERT(t001.length() == 5);

	}

	void testVector3()
	{



	}

	void testVectorN()
	{



	}

	void testSparseVector()
	{



	}

	void testVector()
	{
		testVector2();
		testVector3();
		testVectorN();
		testSparseVector();
	}

	void testLosslessConversion()
	{
		//bool to anything
		testTypesForLosslesConversion<bool, bool, char, signed char, unsigned char, short, signed short, unsigned short, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, float, double, long double>();

		//char and unsigned char
		testTypesForLosslesConversion<char, char, signed char, unsigned char, short, signed short, unsigned short, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, float, double, long double>();
		testTypesForLosslesConversion<signed char, char, signed char, unsigned char, short, signed short, unsigned short, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, float, double, long double>();
		testTypesForLosslesConversion<unsigned char, char, signed char, unsigned char, short, signed short, unsigned short, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, float, double, long double>();

		//short and unsigned short
		testTypesForLosslesConversion<short, short, signed short, unsigned short, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, float, double, long double>();
		testTypesForLosslesConversion<signed short, short, signed short, unsigned short, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, float, double, long double>();
		testTypesForLosslesConversion<unsigned short, short, signed short, unsigned short, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, float, double, long double>();

		//int and unsigned int
		testTypesForLosslesConversion<int, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, double, long double>();
		testTypesForLosslesConversion<signed int, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, double, long double>();
		testTypesForLosslesConversion<unsigned int, int, signed int, unsigned int, long, signed long, unsigned long, long long, signed long long, unsigned long long, double, long double>();

		//long and unsigned long
		testTypesForLosslesConversion<long, long, signed long, unsigned long, long long, signed long long, unsigned long long, double, long double>();
		testTypesForLosslesConversion<signed long, long, signed long, unsigned long, long long, signed long long, unsigned long long, double, long double>();
		testTypesForLosslesConversion<unsigned long, long, signed long, unsigned long, long long, signed long long, unsigned long long, double, long double>();

		//long long and unsigned long long
		testTypesForLosslesConversion<long long, long long, signed long long, unsigned long long, long double>();
		testTypesForLosslesConversion<signed long long, long long, signed long long, unsigned long long, long double>();
		testTypesForLosslesConversion<unsigned long long, long long, signed long long, unsigned long long, long double>();

		//float
		testTypesForLosslesConversion<float, float, double, long double>();

		//double
		testTypesForLosslesConversion<double, double, long double>();

		//long double
		testTypesForLosslesConversion<long double, long double>();
	}

	void testHistogram()
	{
		for(unsigned int binSize = 1;binSize<=10;++binSize)
		{
			const unsigned int sampleSize = 100;
			const double idealValue = sampleSize / double(binSize);
			//std::cout << "Bin size: " << binSize << " average: " << idealValue << std::endl;

			//TODO further investigate histogram, as for some values it showed inconsistencies
			aiTools::EvenlySpacedHistogram<float> hist(aiTools::Range<float>(0, 100), binSize, false);

			for(unsigned int i=0;i<sampleSize;++i)
			{
				++hist.getBin(float(i%sampleSize));
			}

			auto& result = hist.getResult();
			QASSERT(result.size() == binSize);

			std::size_t sum = 0;
			for(auto r : result)
			{
				//std::cout << r << std::endl;
				sum += r;
				QASSERT(std::abs(double(r)-idealValue) <=1);
			}
			QASSERT(sum == sampleSize);
		}
	}

	void testLinearTransformation()
	{
		aiTools::Math::LinearInterpolation<int> interp01(aiTools::Range<int>(0, 10));
		aiTools::Math::LinearInterpolation<aiTools::Math::Vector3f> interp02({0,0,0}, {10, 20, 30});

		double q = 0;

		for(int i = -10;i<=20;++i)
		{
			q = i / 10.0;

			auto r = interp01(q);
			QASSERT(r == i);
		}

		for(int i = 0;i<=10;++i)
		{
			q = i / 10.0;

			auto r = interp02(q);
			QASSERT(r.x == i);
			QASSERT(r.y == i*2);
			QASSERT(r.z == i*3);
			//std::cout << r.x << ", " << r.y << ", " << r.z << std::endl;
		}


		auto transform01 = aiTools::Math::transformBetweenRanges<double>({0, 10}, {100, 0});

		for(int i = 0;i<=10;++i)
		{
			q = static_cast<float>(i);
			auto r = transform01(q);
			QASSERT(r == (10-i)*10);
		}
	}

	void testRestrictedValue()
	{
		//using ufloat = aiTools::RestrictedValue<float, aiTools::RangeRestriction<aiTools::MinusPiToPi, aiTools::WrapAround>>;
		//using ufloat2 = aiTools::RestrictedValue<float, aiTools::UnsignedRestriction>;

	//<aiTools::MinusPiToPi, aiTools::WrapAround>

		//ufloat f = 0.5f;

	}

	void testIterateMultiple()
	{
		auto action1 = [](int& val){static int comp = 1;QASSERT(comp == val);++comp;};
		auto action2 = [](int& val1, int& val2){static int comp1 = 1;static int comp2 = 4;QASSERT(comp1 == val1);QASSERT(comp2 == val2);++comp1;++comp2;};
		auto action3 = [](int& val1, int& val2, int& val3){static int comp1 = 1;static int comp2 = 4;static int comp3 = 7;QASSERT(comp1 == val1);QASSERT(comp2 == val2);QASSERT(comp3 == val3);++comp1;++comp2;++comp3;/*std::cout << val1 << ", " << val2 << ", " << val3 << std::endl;*/};

		std::vector<int> vec1 = {1, 2, 3};
		std::vector<int> vec2 = {4, 5, 6};
		std::vector<int> vec3 = {7, 8, 9};

		const auto beg1 = vec1.begin();
		const auto end1 = vec1.end();
		const auto beg2 = vec2.begin();
		const auto end2 = vec2.end();
		const auto beg3 = vec3.begin();
		const auto end3 = vec3.end();

		aiTools::for_each_multiple(action1, beg1, end1);
		aiTools::for_each_multiple(action2, beg1, end1, beg2, end2);
		aiTools::for_each_multiple(action3, beg1, end1, beg2, end2, beg3, end3);
	}

	void run_all_tests()
	{
		testForEachPairs();
		testRandomizedForEach();
		testRange();
		testRestoreStreamState();
		testTrie();
		testVector();
		testLosslessConversion();
		testHistogram();
		testLinearTransformation();
		testRestrictedValue();
		testIterateMultiple();
		testGridIndexWrapper();
		testGrid();
	}
}

int main()
{
	try
	{
		aiTests::run_all_tests();
	}
	catch(const std::exception& e)
	{
		std::cerr << "exception caught: " << e.what() << std::endl;
		std::cerr << "test failed." << std::endl;
		return 1;
	}
	catch(...)
	{
		std::cerr << "something else caught." << std::endl;
		std::cerr << "test failed." << std::endl;
		return 1;
	}

	std::cout << "all tests succeeded" << std::endl;

	return 0;
}
