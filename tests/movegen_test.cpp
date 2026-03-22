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
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <gtest/gtest.h>

#include "movegen.h"
#include "position.h"

using namespace chess_again;

// TEST(Movegen, starting_pos)
// {
// 	const std::vector<Move> moves = generate_legal_moves(Position(Position::DEFAULT_FEN));
// 	// White Pawn moves (8 single push, 8 double push)
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::A2, squares::A3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::B2, squares::B3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::C2, squares::C3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::D2, squares::D3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::E2, squares::E3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::F2, squares::F3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::G2, squares::G3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::H2, squares::H3}));
//
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::A2, squares::A4}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::B2, squares::B4}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::C2, squares::C4}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::D2, squares::D4}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::E2, squares::E4}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::F2, squares::F4}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::G2, squares::G4}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::H2, squares::H4}));
//
// 	// White Knight moves (2 for each knight)
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::B1, squares::A3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::B1, squares::C3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::G1, squares::F3}));
// 	ASSERT_TRUE(std::ranges::contains(moves, Move{squares::G1, squares::H3}));
//
// 	if (moves.size() != 20)
// 	{
// 		std::stringstream ss{};
// 		// std::println(ss, "Expected 20 moves, got {}:", moves.size());
// 		// for (Move m : moves)
// 		// {
// 		// 	std::println(ss, "\t{}", m);
// 		// }
// 		FAIL() << ss.str();
// 	}
//
// 	ASSERT_EQ(moves.size(), 20);
// }

TEST(PerftTest, DISABLED_starting_pos_depth_5)
{
	const std::vector<uint64_t> result = perft_test(Position(Position::DEFAULT_FEN), 5);
	const std::vector<uint64_t> expected_results = {1, 20, 400, 8902, 197281, 4865609};
	ASSERT_EQ(result, expected_results);
}

TEST(PerftTest, DISABLED_kiwipete_pos_depth_4)
{
	const std::vector<uint64_t> result = perft_test(
		Position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"), 4);
	const std::vector<uint64_t> expected_results = {1, 48, 2039, 97862, 4085603};
	ASSERT_EQ(result, expected_results);
}

TEST(PerftTest, DISABLED_endgame_pos_depth_6)
{
	const std::vector<uint64_t> result = perft_test(
		Position("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"), 6);
	const std::vector<uint64_t> expected_results = {1, 14, 191, 2812, 43238, 674624, 11030083};
	ASSERT_EQ(result, expected_results);
}

TEST(MoveGenBenchmark, Average_Perft_Performance)
{
	// 1. Setup Configuration
	constexpr std::string_view fen =
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	constexpr int depth = 6;
	constexpr uint64_t expected_nodes = 119060324;
	constexpr int iterations = 2;

	std::vector<double> run_times;
	run_times.reserve(iterations);

	const Position pos{fen};

	std::cout << "\n[ BENCHMARK ] Running " << iterations << " iterations at Depth " << depth <<
			"..." << std::endl;

	// 2. Execution Loop
	std::vector<uint64_t> result;
	for (int i = 0; i < iterations; ++i)
	{
		auto start = std::chrono::high_resolution_clock::now();

		result = chess_again::perft_test(pos, depth);
		auto end = std::chrono::high_resolution_clock::now();

		// Basic correctness check on every run
		ASSERT_EQ(result[depth], expected_nodes);

		std::chrono::duration<double> elapsed = end - start;
		run_times.push_back(elapsed.count());

		std::cout << "  Run " << (i + 1) << ": " << std::fixed << std::setprecision(4) << elapsed.
				count() << "s" << std::endl;
	}

	// 3. Statistical Analysis
	const double total_time = std::accumulate(run_times.begin(), run_times.end(), 0.0);
	const double avg_time = total_time / iterations;
	const double nps = std::accumulate(result.begin(), result.end(), uint64_t{0}) / avg_time;

	// 4. Final Report
	std::cout << "\n=================================================" << std::endl;
	std::cout << "  BENCHMARK RESULTS (Average of " << iterations << " runs)" << std::endl;
	std::cout << "-------------------------------------------------" << std::endl;
	std::cout << "  Average Time : " << std::fixed << std::setprecision(5) << avg_time << " s" <<
			std::endl;
	std::cout << "  Average NPS  : " << nps << " nodes/sec" << std::endl;
	std::cout << "  Total Nodes  : " << expected_nodes << std::endl;
	std::cout << "=================================================\n" << std::endl;
}
