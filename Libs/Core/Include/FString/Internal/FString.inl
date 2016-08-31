#pragma once

#include "FString.h"

namespace Flourish
{
	//A simple wrapper around std::string that allows an FAllocator to be specificed along with extra helper functions

	template<typename CharType>
	FStringBase<CharType>::FStringBase()
	{
		
	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(SizeType numTimesToRepeat, CharType charToRepeat)
		: mString(numTimesToRepeat, charToRepeat)
	{

	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(const InternaStdStringType& other)
		: mString(other)
	{

	}
	

	template<typename CharType>
	FStringBase<CharType>::FStringBase(const FStringBase<CharType>& other, SizeType pos)
		: mString(other.mString, pos, NPos)
	{

	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(const FStringBase<CharType>& other, SizeType pos, SizeType count)
		: mString(other.mString, pos, count)
	{

	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(const CharType* charArray, SizeType count)
		: mString(charArray, count)
	{
		//assert(s); 
	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(const CharType* s)
		: mString(s)
	{
		//assert(s); 
	}

	template<typename CharType>
	template<typename InputIterator>
	FStringBase<CharType>::FStringBase(InputIterator first, InputIterator last)
		: mString(first, last)
	{

	}


	template<typename CharType>
	FStringBase<CharType>::FStringBase(const FStringBase<CharType>& other)
		: mString(other.mString)
	{

	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(FStringBase<CharType>&& other)
		: mString(std::move(other.mString))
	{

	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(InternaStdStringType&& other)
		: mString(std::move(other))
	{

	}

	template<typename CharType>
	FStringBase<CharType>::~FStringBase()
	{

	}

	//Functions
	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Size() const
	{
		return mString.size();
	}
	
	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Length() const
	{
		return mString.length();
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::MaxSize() const
	{
		return mString.max_size();
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Capacity() const
	{
		return mString.capacity();
	}

	template<typename CharType>
	void FStringBase<CharType>::Reserve(SizeType newCapacity = 0)
	{
		mString.reserve(newCapacity);
	}

	template<typename CharType>
	bool FStringBase<CharType>::Empty() const
	{
		return mString.empty();
	}

	template<typename CharType>
	void FStringBase<CharType>::ShrinkToFit()
	{
		mString.shrink_to_fit();
	}

	template<typename CharType>
	void FStringBase<CharType>::Clear()
	{
		mString.clear();
	}

	template<typename CharType>
	void FStringBase<CharType>::Resize(SizeType count)
	{
		mString.resize(count);
	}

	template<typename CharType>
	void FStringBase<CharType>::Resize(SizeType count, CharType ch)
	{
		mString.resize(count, ch);
	}

	template<typename CharType>
	FStringBase<CharType>&  FStringBase<CharType>::Assign(SizeType count, CharType ch)
	{
		mString.append(count, ch);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Assign(const FStringBase& other)
	{
		mString.append(other.mString);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Assign(const FStringBase& other, SizeType pos)
	{
		mString.append(other.mString, pos, NPos);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Assign(const FStringBase& other, SizeType pos, SizeType count)
	{
		mString.append(other.mString, pos, count);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Assign(FStringBase&& other)
	{
		mString.append(std::move(other.mString));
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Assign(const CharType* charArray, SizeType count)
	{
		mString.append(charArray, count);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Assign(const CharType* charArray)
	{
		mString.append(charArray);
		return *this;
	}

	template<typename CharType>
	template<typename InputIterator >
	FStringBase<CharType>& FStringBase<CharType>::Assign(InputIterator first, InputIterator last)
	{
		mString.append(first, last);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Insert(SizeType index, SizeType count, CharType ch)
	{
		mString.insert(index, count, ch);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Insert(SizeType index, const CharType* charArray)
	{
		mString.insert(index, charArray, NPos);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Insert(SizeType index, const CharType* charArray, SizeType count)
	{
		mString.insert(index, charArray, count);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Insert(SizeType index, const FStringBase<CharType>& other)
	{
		mString.insert(index, other.mString);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Insert(SizeType index, const FStringBase<CharType>& other, SizeType otherSubStrIndex)
	{
		mString.insert(index, other.mString, otherSubStrIndex, NPos);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Insert(SizeType index, const FStringBase<CharType>& other, SizeType otherSubStrIndex, SizeType otherSubStrCount)
	{
		mString.insert(index, other.mString, otherSubStrIndex, otherSubStrCount);
		return *this;
	}

	template<typename CharType>
	typename FStringBase<CharType>::Iterator FStringBase<CharType>::Insert(ConstIterator pos, CharType ch)
	{
		return mString.insert(pos, ch);
	}

	template<typename CharType>
	typename FStringBase<CharType>::Iterator FStringBase<CharType>::Insert(ConstIterator pos, SizeType count, CharType ch)
	{
		return mString.insert(pos, count, ch);
	}

	template<typename CharType>
	template< typename InputIterator >
	typename FStringBase<CharType>::Iterator FStringBase<CharType>::Insert(ConstIterator pos, InputIterator first, InputIterator last)
	{
		return mString.insert(pos, first, last);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Erase(SizeType index = 0)
	{
		mString.erase(index, NPos);
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Erase(SizeType index, SizeType count)
	{
		mString.erase(index, count);
		return *this;
	}

	template<typename CharType>
	typename FStringBase<CharType>::Iterator FStringBase<CharType>::Erase(ConstIterator position)
	{
		return mString.erase(position);
	}

	template<typename CharType>
	typename FStringBase<CharType>::Iterator FStringBase<CharType>::Erase(ConstIterator first, ConstIterator last)
	{
		return mString.erase(first, last);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::EraseAll()
	{
		mString.erase(0, NPos);
		return *this;
	}

	

	template<typename CharType>
	void FStringBase<CharType>::PushBack(CharType ch)
	{
		mString.push_back(ch);
	}

	template<typename CharType>
	void FStringBase<CharType>::PopBack()
	{
		mString.pop_back();
	}

	template<typename CharType>
	int FStringBase<CharType>::Compare(const FStringBase<CharType>& other) const
	{
		return mString.compare(other.mString);
	}

	template<typename CharType>
	int FStringBase<CharType>::Compare(SizeType pos1, SizeType count1, const FStringBase<CharType>& other) const
	{
		return mString.compare(pos1, count1, other.mString);
	}

	template<typename CharType>
	int FStringBase<CharType>::Compare(SizeType pos1, SizeType count1, const FStringBase<CharType>& other, SizeType pos2) const
	{
		return mString.compare(pos1, count1, other.mString, pos2, NPos);
	}

	template<typename CharType>
	int FStringBase<CharType>::Compare(SizeType pos1, SizeType count1, const FStringBase<CharType>& other, SizeType pos2, SizeType count2) const
	{
		return mString.compare(pos1, count1, other.mString, pos2, count2);
	}

	template<typename CharType>
	int FStringBase<CharType>::Compare(const CharType* charArray) const
	{
		return mString.compare(charArray);
	}

	template<typename CharType>
	int FStringBase<CharType>::Compare(SizeType pos1, SizeType count1, const CharType* charArray) const
	{
		return mString.compare(pos1, count1, charArray);
	}

	template<typename CharType>
	int FStringBase<CharType>::Compare(SizeType pos1, SizeType count1, const CharType* charArray, SizeType count2) const
	{
		return mString.compare(pos1, count1, charArray, count2);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(SizeType pos, SizeType count, const FStringBase& str)
	{
		return mString.replace(pos, count, str.mString);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, const FStringBase& str)
	{
		return mString.replace(first, last, str.mString);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(SizeType pos, SizeType count, const FStringBase& str, SizeType pos2)
	{
		return mString.replace(pos, count, str.mString, pos2, NPos);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(SizeType pos, SizeType count, const FStringBase& str, SizeType pos2, SizeType count2)
	{
		return mString.replace(pos, count, str.mString, pos2, count2);
	}

	template<typename CharType>
	template<typename InputIterator >
	FStringBase<CharType>& FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, InputIterator first2, InputIterator last2)
	{
		return mString.replace(first, last, first2, last2);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(SizeType pos, SizeType count, const CharType* cstr)
	{
		return mString.replace(pos, count, cstr);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(SizeType pos, SizeType count, const CharType* cstr, SizeType count2)
	{
		return mString.replace(pos, count, cstr, count2);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, const CharType* cstr)
	{
		return mString.replace(first, last, cstr);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, const CharType* cstr, SizeType count2)
	{
		return mString.replace(first, last, cstr, count2);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(SizeType pos, SizeType count, SizeType count2, CharType ch)
	{
		return mString.replace(pos, count, count2, ch);
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, SizeType count2, CharType ch)
	{
		return mString.replace(first, last, count2, ch);
	}

	template<typename CharType>
	FStringBase<CharType> FStringBase<CharType>::SubString(SizeType pos) const
	{
		return FStringBase<CharType>(*this, pos, NPos);
	}

	template<typename CharType>
	FStringBase<CharType> FStringBase<CharType>::SubString(SizeType pos, SizeType count) const
	{
		return FStringBase<CharType>(*this, pos, count);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Copy(CharType* dest, SizeType count) const
	{
		return mString.copy(dest, count, 0);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Copy(CharType* dest, SizeType count, SizeType pos) const
	{
		return mString.copy(dest, count, pos);
	}

	template<typename CharType>
	void FStringBase<CharType>::Swap(FStringBase<CharType>& str)
	{
		mString.swap(str.mString);
	}

	template<typename CharType>
	void Swap(FStringBase<CharType>& lhs, FStringBase<CharType>& rhs)
	{
		lhs.mString.swap(rhs.mString);
	}

	template<typename CharType>
	void FStringBase<CharType>::Swap(InternaStdStringType& str)
	{
		mString.swap(str);
	}

	template<typename CharType>
	void Swap(FStringBase<CharType>& lhs, typename FStringBase<CharType>::InternaStdStringType& rhs)
	{
		lhs.mString.swap(rhs);
	}

	template<typename CharType>
	void Swap(typename FStringBase<CharType>::InternaStdStringType& lhs, FStringBase<CharType>& rhs)
	{
		lhs.swap(rhs.mString);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Find(const FStringBase& str) const
	{
		return mString.find(str.mString, 0);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Find(const FStringBase& str, SizeType pos) const
	{
		return mString.find(str.mString, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Find(const CharType* s) const
	{
		return mString.find(s, 0);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Find(const CharType* s, SizeType pos) const
	{
		return mString.find(s, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Find(const CharType* s, SizeType pos, SizeType count) const
	{
		return mString.find(s, pos, count);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Find(CharType ch) const
	{
		return mString.find(ch, 0);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::Find(CharType ch, SizeType pos) const
	{
		return mString.find(ch, pos);
	}


	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::ReverseFind(const FStringBase& str) const
	{
		return mString.rfind(str.mString, NPos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::ReverseFind(const FStringBase& str, SizeType pos) const
	{
		return mString.rfind(str.mString, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::ReverseFind(const CharType* s) const
	{
		return mString.rfind(s, NPos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::ReverseFind(const CharType* s, SizeType pos) const
	{
		return mString.rfind(s, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::ReverseFind(const CharType* s, SizeType pos, SizeType count) const
	{
		return mString.rfind(s, pos, count);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::ReverseFind(CharType ch) const
	{
		return mString.rfind(s, NPos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::ReverseFind(CharType ch, SizeType pos) const
	{
		return mString.rfind(s, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstOf(const FStringBase& str) const
	{
		return mString.find_first_of(str.mString, 0);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstOf(const FStringBase& str, SizeType pos) const
	{
		return mString.find_first_of(str.mString, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstOf(const CharType* s) const
	{
		return mString.find_first_of(s, 0);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstOf(const CharType* s, SizeType pos) const
	{
		return mString.find_first_of(s, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstOf(const CharType* s, SizeType pos, SizeType count) const
	{
		return mString.find_first_of(s, pos, count);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstOf(CharType ch) const
	{
		return mString.find_first_of(ch);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstOf(CharType ch, SizeType pos) const
	{
		return mString.find_first_of(ch, pos);
	}


	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstNotOf(const FStringBase& str) const
	{
		return mString.find_first_not_of(str.mString, 0);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstNotOf(const FStringBase& str, SizeType pos) const
	{
		return mString.find_first_not_of(str.mString, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstNotOf(const CharType* s) const
	{
		return mString.find_first_not_of(s, 0);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstNotOf(const CharType* s, SizeType pos) const
	{
		return mString.find_first_not_of(s, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstNotOf(const CharType* s, SizeType pos, SizeType count) const
	{
		return mString.find_first_not_of(s, pos, count);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstNotOf(CharType ch) const
	{
		return mString.find_first_not_of(ch, 0);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindFirstNotOf(CharType ch, SizeType pos) const
	{
		return mString.find_first_not_of(ch, pos);
	}


	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastOf(const FStringBase& str) const
	{
		return mString.find_last_of(str.mString, NPos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastOf(const FStringBase& str, SizeType pos) const
	{
		return mString.find_last_of(str.mString, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastOf(const CharType* s) const
	{
		return mString.find_last_of(s, NPos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastOf(const CharType* s, SizeType pos) const
	{
		return mString.find_last_of(s, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastOf(const CharType* s, SizeType pos, SizeType count) const
	{
		return mString.find_last_of(s, pos, count);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastOf(CharType ch) const
	{
		return mString.find_last_of(ch, NPos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastOf(CharType ch, SizeType pos) const
	{
		return mString.find_last_of(ch, pos);
	}


	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastNotOf(const FStringBase& str) const
	{
		return mString.find_last_not_of(str.mString, NPos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastNotOf(const FStringBase& str, SizeType pos) const
	{
		return mString.find_last_not_of(str.mString, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastNotOf(const CharType* s) const
	{
		return mString.find_last_not_of(s, NPos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastNotOf(const CharType* s, SizeType pos) const
	{
		return mString.find_last_not_of(s, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastNotOf(const CharType* s, SizeType pos, SizeType count) const
	{
		return mString.find_last_not_of(s, pos, count);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastNotOf(CharType ch) const
	{
		return mString.find_last_not_of(ch, NPos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::SizeType FStringBase<CharType>::FindLastNotOf(CharType ch, SizeType pos) const
	{
		return mString.find_last_not_of(ch, pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::Reference FStringBase<CharType>::At(SizeType pos)
	{
		return mString.at(pos);
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstReference FStringBase<CharType>::At(SizeType pos) const
	{
		return mString.at(pos);
	}

	template<typename CharType>
	CharType& FStringBase<CharType>::Front()
	{
		return mString.front();
	}

	template<typename CharType>
	const CharType& FStringBase<CharType>::Front() const
	{
		return mString.front();
	}

	template<typename CharType>
	CharType& FStringBase<CharType>::Back()
	{
		return mString.back();
	}

	template<typename CharType>
	const CharType* FStringBase<CharType>::Data() const
	{
		return mString.data();
	}


	template<typename CharType>
	const CharType* FStringBase<CharType>::AsCharArray() const
	{
		return mString.c_str();
	}

	template<typename CharType>
	const typename FStringBase<CharType>::InternaStdStringType& FStringBase<CharType>::AsStdString() const
	{
		return mString;
	}

	template<typename CharType>
	const CharType& FStringBase<CharType>::Back() const
	{
		return mString.back();
	}

	//Iterators
	template<typename CharType>
	typename FStringBase<CharType>::Iterator FStringBase<CharType>::BeginItr()
	{
		return mString.begin();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstIterator FStringBase<CharType>::BeginItr() const
	{
		return mString.begin();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstIterator FStringBase<CharType>::ConstBeginItr() const
	{
		return const_cast<const FStringBase<CharType>&>(mString).begin();
	}

	template<typename CharType>
	typename FStringBase<CharType>::Iterator FStringBase<CharType>::EndItr()
	{
		return mString.end();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstIterator FStringBase<CharType>::EndItr() const
	{
		return mString.end();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstIterator FStringBase<CharType>::ConstBEndItr() const
	{
		return const_cast<const FStringBase<CharType>&>(mString).end();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ReverseIterator	FStringBase<CharType>::ReverseBeginItr()
	{
		return mString.rbegin();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstReverseIterator FStringBase<CharType>::ReverseBeginItr() const
	{
		return mString.rbegin();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstIterator FStringBase<CharType>::ConstReverseBeginItr() const
	{
		return const_cast<const FStringBase<CharType>&>(mString).rbegin();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ReverseIterator FStringBase<CharType>::ReverseEndItr()
	{
		return mString.rend();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstReverseIterator FStringBase<CharType>::ReverseEndItr() const
	{
		return mString.rend();
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstIterator FStringBase<CharType>::ConstReverseEndItr() const
	{
		return const_cast<const FStringBase<CharType>&>(mString).rend();
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::operator=(FStringBase<CharType>&& str)
	{
		mString = std::move(str.mString); 
		return *this;
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::operator=(const FStringBase<CharType>& str)
	{ 
		mString = str.mString; 
		return *this; 
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::operator=(const CharType* s)
	{ 
		//assert(s); 
		mString = s; 
		return *this; 
	}
	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::operator=(CharType c)
	{ 
		mString = c; 
		return *this; 
	}

	template<typename CharType>
	typename FStringBase<CharType>::Reference FStringBase<CharType>::operator[](SizeType pos)
	{ 
		return mString[pos]; 
	}

	template<typename CharType>
	typename FStringBase<CharType>::ConstReference FStringBase<CharType>::operator[](SizeType pos) const
	{ 
		return mString[pos]; 
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::operator+=(const FStringBase<CharType>& str)
	{ 
		mString += (str.mString); 
		return *this; 
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::operator+=(const CharType* s)
	{ 
		//assert(s); 
		mString += (s); 
		return *this; 
	}

	template<typename CharType>
	FStringBase<CharType>& FStringBase<CharType>::operator+=(CharType c)
	{ 
		mString += (c); 
		return *this; 
	}

	template<typename CharType>
	FStringBase<CharType> operator+(const FStringBase<CharType>& lhs, const FStringBase<CharType>& rhs)
	{
		FString newVal;
		newVal.Reserve(lhs.Size() + rhs.Size());
		newVal += lhs;
		newVal += rhs;
		return newVal;
	}

	template<typename CharType>
	FStringBase<CharType> operator+(const CharType* lhs, const FStringBase<CharType>& rhs)
	{
		//assert(lhs);
		FStringBase<CharType> newVal;
		FStringBase<CharType>::SizeType lhsLen = std::char_traits<CharType>::length(lhs);
		newVal.Reserve(lhsLen + rhs.Size());
		newVal += lhs;
		newVal += rhs;
		return newVal;
	}

	template<typename CharType>
	FStringBase<CharType> operator+(CharType lhs, const FStringBase<CharType>& rhs)
	{
		FStringBase<CharType> newVal;
		newVal.Reserve(1 + rhs.Size());
		newVal += lhs;
		newVal += rhs;
		return newVal;
	}

	template<typename CharType>
	FStringBase<CharType> operator+(const FStringBase<CharType>& lhs, const CharType* rhs)
	{
		FStringBase<CharType> newVal;
		FStringBase<CharType>::SizeType rhsLen = std::char_traits<char>::length(rhs);
		newVal.Reserve(lhs.Size() + rhsLen);
		newVal += lhs;
		newVal += rhs;
		return newVal;
	}

	template<typename CharType>
	FStringBase<CharType> operator+(const FStringBase<CharType>& lhs, CharType rhs)
	{
		FStringBase<CharType> newVal;
		newVal.Reserve(lhs.Size() + 1);
		newVal += lhs;
		newVal += rhs;
		return newVal;
	}

	template<typename CharType>
	FStringBase<CharType> operator+(FStringBase<CharType>&& lhs, const FStringBase<CharType>& rhs)
	{
		return std::move(lhs.Append(rhs));
	}

	template<typename CharType>
	FStringBase<CharType> operator+(const FStringBase<CharType>& lhs, FStringBase<CharType>&& rhs)
	{
		return std::move(rhs.Append(lhs));
	}

	template<typename CharType>
	FStringBase<CharType> operator+(FStringBase<CharType>&& lhs, FStringBase<CharType>&& rhs)
	{
		if (rhs.Size() <= lhs.Capacity() - lhs.Size()
			|| rhs.Capacity() - rhs.Size() < lhs.Size())
		{
			return (std::move(lhs.Append(rhs)));
		}
		else
		{
			return (std::move(rhs.Insert(0, lhs)));
		}
	}

	template<typename CharType>
	FStringBase<CharType> operator+(const CharType* lhs, FStringBase<CharType>&& rhs)
	{
		//assert(lhs);
		return std::move(rhs.Insert(0, lhs));
	}

	template<typename CharType>
	FStringBase<CharType> operator+(CharType lhs, FStringBase<CharType>&& rhs)
	{
		return std::move(rhs.Insert(
			static_cast<FStringBase<CharType>::SizeType>(0),
			static_cast<FStringBase<CharType>::SizeType>(1),
			lhs));
	}

	template<typename CharType>
	FStringBase<CharType>operator+(FStringBase<CharType>&& lhs, const CharType* rhs)
	{
		//assert(rhs);
		return std::move(lhs.Insert(0, rhs));
	}

	template<typename CharType>
	FStringBase<CharType> operator+(FStringBase<CharType>&& lhs, CharType rhs)
	{
		return std::move(lhs.Insert(
			static_cast<FStringBase<CharType>::SizeType>(0),
			static_cast<FStringBase<CharType>::SizeType>(1),
			rhs));
	}

	template<typename CharType>
	bool operator==(const FStringBase<CharType>& lhs, const FStringBase<CharType>& rhs) 
	{ 
		return lhs.mString == rhs.mString; 
	}

	template<typename CharType>
	bool operator==(const CharType* lhs, const FStringBase<CharType>& rhs)
	{
		////assert(lhs);
		return lhs == rhs.mString;
	}

	template<typename CharType>
	bool operator==(const FStringBase<CharType>& lhs, const CharType* rhs)
	{
		////assert(rhs);
		return lhs.mString == rhs;
	}

	template<typename CharType>
	bool operator<(const FStringBase<CharType>& lhs, const FStringBase<CharType>& rhs) 
	{ 
		return lhs.mString < rhs.mString; 
	}

	template<typename CharType>
	bool operator<(const FStringBase<CharType>& lhs, const CharType* rhs)
	{
		////assert(rhs);
		return lhs.mString < rhs;
	}

	template<typename CharType>
	bool operator<(const CharType* lhs, const FStringBase<CharType>& rhs)
	{
		////assert(lhs);
		return lhs < rhs.mString;
	}

	template<typename CharType>
	bool operator!=(const FStringBase<CharType>& lhs, const FStringBase<CharType>& rhs) 
	{ 
		return !(lhs == rhs); 
	}

	template<typename CharType>
	bool operator!=(const CharType* lhs, const FStringBase<CharType>& rhs)
	{ 
		return !(lhs == rhs); 
	}

	template<typename CharType>
	bool operator!=(const FStringBase<CharType>& lhs, const CharType* rhs)
	{ 
		return !(lhs == rhs); 
	}

	template<typename CharType>
	bool operator>(const FStringBase<CharType>& lhs, const FStringBase<CharType>& rhs) 
	{ 
		return rhs < lhs; 
	}

	template<typename CharType>
	bool operator>(const FStringBase<CharType>& lhs, const CharType* rhs)
	{ 
		return rhs < lhs; 
	}

	template<typename CharType>
	bool operator>(const CharType* lhs, const FStringBase<CharType>& rhs)
	{ 
		return rhs < lhs; 
	}

	template<typename CharType>
	bool operator<=(const FStringBase<CharType>& lhs, const FStringBase<CharType>& rhs) 
	{ 
		return !(rhs < lhs); 
	}

	template<typename CharType>
	bool operator<=(const FStringBase<CharType>& lhs, const CharType* rhs)
	{ 
		return !(rhs < lhs); 
	}

	template<typename CharType>
	bool operator<=(const CharType* lhs, const FStringBase<CharType>& rhs)
	{ 
		return !(rhs < lhs); 
	}

	template<typename CharType>
	bool operator>=(const FStringBase<CharType>& lhs, const FStringBase<CharType>& rhs) 
	{ 
		return !(lhs < rhs); 
	}

	template<typename CharType>
	bool operator>=(const FStringBase<CharType>& lhs, const CharType* rhs)
	{ 
		return !(lhs < rhs); 
	}

	template<typename CharType>
	bool operator>=(const CharType* lhs, const FStringBase<CharType>& rhs)
	{ 
		return !(lhs < rhs); 
	}

	template<typename CharType>
	typename FStringBase<CharType>::FStringOStream& operator<<(typename FStringBase<CharType>::FStringOStream& oStream, const FStringBase<CharType>& str)
	{
		return oStream << str.mString;
	}

	template<typename CharType>
	typename FStringBase<CharType>::FStringIStream& operator>>(typename FStringBase<CharType>::FStringIStream& iStream, FStringBase<CharType>& str)
	{
		iStream >> str.mString;
		return iStream;
	}
}
