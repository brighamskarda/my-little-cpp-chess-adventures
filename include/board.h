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
#ifndef CHESS_AGAIN_BOARD_H
#define CHESS_AGAIN_BOARD_H
#include <optional>

#include "bitboard.h"
#include "piece.h"

namespace chess_again
{
	/**
	 * @brief Board represents the location of all pieces on a chess board.
	 *
	 * It does not include the full game state. Just the pieces on the board.
	 */
	class Board
	{
	private:
		std::array<std::array<bitboard, 6>, 2> piece_bitboards{};

		/**
		 * @brief Parses a single row in a board FEN
		 *
		 * @param rank The rank in which to insert the values.
		 * @param row The FEN formatted chess pieces that should go in the rank.
		 * @throws std::invalid_argument When the row is invalid.
		 */
		void parse_rank(Rank rank, std::string_view row);

	public:
		/**
		 * @brief The default starting position of all pieces on the chess board.
		 */
		static constexpr std::string_view DEFAULT_FEN =
				"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

		/**
		 * @brief Initializes and empty board.
		 */
		[[nodiscard]] constexpr Board() = default;

		/**
		 * @brief Creates a board from the first field in an FEN string.
		 *
		 * @param fen The first field from a Forsyth–Edwards Notation formatted board position.
		 * @throws std::invalid_argument When fen is invalid.
		 */
		[[nodiscard]] explicit Board(std::string_view fen);

		bool operator==(const Board&) const = default;

		/**
		 * @brief Returns a bitboard indicating all the locations of the specified piece.
		 */
		[[nodiscard]] constexpr bitboard get_bitboard(const Piece piece) const
		{
			return piece_bitboards[static_cast<int>(piece.color)][static_cast<int>(piece.type)];
		}

		/**
		 * @brief Returns a bitboard indicating the locations of all pieces for the specified color.
		 */
		[[nodiscard]] constexpr bitboard get_bitboard(const Color color) const
		{
			bitboard color_bitboard;

			switch (color)
			{
				case Color::WHITE:
					color_bitboard = piece_bitboards[0][0] | piece_bitboards[0][1] | piece_bitboards
									[0][2] | piece_bitboards[0][3] |
									piece_bitboards[0][4] | piece_bitboards[0][5];
					break;
				case Color::BLACK:
					color_bitboard = piece_bitboards[1][0] | piece_bitboards[1][1] | piece_bitboards
									[1][2] | piece_bitboards[1][3] |
									piece_bitboards[1][4] | piece_bitboards[1][5];
					break;
				default:
					throw std::invalid_argument("Could not get bitboard: Invalid color");
			}

			return color_bitboard;
		}

		/**
		 * @brief Returns a bitboard indicating all occupied squares.
		 */
		[[nodiscard]] constexpr bitboard get_occupied() const
		{
			return piece_bitboards[0][0] | piece_bitboards[0][1] | piece_bitboards[0][2] |
					piece_bitboards[0][3] | piece_bitboards[0][4] | piece_bitboards[0][5] |
					piece_bitboards[1][0] | piece_bitboards[1][1] | piece_bitboards[1][2] |
					piece_bitboards[1][3] | piece_bitboards[1][4] | piece_bitboards[1][5];
		}

		/**
		 * @brief Returns the piece on the specified square.
		 */
		[[nodiscard]] constexpr std::optional<Piece> get_piece(const Square square) const
		{
			if (bitboards::get_square(get_bitboard(pieces::WHITE_PAWN), square))
			{
				return pieces::WHITE_PAWN;
			} else if (bitboards::get_square(get_bitboard(pieces::WHITE_KNIGHT), square))
			{
				return pieces::WHITE_KNIGHT;
			} else if (bitboards::get_square(get_bitboard(pieces::WHITE_BISHOP), square))
			{
				return pieces::WHITE_BISHOP;
			} else if (bitboards::get_square(get_bitboard(pieces::WHITE_ROOK), square))
			{
				return pieces::WHITE_ROOK;
			} else if (bitboards::get_square(get_bitboard(pieces::WHITE_QUEEN), square))
			{
				return pieces::WHITE_QUEEN;
			} else if (bitboards::get_square(get_bitboard(pieces::WHITE_KING), square))
			{
				return pieces::WHITE_KING;
			} else if (bitboards::get_square(get_bitboard(pieces::BLACK_PAWN), square))
			{
				return pieces::BLACK_PAWN;
			} else if (bitboards::get_square(get_bitboard(pieces::BLACK_KNIGHT), square))
			{
				return pieces::BLACK_KNIGHT;
			} else if (bitboards::get_square(get_bitboard(pieces::BLACK_BISHOP), square))
			{
				return pieces::BLACK_BISHOP;
			} else if (bitboards::get_square(get_bitboard(pieces::BLACK_ROOK), square))
			{
				return pieces::BLACK_ROOK;
			} else if (bitboards::get_square(get_bitboard(pieces::BLACK_QUEEN), square))
			{
				return pieces::BLACK_QUEEN;
			} else if (bitboards::get_square(get_bitboard(pieces::BLACK_KING), square))
			{
				return pieces::BLACK_KING;
			}
			return std::nullopt;
		}


		/**
		 * @brief Set the specified piece on the square.
		 *
		 * Replaces any piece already present.
		 */
		constexpr void set_piece(const Piece piece, const Square square)
		{
			clear_piece(square);
			piece_bitboards[static_cast<int>(piece.color)][static_cast<int>(piece.type)] |= 1ULL <<
					bitboards::detail::square_to_index(square);
		}

		/**
		 * @brief Clears the specified square of any piece.
		 */
		constexpr void clear_piece(const Square square)
		{
			for (auto& a : piece_bitboards)
			{
				for (bitboard& b : a)
				{
					b = bitboards::clear_square(b, square);
				}
			}
		};
	};
} // chess_again

#endif //CHESS_AGAIN_BOARD_H
