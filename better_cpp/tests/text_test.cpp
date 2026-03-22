/*
	Copyright (C) 2026 Brigham Skarda

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include "text.h"

TEST(TextTest, to_ascii_lowercase)
{
	ASSERT_EQ(better_cpp::text::to_ascii_lowercase("H@lL😮"), "h@ll😮");
}

TEST(TextTest, to_ascii_uppercase)
{
	ASSERT_EQ(better_cpp::text::to_ascii_uppercase("h@lL😮"), "H@LL😮");
}

TEST(TextTest, to_ascii_lowercase_inplace)
{
	std::string value = "H@lL😮";
	better_cpp::text::to_ascii_lowercase(value);
	ASSERT_EQ(value, "h@ll😮");
}

TEST(TextTest, to_ascii_uppercase_inplace)
{
	std::string value = "h@lL😮";
	better_cpp::text::to_ascii_uppercase(value);
	ASSERT_EQ(value, "H@LL😮");
}

TEST(TextTest, split_into_strings_on_char)
{
	const std::vector<std::string> result = better_cpp::text::split("hello worldl", 'l');
	const std::vector<std::string> expected_result{"he", "", "o wor", "d", ""};
	ASSERT_EQ(result, expected_result);
}

TEST(TextTest, split_into_views_on_char)
{
	const std::string s = "hello worldl";
	const std::vector<std::string_view> result = better_cpp::text::split_view(s, 'l');
	const std::vector<std::string_view> expected_result{"he", "", "o wor", "d", ""};
	ASSERT_EQ(result, expected_result);
}

TEST(TextTest, split_into_strings_on_string)
{
	const std::vector<std::string> result = better_cpp::text::split("hello worldl", "l");
	const std::vector<std::string> expected_result{"he", "", "o wor", "d", ""};
	ASSERT_EQ(result, expected_result);
}

TEST(TextTest, split_into_strings_on_empty_string)
{
	const std::vector<std::string> result = better_cpp::text::split("hello worldl", "");
	const std::vector<std::string> expected_result{"hello worldl"};
	ASSERT_EQ(result, expected_result);
}


TEST(TextTest, split_into_views_on_string)
{
	const std::string s = "hello worldl";
	const std::vector<std::string_view> result = better_cpp::text::split_view(s, "l");
	const std::vector<std::string_view> expected_result{"he", "", "o wor", "d", ""};
	ASSERT_EQ(result, expected_result);
}

TEST(TextTest, split_into_views_on_empty_string)
{
	const std::string s = "hello worldl";
	const std::vector<std::string_view> result = better_cpp::text::split_view(s, "");
	const std::vector<std::string_view> expected_result{"hello worldl"};
	ASSERT_EQ(result, expected_result);
}
