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

#ifndef CHESS_AGAIN_PIECE_TYPE_H
#define CHESS_AGAIN_PIECE_TYPE_H
#include <array>
#include <format>

namespace chess_again
{
	/**
	 * @brief Represents a kind of chess piece. Does not include color.
	 */
	enum class PieceType : uint8_t
	{
		PAWN,
		KNIGHT,
		BISHOP,
		ROOK,
		QUEEN,
		KING,
	};

	/**
	 * @brief All the piece types in chess.
	 */
	inline constexpr std::array<PieceType, 6> ALL_PIECE_TYPES{
		PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN,
		PieceType::KING
	};

	/**
	 * @brief Converts a character to a piece type.
	 *
	 * @throws std::invalid_argument If c is not one of prnbqkPRNBQK.
	 */
	[[nodiscard]] constexpr PieceType char_to_piece_type(const char c)
	{
		switch (c)
		{
			case 'p':
			case 'P':
				return PieceType::PAWN;
			case 'n':
			case 'N':
				return PieceType::KNIGHT;
			case 'b':
			case 'B':
				return PieceType::BISHOP;
			case 'r':
			case 'R':
				return PieceType::ROOK;
			case 'q':
			case 'Q':
				return PieceType::QUEEN;
			case 'k':
			case 'K':
				return PieceType::KING;
			default:
				throw std::invalid_argument(std::format("Invalid piece type: {:?}", c));
		}
	}
}

/**
 * @brief Produces a single lowercase character (p, n, b, r, q, k).
 */
template<>
struct std::formatter<chess_again::PieceType> : std::formatter<char>
{
	template<typename FormatContext>
	auto format(chess_again::PieceType pt, FormatContext& ctx) const
	{
		const char c = "pnbrqk"[static_cast<int>(pt)];
		return std::formatter<char>::format(c, ctx);
	}
};
#endif //CHESS_AGAIN_PIECE_TYPE_H
