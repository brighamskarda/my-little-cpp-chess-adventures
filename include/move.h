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
#ifndef CHESS_AGAIN_MOVE_H
#define CHESS_AGAIN_MOVE_H

#include <format>
#include <optional>

#include "piece_type.h"
#include "square.h"

namespace chess_again
{
	/**
	* @brief A move has a <b>from-square</b>, a <b>to-square</b>, and an optional promotion.
	*/
	struct Move
	{
		Square from_square{};
		Square to_square{};
		std::optional<PieceType> promotion{};

		bool operator==(const Move&) const = default;
	};
}

/**
 * @brief Produces UCI-compliant strings like "e2e4" or "e7e8q".
 */
template<>
struct std::formatter<chess_again::Move> : std::formatter<std::string_view>
{
	template<typename FormatContext>
	auto format(const chess_again::Move& m, FormatContext& ctx) const
	{
		std::string buffer;
		if (m.promotion.has_value())
		{
			buffer = std::format("{}{}{}", m.from_square, m.to_square, m.promotion.value());
		} else
		{
			buffer = std::format("{}{}", m.from_square, m.to_square);
		}

		return std::formatter<std::string_view>::format(buffer, ctx);
	}
};

#endif //CHESS_AGAIN_MOVE_H
