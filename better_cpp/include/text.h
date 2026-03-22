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

#ifndef CHESS_AGAIN_UTF8_H
#define CHESS_AGAIN_UTF8_H

#include <algorithm>
#include <ranges>
#include <string>
#include <vector>


namespace better_cpp::text
{
	/**
	 * @brief Converts the character to its ascii lowercase counterpart.
	 *
	 * If the character doesn't have a lowercase version, it is returned unchanged.
	 * Only letters A-Z are changed.
	 */
	[[nodiscard]] constexpr char to_ascii_lowercase(const char c)
	{
		if (c >= 'A' && c <= 'Z')
		{
			return static_cast<char>(c + 'a' - 'A');
		}
		return c;
	}

	/**
	 * @brief Converts all uppercase ascii characters in the string in place.
	 *
	 * Only letters A-Z are changed to their lowercase counterparts..
	 */
	constexpr void to_ascii_lowercase(std::string& s)
	{
		std::ranges::transform(s, s.begin(), [](const char c)
		{
			return to_ascii_lowercase(c);
		});
	}

	/**
	 * @brief Converts all uppercase ascii characters to their lowercase counterparts.
	 *
	 * Returns a copy of the string. Only letters A-Z are changed.
	 */
	[[nodiscard]] constexpr std::string to_ascii_lowercase(const std::string_view s)
	{
		std::string result;
		result.resize_and_overwrite(s.size(), [&](char* buf, const std::size_t n)
		{
			std::ranges::transform(s, buf, [](const char c)
			{
				return to_ascii_lowercase(c);
			});
			return n;
		});
		return result;
	}

	/**
	* @brief Converts the character to its ascii uppercase counterpart.
	*
	* If the character doesn't have an uppercase version, it is returned unchanged.
	* Only letters a-z are changed.
	*/
	[[nodiscard]] constexpr char to_ascii_uppercase(const char c)
	{
		if (c >= 'a' && c <= 'z')
		{
			return static_cast<char>(c + 'A' - 'a');
		}
		return c;
	}

	/**
	 * @brief Converts all lowercase ascii characters in the string in place.
	 *
	 * Only letters a-z are changed to their uppercase counterparts.
	 */
	constexpr void to_ascii_uppercase(std::string& s)
	{
		std::ranges::transform(s, s.begin(), [](const char c)
		{
			return to_ascii_uppercase(c);
		});
	}

	/**
	* @brief Converts all lowercase ascii characters to their uppercase counterparts.
	*
	* Returns a copy of the string. Only letters a-z are changed.
	*/
	[[nodiscard]] constexpr std::string to_ascii_uppercase(const std::string_view s)
	{
		std::string result;
		result.resize_and_overwrite(s.size(), [&](char* buf, const std::size_t n)
		{
			std::ranges::transform(s, buf, [](const char c)
			{
				return to_ascii_uppercase(c);
			});
			return n;
		});
		return result;
	}

	/**
	 * @brief Splits a string into separate sub-strings around a delimiter.
	 *
	 * The resulting sub-strings do not contain the delimiter. If the delimiter is not found, then
	 * the whole string is returned as a single value. If there are two delimiters next to each
	 * other, then an empty string is included. If the string ends in the delimiter, then there is
	 * also an empty string included at the end.
	 *
	 * @return Is only as valid as long as s is valid in the scope.
	 */
	[[nodiscard]] constexpr std::vector<std::string_view> split_view(const std::string_view s,
		const char delimiter)
	{
		return s
				| std::views::split(delimiter)
				| std::views::transform([](auto range)
				{
					return std::string_view(range);
				})
				| std::ranges::to<std::vector<std::string_view>>();
	}

	/**
	 * @brief Splits a string into separate sub-strings around a delimiter.
	 *
	 * The resulting sub-strings do not contain the delimiter. If the delimiter is not found, then
	 * the whole string is returned as a single value. If there are two delimiters next to each
	 * other, then an empty string is included. If the string ends in the delimiter, then there is
	 * also an empty string included at the end.
	 */
	[[nodiscard]] constexpr std::vector<std::string> split(const std::string_view s,
		const char delimiter)
	{
		return s
				| std::views::split(delimiter)
				| std::ranges::to<std::vector<std::string>>();
	}

	/**
	 * @brief Splits a string into separate sub-strings around a delimiter.
	 *
	 * The resulting sub-strings do not contain the delimiter. If the delimiter is not found, then
	 * the whole string is returned as a single value. If there are two delimiters next to each
	 * other, then an empty string is included. If the string ends in the delimiter, then there is
	 * also an empty string included at the end.
	 *
	 * @param delimiter If delimiter is an empty string then s is returned as a single value.
	 *
	 * @return Is only as valid as long as s is valid in the scope.
	 */
	[[nodiscard]] constexpr std::vector<std::string_view> split_view(const std::string_view s,
		const std::string_view delimiter)
	{
		if (delimiter.empty())
		{
			return std::vector<std::string_view>{s};
		}
		return s
				| std::views::split(delimiter)
				| std::views::transform([](auto range)
				{
					return std::string_view(range);
				})
				| std::ranges::to<std::vector<std::string_view>>();
	}

	/**
	 * @brief Splits a string into separate sub-strings around a string_view delimiter.
	 *
	 * The resulting sub-strings do not contain the delimiter. If the delimiter is not found, then
	 * the whole string is returned as a single value. If there are two delimiters next to each
	 * other, then an empty string is included. If the string ends in the delimiter, then there is
	 * also an empty string included at the end.
	 *
 	 * @param delimiter If delimiter is an empty string then s is returned as a single value.
	 */
	[[nodiscard]] constexpr std::vector<std::string> split(const std::string_view s,
		const std::string_view delimiter)
	{
		if (delimiter.empty())
		{
			return std::vector<std::string>{std::string(s)};
		}
		return s
				| std::views::split(delimiter)
				| std::ranges::to<std::vector<std::string>>();
	}
}

#endif //CHESS_AGAIN_UTF8_H
