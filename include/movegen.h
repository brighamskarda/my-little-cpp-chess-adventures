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
#ifndef CHESS_AGAIN_MOVEGEN_H
#define CHESS_AGAIN_MOVEGEN_H

#include <vector>

#include "move.h"
#include "position.h"

namespace chess_again
{
	struct MoveList
	{
		std::array<Move, 256> moves; // Fixed size, no heap
		size_t count = 0;

		void push_back(const Move& m)
		{
			moves[count++] = m;
		}

		void clear()
		{
			count = 0;
		}

		// Allow iteration like a vector
		Move* begin() { return moves.data(); }
		Move* end() { return moves.data() + count; }
		size_t size() const { return count; }
	};

	namespace detail
	{
		/**
		 * @brief A standard starting capacity for pseudo legal move generation.
		 */
		inline constexpr size_t DEFAULT_PSEUDOLEGAL_MOVE_CAPACITY = 64;
		/**
		 * @brief A standard starting capacity for legal move generation.
		 */
		inline constexpr size_t DEFAULT_LEGAL_MOVE_CAPACITY = 64;
	}

	/**
	 * @brief Appends all pseudo-legal moves for the position to the vector.
	 *
	 * Uses the same move generation as @ref generate_pseudolegal_moves(const Position&). But uses
	 * the provided vector to store them. Doing this can reduce heap allocations in scenarios
	 * where lots of moves are being generated.
	 */
	void generate_pseudolegal_moves(const Position& pos, MoveList& moves);

	/**
	 * @brief Generates all pseudo-legal moves for the position.
	 *
	 * Pseudo-legal moves are moves that would be legal if they didn't leave one's own king in
	 * check.
	 */
	[[nodiscard]] inline MoveList generate_pseudolegal_moves(const Position& pos)
	{
		MoveList moves{};
		generate_pseudolegal_moves(pos, moves);
		return moves;
	}

	/**
	 * @brief Appends all legal moves for the position to the vector.
	 *
	 * Uses the same move generation as @ref generate_legal_moves(const Position&). But uses
	 * the provided vector to store them. Doing this can reduce heap allocations in scenarios
	 * where lots of moves are being generated.
	 */
	void generate_legal_moves(const Position& pos, MoveList& moves);

	/**
	 * @brief Generates all legal moves for the position.
	 */
	[[nodiscard]] inline MoveList generate_legal_moves(const Position& pos)
	{
		MoveList moves{};
		generate_legal_moves(pos, moves);
		return moves;
	}

	/**
	 * @brief Counts the number of legal positions found at a certain move depth.
	 *
	 * Perft (performance test) is used for testing move generation speed and accuracy. While it is
	 * mostly helpful for library debugging purposes, it is provided to the user as an easy way
	 * to test the performance of the library's move generation on their own machine.
	 *
	 * https://www.chessprogramming.org/Perft
	 *
	 * @param position The starting position for the performance test. Some positions lead to more
	 * branches in the game and thus take longer to evaluate all moves to the desired depth.
	 * @param depth The depth to which moves should be generated. Depth = 0 returns 1 since it is
	 * just the starting position. Setting depth higher than 6 results in exponentially longer perft
	 * times.
	 * @return A vector with the number of nodes calculated at each depth.
	 */
	std::vector<uint64_t> perft_test(const Position& position, unsigned int depth);
}

#endif //CHESS_AGAIN_MOVEGEN_H
