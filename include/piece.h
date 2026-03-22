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

#ifndef CHESS_AGAIN_PIECE_H
#define CHESS_AGAIN_PIECE_H
#include <format>

#include "color.h"
#include "piece_type.h"

namespace chess_again
{
	/**
	 * @brief A specific kind of chess piece with type and color.
	 */
	struct Piece
	{
		Color color;
		PieceType type;

		bool operator==(const Piece& piece) const = default;
	};

	/**
	 * @brief Predefined constants for all possible chess pieces.
	 */
	namespace pieces
	{
		inline constexpr Piece WHITE_PAWN{Color::WHITE, PieceType::PAWN};
		inline constexpr Piece WHITE_KNIGHT{Color::WHITE, PieceType::KNIGHT};
		inline constexpr Piece WHITE_BISHOP{Color::WHITE, PieceType::BISHOP};
		inline constexpr Piece WHITE_ROOK{Color::WHITE, PieceType::ROOK};
		inline constexpr Piece WHITE_QUEEN{Color::WHITE, PieceType::QUEEN};
		inline constexpr Piece WHITE_KING{Color::WHITE, PieceType::KING};

		inline constexpr Piece BLACK_PAWN{Color::BLACK, PieceType::PAWN};
		inline constexpr Piece BLACK_KNIGHT{Color::BLACK, PieceType::KNIGHT};
		inline constexpr Piece BLACK_BISHOP{Color::BLACK, PieceType::BISHOP};
		inline constexpr Piece BLACK_ROOK{Color::BLACK, PieceType::ROOK};
		inline constexpr Piece BLACK_QUEEN{Color::BLACK, PieceType::QUEEN};
		inline constexpr Piece BLACK_KING{Color::BLACK, PieceType::KING};
	}

	/**
	 * @brief All possible chess pieces.
	 */
	inline constexpr std::array<Piece, 12> ALL_PIECES{
		pieces::WHITE_PAWN, pieces::WHITE_KNIGHT, pieces::WHITE_BISHOP, pieces::WHITE_ROOK,
		pieces::WHITE_QUEEN, pieces::WHITE_KING, pieces::BLACK_PAWN, pieces::BLACK_KNIGHT,
		pieces::BLACK_BISHOP, pieces::BLACK_ROOK, pieces::BLACK_QUEEN, pieces::BLACK_KING
	};

	/**
	 * @brief Converts a character to a piece.
	 *
	 * @throws std::invalid_argument If c is not one of prnbqkPRNBQK.
	 */
	constexpr Piece char_to_piece(const char c)
	{
		Piece piece;
		piece.color = c < 'a' ? Color::WHITE : Color::BLACK;
		piece.type = char_to_piece_type(c);
		return piece;
	}

	namespace detail
	{
		char piece_to_char(Piece piece);
	}
}

/**
 * @brief Provides a single letter representation of a chess piece.
 *
 * Uppercase for White, lowercase for Black (e.g., 'R' vs 'r').
 */
template<>
struct std::formatter<chess_again::Piece> : std::formatter<char>
{
	template<typename FormatContext>
	auto format(const chess_again::Piece p, FormatContext& ctx) const
	{
		return std::formatter<char>::format(chess_again::detail::piece_to_char(p), ctx);
	}
};


#endif //CHESS_AGAIN_PIECE_H
