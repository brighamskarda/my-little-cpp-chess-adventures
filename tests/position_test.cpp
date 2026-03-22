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

#include "position.h"
#include "move.h"
#include "movegen.h"
#include "piece.h"
#include "square.h"

using namespace chess_again;

// ==============================================================================
// 1. STATE & COUNTER TESTS (RULES 1, 4, 5 & EMPTY SQUARE FALLBACK)
// ==============================================================================

TEST(PositionTest, move_swaps_side_and_updates_counters)
{
	Position p;
	p.set_piece(pieces::WHITE_PAWN, squares::E2);

	// White moves: side swaps, halfmove increments, fullmove stays same
	p.perform_move(Move{squares::E2, squares::E3});
	ASSERT_EQ(p.get_side_to_move(), Color::BLACK);
	ASSERT_EQ(p.get_halfmove(), 1);
	ASSERT_EQ(p.get_fullmove(), 1);

	p.set_piece(pieces::BLACK_PAWN, squares::E7);

	// Black moves: side swaps, halfmove increments, fullmove increments
	p.perform_move(Move{squares::E7, squares::E6});
	ASSERT_EQ(p.get_side_to_move(), Color::WHITE);
	ASSERT_EQ(p.get_halfmove(), 2);
	ASSERT_EQ(p.get_fullmove(), 2);
}

TEST(PositionTest, empty_from_square_updates_state_but_not_board)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	p.set_halfmove(10);
	p.set_en_passant(squares::A3);
	p.set_piece(pieces::BLACK_QUEEN, squares::H8);

	// Act: Move from empty E4 to E5
	p.perform_move(Move{squares::E4, squares::E5});

	// Side to move, counters, and en_passant are updated
	ASSERT_EQ(p.get_side_to_move(), Color::BLACK);
	ASSERT_EQ(p.get_halfmove(), 11);
	ASSERT_EQ(p.get_en_passant(), std::nullopt);

	// The board is not changed
	ASSERT_FALSE(p.get_piece(squares::E4).has_value());
	ASSERT_FALSE(p.get_piece(squares::E5).has_value());
	ASSERT_EQ(p.get_piece(squares::H8), pieces::BLACK_QUEEN);
}

TEST(PositionTest, moving_any_piece_from_castling_squares_removes_rights)
{
	Position p;
	p.set_white_ks_castle(true);
	p.set_white_qs_castle(true);

	// Place non-King/Rook pieces on the starting squares
	p.set_piece(pieces::WHITE_KNIGHT, squares::H1);
	p.set_piece(pieces::WHITE_BISHOP, squares::E1);
	p.set_side_to_move(Color::WHITE);

	// Moving ANY piece off H1 clears White's kingside right
	p.perform_move(Move{squares::H1, squares::G3});
	ASSERT_FALSE(p.get_white_ks_castle());
	ASSERT_TRUE(p.get_white_qs_castle());

	// Reset turn to White to test the King's square
	p.set_side_to_move(Color::WHITE);

	// Moving ANY piece off E1 clears the remaining castling rights
	p.perform_move(Move{squares::E1, squares::D2});
	ASSERT_FALSE(p.get_white_qs_castle());
}

TEST(PositionTest, empty_from_square_preserves_rights)
{
	Position p;
	p.set_piece(pieces::WHITE_PAWN, squares::E2);
	p.set_white_ks_castle(true);

	// Act: Move from an empty square to a square that currently has a piece
	p.perform_move(Move{squares::D1, squares::E2});

	// The board is not changed
	ASSERT_TRUE(p.get_piece(squares::E2).has_value());
	ASSERT_EQ(p.get_piece(squares::E2).value(), pieces::WHITE_PAWN);
	// Rights only change if a king/rook moves. Empty square move shouldn't change them.
	ASSERT_TRUE(p.get_white_ks_castle());
}

TEST(PositionTest, empty_from_square_ignores_promotion)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	ASSERT_FALSE(p.get_piece(squares::A8).has_value());

	// Move from empty A7 to A8 with a promotion
	p.perform_move(Move{squares::A7, squares::A8, PieceType::QUEEN});

	// The board is not changed - No queen should appear
	ASSERT_FALSE(p.get_piece(squares::A8).has_value());
	ASSERT_EQ(p.get_side_to_move(), Color::BLACK); // But turn still swaps
}

TEST(PositionTest, illegal_move_updates_state_regardless_of_legality)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	p.set_piece(pieces::WHITE_ROOK, squares::A1); // Starting square for Rook
	p.set_white_qs_castle(true);

	// Perform an illegal move (e.g., Rook teleporting across the board)
	p.perform_move(Move{squares::A1, squares::H8});

	// 1. Side to move is swapped
	ASSERT_EQ(p.get_side_to_move(), Color::BLACK);
	// 2. Castling right is set to false because rook moved off starting square
	ASSERT_FALSE(p.get_white_qs_castle());
	// 4. Half-move counter increments
	ASSERT_EQ(p.get_halfmove(), 1);

	// Check board state via default logic: clear from, replace to
	ASSERT_FALSE(p.get_piece(squares::A1).has_value());
	ASSERT_EQ(p.get_piece(squares::H8).value(), pieces::WHITE_ROOK);
}

// ==============================================================================
// 2. PAWN & EN-PASSANT TESTS (RULE 3 & EP CAPTURE LOGIC)
// ==============================================================================

TEST(PositionTest, pawn_double_push_sets_en_passant)
{
	Position p;
	p.set_piece(pieces::WHITE_PAWN, squares::E2);

	// Double push from rank 2 sets EP square to the jumped square (E3)
	p.perform_move(Move{squares::E2, squares::E4});
	ASSERT_TRUE(p.get_en_passant().has_value());
	ASSERT_EQ(p.get_en_passant().value(), squares::E3);

	p.set_piece(pieces::BLACK_PAWN, squares::D7);

	// Any move that isn't a double push should clear it
	p.perform_move(Move{squares::D7, squares::D6});
	ASSERT_FALSE(p.get_en_passant().has_value());
}

TEST(PositionTest, pawn_double_push_only_sets_ep_from_starting_rank)
{
	Position p;
	// White pawn starting rank is Rank 2, place it on Rank 3
	p.set_piece(pieces::WHITE_PAWN, squares::E3);

	// Move from Rank 3 to Rank 5 (Forward two squares)
	p.perform_move(Move{squares::E3, squares::E5});

	// EP only set if "moves from its starting rank forward two squares"
	// Since it started on Rank 3, EP should not be set.
	ASSERT_FALSE(p.get_en_passant().has_value());
}

TEST(PositionTest, en_passant_capture_removes_pawn)
{
	Position p;
	p.set_piece(pieces::WHITE_PAWN, squares::E5);
	p.set_piece(pieces::BLACK_PAWN, squares::D5);
	p.set_side_to_move(Color::WHITE);
	p.set_en_passant(squares::D6);

	// White captures en-passant on D6
	p.perform_move(Move{squares::E5, squares::D6});

	ASSERT_FALSE(p.get_piece(squares::E5).has_value());
	ASSERT_FALSE(p.get_piece(squares::D5).has_value()); // The black pawn should be cleared
	ASSERT_EQ(p.get_piece(squares::D6).value(), pieces::WHITE_PAWN);
}

TEST(PositionTest, en_passant_clears_correct_square_for_black)
{
	Position p;
	p.set_side_to_move(Color::BLACK);
	p.set_piece(pieces::BLACK_PAWN, squares::D4);
	p.set_piece(pieces::WHITE_PAWN, squares::E4);
	p.set_en_passant(squares::E3);

	// Black captures en-passant: D4 to E3
	// "square directly above ... the en-passant square is cleared" for black moving down.
	p.perform_move(Move{squares::D4, squares::E3});

	ASSERT_EQ(p.get_piece(squares::E3).value(), pieces::BLACK_PAWN);
	ASSERT_FALSE(p.get_piece(squares::E4).has_value()); // White pawn at E4 cleared
}

TEST(PositionTest, en_passant_only_triggers_for_pawns)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	p.set_piece(pieces::WHITE_KNIGHT, squares::E5);
	p.set_piece(pieces::BLACK_PAWN, squares::D5);
	p.set_en_passant(squares::D6);

	// White Knight moves to the en-passant square
	// "En-passant captures are only performed if the piece moving ... is a pawn"
	p.perform_move(Move{squares::E5, squares::D6});

	ASSERT_EQ(p.get_piece(squares::D6).value(), pieces::WHITE_KNIGHT);
	// The black pawn at D5 should NOT be cleared because the moving piece was not a pawn
	ASSERT_TRUE(p.get_piece(squares::D5).has_value());
	ASSERT_EQ(p.get_piece(squares::D5).value(), pieces::BLACK_PAWN);
}

TEST(PositionTest, en_passant_requires_attacking_diagonal)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	p.set_piece(pieces::WHITE_PAWN, squares::D4);
	p.set_piece(pieces::BLACK_PAWN, squares::D5);
	p.set_en_passant(squares::D6);

	// Invalid EP: pawn moves forward 2 squares to target, not a diagonal attack
	p.perform_move(Move{squares::D4, squares::D6});

	// EP captures only if "attacking 1-square diagonal"
	ASSERT_EQ(p.get_piece(squares::D6).value(), pieces::WHITE_PAWN);
	ASSERT_TRUE(p.get_piece(squares::D5).has_value()); // Black pawn should survive
}

TEST(PositionTest, en_passant_logic_diagonal_distance_check)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	p.set_piece(pieces::WHITE_PAWN, squares::B5);
	p.set_piece(pieces::BLACK_PAWN, squares::D5);
	p.set_en_passant(squares::D6);

	// White pawn moves from B5 to D6 (diagonal, but distance is 2 files)
	p.perform_move(Move{squares::B5, squares::D6});

	ASSERT_EQ(p.get_piece(squares::D6).value(), pieces::WHITE_PAWN);
	// The black pawn at D5 should NOT be cleared because the move was not a 1-square diagonal
	ASSERT_TRUE(p.get_piece(squares::D5).has_value());
}

TEST(PositionTest, en_passant_requires_correct_side_to_move)
{
	Position p;
	// It is White's turn, but we try to move a Black piece (illegal)
	p.set_side_to_move(Color::WHITE);
	p.set_piece(pieces::BLACK_PAWN, squares::E4);
	p.set_piece(pieces::WHITE_PAWN, squares::D4);
	p.set_en_passant(squares::D3);

	p.perform_move(Move{squares::E4, squares::D3});

	// Default logic applies: move the piece, but do not trigger the EP capture
	ASSERT_EQ(p.get_piece(squares::D3).value(), pieces::BLACK_PAWN);
	ASSERT_TRUE(p.get_piece(squares::D4).has_value()); // White pawn at D4 should still be there
}

TEST(PositionTest, promotion_updates_piece_type)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	p.set_piece(pieces::WHITE_PAWN, squares::B7);

	// Move with promotion to Queen
	p.perform_move(Move{squares::B7, squares::B8, PieceType::QUEEN});

	ASSERT_FALSE(p.get_piece(squares::B7).has_value());
	ASSERT_EQ(p.get_piece(squares::B8).value(), pieces::WHITE_QUEEN);
}

TEST(PositionTest, illegal_pawn_move_with_promotion)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	p.set_piece(pieces::WHITE_PAWN, squares::E2);

	// Teleporting pawn from E2 to E8 with a promotion (illegal move)
	p.perform_move(Move{squares::E2, squares::E8, PieceType::ROOK});

	ASSERT_FALSE(p.get_piece(squares::E2).has_value());
	ASSERT_EQ(p.get_piece(squares::E8).value(), pieces::WHITE_ROOK);

	// Counters should still update
	ASSERT_EQ(p.get_side_to_move(), Color::BLACK);
	ASSERT_EQ(p.get_halfmove(), 1);
}

// ==============================================================================
// 3. CASTLING TESTS (RULE 2 & CASTLE EXECUTION LOGIC)
// ==============================================================================

TEST(PositionTest, moving_king_or_rook_removes_castling_rights)
{
	Position p;
	p.set_white_ks_castle(true);
	p.set_white_qs_castle(true);
	p.set_piece(pieces::WHITE_KING, squares::E1);
	p.set_piece(pieces::WHITE_ROOK, squares::H1);

	// Moving a rook off starting square clears its right
	p.perform_move(Move{squares::H1, squares::H2});
	ASSERT_FALSE(p.get_white_ks_castle());
	ASSERT_TRUE(p.get_white_qs_castle());

	// Moving the king clears all rights for that color
	p.set_side_to_move(Color::WHITE);
	p.perform_move(Move{squares::E1, squares::E2});
	ASSERT_FALSE(p.get_white_qs_castle());
}

TEST(PositionTest, castling_execution_updates_both_pieces)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	p.set_white_qs_castle(true);
	p.set_piece(pieces::WHITE_KING, squares::E1);
	p.set_piece(pieces::WHITE_ROOK, squares::A1);

	// Execute Queenside Castle
	p.perform_move(Move{squares::E1, squares::C1});

	// King and Rook squares are cleared and filled in destinations
	ASSERT_FALSE(p.get_piece(squares::E1).has_value());
	ASSERT_FALSE(p.get_piece(squares::A1).has_value());
	ASSERT_EQ(p.get_piece(squares::C1).value(), pieces::WHITE_KING);
	ASSERT_EQ(p.get_piece(squares::D1).value(), pieces::WHITE_ROOK);
}

TEST(PositionTest, castling_overwrites_intermediate_pieces)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	p.set_white_ks_castle(true);
	p.set_piece(pieces::WHITE_KING, squares::E1);
	p.set_piece(pieces::WHITE_ROOK, squares::H1);

	// Place "blocker" pieces that should be overwritten
	p.set_piece(pieces::BLACK_KNIGHT, squares::F1);
	p.set_piece(pieces::BLACK_BISHOP, squares::G1);

	// Perform Kingside Castle: E1 to G1
	// "The space in between the king and rook is not checked."
	p.perform_move(Move{squares::E1, squares::G1});

	// Original squares cleared
	ASSERT_FALSE(p.get_piece(squares::E1).has_value());
	ASSERT_FALSE(p.get_piece(squares::H1).has_value());

	// Destination squares now contain King and Rook, overwriting the Black pieces
	ASSERT_EQ(p.get_piece(squares::G1).value(), pieces::WHITE_KING);
	ASSERT_EQ(p.get_piece(squares::F1).value(), pieces::WHITE_ROOK);
}

TEST(PositionTest, castling_fails_if_king_not_on_starting_square)
{
	Position p;
	p.set_side_to_move(Color::WHITE);
	// Move king to E2 (not its starting square E1)
	p.set_piece(pieces::WHITE_KING, squares::E2);
	p.set_piece(pieces::WHITE_ROOK, squares::H1);

	// Attempt a "castle-like" move from E2 to G1
	// "Castle moves are only performed if ... the king and rook are on their starting squares."
	p.perform_move(Move{squares::E2, squares::G1});

	// Default logic applies: clear from, replace to. Rook on H1 should NOT move.
	ASSERT_FALSE(p.get_piece(squares::E2).has_value());
	ASSERT_EQ(p.get_piece(squares::G1).value(), pieces::WHITE_KING);
	ASSERT_EQ(p.get_piece(squares::H1).value(), pieces::WHITE_ROOK); // Rook stayed put
}

TEST(PositionTest, parse_fen_valid_start_pos)
{
	// Testing the default starting position
	const Position p{Position::DEFAULT_FEN};

	// Check board state via get_board()
	ASSERT_EQ(p.get_board().get_piece(squares::E1), pieces::WHITE_KING);
	ASSERT_EQ(p.get_board().get_piece(squares::E8), pieces::BLACK_KING);

	// Check state flags
	ASSERT_EQ(p.get_side_to_move(), Color::WHITE);
	ASSERT_TRUE(p.get_white_ks_castle());
	ASSERT_TRUE(p.get_white_qs_castle());
	ASSERT_TRUE(p.get_black_ks_castle());
	ASSERT_TRUE(p.get_black_qs_castle());
	ASSERT_EQ(p.get_en_passant(), std::nullopt);
	ASSERT_EQ(p.get_halfmove(), 0);
	ASSERT_EQ(p.get_fullmove(), 1);
}

TEST(PositionTest, parse_fen_complex_midgame)
{
	// A position with specific flags: Black to move, partial castling, en passant target, counters
	// FEN: "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq e3 0 1"
	constexpr std::string_view complex_fen =
			"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq e3 0 1";
	const Position p{complex_fen};

	ASSERT_EQ(p.get_side_to_move(), Color::BLACK);

	// Check castling
	ASSERT_TRUE(p.get_white_ks_castle());
	ASSERT_TRUE(p.get_white_qs_castle());
	ASSERT_TRUE(p.get_black_ks_castle());
	ASSERT_TRUE(p.get_black_qs_castle());

	// Check en passant
	ASSERT_EQ(p.get_en_passant(), squares::E3);

	// Check counters
	ASSERT_EQ(p.get_halfmove(), 0);
	ASSERT_EQ(p.get_fullmove(), 1);
}

TEST(PositionTest, parse_fen_no_rights_and_counters)
{
	// FEN with no castling rights ("-") and high counter values
	constexpr std::string_view fen = "8/8/8/8/8/8/8/8 w - - 10 50";
	const Position p{fen};

	ASSERT_FALSE(p.get_white_ks_castle());
	ASSERT_FALSE(p.get_white_qs_castle());
	ASSERT_FALSE(p.get_black_ks_castle());
	ASSERT_FALSE(p.get_black_qs_castle());
	ASSERT_EQ(p.get_en_passant(), std::nullopt);
	ASSERT_EQ(p.get_halfmove(), 10);
	ASSERT_EQ(p.get_fullmove(), 50);
}

TEST(PositionTest, parse_fen_throws_on_invalid_format)
{
	// 1. Missing fields (FEN requires 6 fields)
	ASSERT_THROW(Position p{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"},
		std::invalid_argument);

	// 2. Invalid side to move
	ASSERT_THROW(Position p{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR x KQkq - 0 1"},
		std::invalid_argument);

	// 3. Invalid castling flags
	ASSERT_THROW(Position p{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w ABCq - 0 1"},
		std::invalid_argument);

	// 4. Invalid en passant square
	ASSERT_THROW(Position p{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq i9 0 1"},
		std::invalid_argument);

	// 5. Non-numeric counters
	ASSERT_THROW(Position p{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - a b"},
		std::invalid_argument);

	// 6. Empty string
	ASSERT_THROW(Position p{""}, std::invalid_argument);
}

