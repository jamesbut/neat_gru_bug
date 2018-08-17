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

#ifndef __AITOOLS_TRIE_H_INCLUDED
#define __AITOOLS_TRIE_H_INCLUDED

#include <aiTools/Util/Helper.h>

#include <boost/optional.hpp>


#include <map>
#include <memory>
#include <string>
#include <ostream>
#include <vector>

namespace aiTools
{
	template <typename T>
	class Trie;

	template <typename T, typename Storage>
	class TrieNode;

	template <typename T, typename Storage>
	std::ostream& operator<<(std::ostream& stream, const TrieNode<T, Storage>& trie)
	{
		for(auto iter=trie.mChildren.begin();iter!=trie.mChildren.end();++iter)
		{
			stream.put(iter->first);
			stream  << ": ";
			stream << iter->second;
			stream <<  std::endl;
		}
		return stream;
	}

	template <typename T>
	std::ostream& operator<<(std::ostream& stream, const Trie<T>& trie)
	{
		stream << trie.mRootNode;
		return stream;
	}


	template <typename T, typename Storage>
	class TrieNode
	{
	public:
		using Container = std::map<char, TrieNode>;

		bool containsSubstring(std::string::const_iterator start, std::string::const_iterator end) const
		{
			if(start == end)
				return true;

			auto followupNode = mChildren.find(*start);
			if(followupNode != mChildren.end())
				return followupNode->second.containsSubstring(++start, end);//TODO get rid of recurrency here

			return false;
		}

		const T& retrieve(std::string::const_iterator start, std::string::const_iterator end) const
		{
			if(start == end)
				return mStoredData;

			auto followupNode = mChildren.find(*start);
			if(followupNode != mChildren.end())
				return followupNode->second.retrieve(++start, end);//TODO get rid of recurrency here

			THROW_RUNTIME_ERROR("could not retrieve element from trie");
		}

		const T* tryRetrieve(std::string::const_iterator start, std::string::const_iterator end) const
		{
			if(start == end)
				return (mStoredData) ? &*mStoredData : nullptr;

			auto followupNode = mChildren.find(*start);
			if(followupNode != mChildren.end())
				return followupNode->second.tryRetrieve(++start, end);//TODO get rid of recurrency here

			return nullptr;
		}

		T& retrieve(std::string::const_iterator start, std::string::const_iterator end)
		{
			if(start == end)
				return *mStoredData;

			auto followupNode = mChildren.find(*start);
			if(followupNode != mChildren.end())
				return followupNode->second.retrieve(++start, end);//TODO get rid of recurrency here

			THROW_RUNTIME_ERROR("could not retrieve element from trie");
		}

		T* tryRetrieve(std::string::const_iterator start, std::string::const_iterator end)
		{
			if(start == end)
				return (mStoredData) ? &*mStoredData : nullptr;

			auto followupNode = mChildren.find(*start);
			if(followupNode != mChildren.end())
				return followupNode->second.tryRetrieve(++start, end);//TODO get rid of recurrency here

			return nullptr;
		}

		void insertSubstring(std::string::const_iterator start, std::string::const_iterator end, const T& data)
		{
			if(start == end)
			{
				mStoredData = data;
				return;
			}

			auto foundKids = mChildren.find(*start);
			if(foundKids == mChildren.end()) //no kid with current substr
			{
				// first create child, add substring, then add this subtree to map
				// this way, stronger exception safety in case something goes awry
				// child is moved into pair, then moved into map, should be decent

				TrieNode childNode;
				const char newKidName = *start;
				childNode.insertSubstring(++start, end, data);
				mChildren.insert(std::make_pair(newKidName, std::move(childNode)));
			}
			else
				foundKids->second.insertSubstring(++start, end, data);
		}

		template <typename Action, typename Condition>
		void traverse(std::vector<char>& word, Action& action, const Condition& condition) const
		{
			if(condition(mStoredData)) // TODO leave like this or if(mStoredData && condition(*mStoredData)) ?
				action(std::string(word.begin(), word.end()), mStoredData);

			//todo: can this be made strongly exception safe?

			for(auto iter=mChildren.begin();iter!=mChildren.end();++iter)
			{
				word.push_back(iter->first);
				iter->second->traverse(word, action, condition);
				word.pop_back();
			}
		}

		friend std::ostream& operator<< <>(std::ostream& stream, const TrieNode<T, Storage>& trie);

	private:
		std::map<char, TrieNode> mChildren;
		Storage mStoredData;
	};

	///a fairly inefficient trie implementation with some issues, not to be trusted too much.
	template <typename T>
	class Trie
	{
	public:
		using Storage = boost::optional<T>;
		using NodeType = TrieNode<T, Storage>;

		bool contains(const std::string& entry)
		{
			return mRootNode.containsSubstring(entry.begin(), entry.end());
		}

		bool hasValue(const std::string& entry) const
		{
			return tryRetrieve(entry) != nullptr;
		}

		const T* tryRetrieve(const std::string& entry) const
		{
			return mRootNode.tryRetrieve(entry.begin(), entry.end());
		}

		const T& retrieve(const std::string& entry) const
		{
			return mRootNode.retrieve(entry.begin(), entry.end());
		}

		T* tryRetrieve(const std::string& entry)
		{
			return mRootNode.tryRetrieve(entry.begin(), entry.end());
		}

		T& retrieve(const std::string& entry)
		{
			return mRootNode.retrieve(entry.begin(), entry.end());
		}

		void insert(const std::string& entry, const T& data = T())
		{
			mRootNode.insertSubstring(entry.begin(), entry.end(), data);
		}

		template <typename Action, typename Condition>
		void traverse(Action& action, const Condition& condition) const
		{
			std::vector<char> word;
			mRootNode.traverse(word, action, condition);
		}

		void clear()
		{
			mRootNode = NodeType();
		}

		friend std::ostream& operator<< <>(std::ostream& stream, const Trie<T>& trie);

	private:
		NodeType mRootNode;
	};
}

#endif // __AITOOLS_TRIE_H_INCLUDED
