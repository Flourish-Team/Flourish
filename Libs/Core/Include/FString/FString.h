#pragma once

#include <iterator>
#include <string>
#include <istream>
#include <ostream>
#include <stdarg.h>  

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
		FStringBase(CharType charToRepeat, SizeType numTimesToRepeat);
		FStringBase(const InternaStdStringType& stdString);
		FStringBase(const FStringBase& other, SizeType substringStartPos, SizeType charCount = NPos);
		FStringBase(const CharType* charArray, SizeType count);
		FStringBase(const CharType* cString);
		template<typename InputIterator>
		FStringBase(InputIterator first, InputIterator last);
		FStringBase(const FStringBase& other);
		FStringBase(FStringBase&& other);
		FStringBase(InternaStdStringType&& stdString);
		~FStringBase();

		const CharType* AsCString() const;
		const InternaStdStringType& AsStdString() const;
		const CharType* Data() const;

		//CB TODO: Implement this once allocators are in
		//FAllocator GetAllocator()

		SizeType Size() const;

		SizeType Length() const;

		SizeType MaxSize() const;

		void Resize(SizeType newSize, CharType charToFillNewSpaceWith);
		void Resize(SizeType newSize);

		SizeType Capacity() const;

		void Reserve(SizeType newSize);

		void Clear();

		bool IsEmpty() const;

		void ShrinkToFit();

		void Set(CharType charToAppend, SizeType numTimesToAppend = 1);
		void Set(const FStringBase& other, SizeType substringStartPos = 0, SizeType charCount = NPos);
		void Set(FStringBase&& string);
		void Set(InternaStdStringType&& stdString);
		void Set(const CharType* charArray, SizeType count);
		void Set(const CharType* cString);
		template< typename InputIterator >
		void Set(InputIterator first, InputIterator last);

		int SetFormat(const CharType* formatCString, ...);

		void Append(CharType charToAppend, SizeType numTimesToAppend = 1);
		void Append(const FStringBase& other, SizeType substringStartPos = 0, SizeType charCount = NPos);
		void Append(FStringBase&& string);
		void Append(InternaStdStringType&& stdString);
		void Append(const CharType* charArray, SizeType count);
		void Append(const CharType* cString);
		template< typename InputIterator >
		void Append(InputIterator first, InputIterator last);

		int AppendFormat(const CharType* formatCString, ...);

		void Insert(SizeType index, CharType charToInsert, int numTimesToInsert = 1);
		void Insert(SizeType index, const CharType* charArray, SizeType count);
		void Insert(SizeType index, const CharType* cString);
		void Insert(SizeType index, const FStringBase& stringToInsert, SizeType otherSubStrIndex = 0, SizeType otherSubStrCount = NPos);
		void Insert(SizeType index, const InternaStdStringType& stdStringToInsert, SizeType otherSubStrIndex = 0, SizeType otherSubStrCount = NPos);
		void Insert(ConstIterator pos, CharType charToInsert, SizeType numTimesToInsert = 1);
		template< typename InputIterator >
		void Insert(ConstIterator pos, InputIterator first, InputIterator last);

		void Erase(SizeType index = 0, SizeType count = NPos);
		void Erase(ConstIterator position);
		void Erase(ConstIterator first, ConstIterator last);

		void EraseAll();

		void Replace(SizeType pos, SizeType count, const FStringBase& str, SizeType pos2 = 0, SizeType count2 = NPos);
		void Replace(SizeType pos, SizeType count, const CharType* cString);
		void Replace(SizeType pos, SizeType count, const CharType* charArray, SizeType charArraySize);
		void Replace(SizeType pos, SizeType count, CharType charToReplace, SizeType numCharsToInsert);
		void Replace(ConstIterator first, ConstIterator last, const FStringBase& str);
		void Replace(ConstIterator first, ConstIterator last, const CharType* cString);
		void Replace(ConstIterator first, ConstIterator last, const CharType* charArray, SizeType charArraySize);
		void Replace(ConstIterator first, ConstIterator last, CharType charToReplace, SizeType numCharsToInsert);
		template< typename InputIterator >
		void Replace(ConstIterator first, ConstIterator last, InputIterator first2, InputIterator last2);

		FStringBase SubString(SizeType startPos, SizeType count = NPos) const;

		void Swap(FStringBase& str);
		void Swap(InternaStdStringType& str);

		friend void Swap(FStringBase& lhs, FStringBase& rhs);
		friend void Swap(FStringBase& lhs, InternaStdStringType& rhs);
		friend void Swap(InternaStdStringType& lhs, FStringBase& rhs);

		int Compare(const FStringBase& other) const;
		int Compare(SizeType pos1, SizeType count1, const FStringBase& other) const;
		int Compare(SizeType pos1, SizeType count1, const FStringBase& other, SizeType pos2) const;
		int Compare(SizeType pos1, SizeType count1, const FStringBase& other, SizeType pos2, SizeType count2) const;
		int Compare(const CharType* cString) const;
		int Compare(SizeType pos1, SizeType count1, const CharType* cString) const;
		int Compare(SizeType pos1, SizeType count1, const CharType* charArray, SizeType count2) const;

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

		CharType& Front();
		const CharType& Front() const;

		CharType& Back();
		const CharType& Back() const;

		//Iterators
		Iterator BeginItr();
		ConstIterator BeginItr() const;
		ConstIterator ConstBeginItr() const;

		Iterator EndItr();
		ConstIterator EndItr() const;
		ConstIterator ConstEndItr() const;

		ReverseIterator	ReverseBeginItr();
		ConstReverseIterator ReverseBeginItr() const;
		ConstReverseIterator ConstReverseBeginItr() const;

		ReverseIterator ReverseEndItr();
		ConstReverseIterator ReverseEndItr() const;
		ConstReverseIterator ConstReverseEndItr() const;

		//Operators
		FStringBase& operator=(FStringBase&& str);
		FStringBase& operator=(const FStringBase& str);
		FStringBase& operator=(const CharType* s);
		FStringBase& operator=(CharType c);

		Reference operator[] (SizeType pos);
		ConstReference operator[] (SizeType pos) const;

		Reference At(SizeType pos);
		ConstReference At(SizeType pos) const;


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

	//Wide string type (Assumed UTF-16)
	using FWideString = FStringBase<wchar_t>;
}

//Include inline impimentation
#define INCLUDING_FSTRING_IMP_FILE
#include "Internal/FString.inl"
#undef INCLUDING_FSTRING_IMP_FILE
