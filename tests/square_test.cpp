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

#include <sstream>
#include <gtest/gtest.h>

#include "square.h"
#include "file.h"
#include "rank.h"

using namespace chess_again;

TEST(RankTest, to_string)
{
	ASSERT_EQ(std::format("{}", Rank::ONE), "1");
}

TEST(FileTest, to_string)
{
	ASSERT_EQ(std::format("{}", File::A), "a");
}

TEST(SquareTest, to_string)
{
	ASSERT_EQ(std::format("{}", squares::A1), "a1");
}
