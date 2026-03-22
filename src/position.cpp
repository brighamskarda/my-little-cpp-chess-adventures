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

#include "position.h"
#include "text.h"

namespace chess_again
{
	[[nodiscard]] constexpr Color parse_color(const std::string_view str)
	{
		if (str.size() != 1)
		{
			throw std::invalid_argument(std::format("Invalid color: {:?}: Expected 1 character",
				str));
		}
		switch (str[0])
		{
			case 'w':
			case 'W':
				return Color::WHITE;
			case 'b':
			case 'B':
				return Color::BLACK;
			default:
				throw std::invalid_argument(std::format("Invalid color: {:?}: Expected 'w' or 'b'",
					str));
		}
	}


	void Position::set_castling_rights(const std::string_view castling_rights)
	{
		// Check if the string is too long or too short.
		if (castling_rights.size() > 4 || castling_rights.size() < 1)
		{
			throw std::invalid_argument(std::format(
				"Invalid castling rights: {:?}: Expected 4 characters",
				castling_rights));
		}

		// There is nothing to do as there are no castling rights.
		if (castling_rights == "-")
		{
			return;
		}

		for (const char c : castling_rights)
		{
			switch (c)
			{
				case 'K':
					if (!get_white_ks_castle())
					{
						set_white_ks_castle(true);
					} else
					{
						throw std::invalid_argument(std::format(
							"Invalid castling rights: {:?}: Duplicate rights {:?}",
							castling_rights, c));
					}
					break;
				case 'Q':
					if (!get_white_qs_castle())
					{
						set_white_qs_castle(true);
					} else
					{
						throw std::invalid_argument(std::format(
							"Invalid castling rights: {:?}: Duplicate rights {:?}",
							castling_rights, c));
					}
					break;
				case 'k':
					if (!get_black_ks_castle())
					{
						set_black_ks_castle(true);
					} else
					{
						throw std::invalid_argument(std::format(
							"Invalid castling rights: {:?}: Duplicate rights {:?}",
							castling_rights, c));
					}
					break;
				case 'q':
					if (!get_black_qs_castle())
					{
						set_black_qs_castle(true);
					} else
					{
						throw std::invalid_argument(std::format(
							"Invalid castling rights: {:?}: Duplicate rights {:?}",
							castling_rights, c));
					}
					break;
				default:
					throw std::invalid_argument(std::format(
						"Invalid castling rights: {:?}: Expected 'K', 'Q', 'k', or 'q'",
						castling_rights));
			}
		}
	}

	[[nodiscard]] constexpr std::optional<Square> parse_square(const std::string_view str)
	{
		if (str == "-")
		{
			return std::nullopt;
		}
		return str_to_square(str);
	}

	[[nodiscard]] constexpr uint16_t parse_halfmove(const std::string_view str)
	{
		uint16_t half_move;
		const std::from_chars_result errs = std::from_chars(str.data(), str.data() + str.size(),
			half_move);
		if (errs.ec != std::errc{})
		{
			throw std::invalid_argument(std::format("Invalid halfmove {:?}", str));
		}

		return half_move;
	}

	[[nodiscard]] constexpr uint16_t parse_fullmove(const std::string_view str)
	{
		uint16_t full_move;
		const std::from_chars_result errs = std::from_chars(str.data(), str.data() + str.size(),
			full_move);
		if (errs.ec != std::errc{})
		{
			throw std::invalid_argument(std::format("Invalid fullmove {:?}", str));
		}

		return full_move;
	}

	Position::Position(const std::string_view fen)
	{
		const std::vector<std::string_view> fen_parts = better_cpp::text::split_view(fen, ' ');

		if (fen_parts.size() != 6)
		{
			throw std::invalid_argument(std::format("Invalid FEN {:?}: Found {} parts, expected 6",
				fen,
				fen_parts.size()));
		}

		try
		{
			board = Board(fen_parts[0]);
			set_side_to_move(parse_color(fen_parts[1]));
			set_castling_rights(fen_parts[2]);
			en_passant = parse_square(fen_parts[3]);
			set_halfmove(parse_halfmove(fen_parts[4]));
			set_fullmove(parse_fullmove(fen_parts[5]));
		} catch (const std::invalid_argument& e)
		{
			throw std::invalid_argument(std::format("Invalid FEN {:?}: {}", fen, e.what()));
		}
	}
}
