#pragma once

#include <iterator>
#include <string>
#include <istream>
#include <ostream>

namespace Flourish
{
	//A simple wrapper around std::string that allows an FAllocator to be specificed along with extra helper functions
	template<typename CharType>
	class FStringBase
	{
	public:
		//Types
		//CB TODO: replace with allocator
		using InternalCharType = CharType;
		using InternaStdStringType = std::basic_string<CharType, std::char_traits<CharType>, std::allocator<CharType> >;
		using TraitsType = typename InternaStdStringType::traits_type;
		using ValueType = typename InternaStdStringType::value_type;
		using AllocatorType = typename InternaStdStringType::allocator_type;
		using SizeType = typename InternaStdStringType::size_type;
		using Difference_type = typename InternaStdStringType::difference_type;
		using Reference = typename InternaStdStringType::reference;
		using ConstReference = typename InternaStdStringType::const_reference;
		using Pointer = typename InternaStdStringType::pointer;
		using ConstPointer = typename InternaStdStringType::const_pointer;
		using Iterator = typename InternaStdStringType::iterator;
		using ConstIterator = typename InternaStdStringType::const_iterator;
		using ReverseIterator = typename InternaStdStringType::reverse_iterator;
		using ConstReverseIterator = typename InternaStdStringType::const_reverse_iterator;
		using FStringOStream = std::basic_ostream<CharType, typename InternaStdStringType::traits_type>;
		using FStringIStream = std::basic_istream<CharType, typename InternaStdStringType::traits_type>;

		static const SizeType NPos = static_cast<SizeType>(-1);

		//Ctors/Dtors
		//CB TODO Add allocator paramater to ctors
		FStringBase();
		FStringBase(SizeType numTimesToRepeat, CharType charToRepeat);
		FStringBase(const InternaStdStringType& other);
		FStringBase(const FStringBase& other, SizeType pos);
		FStringBase(const FStringBase& other, SizeType pos, SizeType count);
		FStringBase(const CharType* charArray, SizeType count);
		FStringBase(const CharType* s);
		template<typename InputIterator>
		FStringBase(InputIterator first, InputIterator last);
		FStringBase(const FStringBase& other);
		FStringBase(FStringBase&& other);
		FStringBase(InternaStdStringType&& other);
		~FStringBase();

		//Functions
		const CharType* AsCharArray() const;

		const InternaStdStringType& AsStdString() const;

		//CB TODO: Implement this once allocators are int
		//FAllocator GetAllocator()

		//CB TODO: Perhaps we can remove some of this and make the interface simpler? 
		//Functions From Std:string (Should map directly onto function in std:string)
		SizeType Size() const;

		SizeType Length() const;

		SizeType MaxSize() const;

		void Resize(SizeType count, CharType ch);
		void Resize(SizeType n);

		SizeType Capacity() const;

		void Reserve(SizeType n);

		void Clear();

		bool Empty() const;

		void ShrinkToFit();

		FStringBase& Assign(SizeType count, CharType ch);
		FStringBase& Assign(const FStringBase& other);
		FStringBase& Assign(const FStringBase& other, SizeType pos);
		FStringBase& Assign(const FStringBase& other, SizeType pos, SizeType count);
		FStringBase& Assign(FStringBase&& other);
		FStringBase& Assign(const CharType* charArray, SizeType count);
		FStringBase& Assign(const CharType* charArray);
		template< typename  InputIterator >
		FStringBase& Assign(InputIterator first, InputIterator last);

		FStringBase& Insert(SizeType index, SizeType count, CharType ch);
		FStringBase& Insert(SizeType index, const CharType* charArray);
		FStringBase& Insert(SizeType index, const CharType* charArray, SizeType count);
		FStringBase& Insert(SizeType index, const FStringBase& other);
		FStringBase& Insert(SizeType index, const FStringBase& other, SizeType otherSubStrIndex);
		FStringBase& Insert(SizeType index, const FStringBase& other, SizeType otherSubStrIndex, SizeType otherSubStrCount);
		Iterator Insert(ConstIterator pos, CharType ch);
		Iterator Insert(ConstIterator pos, SizeType count, CharType ch);
		template< typename InputIterator >
		Iterator Insert(ConstIterator pos, InputIterator first, InputIterator last);

		FStringBase& Erase(SizeType index);
		FStringBase& Erase(SizeType index, SizeType count);
		Iterator Erase(ConstIterator position);
		Iterator Erase(ConstIterator first, ConstIterator last);

		FStringBase& EraseAll();

		void PushBack(CharType ch);

		void PopBack();

		int Compare(const FStringBase& other) const;
		int Compare(SizeType pos1, SizeType count1, const FStringBase& other) const;
		int Compare(SizeType pos1, SizeType count1, const FStringBase& other, SizeType pos2) const;
		int Compare(SizeType pos1, SizeType count1, const FStringBase& other, SizeType pos2, SizeType count2) const;
		int Compare(const CharType* charArray) const;
		int Compare(SizeType pos1, SizeType count1, const CharType* charArray) const;
		int Compare(SizeType pos1, SizeType count1, const CharType* charArray, SizeType count2) const;

		FStringBase& Replace(SizeType pos, SizeType count, const FStringBase& str);
		FStringBase& Replace(ConstIterator first, ConstIterator last, const FStringBase& str);
		FStringBase& Replace(SizeType pos, SizeType count, const FStringBase& str, SizeType pos2);
		FStringBase& Replace(SizeType pos, SizeType count, const FStringBase& str, SizeType pos2, SizeType count2);
		template< typename InputIterator >
		FStringBase& Replace(ConstIterator first, ConstIterator last, InputIterator first2, InputIterator last2);
		FStringBase& Replace(SizeType pos, SizeType count, const CharType* cstr);
		FStringBase& Replace(SizeType pos, SizeType count, const CharType* cstr, SizeType count2);
		FStringBase& Replace(ConstIterator first, ConstIterator last, const CharType* cstr);
		FStringBase& Replace(ConstIterator first, ConstIterator last, const CharType* cstr, SizeType count2);
		FStringBase& Replace(SizeType pos, SizeType count, SizeType count2, CharType ch);
		FStringBase& Replace(ConstIterator first, ConstIterator last, SizeType count2, CharType ch);

		FStringBase SubString(SizeType pos) const;
		FStringBase SubString(SizeType pos, SizeType count) const;

		SizeType Copy(CharType* dest, SizeType count) const;
		SizeType Copy(CharType* dest, SizeType count, SizeType pos) const;

		void Swap(FStringBase& str);
		friend void Swap(FStringBase& lhs, FStringBase& rhs);

		//CB: Extra swap functions to allow swapping with an equivalent std:string
		void Swap(InternaStdStringType& str);
		friend void Swap(FStringBase& lhs, InternaStdStringType& rhs);
		friend void Swap(InternaStdStringType& lhs, FStringBase& rhs);

		SizeType Find(const FStringBase& str) const;
		SizeType Find(const FStringBase& str, SizeType pos) const;
		SizeType Find(const CharType* s) const;
		SizeType Find(const CharType* s, SizeType pos) const;
		SizeType Find(const CharType* s, SizeType pos, SizeType count) const;
		SizeType Find(CharType ch) const;
		SizeType Find(CharType ch, SizeType pos) const;

		SizeType ReverseFind(const FStringBase& str) const;
		SizeType ReverseFind(const FStringBase& str, SizeType pos) const;
		SizeType ReverseFind(const CharType* s) const;
		SizeType ReverseFind(const CharType* s, SizeType pos) const;
		SizeType ReverseFind(const CharType* s, SizeType pos, SizeType count) const;
		SizeType ReverseFind(CharType ch) const;
		SizeType ReverseFind(CharType ch, SizeType pos ) const;

		SizeType FindFirstOf(const FStringBase& str) const;
		SizeType FindFirstOf(const FStringBase& str, SizeType pos) const;
		SizeType FindFirstOf(const CharType* s) const;
		SizeType FindFirstOf(const CharType* s, SizeType pos) const;
		SizeType FindFirstOf(const CharType* s, SizeType pos, SizeType count) const;
		SizeType FindFirstOf(CharType ch) const;
		SizeType FindFirstOf(CharType ch, SizeType pos) const;

		SizeType FindFirstNotOf(const FStringBase& str) const;
		SizeType FindFirstNotOf(const FStringBase& str, SizeType pos) const;
		SizeType FindFirstNotOf(const CharType* s) const;
		SizeType FindFirstNotOf(const CharType* s, SizeType pos) const;
		SizeType FindFirstNotOf(const CharType* s, SizeType pos, SizeType count) const;
		SizeType FindFirstNotOf(CharType ch) const;
		SizeType FindFirstNotOf(CharType ch, SizeType pos) const;

		SizeType FindLastOf(const FStringBase& str) const;
		SizeType FindLastOf(const FStringBase& str, SizeType pos) const;
		SizeType FindLastOf(const CharType* s) const;
		SizeType FindLastOf(const CharType* s, SizeType pos) const;
		SizeType FindLastOf(const CharType* s, SizeType pos, SizeType count) const;
		SizeType FindLastOf(CharType ch) const;
		SizeType FindLastOf(CharType ch, SizeType pos) const;

		SizeType FindLastNotOf(const FStringBase& str) const;
		SizeType FindLastNotOf(const FStringBase& str, SizeType pos) const;
		SizeType FindLastNotOf(const CharType* s) const;
		SizeType FindLastNotOf(const CharType* s, SizeType pos) const;
		SizeType FindLastNotOf(const CharType* s, SizeType pos, SizeType count) const;
		SizeType FindLastNotOf(CharType ch) const;
		SizeType FindLastNotOf(CharType ch, SizeType pos) const;

		Reference At(SizeType pos);
		ConstReference At(SizeType pos) const;

		CharType& Front();
		const CharType& Front() const;

		CharType& Back();
		const CharType& Back() const;

		const CharType* Data() const;

		//Iterators
		Iterator BeginItr();
		ConstIterator BeginItr() const;
		ConstIterator ConstBeginItr() const;

		Iterator EndItr();
		ConstIterator EndItr() const;
		ConstIterator ConstBEndItr() const;

		ReverseIterator	ReverseBeginItr();
		ConstReverseIterator ReverseBeginItr() const;
		ConstIterator ConstReverseBeginItr() const;

		ReverseIterator ReverseEndItr();
		ConstReverseIterator ReverseEndItr() const;
		ConstIterator ConstReverseEndItr() const;

		//Operators
		FStringBase& operator=(FStringBase&& str);
		FStringBase& operator=(const FStringBase& str);
		FStringBase& operator=(const CharType* s);
		FStringBase& operator=(CharType c);

		Reference operator[] (SizeType pos);
		ConstReference operator[] (SizeType pos) const;

		FStringBase& operator+= (const FStringBase& str);
		FStringBase& operator+= (const CharType* s);
		FStringBase& operator+= (CharType c);

		friend FStringBase operator+ (const FStringBase& lhs, const FStringBase& rhs);
		friend FStringBase operator+ (const CharType* lhs, const FStringBase& rhs);
		friend FStringBase operator+ (CharType lhs, const FStringBase& rhs);
		friend FStringBase operator+ (const FStringBase& lhs, const CharType* rhs);
		friend FStringBase operator+ (const FStringBase& lhs, CharType rhs);
		friend FStringBase operator+(FStringBase&& lhs,	const FStringBase& rhs);
		friend FStringBase operator+(const FStringBase& lhs, FStringBase&& rhs);
		friend FStringBase operator+(FStringBase&& lhs,	FStringBase&& rhs);
		friend FStringBase operator+(const CharType* lhs, FStringBase&& rhs);
		friend FStringBase operator+(CharType lhs, FStringBase&& rhs);
		friend FStringBase operator+(FStringBase&& lhs,	const CharType* rhs);
		friend FStringBase operator+(FStringBase&& lhs, CharType rhs);


		friend bool operator== (const FStringBase& lhs, const FStringBase& rhs);
		friend bool operator== (const CharType* lhs, const FStringBase& rhs);
		friend bool operator== (const FStringBase& lhs, const CharType* rhs);

		friend bool operator<  (const FStringBase& lhs, const FStringBase& rhs);
		friend bool operator<  (const FStringBase& lhs, const CharType* rhs);
		friend bool operator<  (const CharType* lhs, const FStringBase& rhs);

		friend bool operator!= (const FStringBase& lhs, const FStringBase& rhs);
		friend bool operator!= (const CharType* lhs, const FStringBase& rhs);
		friend bool operator!= (const FStringBase& lhs, const CharType* rhs);

		friend bool operator>  (const FStringBase& lhs, const FStringBase& rhs);
		friend bool operator>  (const FStringBase& lhs, const CharType* rhs);
		friend bool operator>  (const CharType* lhs, const FStringBase& rhs);

		friend bool operator<= (const FStringBase& lhs, const FStringBase& rhs);
		friend bool operator<= (const FStringBase& lhs, const CharType* rhs);
		friend bool operator<= (const CharType* lhs, const FStringBase& rhs);

		friend bool operator>= (const FStringBase& lhs, const FStringBase& rhs);
		friend bool operator>= (const FStringBase& lhs, const CharType* rhs);
		friend bool operator>= (const CharType* lhs, const FStringBase& rhs);

		friend FStringOStream& operator<< (FStringOStream& oStream, const FStringBase& str);

		friend FStringIStream& operator>> (FStringIStream& iStream, FStringBase& str);

	private:
		InternaStdStringType mString;
	};

	//Standard String type (Assumed UTF-8)
	using FString = FStringBase<char>;

	//Wide string type (Assuymed UTF-16)
	using FWideString = FStringBase<wchar_t>;
}

//Include inline impimentation
#include "Internal/FString.inl"
