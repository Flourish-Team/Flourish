#pragma once

#include "gtest/gtest.h"

// Nicer assert/expect defines

#define ASSERT_EQUAL								GTEST_ASSERT_EQ
#define ASSERT_NOT_EQUAL							GTEST_ASSERT_NE
#define ASSERT_LESS_THAN							GTEST_ASSERT_LE
#define ASSERT_LESS_THAN_OR_EQUAL					GTEST_ASSERT_LE
#define ASSERT_GREATER_THAN							GTEST_ASSERT_GT
#define ASSERT_GREATER_THAN_OR_EQUAL				GTEST_ASSERT_GE
#define ASSERT_STRING_EQUAL							ASSERT_STREQ
#define ASSERT_STRING_NOT_EQUAL						ASSERT_STRNE
#define ASSERT_STRING_EQUAL_CASE_INSENSATIVE		ASSERT_STRCASEEQ
#define ASSERT_STRING_NOT_EQUAL_CASE_INSENSATIVE	ASSERT_STRCASENE

#define EXPECT_EQUAL								EXPECT_EQ
#define EXPECT_NOT_EQUAL							EXPECT_NE
#define EXPECT_LESS_THAN							EXPECT_LE
#define EXPECT_LESS_THAN_OR_EQUAL					EXPECT_LE
#define EXPECT_GREATER_THAN                         EXPECT_GT
#define EXPECT_GREATER_THAN_OR_EQUAL				EXPECT_GE
#define EXPECT_STRING_EQUAL                         EXPECT_STREQ
#define EXPECT_STRING_NOT_EQUAL                     EXPECT_STRNE
#define EXPECT_STRING_EQUAL_CASE_INSENSATIVE		EXPECT_STRCASEEQ
#define EXPECT_STRING_NOT_EQUAL_CASE_INSENSATIVE	EXPECT_STRCASENE

#define TEST_NOT_IMPLEMENTED                        GTEST_FATAL_FAILURE_("Not implemented");
