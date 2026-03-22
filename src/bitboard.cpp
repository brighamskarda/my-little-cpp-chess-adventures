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

#include <array>
#include <mutex>

#include "bitboard.h"

#include <vector>

#include "file.h"
#include "rank.h"
#include "square.h"

using namespace chess_again;

// Huge thanks to Chess Programming and his example implementation of magic bitboards. It was extremely useful. https://youtu.be/4ohJQ9pCkHI?si=4QQG2BntrELO2JYi

namespace
{
	// @formatter:off
	/**
	 * @brief Number of relevant bits to look at for every rook position on the board.
	 */
	constexpr std::array<int, 64> ROOK_RELEVANT_BITS{
		12, 11, 11, 11, 11, 11, 11, 12,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		12, 11, 11, 11, 11, 11, 11, 12
	};

	/**
	 * @brief Number of relevant bits to look at for every bishop position on the board.
	 */
	constexpr std::array<int, 64> BISHOP_RELEVANT_BITS{
		6, 5, 5, 5, 5, 5, 5, 6,
		5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 7, 7, 7, 7, 5, 5,
		5, 5, 7, 9, 9, 7, 5, 5,
		5, 5, 7, 9, 9, 7, 5, 5,
		5, 5, 7, 7, 7, 7, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5,
		6, 5, 5, 5, 5, 5, 5, 6
	};

	// @formatter:on

	/**
	 * @brief magic numbers to generate rook attacks.
	 */
	constexpr std::array<uint64_t, 64> ROOK_MAGICS{
		0xa8002c000108020, 0x6c00049b0002001, 0x100200010090040, 0x2480041000800801,
		0x280028004000800, 0x900410008040022, 0x280020001001080, 0x2880002041000080,
		0xa000800080400034, 0x4808020004000, 0x2290802004801000, 0x411000d00100020,
		0x402800800040080, 0xb000401004208, 0x2409000100040200, 0x1002100004082, 0x22878001e24000,
		0x1090810021004010, 0x801030040200012, 0x500808008001000, 0xa08018014000880,
		0x8000808004000200, 0x201008080010200, 0x801020000441091, 0x800080204005,
		0x1040200040100048, 0x120200402082, 0xd14880480100080, 0x12040280080080, 0x100040080020080,
		0x9020010080800200, 0x813241200148449, 0x491604001800080, 0x100401000402001,
		0x4820010021001040, 0x400402202000812, 0x209009005000802, 0x810800601800400,
		0x4301083214000150, 0x204026458e001401, 0x40204000808000, 0x8001008040010020,
		0x8410820820420010, 0x1003001000090020, 0x804040008008080, 0x12000810020004,
		0x1000100200040208, 0x430000a044020001, 0x280009023410300, 0xe0100040002240, 0x200100401700,
		0x2244100408008080, 0x8000400801980, 0x2000810040200, 0x8010100228810400,
		0x2000009044210200, 0x4080008040102101, 0x40002080411d01, 0x2005524060000901,
		0x502001008400422, 0x489a000810200402, 0x1004400080a13, 0x4000011008020084,
		0x26002114058042,
	};

	/**
	 * @brief magic numbers to generate bishop attacks.
	 */
	constexpr std::array<uint64_t, 64> BISHOP_MAGICS{
		0x89a1121896040240, 0x2004844802002010, 0x2068080051921000, 0x62880a0220200808,
		0x4042004000000, 0x100822020200011, 0xc00444222012000a, 0x28808801216001, 0x400492088408100,
		0x201c401040c0084, 0x840800910a0010, 0x82080240060, 0x2000840504006000, 0x30010c4108405004,
		0x1008005410080802, 0x8144042209100900, 0x208081020014400, 0x4800201208ca00,
		0xf18140408012008, 0x1004002802102001, 0x841000820080811, 0x40200200a42008, 0x800054042000,
		0x88010400410c9000, 0x520040470104290, 0x1004040051500081, 0x2002081833080021,
		0x400c00c010142, 0x941408200c002000, 0x658810000806011, 0x188071040440a00,
		0x4800404002011c00, 0x104442040404200, 0x511080202091021, 0x4022401120400,
		0x80c0040400080120, 0x8040010040820802, 0x480810700020090, 0x102008e00040242,
		0x809005202050100, 0x8002024220104080, 0x431008804142000, 0x19001802081400,
		0x200014208040080, 0x3308082008200100, 0x41010500040c020, 0x4012020c04210308,
		0x208220a202004080, 0x111040120082000, 0x6803040141280a00, 0x2101004202410000,
		0x8200000041108022, 0x21082088000, 0x2410204010040, 0x40100400809000, 0x822088220820214,
		0x40808090012004, 0x910224040218c9, 0x402814422015008, 0x90014004842410, 0x1000042304105,
		0x10008830412a00, 0x2520081090008908, 0x40102000a0a60140,
	};

	/**
	 * @brief Rook attack masks, essential for generating rook attacks.
	 *
	 * Will contain all possible rook attacks on every square except edges.
	 */
	std::array<bitboard, 64> rook_masks{}; // NOLINT

	/**
	 * @brief Bishop attack masks, essential for generating bishop attacks.
	 *
	 * Will contain all possible bishop attacks on every square except edges.
	 */
	std::array<bitboard, 64> bishop_masks{}; // NOLINT

	/**
	 * @brief All possible single rook attacks, just gotta know the magic numbers.
	 */
	std::array<std::array<bitboard, 4096>, 64> rook_attacks{}; // NOLINT

	/**
	 * @brief All possible single bishop attacks, just gotta know the magic numbers.
	 */
	std::array<std::array<bitboard, 512>, 64> bishop_attacks{}; // NOLINT

	/**
	 * @brief Generates a mask of all possible bishop moves at a certain square.
	 *
	 * Excludes edges.
	 */
	[[nodiscard]] constexpr bitboard mask_bishop_attacks(const Square square)
	{
		// I admit I did this in a kind of convoluted way with all the casts. At least I don't have
		// to rewrite the set_square function.
		bitboard attacks = 0;

		uint8_t rank = static_cast<uint8_t>(square.rank) + 1;
		uint8_t file = static_cast<uint8_t>(square.file) + 1;
		while (rank <= static_cast<uint8_t>(Rank::SEVEN) && file <= static_cast<uint8_t>(File::G))
		{
			attacks = bitboards::set_square(attacks, Square{
				static_cast<File>(file), static_cast<Rank>(rank)
			});
			++rank;
			++file;
		}

		rank = static_cast<uint8_t>(square.rank) + 1;
		file = static_cast<uint8_t>(square.file) - 1;
		while (rank <= static_cast<uint8_t>(Rank::SEVEN) && file >= static_cast<uint8_t>(File::B) &&
				file <= static_cast<uint8_t>(File::H))
		{
			attacks = bitboards::set_square(attacks, Square{
				static_cast<File>(file), static_cast<Rank>(rank)
			});
			++rank;
			--file;
		}

		rank = static_cast<uint8_t>(square.rank) - 1;
		file = static_cast<uint8_t>(square.file) + 1;
		while (rank >= static_cast<uint8_t>(Rank::TWO) && rank <= static_cast<uint8_t>(Rank::EIGHT)
				&& file <= static_cast<uint8_t>(File::G))
		{
			attacks = bitboards::set_square(attacks, Square{
				static_cast<File>(file), static_cast<Rank>(rank)
			});
			--rank;
			++file;
		}

		rank = static_cast<uint8_t>(square.rank) - 1;
		file = static_cast<uint8_t>(square.file) - 1;
		while (rank >= static_cast<uint8_t>(Rank::TWO) && rank <= static_cast<uint8_t>(Rank::EIGHT)
				&& file >= static_cast<uint8_t>(File::B) && file <= static_cast<uint8_t>(File::H))
		{
			attacks = bitboards::set_square(attacks, Square{
				static_cast<File>(file), static_cast<Rank>(rank)
			});
			--rank;
			--file;
		}

		return attacks;
	}

	/**
	 * @brief Generates a mask of all possible rook moves at a certain square.
	 *
	 * Excludes edges.
	 */
	[[nodiscard]] constexpr bitboard mask_rook_attacks(const Square square)
	{
		bitboard attacks = 0;

		int rank = static_cast<uint8_t>(square.rank) + 1;
		while (rank <= static_cast<uint8_t>(Rank::SEVEN))
		{
			attacks = bitboards::set_square(attacks, Square{square.file, static_cast<Rank>(rank)});
			++rank;
		}

		rank = static_cast<uint8_t>(square.rank) - 1;
		while (rank >= static_cast<uint8_t>(Rank::TWO) && rank <= static_cast<uint8_t>(Rank::EIGHT))
		{
			attacks = bitboards::set_square(attacks, Square{square.file, static_cast<Rank>(rank)});
			--rank;
		}

		int file = static_cast<uint8_t>(square.file) + 1;
		while (file <= static_cast<uint8_t>(File::G))
		{
			attacks = bitboards::set_square(attacks, Square{static_cast<File>(file), square.rank});
			++file;
		}

		file = static_cast<uint8_t>(square.file) - 1;
		while (file >= static_cast<uint8_t>(File::B) && file <= static_cast<uint8_t>(File::H))
		{
			attacks = bitboards::set_square(attacks, Square{static_cast<File>(file), square.rank});
			--file;
		}

		return attacks;
	}

	/**
	 * @brief Gets the index of the lowest activated bit.
	 *
	 * Returns -1 if board == 0.
	 */
	[[nodiscard]] constexpr int get_lowest_bit(const bitboard board)
	{
		if (board != 0)
		{
			return std::countr_zero(board);
		} else
		{
			return -1;
		}
	}

	/**
	 * @brief It's about here that I'm not really sure what we're doing to make these magic
	 * bitboards work. ¯\_(ツ)_/¯
	 */
	[[nodiscard]] constexpr bitboard set_occupancy(const uint64_t index, const int bit_count,
		bitboard mask)
	{
		bitboard occupancy = 0;
		for (int count = 0; count < bit_count; ++count)
		{
			const uint8_t square = static_cast<uint8_t>(get_lowest_bit(mask));
			mask = bitboards::clear_bit_unchecked(mask, square);
			if ((index & (uint64_t{1} << count)) != 0)
			{
				occupancy |= uint64_t{1} << square;
			}
		}

		return occupancy;
	}

	/**
	 * @brief calculates all squares attacked by a single bishop on the specified square.
	 */
	[[nodiscard]] constexpr bitboard single_bishop_attacks_uncached(const Square square,
		const bitboard occupied)
	{
		bitboard attacks = 0;

		const uint8_t start_rank = static_cast<uint8_t>(square.rank);
		const uint8_t start_file = static_cast<uint8_t>(square.file);

		// Northeast attacks
		for (uint8_t rank = start_rank + 1, file = start_file + 1;
			rank <= static_cast<uint8_t>(Rank::EIGHT) && file <= static_cast<uint8_t>(File::H);
			++rank, ++file)
		{
			const bitboard attacked_square = bitboard{1} << (rank * 8 + file);
			attacks |= attacked_square;
			if ((occupied & attacked_square) != 0)
			{
				break;
			}
		}

		// Southeast attacks
		for (uint8_t rank = start_rank - 1, file = start_file + 1;
			rank <= static_cast<uint8_t>(Rank::EIGHT) && file <= static_cast<uint8_t>(File::H);
			--rank, ++file)
		{
			const bitboard attacked_square = bitboard{1} << (rank * 8 + file);
			attacks |= attacked_square;
			if ((occupied & attacked_square) != 0)
			{
				break;
			}
		}

		// Northwest attacks
		for (uint8_t rank = start_rank + 1, file = start_file - 1;
			rank <= static_cast<uint8_t>(Rank::EIGHT) && file <= static_cast<uint8_t>(File::H);
			++rank, --file)
		{
			const bitboard attacked_square = bitboard{1} << (rank * 8 + file);
			attacks |= attacked_square;
			if ((occupied & attacked_square) != 0)
			{
				break;
			}
		}

		// Southwest attacks
		for (uint8_t rank = start_rank - 1, file = start_file - 1;
			rank <= static_cast<uint8_t>(Rank::EIGHT) && file <= static_cast<uint8_t>(File::H);
			--rank, --file)
		{
			const bitboard attacked_square = bitboard{1} << (rank * 8 + file);
			attacks |= attacked_square;
			if ((occupied & attacked_square) != 0)
			{
				break;
			}
		}

		return attacks;
	}

	/**
	 * @brief calculates all squares attacked by a single rook on the specified square.
	 */
	[[nodiscard]] constexpr bitboard single_rook_attacks_uncached(const Square square,
		const bitboard occupied)
	{
		bitboard attacks = 0;
		const uint8_t start_rank = static_cast<uint8_t>(square.rank);
		const uint8_t start_file = static_cast<uint8_t>(square.file);

		// North attacks
		for (uint8_t rank = start_rank + 1; rank <= static_cast<uint8_t>(Rank::EIGHT); ++rank)
		{
			const bitboard attacked_square = bitboard{1} << (rank * 8 + start_file);
			attacks |= attacked_square;
			if ((occupied & attacked_square) != 0)
			{
				break;
			}
		}
		// South attacks
		for (uint8_t rank = start_rank - 1; rank <= static_cast<uint8_t>(Rank::EIGHT); --rank)
		{
			const bitboard attacked_square = bitboard{1} << (rank * 8 + start_file);
			attacks |= attacked_square;
			if ((occupied & attacked_square) != 0)
			{
				break;
			}
		}

		// East attacks
		for (uint8_t file = start_file + 1; file <= static_cast<uint8_t>(File::H); ++file)
		{
			const bitboard attacked_square = bitboard{1} << (start_rank * 8 + file);
			attacks |= attacked_square;
			if ((occupied & attacked_square) != 0)
			{
				break;
			}
		}

		// West attacks
		for (uint8_t file = start_file - 1; file <= static_cast<uint8_t>(File::H); --file)
		{
			const bitboard attacked_square = bitboard{1} << (start_rank * 8 + file);
			attacks |= attacked_square;
			if ((occupied & attacked_square) != 0)
			{
				break;
			}
		}

		return attacks;
	}

	/**
	* @brief Magic bitboard initialization flag.
	*/
	std::once_flag slider_attacks_initialized{}; // NOLINT

	/**
	* @brief Initializes masks and attacks for magic bitboard move generation.
	*/
	void init_slider_attacks()
	{
		std::call_once(slider_attacks_initialized, []()
		{
			for (const Square square : ALL_SQUARES)
			{
				const uint8_t square_index = bitboards::detail::square_to_index(square);
				const bitboard current_bishop_mask = mask_bishop_attacks(square);
				const bitboard current_rook_mask = mask_rook_attacks(square);

				bishop_masks.at(square_index) = current_bishop_mask;
				rook_masks.at(square_index) = current_rook_mask;

				const int bishop_bit_count = std::popcount(current_bishop_mask);
				const int rook_bit_count = std::popcount(current_rook_mask);

				const uint64_t bishop_occupancy_variations = uint64_t{1} << bishop_bit_count;
				const uint64_t rook_occupancy_variations = uint64_t{1} << rook_bit_count;

				for (uint64_t count = 0; count < bishop_occupancy_variations; ++count)
				{
					const bitboard occupancy = set_occupancy(count, bishop_bit_count,
						current_bishop_mask);
					const size_t magic_index =
							(occupancy * BISHOP_MAGICS.at(square_index)) >> (
								64 - BISHOP_RELEVANT_BITS.at(square_index));
					bishop_attacks.at(square_index).at(magic_index) =
							single_bishop_attacks_uncached(square, occupancy);
				}

				for (uint64_t count = 0; count < rook_occupancy_variations; ++count)
				{
					const bitboard occupancy = set_occupancy(count, rook_bit_count,
						current_rook_mask);
					const size_t magic_index =
							(occupancy * ROOK_MAGICS.at(square_index)) >> (
								64 - ROOK_RELEVANT_BITS.at(square_index));
					rook_attacks.at(square_index).at(magic_index) = single_rook_attacks_uncached(
						square, occupancy);
				}
			}
		});
	}

	/**
	 * @brief Returns a list of all activated squares on the bitboard.
	 */
	[[nodiscard]] constexpr std::vector<Square> extract_squares(bitboard board)
	{
		std::vector<Square> squares;
		squares.reserve(std::popcount(board));
		int lowest_bit = get_lowest_bit(board);
		while (lowest_bit >= 0)
		{
			squares.push_back(bitboards::detail::index_to_square(static_cast<uint8_t>(lowest_bit)));
			board ^= uint64_t{1} << lowest_bit;
			lowest_bit = get_lowest_bit(board);
		}
		return squares;
	}

	/**
	 * @brief calculates all squares attacked by a single bishop using magic bitboards.
	 */
	[[nodiscard]] bitboard single_bishop_attacks(const Square square, bitboard occupied)
	{
		const uint8_t square_index = bitboards::detail::square_to_index(square);
		occupied &= bishop_masks[square_index];
		occupied *= BISHOP_MAGICS[square_index];
		occupied >>= 64 - BISHOP_RELEVANT_BITS[square_index];
		return bishop_attacks[square_index][occupied];
	}

	/**
	 * @brief calculates all squares attacked by a single rook using magic bitboards.
	 */
	[[nodiscard]] bitboard single_rook_attacks(const Square square, bitboard occupied)
	{
		const uint8_t square_index = bitboards::detail::square_to_index(square);
		occupied &= rook_masks[square_index];
		occupied *= ROOK_MAGICS[square_index];
		occupied >>= 64 - ROOK_RELEVANT_BITS[square_index];
		return rook_attacks[square_index][occupied];
	}
}

[[nodiscard]] bitboard bitboards::bishop_attacks_uncached(const bitboard bishops,
	const bitboard occupied)
{
	bitboard attacks = 0;

	const std::vector<Square> bishop_squares = extract_squares(bishops);
	for (const Square square : bishop_squares)
	{
		attacks |= single_bishop_attacks_uncached(square, occupied);
	}

	return attacks;
}


[[nodiscard]] bitboard bitboards::bishop_attacks(bitboard bishops, const bitboard occupied)
{
	init_slider_attacks();
	bitboard attacks = 0;
	while (bishops > 0)
	{
		const uint8_t square_idx = static_cast<uint8_t>(std::countr_zero(bishops));
		attacks |= single_bishop_attacks(detail::index_to_square(square_idx), occupied);
		bishops &= bishops - 1;
	}
	return attacks;
}

[[nodiscard]] bitboard bitboards::rook_attacks_uncached(const bitboard rooks,
	const bitboard occupied)
{
	bitboard attacks = 0;

	const std::vector<Square> rook_squares = extract_squares(rooks);
	for (const Square square : rook_squares)
	{
		attacks |= single_rook_attacks_uncached(square, occupied);
	}

	return attacks;
}

[[nodiscard]] bitboard bitboards::rook_attacks(bitboard rooks, const bitboard occupied)
{
	init_slider_attacks();
	bitboard attacks = 0;
	while (rooks > 0)
	{
		const uint8_t square_idx = static_cast<uint8_t>(std::countr_zero(rooks));
		attacks |= single_rook_attacks(detail::index_to_square(square_idx), occupied);
		rooks &= rooks - 1;
	}
	return attacks;
}

[[nodiscard]] bitboard bitboards::queen_attacks_uncached(const bitboard queens,
	const bitboard occupied)
{
	bitboard attacks = 0;

	attacks |= bishop_attacks_uncached(queens, occupied);
	attacks |= rook_attacks_uncached(queens, occupied);

	return attacks;
}

[[nodiscard]] bitboard bitboards::queen_attacks(const bitboard queens, const bitboard occupied)
{
	bitboard attacks = 0;

	attacks |= bishop_attacks(queens, occupied);
	attacks |= rook_attacks(queens, occupied);

	return attacks;
}
