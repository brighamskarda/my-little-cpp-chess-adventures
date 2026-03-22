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

#include "movegen.h"

namespace chess_again
{
	void generate_white_pawn_promotions(const Square from_square, const Square to_square,
		MoveList& moves)
	{
		moves.push_back(Move{from_square, to_square, PieceType::QUEEN});
		moves.push_back(Move{from_square, to_square, PieceType::KNIGHT});
		moves.push_back(Move{from_square, to_square, PieceType::BISHOP});
		moves.push_back(Move{from_square, to_square, PieceType::ROOK});
	}

	/**
	* @brief Processes North-East captures (Origin: Square - 9)
	*/
	void process_white_pawn_ne_attacks(const bitboard ne_hits, MoveList& moves)
	{
		for (const Square to_sq : BitboardRange(ne_hits))
		{
			// Calculate origin: index - 9 corresponds to Rank-1, File-1
			const uint8_t from_idx = bitboards::detail::square_to_index(to_sq) - 9;
			const Square from_sq = bitboards::detail::index_to_square(from_idx);

			if (to_sq.rank == Rank::EIGHT)
			{
				generate_white_pawn_promotions(from_sq, to_sq, moves);
			} else
			{
				moves.push_back(Move{from_sq, to_sq});
			}
		}
	}

	/**
	* @brief Processes North-West captures (Origin: Square - 7)
	*/
	void process_white_pawn_nw_attacks(const bitboard nw_hits, MoveList& moves)
	{
		for (const Square to_sq : BitboardRange(nw_hits))
		{
			// Calculate origin: index - 7 corresponds to Rank-1, File+1
			const uint8_t from_idx = bitboards::detail::square_to_index(to_sq) - 7;
			const Square from_sq = bitboards::detail::index_to_square(from_idx);

			if (to_sq.rank == Rank::EIGHT)
			{
				generate_white_pawn_promotions(from_sq, to_sq, moves);
			} else
			{
				moves.push_back(Move{from_sq, to_sq});
			}
		}
	}

	void generate_white_pawn_attack_moves(const Position& pos, MoveList& moves)
	{
		const Board& board = pos.get_board();
		const bitboard white_pawns = board.get_bitboard(pieces::WHITE_PAWN);

		// Define capture targets: Black pieces + En Passant square
		bitboard targets = board.get_bitboard(Color::BLACK);
		if (const std::optional<Square> ep_square = pos.get_en_passant())
		{
			// Using your bitboards::set_square utility from bitboard.h
			targets = bitboards::set_square(targets, ep_square.value());
		}

		// Mask attack bitboards with targets to find valid captures
		const bitboard ne_hits = bitboards::white_pawn_attacks_ne(white_pawns) & targets;
		const bitboard nw_hits = bitboards::white_pawn_attacks_nw(white_pawns) & targets;

		process_white_pawn_ne_attacks(ne_hits, moves);
		process_white_pawn_nw_attacks(nw_hits, moves);
	}

	void generate_white_pawn_single_move_forward(const Board& board, MoveList& moves)
	{
		const bitboard move_forward_one =
				(board.get_bitboard(pieces::WHITE_PAWN) << 8) & ~board.get_occupied();
		for (const Square to_square : BitboardRange(move_forward_one))
		{
			const Square from_square{
				to_square.file, static_cast<Rank>(static_cast<uint8_t>(to_square.rank) - 1)
			};
			if (to_square.rank == Rank::EIGHT)
			{
				generate_white_pawn_promotions(from_square, to_square, moves);
			} else
			{
				moves.push_back(Move{from_square, to_square});
			}
		}
	}

	void generate_white_pawn_double_move_forward(const Board& board, MoveList& moves)
	{
		const bitboard move_forward_two =
				((board.get_bitboard(pieces::WHITE_PAWN) & 0xFF00ULL) << 16) &
				~board.get_occupied();
		for (const Square to_square : BitboardRange(move_forward_two))
		{
			const Square from_square{
				to_square.file, static_cast<Rank>(static_cast<uint8_t>(to_square.rank) - 2)
			};
			const Square possible_blocking_square{
				to_square.file, static_cast<Rank>(static_cast<uint8_t>(to_square.rank) - 1)
			};
			if (!board.get_piece(possible_blocking_square).has_value())
			{
				moves.push_back(Move{from_square, to_square});
			}
		}
	}

	void generate_white_pawn_moves(const Position& pos, MoveList& moves)
	{
		generate_white_pawn_attack_moves(pos, moves);
		generate_white_pawn_single_move_forward(pos.get_board(), moves);
		generate_white_pawn_double_move_forward(pos.get_board(), moves);
	}

	void generate_black_pawn_promotions(const Square from_square, const Square to_square,
		MoveList& moves)
	{
		moves.push_back(Move{from_square, to_square, PieceType::QUEEN});
		moves.push_back(Move{from_square, to_square, PieceType::KNIGHT});
		moves.push_back(Move{from_square, to_square, PieceType::BISHOP});
		moves.push_back(Move{from_square, to_square, PieceType::ROOK});
	}

	/**
 * @brief Processes South-East captures (Origin: Square + 7)
 * For a black pawn to hit a square SE, it must have come from the NW (File-1, Rank+1).
 */
	void process_black_pawn_se_attacks(const bitboard se_hits, MoveList& moves)
	{
		for (const Square to_sq : BitboardRange(se_hits))
		{
			// SE shift is >> 7, so the origin is at index + 7
			const uint8_t from_idx = bitboards::detail::square_to_index(to_sq) + 7;
			const Square from_sq = bitboards::detail::index_to_square(from_idx);

			if (to_sq.rank == Rank::ONE)
			{
				generate_black_pawn_promotions(from_sq, to_sq, moves);
			} else
			{
				moves.push_back(Move{from_sq, to_sq});
			}
		}
	}

	/**
	* @brief Processes South-West captures (Origin: Square + 9)
	* For a black pawn to hit a square SW, it must have come from the NE (File+1, Rank+1).
	*/
	void process_black_pawn_sw_attacks(const bitboard sw_hits, MoveList& moves)
	{
		for (const Square to_sq : BitboardRange(sw_hits))
		{
			// SW shift is >> 9, so the origin is at index + 9
			const uint8_t from_idx = bitboards::detail::square_to_index(to_sq) + 9;
			const Square from_sq = bitboards::detail::index_to_square(from_idx);

			if (to_sq.rank == Rank::ONE)
			{
				generate_black_pawn_promotions(from_sq, to_sq, moves);
			} else
			{
				moves.push_back(Move{from_sq, to_sq});
			}
		}
	}

	void generate_black_pawn_attack_moves(const Position& pos, MoveList& moves)
	{
		const Board& board = pos.get_board();
		const bitboard black_pawns = board.get_bitboard(pieces::BLACK_PAWN);

		// Define capture targets: White pieces + En Passant square
		bitboard targets = board.get_bitboard(Color::WHITE);
		if (const std::optional<Square> ep_square = pos.get_en_passant())
		{
			// Use set_square utility to handle EP targets
			targets = bitboards::set_square(targets, ep_square.value());
		}

		// Mask the specific direction bitboards with valid targets
		const bitboard se_hits = bitboards::black_pawn_attacks_se(black_pawns) & targets;
		const bitboard sw_hits = bitboards::black_pawn_attacks_sw(black_pawns) & targets;

		process_black_pawn_se_attacks(se_hits, moves);
		process_black_pawn_sw_attacks(sw_hits, moves);
	}

	void generate_black_pawn_single_move_forward(const Board& board, MoveList& moves)
	{
		// Shift right by 8 to move "down" one rank
		const bitboard move_forward_one =
				(board.get_bitboard(pieces::BLACK_PAWN) >> 8) & ~board.get_occupied();

		for (const Square to_square : BitboardRange(move_forward_one))
		{
			const Square from_square{
				to_square.file, static_cast<Rank>(static_cast<uint8_t>(to_square.rank) + 1)
			};
			if (to_square.rank == Rank::ONE)
			{
				generate_black_pawn_promotions(from_square, to_square, moves);
			} else
			{
				moves.push_back(Move{from_square, to_square});
			}
		}
	}

	void generate_black_pawn_double_move_forward(const Board& board, MoveList& moves)
	{
		// Rank 7 mask: 0x00FF000000000000ULL
		const bitboard move_forward_two =
				((board.get_bitboard(pieces::BLACK_PAWN) & 0x00FF000000000000ULL) >> 16) &
				~board.get_occupied();

		for (const Square to_square : BitboardRange(move_forward_two))
		{
			const Square from_square{
				to_square.file, static_cast<Rank>(static_cast<uint8_t>(to_square.rank) + 2)
			};
			const Square possible_blocking_square{
				to_square.file, static_cast<Rank>(static_cast<uint8_t>(to_square.rank) + 1)
			};
			if (!board.get_piece(possible_blocking_square).has_value())
			{
				moves.push_back(Move{from_square, to_square});
			}
		}
	}

	void generate_black_pawn_moves(const Position& pos, MoveList& moves)
	{
		generate_black_pawn_attack_moves(pos, moves);
		generate_black_pawn_single_move_forward(pos.get_board(), moves);
		generate_black_pawn_double_move_forward(pos.get_board(), moves);
	}

	void generate_pawn_moves(const Position& pos, MoveList& moves)
	{
		switch (pos.get_side_to_move())
		{
			case Color::WHITE:
				generate_white_pawn_moves(pos, moves);
				break;
			case Color::BLACK:
				generate_black_pawn_moves(pos, moves);
				break;
			default:
				throw std::invalid_argument(
					"Could not generate pseudolegal moves: Invalid side to move");
		}
	}

	void generate_single_knight_moves(const Square from_square, const bitboard allies,
		MoveList& moves)
	{
		const bitboard knight_moves = bitboards::knight_attacks(
										bitboards::set_square(0, from_square)) & ~allies;
		for (const Square to_square : BitboardRange(knight_moves))
		{
			moves.push_back(Move{from_square, to_square});
		}
	}

	void generate_knight_moves(const Position& pos, MoveList& moves)
	{
		const bitboard allies = pos.get_board().get_bitboard(
			pos.get_side_to_move() == Color::WHITE ? Color::WHITE : Color::BLACK);
		const bitboard knights = pos.get_board().get_bitboard(Piece{
			pos.get_side_to_move(), PieceType::KNIGHT
		});

		for (const Square from_square : BitboardRange(knights))
		{
			generate_single_knight_moves(from_square, allies, moves);
		}
	}

	void generate_single_king_moves(const Square from_square, const bitboard allies,
		MoveList& moves)
	{
		const bitboard king_moves = bitboards::king_attacks(bitboards::set_square(0, from_square)) &
									~allies;
		for (const Square to_square : BitboardRange(king_moves))
		{
			moves.push_back(Move{from_square, to_square});
		}
	}

	void generate_king_moves(const Position& pos, MoveList& moves)
	{
		const bitboard allies = pos.get_board().get_bitboard(
			pos.get_side_to_move() == Color::WHITE ? Color::WHITE : Color::BLACK);
		const bitboard kings = pos.get_board().get_bitboard(Piece{
			pos.get_side_to_move(), PieceType::KING
		});
		for (const Square from_square : BitboardRange(kings))
		{
			generate_single_king_moves(from_square, allies, moves);
		}
	}

	void generate_single_bishop_moves(const Square from_square, const bitboard allies,
		const bitboard enemies, MoveList& moves)
	{
		const bitboard bishop_moves = bitboards::bishop_attacks(
										bitboards::set_square(0, from_square),
										enemies | allies) & ~allies;
		for (const Square to_square : BitboardRange(bishop_moves))
		{
			moves.push_back(Move{from_square, to_square});
		}
	}

	void generate_bishop_moves(const Position& pos, MoveList& moves)
	{
		const bitboard allies = pos.get_board().get_bitboard(
			pos.get_side_to_move() == Color::WHITE ? Color::WHITE : Color::BLACK);
		const bitboard enemies = pos.get_board().get_bitboard(
			pos.get_side_to_move() == Color::WHITE ? Color::BLACK : Color::WHITE);
		const bitboard bishops = pos.get_board().get_bitboard(Piece{
			pos.get_side_to_move(), PieceType::BISHOP
		});
		for (const Square from_square : BitboardRange(bishops))
		{
			generate_single_bishop_moves(from_square, allies, enemies, moves);
		}
	}

	void generate_single_rook_moves(const Square from_square, const bitboard allies,
		const bitboard enemies, MoveList& moves)
	{
		// Rooks move orthogonally; we use the same occupancy (allies | enemies) to calculate sliding attacks
		const bitboard rook_moves = bitboards::rook_attacks(
										bitboards::set_square(0, from_square),
										enemies | allies) & ~allies;

		for (const Square to_square : BitboardRange(rook_moves))
		{
			moves.push_back(Move{from_square, to_square});
		}
	}

	void generate_rook_moves(const Position& pos, MoveList& moves)
	{
		const bitboard allies = pos.get_board().get_bitboard(pos.get_side_to_move());
		const bitboard enemies = pos.get_board().get_bitboard(
			pos.get_side_to_move() == Color::WHITE ? Color::BLACK : Color::WHITE);
		const bitboard rooks = pos.get_board().get_bitboard(Piece{
			pos.get_side_to_move(), PieceType::ROOK
		});

		for (const Square from_square : BitboardRange(rooks))
		{
			generate_single_rook_moves(from_square, allies, enemies, moves);
		}
	}

	void generate_single_queen_moves(const Square from_square, const bitboard allies,
		const bitboard enemies, MoveList& moves)
	{
		// Queens combine both orthogonal and diagonal movement
		const bitboard queen_moves = bitboards::queen_attacks(
										bitboards::set_square(0, from_square),
										enemies | allies) & ~allies;

		for (const Square to_square : BitboardRange(queen_moves))
		{
			moves.push_back(Move{from_square, to_square});
		}
	}

	void generate_queen_moves(const Position& pos, MoveList& moves)
	{
		const bitboard allies = pos.get_board().get_bitboard(pos.get_side_to_move());
		const bitboard enemies = pos.get_board().get_bitboard(
			pos.get_side_to_move() == Color::WHITE ? Color::BLACK : Color::WHITE);
		const bitboard queens = pos.get_board().get_bitboard(Piece{
			pos.get_side_to_move(), PieceType::QUEEN
		});

		for (const Square from_square : BitboardRange(queens))
		{
			generate_single_queen_moves(from_square, allies, enemies, moves);
		}
	}

	void generate_white_castle_moves(const Position& pos, MoveList& moves)
	{
		if (pos.get_white_ks_castle() &&
			pos.get_piece(squares::E1) == pieces::WHITE_KING &&
			pos.get_piece(squares::H1) == pieces::WHITE_ROOK &&
			pos.get_piece(squares::F1) == std::nullopt &&
			pos.get_piece(squares::G1) == std::nullopt)
		{
			moves.push_back(Move{squares::E1, squares::G1});
		}

		if (pos.get_white_qs_castle() &&
			pos.get_piece(squares::E1) == pieces::WHITE_KING &&
			pos.get_piece(squares::A1) == pieces::WHITE_ROOK &&
			pos.get_piece(squares::D1) == std::nullopt &&
			pos.get_piece(squares::C1) == std::nullopt &&
			pos.get_piece(squares::B1) == std::nullopt)
		{
			moves.push_back(Move{squares::E1, squares::C1});
		}
	}

	void generate_black_castle_moves(const Position& pos, MoveList& moves)
	{
		if (pos.get_black_ks_castle() &&
			pos.get_piece(squares::E8) == pieces::BLACK_KING &&
			pos.get_piece(squares::H8) == pieces::BLACK_ROOK &&
			pos.get_piece(squares::F8) == std::nullopt &&
			pos.get_piece(squares::G8) == std::nullopt)
		{
			moves.push_back(Move{squares::E8, squares::G8});
		}

		if (pos.get_black_qs_castle() &&
			pos.get_piece(squares::E8) == pieces::BLACK_KING &&
			pos.get_piece(squares::A8) == pieces::BLACK_ROOK &&
			pos.get_piece(squares::D8) == std::nullopt &&
			pos.get_piece(squares::C8) == std::nullopt &&
			pos.get_piece(squares::B8) == std::nullopt)
		{
			moves.push_back(Move{squares::E8, squares::C8});
		}
	}

	void generate_castle_moves(const Position& pos, MoveList& moves)
	{
		switch (pos.get_side_to_move())
		{
			case Color::WHITE:
				generate_white_castle_moves(pos, moves);
				break;
			case Color::BLACK:
				generate_black_castle_moves(pos, moves);
				break;
			default:
				throw std::invalid_argument(
					"Could not generate pseudolegal moves: Invalid side to move");
		}
	}

	void generate_pseudolegal_moves(const Position& pos, MoveList& moves)
	{
		generate_pawn_moves(pos, moves);
		generate_knight_moves(pos, moves);
		generate_king_moves(pos, moves);
		generate_bishop_moves(pos, moves);
		generate_rook_moves(pos, moves);
		generate_queen_moves(pos, moves);
		generate_castle_moves(pos, moves);
	}

	bool is_white_castle_move(const Position& pos, const Move move)
	{
		if (pos.get_white_ks_castle() && move.from_square == squares::E1 && move.to_square
			== squares::G1 && pos.get_piece(squares::E1) == pieces::WHITE_KING)
		{
			return true;
		} else if (pos.get_white_qs_castle() && move.from_square == squares::E1 && move.
					to_square == squares::C1 && pos.get_piece(squares::E1) ==
					pieces::WHITE_KING)
		{
			return true;
		} else
		{
			return false;
		}
	}

	bool is_black_castle_move(const Position& pos, const Move move)
	{
		if (pos.get_black_ks_castle() && move.from_square == squares::E8 && move.to_square ==
			squares::G8 && pos.get_piece(squares::E8) == pieces::BLACK_KING)
		{
			return true;
		} else if (pos.get_black_qs_castle() && move.from_square == squares::E8 && move.to_square ==
					squares::C8 && pos.get_piece(squares::E8) == pieces::BLACK_KING)
		{
			return true;
		} else
		{
			return false;
		}
	}

	/**
	 * @brief Returns true if the move seems to be a castling move and thus needs extra checks
	 * for move legality.
	 */
	bool is_castling_move(const Position& pos, const Move move)
	{
		switch (pos.get_side_to_move())
		{
			case Color::WHITE:
				return is_white_castle_move(pos, move);
			case Color::BLACK:
				return is_black_castle_move(pos, move);
			default:
				throw std::invalid_argument(
					"Could not generate legal moves: Invalid side to move");
		}
	}

	[[nodiscard]] bitboard get_attacked_squares(const Board& board, const Color attacking_side)
	{
		const bitboard occupied = board.get_occupied();
		bitboard attacks = 0;

		const bitboard pawns = board.get_bitboard(Piece{attacking_side, PieceType::PAWN});
		attacks |= (attacking_side == Color::WHITE
						? bitboards::white_pawn_attacks(pawns)
						: bitboards::black_pawn_attacks(pawns));
		// Knights and Kings
		attacks |= bitboards::knight_attacks(board.get_bitboard(Piece{
			attacking_side, PieceType::KNIGHT
		}));
		attacks |= bitboards::king_attacks(
			board.get_bitboard(Piece{attacking_side, PieceType::KING}));

		// Sliding pieces (Sliders use the occupancy bitboard)
		attacks |= bitboards::bishop_attacks(board.get_bitboard(Piece{
			attacking_side, PieceType::BISHOP
		}), occupied);
		attacks |= bitboards::rook_attacks(
			board.get_bitboard(Piece{attacking_side, PieceType::ROOK}), occupied);
		attacks |= bitboards::queen_attacks(board.get_bitboard(Piece{
			attacking_side, PieceType::QUEEN
		}), occupied);

		return attacks;
	}

	/**
	 * @brief Tests if a castling move is legal.
	 *
	 * Since the king can never go into check, the legality check for castling is a bit more
	 * involved.
	 *
	 * This function assumes that there are no pieces between the king and the rook.
	 */
	bool is_castle_move_legal(const Position& pos, const Move move)
	{
		const bitboard attacked_squares = get_attacked_squares(pos.get_board(),
			pos.get_side_to_move() == Color::WHITE ? Color::BLACK : Color::WHITE);
		// White KS castle
		if (move == Move{squares::E1, squares::G1})
		{
			return !(bitboards::get_square(attacked_squares, squares::E1) ||
					bitboards::get_square(attacked_squares, squares::F1) ||
					bitboards::get_square(attacked_squares, squares::G1));
		}
		// White QS castle
		else if (move == Move{squares::E1, squares::C1})
		{
			return !(bitboards::get_square(attacked_squares, squares::E1) ||
					bitboards::get_square(attacked_squares, squares::D1) ||
					bitboards::get_square(attacked_squares, squares::C1));
		}
		// Black KS castle
		else if (move == Move{squares::E8, squares::G8})
		{
			return !(bitboards::get_square(attacked_squares, squares::E8) ||
					bitboards::get_square(attacked_squares, squares::F8) ||
					bitboards::get_square(attacked_squares, squares::G8));
		}
		// Black QS castle
		else if (move == Move{squares::E8, squares::C8})
		{
			return !(bitboards::get_square(attacked_squares, squares::E8) ||
					bitboards::get_square(attacked_squares, squares::D8) ||
					bitboards::get_square(attacked_squares, squares::C8));
		} else
		{
			throw std::invalid_argument("Invalid move received in is_castle_move_legal.");
		}
	}

	void generate_legal_moves(const Position& pos, MoveList& moves)
	{
		// 1. Reset the list and generate pseudo-legal moves
		moves.count = 0;		generate_pseudolegal_moves(pos, moves);

		// 2. Filter illegal moves in-place using two indices
		size_t write_idx = 0;
		for (size_t read_idx = 0; read_idx < moves.count; ++read_idx)
		{
			const Move m = moves.moves[read_idx];
			bool legal = true;

			if (is_castling_move(pos, m))
			{
				// Castling is illegal if currently in check or if the path is attacked
				if (pos.is_check() || !is_castle_move_legal(pos, m))
				{
					legal = false;
				}
			} else
			{
				// Test legality by simulating the move on a copy
				Position test_pos = pos;
				test_pos.perform_move(m);

				// perform_move() swaps the side; we must check if the king
				// is in check for the side that just moved
				test_pos.set_side_to_move(pos.get_side_to_move());

				if (test_pos.is_check())
				{
					legal = false;
				}
			}

			if (legal)
			{
				// Keep the move by writing it to the next available slot
				moves.moves[write_idx++] = m;
			}
		}

		// 3. Update the move count to the final number of legal moves
		moves.count = write_idx;
	}

	constexpr size_t PERFT_MOVE_VECTOR_SIZE = 256;

	void perft_test_recursive(const Position& current_position, const unsigned int current_depth,
		std::vector<uint64_t>& results, MoveList& reusable_move_vector)
	{
		reusable_move_vector.clear();
		generate_legal_moves(current_position, reusable_move_vector);
		results[current_depth] += reusable_move_vector.size();

		if (current_depth < results.size() - 1)
		{
			MoveList depth_n_reusable_vector;
			for (const Move m : reusable_move_vector)
			{
				Position test_pos = current_position;
				test_pos.perform_move(m);
				perft_test_recursive(test_pos, current_depth + 1, results, depth_n_reusable_vector);
			}
		}
	}

	std::vector<uint64_t> perft_test(const Position& position, const unsigned int depth)
	{
		std::vector<uint64_t> results;
		results.resize(depth + 1);
		results[0] = 1;

		MoveList reusable_move_vector;
		perft_test_recursive(position, 1, results, reusable_move_vector);

		return results;
	}
}
