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

#include "board.h"
#include "bitboard.h"
#include "piece.h"
#include "square.h"
#include "text.h"

namespace chess_again
{
	void Board::parse_rank(const Rank rank, const std::string_view row)
	{
		uint8_t f = static_cast<uint8_t>(File::A);
		for (const char c : row)
		{
			// Skip files that are empty
			if (std::isdigit(c))
			{
				f += c - '0';
			} else
			{
				// Make sure there aren't too many files.
				if (f > static_cast<uint8_t>(File::H))
				{
					throw std::invalid_argument(std::format("Invalid rank {:?}: Too many files",
						row));
				}
				const File file = static_cast<File>(f);

				// Parses the piece
				try
				{
					set_piece(char_to_piece(c), Square{file, rank});
					++f;
				} catch (const std::invalid_argument& e)
				{
					throw std::invalid_argument(std::format("Invalid rank {:?}: {}", row,
						e.what()));
				}
			}
		}

		// Make sure the correct number of files were parsed.
		if (f != static_cast<uint8_t>(File::H) + 1)
		{
			throw std::invalid_argument(std::format(
				"Invalid rank {:?}: Found {} files, expected {}.", row, f - 1,
				static_cast<uint8_t>(File::H) + 1));
		}
	}

	Board::Board(const std::string_view fen)
	{
		const std::vector<std::string_view> rows = better_cpp::text::split_view(fen, '/');

		// Make sure there are 8 rows
		constexpr size_t expected_rows = ALL_RANKS.size();
		if (rows.size() != expected_rows)
		{
			throw std::invalid_argument(std::format("Invalid FEN {:?}: Found {} rows, expected {}",
				fen, rows.size(), expected_rows));
		}

		for (size_t i = 0; i < rows.size(); ++i)
		{
			try
			{
				parse_rank(ALL_RANKS[ALL_RANKS.size() - i - 1], rows[i]);
			} catch (const std::invalid_argument& e)
			{
				throw std::invalid_argument(std::format("Invalid FEN {:?}: {}", fen, e.what()));
			}
		}
	}
} // chess_again
