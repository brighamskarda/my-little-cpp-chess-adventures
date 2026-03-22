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

#ifndef CHESS_AGAIN_RANK_H
#define CHESS_AGAIN_RANK_H

namespace chess_again
{
	/**
	 * @brief Represents a row of squares on a chess board.
	 */
	enum class Rank : uint8_t
	{
		ONE,
		TWO,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
	};

	/**
	 * @brief All chess ranks in order from ONE to EIGHT.
	 */
	inline constexpr std::array<Rank, 8> ALL_RANKS{
		Rank::ONE, Rank::TWO, Rank::THREE, Rank::FOUR, Rank::FIVE, Rank::SIX, Rank::SEVEN,
		Rank::EIGHT
	};

	/**
	 * @brief Converts a character to a chess rank.
	 *
	 * @throws std::invalid_argument If c is not one of 12345678.
	 */
	[[nodiscard]] constexpr Rank char_to_rank(const char c)
	{
		switch (c)
		{
			case '1':
				return Rank::ONE;
			case '2':
				return Rank::TWO;
			case '3':
				return Rank::THREE;
			case '4':
				return Rank::FOUR;
			case '5':
				return Rank::FIVE;
			case '6':
				return Rank::SIX;
			case '7':
				return Rank::SEVEN;
			case '8':
				return Rank::EIGHT;
			default:
				throw std::invalid_argument(std::format("Invalid rank: {:?}", c));
		}
	}
}

/**
 * @brief Produces a single digit '1' through '8'.
 */
template<>
struct std::formatter<chess_again::Rank> : std::formatter<char>
{
	template<typename FormatContext>
	auto format(chess_again::Rank r, FormatContext& ctx) const
	{
		return std::formatter<char>::format(static_cast<char>('1' + static_cast<int>(r)), ctx);
	}
};

#endif //CHESS_AGAIN_RANK_H
