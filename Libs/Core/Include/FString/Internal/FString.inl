#ifndef INCLUDING_FSTRING_IMP_FILE
#error Trying to include FString.inl separately. Include FString.h instead
#else

namespace Flourish
{
	template<typename CharType>
	FStringBase<CharType>::FStringBase()
	{
		
	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(CharType charToRepeat, SizeType numTimesToRepeat)
		: mString(numTimesToRepeat, charToRepeat)
	{

	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(const InternaStdStringType& stdString)
		: mString(stdString)
	{

	}
	

	template<typename CharType>
	FStringBase<CharType>::FStringBase(const FStringBase<CharType>& other, SizeType substringStartPos, SizeType substringCharCount = NPos)
		: mString(other.mString, substringStartPos, substringCharCount)
	{

	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(const CharType* charArray, SizeType count)
		: mString(charArray, count)
	{
		//assert(s); 
	}

	template<typename CharType>
	FStringBase<CharType>::FStringBase(const CharType* cString)
		: mString(cString)
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
	FStringBase<CharType>::FStringBase(InternaStdStringType&& stdString)
		: mString(std::move(stdString))
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
	void FStringBase<CharType>::Reserve(SizeType newSize)
	{
		mString.reserve(newSize);
	}

	template<typename CharType>
	bool FStringBase<CharType>::IsEmpty() const
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
	void FStringBase<CharType>::Resize(SizeType newSize)
	{
		mString.resize(newSize);
	}

	template<typename CharType>
	void FStringBase<CharType>::Resize(SizeType newSize, CharType charToFillNewSpaceWith)
	{
		mString.resize(newSize, ch);
	}

	template<typename CharType>
	void FStringBase<CharType>::Set(CharType charToAppend, SizeType numTimesToAppend = 1)
	{
		mString.assign(numTimesToAppend, charToAppend);
	}

	template<typename CharType>
	void FStringBase<CharType>::Set(const FStringBase& other, SizeType substringStartPos = 0, SizeType charCount = NPos)
	{
		mString.assign(other.mString, substringStartPos, charCount);
	}

	template<typename CharType>
	void FStringBase<CharType>::Set(FStringBase&& string)
	{
		mString.assign(std::move(string.mString));
	}

	template<typename CharType>
	void FStringBase<CharType>::Set(InternaStdStringType&& stdString)
	{
		mString.assign(std::move(stdString));
	}

	template<typename CharType>
	void FStringBase<CharType>::Set(const CharType* charArray, SizeType arraySize)
	{
		mString.assign(charArray, arraySize);
	}

	template<typename CharType>
	void FStringBase<CharType>::Set(const CharType* cString)
	{
		mString.assign(cString);
	}

	template<typename CharType>
	template<typename InputIterator >
	void FStringBase<CharType>::Set(InputIterator first, InputIterator last)
	{
		mString.assign(first, last);
	}

	template<typename CharType>
	int FStringBase<CharType>::SetFormat(const CharType* formatCString, ...)
	{
		va_list args;

		va_start(args, formatCString);
		int length = vsnprintf(Data(), Capacity(), formatCString, args);
		va_end(args);

		//assert(addLength >= 0)

		// +1 for null terminator
		if (Capacity < (length + 1))
		{
			//resize and try again
			Resize(length + 1);
			va_start(args, formatCString);
			length = vsnprintf(Data(), length + 1, formatCString, args);
			va_end(args);
		}

		return length;
	}

	template<typename CharType>
	void FStringBase<CharType>::Append(CharType charToAppend, SizeType numTimesToAppend = 1)
	{
		mString.append(numTimesToAppend, charToAppend);
	}

	template<typename CharType>
	void FStringBase<CharType>::Append(const FStringBase& other, SizeType substringStartPos = 0, SizeType charCount = NPos)
	{
		mString.append(other.mString, substringStartPos, charCount);
	}

	template<typename CharType>
	void FStringBase<CharType>::Append(FStringBase&& string)
	{
		mString.append(std::move(string.mString));
	}

	template<typename CharType>
	void FStringBase<CharType>::Append(InternaStdStringType&& stdString)
	{
		mString.append(std::move(stdString));
	}

	template<typename CharType>
	void FStringBase<CharType>::Append(const CharType* charArray, SizeType arraySize)
	{
		mString.append(charArray, arraySize);
	}

	template<typename CharType>
	void FStringBase<CharType>::Append(const CharType* cString)
	{
		mString.append(cString);
	}

	template<typename CharType>
	template<typename InputIterator >
	void FStringBase<CharType>::Append(InputIterator first, InputIterator last)
	{
		mString.append(first, last);
	}

	template<typename CharType>
	int FStringBase<CharType>::AppendFormat(const CharType* formatCString, ...)
	{
		va_list args;

		int currentLength = Length();

		va_start(args, formatCString);
		int addLength = vsnprintf(Data() + currentLength, Capacity() - currentLength, formatCString, args);
		va_end(args);

		//assert(addLength >= 0)

		// +1 for null terminator
		if (Capacity < currentLength + addLength + 1)
		{
			//resize and try again
			Resize(currentLength + addLength + 1);
			va_start(args, formatCString);
			addLength = vsnprintf(Data() + currentLength, (addLength + 1), formatCString, args);
			va_end(args);
		}

		return addLength;
	}
	
	template<typename CharType>
	void FStringBase<CharType>::Insert(SizeType index, CharType charToInsert, int numTimesToInsert = 1)
	{
		mString.insert(index, numTimesToInsert, charToInsert);
		return *this;
	}

	template<typename CharType>
	void FStringBase<CharType>::Insert(SizeType index, const CharType* cString)
	{
		mString.insert(index, cString);
		return *this;
	}

	template<typename CharType>
	void FStringBase<CharType>::Insert(SizeType index, const CharType* charArray, SizeType count)
	{
		mString.insert(index, charArray, count);
		return *this;
	}

	template<typename CharType>
	void FStringBase<CharType>::Insert(SizeType index, const FStringBase<CharType>& other, SizeType otherSubStrIndex = 0, SizeType otherSubStrCount = NPos)
	{
		mString.insert(index, other.mString, otherSubStrIndex, otherSubStrCount);
		return *this;
	}

	template<typename CharType>
	void FStringBase<CharType>::Insert(SizeType index, const InternaStdStringType& stdStringToInsert, SizeType otherSubStrIndex = 0, SizeType otherSubStrCount = NPos)
	{
		mString.insert(index, stdStringToInsert, otherSubStrIndex, otherSubStrCount);
		return *this;
	}

	template<typename CharType>
	void FStringBase<CharType>::Insert(ConstIterator pos, CharType charToInsert, SizeType numTimesToInsert = 1)
	{
		//assert(numTimesToInsert >= 1);
		return mString.insert(pos, numTimesToInsert, charToInsert);
	}

	template<typename CharType>
	template< typename InputIterator >
	void FStringBase<CharType>::Insert(ConstIterator pos, InputIterator first, InputIterator last)
	{
		return mString.insert(pos, first, last);
	}


	template<typename CharType>
	void FStringBase<CharType>::Erase(SizeType index = 0, SizeType count = NPos)
	{
		mString.erase(index, count);
	}

	template<typename CharType>
	void FStringBase<CharType>::Erase(ConstIterator position)
	{
		mString.erase(position);
	}

	template<typename CharType>
	void FStringBase<CharType>::Erase(ConstIterator first, ConstIterator last)
	{
		mString.erase(first, last);
	}

	template<typename CharType>
	void FStringBase<CharType>::EraseAll()
	{
		mString.erase(0, NPos);
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
	int FStringBase<CharType>::Compare(const CharType* cString) const
	{
		return mString.compare(cString);
	}

	template<typename CharType>
	int FStringBase<CharType>::Compare(SizeType pos1, SizeType count1, const CharType* cString) const
	{
		return mString.compare(pos1, count1, cString);
	}

	template<typename CharType>
	int FStringBase<CharType>::Compare(SizeType pos1, SizeType count1, const CharType* charArray, SizeType count2) const
	{
		return mString.compare(pos1, count1, charArray, count2);
	}

	template<typename CharType>
	void FStringBase<CharType>::Replace(SizeType pos, SizeType count, const FStringBase& str, SizeType pos2 = 0, SizeType count2 = 0)
	{
		return mString.replace(pos, count, str.mString, pos2, count2);
	}

	template<typename CharType>
	void FStringBase<CharType>::Replace(SizeType pos, SizeType count, const CharType* cString)
	{
		return mString.replace(pos, count, cString);
	}

	template<typename CharType>
	void FStringBase<CharType>::Replace(SizeType pos, SizeType count, const CharType* charArray, SizeType charArraySize)
	{
		return mString.replace(pos, count, charArray, charArraySize);
	}

	template<typename CharType>
	void FStringBase<CharType>::Replace(SizeType pos, SizeType count, CharType charToReplace, SizeType numCharsToInsert)
	{
		return mString.replace(pos, count, numCharsToInsert, charToReplace);
	}

	template<typename CharType>
	void FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, const FStringBase& str)
	{
		return mString.replace(first, last, str.mString);
	}

	template<typename CharType>
	void FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, const CharType* cString)
	{
		return mString.replace(first, last, cString);
	}

	template<typename CharType>
	void FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, const CharType* charArray, SizeType charArraySize)
	{
		return mString.replace(first, last, charArray, charArraySize);
	}

	template<typename CharType>
	void FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, CharType charToReplace, SizeType numCharsToInsert)
	{
		return mString.replace(first, last, numCharsToInsert, charToReplace);
	}
	
	template<typename CharType>
	template<typename InputIterator >
	void FStringBase<CharType>::Replace(ConstIterator first, ConstIterator last, InputIterator first2, InputIterator last2)
	{
		return mString.replace(first, last, first2, last2);
	}

	template<typename CharType>
	FStringBase<CharType> FStringBase<CharType>::SubString(SizeType startPos, SizeType count = NPos) const
	{
		return FStringBase<CharType>(*this, startPos, count);
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
	const CharType* FStringBase<CharType>::AsCString() const
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
	typename FStringBase<CharType>::ConstIterator FStringBase<CharType>::ConstEndItr() const
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
	typename FStringBase<CharType>::ConstReverseIterator FStringBase<CharType>::ConstReverseBeginItr() const
	{
		return const_cast<const InternaStdStringType>(mString).rbegin();
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
	typename FStringBase<CharType>::ConstReverseIterator FStringBase<CharType>::ConstReverseEndItr() const
	{
		return const_cast<const InternaStdStringType>(mString).rend();
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

#endif