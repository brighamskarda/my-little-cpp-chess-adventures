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

#ifndef CHESS_AGAIN_COLOR_H
#define CHESS_AGAIN_COLOR_H
#include <format>

namespace chess_again
{
	/**
	 * @brief Color represents white or black
	 *
	 * Color can be used to represent the side to move, the color of a piece, or the color of a
	 * square.
	 */
	enum class Color : uint8_t
	{
		WHITE,
		BLACK
	};
}

/**
 * @brief Produces "WHITE" or "BLACK".
 */
template<>
struct std::formatter<chess_again::Color> : std::formatter<std::string_view>
{
	template<typename FormatContext>
	auto format(const chess_again::Color c, FormatContext& ctx) const
	{
		return std::formatter<std::string_view>::format(
			c == chess_again::Color::WHITE ? "WHITE" : "BLACK", ctx);
	}
};

#endif //CHESS_AGAIN_COLOR_H
