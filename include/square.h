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

#ifndef CHESS_AGAIN_SQUARE_H
#define CHESS_AGAIN_SQUARE_H
#include <format>

#include "file.h"
#include "rank.h"

namespace chess_again
{
	/**
	 * @brief Represents a square on a chess board.
	 */
	struct Square
	{
		File file;
		Rank rank;

		bool operator==(const Square&) const = default;
	};


	/**
	 * @brief Predefined constants for all chess squares.
	 */
	namespace squares
	{
		inline constexpr Square A1{File::A, Rank::ONE};
		inline constexpr Square A3{File::A, Rank::THREE};
		inline constexpr Square A4{File::A, Rank::FOUR};
		inline constexpr Square A2{File::A, Rank::TWO};
		inline constexpr Square A5{File::A, Rank::FIVE};
		inline constexpr Square A6{File::A, Rank::SIX};
		inline constexpr Square A7{File::A, Rank::SEVEN};
		inline constexpr Square A8{File::A, Rank::EIGHT};

		inline constexpr Square B1{File::B, Rank::ONE};
		inline constexpr Square B2{File::B, Rank::TWO};
		inline constexpr Square B3{File::B, Rank::THREE};
		inline constexpr Square B4{File::B, Rank::FOUR};
		inline constexpr Square B5{File::B, Rank::FIVE};
		inline constexpr Square B6{File::B, Rank::SIX};
		inline constexpr Square B7{File::B, Rank::SEVEN};
		inline constexpr Square B8{File::B, Rank::EIGHT};

		inline constexpr Square C1{File::C, Rank::ONE};
		inline constexpr Square C2{File::C, Rank::TWO};
		inline constexpr Square C3{File::C, Rank::THREE};
		inline constexpr Square C4{File::C, Rank::FOUR};
		inline constexpr Square C5{File::C, Rank::FIVE};
		inline constexpr Square C6{File::C, Rank::SIX};
		inline constexpr Square C7{File::C, Rank::SEVEN};
		inline constexpr Square C8{File::C, Rank::EIGHT};

		inline constexpr Square D1{File::D, Rank::ONE};
		inline constexpr Square D2{File::D, Rank::TWO};
		inline constexpr Square D3{File::D, Rank::THREE};
		inline constexpr Square D4{File::D, Rank::FOUR};
		inline constexpr Square D5{File::D, Rank::FIVE};
		inline constexpr Square D6{File::D, Rank::SIX};
		inline constexpr Square D7{File::D, Rank::SEVEN};
		inline constexpr Square D8{File::D, Rank::EIGHT};

		inline constexpr Square E1{File::E, Rank::ONE};
		inline constexpr Square E2{File::E, Rank::TWO};
		inline constexpr Square E3{File::E, Rank::THREE};
		inline constexpr Square E4{File::E, Rank::FOUR};
		inline constexpr Square E5{File::E, Rank::FIVE};
		inline constexpr Square E6{File::E, Rank::SIX};
		inline constexpr Square E7{File::E, Rank::SEVEN};
		inline constexpr Square E8{File::E, Rank::EIGHT};

		inline constexpr Square F1{File::F, Rank::ONE};
		inline constexpr Square F2{File::F, Rank::TWO};
		inline constexpr Square F3{File::F, Rank::THREE};
		inline constexpr Square F4{File::F, Rank::FOUR};
		inline constexpr Square F5{File::F, Rank::FIVE};
		inline constexpr Square F6{File::F, Rank::SIX};
		inline constexpr Square F7{File::F, Rank::SEVEN};
		inline constexpr Square F8{File::F, Rank::EIGHT};

		inline constexpr Square G1{File::G, Rank::ONE};
		inline constexpr Square G2{File::G, Rank::TWO};
		inline constexpr Square G3{File::G, Rank::THREE};
		inline constexpr Square G4{File::G, Rank::FOUR};
		inline constexpr Square G5{File::G, Rank::FIVE};
		inline constexpr Square G6{File::G, Rank::SIX};
		inline constexpr Square G7{File::G, Rank::SEVEN};
		inline constexpr Square G8{File::G, Rank::EIGHT};

		inline constexpr Square H1{File::H, Rank::ONE};
		inline constexpr Square H2{File::H, Rank::TWO};
		inline constexpr Square H3{File::H, Rank::THREE};
		inline constexpr Square H4{File::H, Rank::FOUR};
		inline constexpr Square H5{File::H, Rank::FIVE};
		inline constexpr Square H6{File::H, Rank::SIX};
		inline constexpr Square H7{File::H, Rank::SEVEN};
		inline constexpr Square H8{File::H, Rank::EIGHT};
	}

	// @formatter:off
	/**
	 * @brief All squares in order A1, B1, ... G8, H8.
	 */
	inline constexpr std::array<Square, 64> ALL_SQUARES{
		squares::A1, squares::B1, squares::C1, squares::D1, squares::E1, squares::F1, squares::G1, squares::H1,
		squares::A2, squares::B2, squares::C2, squares::D2, squares::E2, squares::F2, squares::G2, squares::H2,
		squares::A3, squares::B3, squares::C3, squares::D3, squares::E3, squares::F3, squares::G3, squares::H3,
		squares::A4, squares::B4, squares::C4, squares::D4, squares::E4, squares::F4, squares::G4, squares::H4,
		squares::A5, squares::B5, squares::C5, squares::D5, squares::E5, squares::F5, squares::G5, squares::H5,
		squares::A6, squares::B6, squares::C6, squares::D6, squares::E6, squares::F6, squares::G6, squares::H6,
		squares::A7, squares::B7, squares::C7, squares::D7, squares::E7, squares::F7, squares::G7, squares::H7,
		squares::A8, squares::B8, squares::C8, squares::D8, squares::E8, squares::F8, squares::G8, squares::H8
	};
	// @formatter:on

	/**
	 * @brief Converts a string to a square.
	 *
	 * @param s Should be a two-character string representing a square in the format A1 or H8.
	 *
	 * @throws std::invalid_argument If s does not represent a valid square.
	 */
	[[nodiscard]] constexpr Square str_to_square(const std::string_view s)
	{
		if (s.size() != 2)
		{
			throw std::invalid_argument(std::format("Invalid square: {:?}: Expected 2 characters",
				s));
		}

		try
		{
			return Square{char_to_file(s[0]), char_to_rank(s[1])};
		} catch (const std::invalid_argument& e)
		{
			throw std::invalid_argument(std::format("Invalid square: {:?}: {}", s, e.what()));
		}
	}
}

/**
 * @brief Produces coordinate strings like "e4".
 */
template<>
struct std::formatter<chess_again::Square> : std::formatter<std::string_view>
{
	template<typename FormatContext>
	auto format(const chess_again::Square& s, FormatContext& ctx) const
	{
		// Format the square into a stack-allocated buffer
		const std::string buffer = std::format("{}{}", s.file, s.rank);

		// Pass the resulting string to the base string_view formatter
		return std::formatter<std::string_view>::format(buffer, ctx);
	}
};
#endif //CHESS_AGAIN_SQUARE_H
