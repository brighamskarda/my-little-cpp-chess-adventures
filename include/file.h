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

#ifndef CHESS_AGAIN_FILE_H
#define CHESS_AGAIN_FILE_H

#include <array>
#include <format>

namespace chess_again
{
	/**
	 * @brief Represents a column of squares on a chess board.
	 */
	enum class File : uint8_t
	{
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H
	};

	/**
	 * @brief All chess files in order from A to H
	 */
	inline constexpr std::array<File, 8> ALL_FILES{
		File::A, File::B, File::C, File::D, File::E, File::F, File::G, File::H
	};

	/**
	 * @brief Converts a character to a chess file.
	 *
	 * @throws std::invalid_argument If c is not one of abcdefghABCDEFGH.
	 */
	[[nodiscard]] constexpr File char_to_file(const char c)
	{
		switch (c)
		{
			case 'a':
			case 'A':
				return File::A;
			case 'b':
			case 'B':
				return File::B;
			case 'c':
			case 'C':
				return File::C;
			case 'd':
			case 'D':
				return File::D;
			case 'e':
			case 'E':
				return File::E;
			case 'f':
			case 'F':
				return File::F;
			case 'g':
			case 'G':
				return File::G;
			case 'h':
			case 'H':
				return File::H;
			default:
				throw std::invalid_argument(std::format("Invalid file: {:?}", c));
		}
	}
}

/**
 * @brief Produces a single lowercase character 'a' through 'h'.
 */
template<>
struct std::formatter<chess_again::File> : std::formatter<char>
{
	template<typename FormatContext>
	auto format(chess_again::File f, FormatContext& ctx) const
	{
		return std::formatter<char>::format(static_cast<char>('a' + static_cast<int>(f)), ctx);
	}
};

#endif //CHESS_AGAIN_FILE_H
