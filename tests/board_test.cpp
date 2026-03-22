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

#include <gtest/gtest.h>

#include "board.h"
#include "color.h"
#include "piece.h"
#include "square.h"

using namespace chess_again;

TEST(BoardTest, basic_functions)
{
	Board b;
	ASSERT_EQ(b.get_occupied(), bitboard{0});

	b.set_piece(pieces::WHITE_KNIGHT, squares::C5);

	ASSERT_EQ(b.get_occupied(), bitboard{1} << 34);
	ASSERT_EQ(b.get_bitboard(pieces::WHITE_KNIGHT), bitboard{1} << 34);
	ASSERT_EQ(b.get_bitboard(pieces::BLACK_KNIGHT), bitboard{0});
	ASSERT_EQ(b.get_bitboard(pieces::WHITE_KING), bitboard{0});
	ASSERT_EQ(b.get_bitboard(Color::WHITE), bitboard{1} << 34);
	ASSERT_EQ(b.get_bitboard(Color::BLACK), bitboard{0});
	const std::optional<Piece> optional_piece = b.get_piece(squares::C5);
	ASSERT_TRUE(optional_piece.has_value());
	ASSERT_EQ(optional_piece.value(), pieces::WHITE_KNIGHT);
	const std::optional<Piece> empty_piece = b.get_piece(squares::C4);
	ASSERT_FALSE(empty_piece.has_value());

	b.clear_piece(squares::C5);
	ASSERT_EQ(b.get_occupied(), bitboard{0});
}

TEST(BoardTest, parse_fen)
{
	const Board b{Board::DEFAULT_FEN};

	ASSERT_EQ(b.get_piece(squares::E1).value(), pieces::WHITE_KING);
	ASSERT_EQ(b.get_piece(squares::E8).value(), pieces::BLACK_KING);
	ASSERT_FALSE(b.get_piece(squares::A6).has_value());
}

TEST(BoardTest, parse_fen_throws)
{
	// 1. Structural Failures (Rank Count)
	// Empty string
	ASSERT_THROW(Board b{""}, std::invalid_argument);
	// Too few ranks (only 7)
	ASSERT_THROW(Board b{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP"}, std::invalid_argument);
	// Too many ranks (9)
	ASSERT_THROW(Board b{"rnbqkbnr/pppppppp/8/8/8/8/8/PPPPPPPP/RNBQKBNR"}, std::invalid_argument);
	// Consecutive slashes (implicitly creates an empty rank)
	ASSERT_THROW(Board b{"rnbqkbnr//8/8/8/8/PPPPPPPP/RNBQKBNR"}, std::invalid_argument);

	// 2. Rank Width Failures (Each rank must sum to exactly 8)
	// Rank 4 is too short (7 squares)
	ASSERT_THROW(Board b{"rnbqkbnr/pppppppp/8/7/8/8/PPPPPPPP/RNBQKBNR"}, std::invalid_argument);
	// Rank 4 is too long (9 squares)
	ASSERT_THROW(Board b{"rnbqkbnr/pppppppp/8/9/8/8/PPPPPPPP/RNBQKBNR"}, std::invalid_argument);
	// Rank 4 is too long (digit sum: 4 + 5 = 9)
	ASSERT_THROW(Board b{"rnbqkbnr/pppppppp/8/45/8/8/PPPPPPPP/RNBQKBNR"}, std::invalid_argument);
	// Rank 4 is too short (pieces only: 7 pieces)
	ASSERT_THROW(Board b{"rnbqkbnr/pppppppp/8/ppppppp/8/8/PPPPPPPP/RNBQKBNR"},
		std::invalid_argument);

	// 3. Invalid Characters
	// 'X' is not a chess piece
	ASSERT_THROW(Board b{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNX"}, std::invalid_argument);
	// Special characters/whitespace
	ASSERT_THROW(Board b{"rnbqkbnr/pppp pppp/8/8/8/8/PPPPPPPP/RNBQKBNR"}, std::invalid_argument);
}
