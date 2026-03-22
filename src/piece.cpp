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

#include "piece.h"
#include "text.h"

namespace chess_again::detail
{
	char piece_to_char(const Piece piece)
	{
		std::array<char, 1> buf; //NOLINT
		std::format_to_n(buf.begin(), 1, "{}", piece.type);
		switch (piece.color)
		{
			case Color::WHITE:
				return better_cpp::text::to_ascii_uppercase(buf[0]);
			case Color::BLACK:
				return buf[0];
			default:
				throw std::invalid_argument("Could not convert piece to char: Invalid color");
		}
	}
}
