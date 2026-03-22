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
#ifndef CHESS_AGAIN_BITBOARD_H
#define CHESS_AGAIN_BITBOARD_H
#include <cstdint>

#include "square.h"

namespace chess_again
{
	/**
	* @brief @ref Bitboard is a 64-bit representation of a chess board.
	*
	* Each bit corresponds to a square with the least significant bit (rightmost bit)
	* representing A1, then B1, all the way up to H8.  There is usually a bitboard for
	* each piece type and color on the board with positive bits indicating squares occupied by
	* that kind of piece. Bitboards are also used to represent all occupied squares and squares
	* that are being attacked.
	*/
	using bitboard = uint64_t;

	/**
	* @brief Bitboard utilities.
	*/
	namespace bitboards
	{
		namespace detail
		{
			/**
			* @brief Converts a rank to a rank index.
			*/
			[[nodiscard]] constexpr uint8_t rank_to_index(const Rank rank)
			{
				return static_cast<uint8_t>(rank);
			}

			/**
			 * @brief Converts a file to a file index.
			 */
			[[nodiscard]] constexpr uint8_t file_to_index(const File file)
			{
				return static_cast<uint8_t>(file);
			}

			/**
			 * @brief Converts a square to a bitboard index.
			 */
			[[nodiscard]] constexpr uint8_t square_to_index(const Square square)
			{
				return file_to_index(square.file) + rank_to_index(square.rank) * 8;
			}

			/**
			 * @brief Converts a rank index to a rank.
			 */
			[[nodiscard]] constexpr Rank index_to_rank(const uint8_t index)
			{
				return static_cast<Rank>(index / 8);
			}

			/**
			 * @brief Converts a file index to a file.
			 */
			[[nodiscard]] constexpr File index_to_file(const uint8_t index)
			{
				return static_cast<File>(index % 8);
			}

			/**
			 * @brief Converts a bitboard index to a square.
			 */
			[[nodiscard]] constexpr Square index_to_square(const uint8_t index)
			{
				return Square{index_to_file(index), index_to_rank(index)};
			}
		}

		/**
		 * @brief Gets the state of the bit in the bitboard at the specified index.
		 *
		 * @param board a bitboard
		 * @param index the bit to return with 0 being the rightmost bit.
		 * @return True if the bit at the specified index is set, otherwise false.
		 * Undefined behavior if index > 63.
		 */
		[[nodiscard]] constexpr bool get_bit_unchecked(const bitboard board, const uint8_t index)
		{
			return static_cast<bool>((board >> index) & uint64_t{1});
		}

		/**
		 * @brief Sets the bit at the specified index to 1.
		 *
		 * @param board a bitboard
		 * @param index the bit to set with 0 being the rightmost bit.
		 * @return A copy of the board with the specified bit set to 1.
		 * Undefined behavior if index > 63.
		 */
		[[nodiscard]] constexpr bitboard set_bit_unchecked(const bitboard board,
			const uint8_t index)
		{
			return board | (uint64_t{1} << index);
		}

		/**
		 * @brief Sets the bit at the specified index to 0.
		 * @param board a bitboard
		 * @param index the bit to clear with 0 being the rightmost bit.
		 * @return A copy of the board with the specified bit cleared to 0.
		 * Undefined behavior if index > 63.
		 */
		[[nodiscard]] constexpr bitboard clear_bit_unchecked(const bitboard board,
			const uint8_t index)
		{
			return board & ~(uint64_t{1} << index);
		}

		/**
		 * @brief Gets the state of the bit in the bitboard at the specified index.
		 *
		 * @param board a bitboard
		 * @param index the bit to return with 0 being the rightmost bit.
		 * @return True if the bit at the specified index is set, otherwise false. Always returns
		 * false if bit > 63;
		 */
		[[nodiscard]] constexpr bool get_bit(const bitboard board, const uint8_t index)
		{
			return (index < 64) && get_bit_unchecked(board, index);
		}

		/**
		 * @brief Sets the bit at the specified index to 1.
		 *
		 * @param board a bitboard
		 * @param index the bit to set with 0 being the rightmost bit.
		 * @return A copy of the board with the specified bit set to 1. If index > 63,
		 * the original board is returned.
		 */
		[[nodiscard]] constexpr bitboard set_bit(const bitboard board, const uint8_t index)
		{
			if (index > 63)
			{
				return board;
			}
			return set_bit_unchecked(board, index);
		}

		/**
		 * @brief Sets the bit at the specified index to 0.
		 * @param board a bitboard
		 * @param index the bit to clear with 0 being the rightmost bit.
		 * @return A copy of the board with the specified bit cleared to 0. If index > 63,
		 * the original board is returned.
		 */
		[[nodiscard]] constexpr bitboard clear_bit(const bitboard board, const uint8_t index)
		{
			if (index > 63)
			{
				return board;
			}
			return clear_bit_unchecked(board, index);
		}

		/**
		 * @brief Gets the state of the bit in the bitboard for the specified square.
		 *
		 * @param board a bitboard
		 * @param square the square to get
		 * @return true if the bit for the specified square is set to 1.
		 */
		[[nodiscard]] constexpr bool get_square(const bitboard board, const Square square)
		{
			return get_bit_unchecked(board, detail::square_to_index(square));
		}

		/**
		 * @brief Sets the bit at the specified square to 1.
		 *
		 * @param board a bitboard
		 * @param square the square to set
		 * @return A copy of the board with the specified square set to 1.
		 */
		[[nodiscard]] constexpr bitboard set_square(const bitboard board, const Square square)
		{
			return set_bit_unchecked(board, detail::square_to_index(square));
		}

		/**
		 * @brief Sets the bit at the specified square to 0.
		 *
		 * @param board a bitboard
		 * @param square the square to set
		 * @return A copy of the board with the specified square set to 0.
		 */
		[[nodiscard]] constexpr bitboard clear_square(const bitboard board, const Square square)
		{
			return clear_bit_unchecked(board, detail::square_to_index(square));
		}

		[[nodiscard]] constexpr bitboard white_pawn_attacks_ne(const bitboard white_pawns)
		{
			return (white_pawns << 9) & 0xFEFEFEFEFEFEFEFE;
		}

		[[nodiscard]] constexpr bitboard white_pawn_attacks_nw(const bitboard white_pawns)
		{
			return (white_pawns << 7) & 0x7F7F7F7F7F7F7F7F;
		}

		/**
		 * @brief Generates all squares attacked by the given bitboard of white pawns.
		 */
		[[nodiscard]] constexpr bitboard white_pawn_attacks(const bitboard white_pawns)
		{
			const bitboard pawn_attacks_ne = white_pawn_attacks_ne(white_pawns);
			const bitboard pawn_attacks_nw = white_pawn_attacks_nw(white_pawns);
			return pawn_attacks_ne | pawn_attacks_nw;
		}

		[[nodiscard]] constexpr bitboard black_pawn_attacks_se(const bitboard black_pawns)
		{
			return (black_pawns >> 7) & 0xFEFEFEFEFEFEFEFE;
		}

		[[nodiscard]] constexpr bitboard black_pawn_attacks_sw(const bitboard black_pawns)
		{
			return (black_pawns >> 9) & 0x7F7F7F7F7F7F7F7F;
		}

		/**
		 * @brief Generates all squares attacked by the given bitboard of black pawns.
		 */
		[[nodiscard]] constexpr bitboard black_pawn_attacks(const bitboard black_pawns)
		{
			const bitboard pawn_attacks_se = black_pawn_attacks_se(black_pawns);
			const bitboard pawn_attacks_sw = black_pawn_attacks_sw(black_pawns);
			return pawn_attacks_se | pawn_attacks_sw;
		}

		/**
		 * @brief Generates all squares attacked by the given bitboard of knights.
		 */
		[[nodiscard]] constexpr bitboard knight_attacks(const bitboard knights)
		{
			return ((knights << 17) & 0xfefefefefefefefe) |
					((knights << 10) & 0xfcfcfcfcfcfcfcfc) |
					((knights >> 6) & 0xfcfcfcfcfcfcfcfc) |
					((knights >> 15) & 0xfefefefefefefefe) |
					((knights >> 17) & 0x7f7f7f7f7f7f7f7f) |
					((knights >> 10) & 0x3f3f3f3f3f3f3f3f) |
					((knights << 6) & 0x3f3f3f3f3f3f3f3f) |
					((knights << 15) & 0x7f7f7f7f7f7f7f7f);
		}

		/**
		 * @brief Generates all squares attacked by the given bitboard of kings.
		 */
		[[nodiscard]] constexpr bitboard king_attacks(const bitboard kings)
		{
			return ((kings >> 9) & 0x7f7f7f7f7f7f7f7f) |
					(kings >> 8) |
					((kings >> 7) & 0xfefefefefefefefe) |
					((kings >> 1) & 0x7f7f7f7f7f7f7f7f) |
					((kings << 1) & 0xfefefefefefefefe) |
					((kings << 7) & 0x7f7f7f7f7f7f7f7f) |
					(kings << 8) |
					((kings << 9) & 0xfefefefefefefefe);
		}


		/**
		 * @brief Calculates bishop attacks without magic bitboards.
		 *
		 * Unless you are calculating only a few moves every time your program runs, it is better to
		 * use @ref bishop_attacks(Square, bitboard).
		 *
		 * @param bishops A bitboard indicating all bishops for which attacks should be generated.
		 * @param occupied A bitboard indicating all occupied squares on the board. This includes
		 * both colors and the bishops.
		 */
		[[nodiscard]] bitboard bishop_attacks_uncached(bitboard bishops,
			bitboard occupied);

		/**
		 * @brief Calculates bishop attacks.
		 *
		 * Uses magic bitboards for efficient move generation.
		 *
		 * @param bishops A bitboard indicating all bishops for which attacks should be generated.
		 * @param occupied A bitboard indicating all occupied squares on the board. This includes
		 * both colors and the bishops.
		 */
		[[nodiscard]] bitboard bishop_attacks(bitboard bishops, bitboard occupied);

		/**
		 * @brief Calculates rook attacks without magic bitboards.
		 *
		 * Unless you are calculating only a few moves every time your program runs, it is better to
		 * use @ref rook_attacks(Square, bitboard).
		 *
		 * @param rooks A bitboard indicating all rooks for which attacks should be generated.
		 * @param occupied A bitboard indicating all occupied squares on the board. This includes
		 * both colors and the rooks.
		 */
		[[nodiscard]] bitboard rook_attacks_uncached(bitboard rooks, bitboard occupied);

		/**
		 * @brief Calculates rook attacks.
		 *
		 * Uses magic bitboards for efficient move generation.
		 *
		 * @param rooks A bitboard indicating all rooks for which attacks should be generated.
		 * @param occupied A bitboard indicating all occupied squares on the board. This includes
		 * both colors and the rooks.
		 */
		[[nodiscard]] bitboard rook_attacks(bitboard rooks, bitboard occupied);

		/**
		 * @brief Calculates queen attacks without magic bitboards.
		 *
		 * Unless you are calculating only a few moves every time your program runs, it is better to
		 * use @ref queen_attacks(Square, bitboard).
		 *
		 * @param queens A bitboard indicating all queens for which attacks should be generated.
		 * @param occupied A bitboard indicating all occupied squares on the board. This includes
		 * both colors and the queens.
		 */
		[[nodiscard]] bitboard queen_attacks_uncached(bitboard queens, bitboard occupied);

		/**
		 * @brief Calculates queen attacks.
		 *
		 * Uses magic bitboards for efficient move generation.
		 *
		 * @param queens A bitboard indicating all queens for which attacks should be generated.
		 * @param occupied A bitboard indicating all occupied squares on the board. This includes
		 * both colors and the queens.
		 */
		[[nodiscard]] bitboard queen_attacks(bitboard queens, bitboard occupied);
	}

	/**
	* @brief Iterates through all active bits of a bitboard.
	*
	* Most of the time you will want to use this through a @ref BitboardRange.
	*/
	class BitboardIterator
	{
		bitboard board;

	public:
		explicit BitboardIterator(const bitboard b) : board(b)
		{
		}

		/**
		 * @brief Gives the current square.
		 */
		Square operator*() const
		{
			return bitboards::detail::index_to_square(
				static_cast<uint8_t>(std::countr_zero(board)));
		}

		// Prefix increment: remove the bit we just processed
		BitboardIterator& operator++()
		{
			board &= (board - 1); // Clears the least significant bit
			return *this;
		}

		bool operator!=(const BitboardIterator& other) const
		{
			return board != other.board;
		}
	};

	/**
	 * @brief Enables for loops over all the active squares in a bitboard.
	 */
	class BitboardRange
	{
	private:
		bitboard board;

	public:
		explicit BitboardRange(const uint64_t b) : board(b)
		{
		}

		BitboardIterator begin() const { return BitboardIterator(board); }
		BitboardIterator end() const { return BitboardIterator(0); }
	};
}

#endif //CHESS_AGAIN_BITBOARD_H
