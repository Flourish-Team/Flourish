#include "Test.h"
#include "DataStore/DataBuffer.h"

using namespace Flourish;

TEST(DataBufferTests, StartsEmpty)
{
    DataBuffer buffer(8);
    
    EXPECT_EQUAL(buffer.SpaceLeftToWrite(), 8U);
    EXPECT_EQUAL(buffer.DataAvailableToRead(), 0U);
}

TEST(DataBufferTests, DataCanBeWritten)
{
    DataBuffer buffer(32);

    const char* someData = "some data";
    buffer.Write(someData, strlen(someData));
    
    EXPECT_EQUAL(buffer.SpaceLeftToWrite(), 32U - strlen(someData));
    EXPECT_EQUAL(buffer.DataAvailableToRead(), strlen(someData));
}

TEST(DataBufferTests, DataCanBeManuallyWritten)
{
    DataBuffer buffer(32);

    const char* someData = "some data";
    memcpy(buffer.WriteData(), someData, strlen(someData));

    EXPECT_STRING_EQUAL(static_cast<const char*>(buffer.ReadData()), someData);
    EXPECT_EQUAL(buffer.SpaceLeftToWrite(), buffer.Size());

    buffer.MarkAsWritten(strlen(someData));
        EXPECT_EQUAL(buffer.SpaceLeftToWrite(), buffer.Size() - strlen(someData));

    const char* moreData = " some more data";
    memcpy(buffer.WriteData(), moreData, strlen(moreData));

        EXPECT_STRING_EQUAL(static_cast<const char*>(buffer.ReadData()), "some data some more data");
}

TEST(DataBufferTests, ClearResetsSpace)
{
    DataBuffer buffer(32);
    const char* someData = "some data";
    buffer.Write(someData, strlen(someData));

    buffer.Clear();

    EXPECT_EQUAL(buffer.SpaceLeftToWrite(), 32u);
    EXPECT_EQUAL(buffer.DataAvailableToRead(), 0u);
}

TEST(DataBufferTests, CanReadBackWhatWasWritten)
{
    DataBuffer buffer(32);
    
    const char* someData = "some data";
    buffer.Write(someData, strlen(someData));
    
    char readData[32] = { 0 };
    memcpy(readData, buffer.ReadData(), buffer.DataAvailableToRead());
    buffer.MarkAsRead(strlen(readData));
    
    EXPECT_STRING_EQUAL(someData, readData);
    EXPECT_EQUAL(buffer.DataAvailableToRead(), 0U);
}

TEST(DataBufferTests, MultipleWritesFillBuffer)
{
    DataBuffer buffer(32);
    
    const char* someData = "some data";
    buffer.Write(someData, strlen(someData));
    
    const char* someOtherData = " and some other data";
    buffer.Write(someOtherData, strlen(someOtherData));
    
    char readData[32] = { 0 };
    memcpy(readData, buffer.ReadData(), buffer.DataAvailableToRead());
    
    EXPECT_STRING_EQUAL("some data and some other data", readData);
}

TEST(DataBufferTests, MultipleReadsExtractsDataInWriteOrder)
{
    DataBuffer buffer(32);
    
    const char* someData = "some data and some other data";
    buffer.Write(someData, strlen(someData));
    
    char readData[32] = { 0 };
    memcpy(readData, buffer.ReadData(), strlen("some data"));
    buffer.MarkAsRead(strlen("some data"));
    
    EXPECT_STRING_EQUAL("some data", readData);
    EXPECT_EQUAL(buffer.DataAvailableToRead(), strlen(" and some other data"));
    
    char secondReadData[32] = { 0 };
    memcpy(secondReadData, buffer.ReadData(), buffer.DataAvailableToRead());
    
    EXPECT_STRING_EQUAL(" and some other data", secondReadData);
}
