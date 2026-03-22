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
#ifndef CHESS_AGAIN_POSITION_H
#define CHESS_AGAIN_POSITION_H

#include <string_view>

#include "board.h"
#include "move.h"

namespace chess_again
{
	namespace detail
	{
		constexpr bool is_from_square_en_passant_valid_white(const Square from_square,
			const Square to_square)
		{
			const uint8_t from_rank = static_cast<uint8_t>(from_square.rank);
			const uint8_t to_rank = static_cast<uint8_t>(to_square.rank);
			const uint8_t from_file = static_cast<uint8_t>(from_square.file);
			const uint8_t to_file = static_cast<uint8_t>(to_square.file);

			return from_rank + 1 == to_rank &&
					(from_file - 1 == to_file || from_file + 1 == to_file);
		}

		constexpr bool is_from_square_en_passant_valid_black(const Square from_square,
			const Square to_square)
		{
			const uint8_t from_rank = static_cast<uint8_t>(from_square.rank);
			const uint8_t to_rank = static_cast<uint8_t>(to_square.rank);
			const uint8_t from_file = static_cast<uint8_t>(from_square.file);
			const uint8_t to_file = static_cast<uint8_t>(to_square.file);

			return from_rank - 1 == to_rank &&
					(from_file - 1 == to_file || from_file + 1 == to_file);
		}

		/**
		 * @brief Returns true if the from_square is 1 diagonal away in the correct direction based
		 * on side_to_move.
		 */
		constexpr bool is_from_square_en_passant_valid(const Square from_square,
			const Square en_passant_square, const Color side_to_move)
		{
			switch (side_to_move)
			{
				case Color::WHITE:
					return is_from_square_en_passant_valid_white(from_square, en_passant_square);
				case Color::BLACK:
					return is_from_square_en_passant_valid_black(from_square, en_passant_square);
				default:
					throw std::invalid_argument(
						"Could not check if from square is en passant valid: Invalid side to move");
			}
		}

		/**
		 * @brief Given a castling move generates the square the rook was coming from.
		 */
		constexpr Square gen_rook_from_square(const Move move)
		{
			if (move.to_square == squares::C1)
			{
				return squares::A1;
			} else if (move.to_square == squares::C8)
			{
				return squares::A8;
			} else if (move.to_square == squares::G1)
			{
				return squares::H1;
			} else if (move.to_square == squares::G8)
			{
				return squares::H8;
			} else
			{
				throw std::invalid_argument(
					"Could not generate rook from square: Invalid castling move");
			}
		}

		/**
		 * @brief Given a castling move generates the square the rook should go to.
		 */
		constexpr Square gen_rook_to_square(const Move move)
		{
			if (move.to_square == squares::C1)
			{
				return squares::D1;
			} else if (move.to_square == squares::C8)
			{
				return squares::D8;
			} else if (move.to_square == squares::G1)
			{
				return squares::F1;
			} else if (move.to_square == squares::G8)
			{
				return squares::F8;
			} else
			{
				throw std::invalid_argument(
					"Could not generate rook to square: Invalid castling move");
			}
		}
	}

	/**
	 * @brief Position represents all parts Forsyth-Edwards Notation chess position.
	 *
	 * Position is not guaranteed to be a valid chess position, but calling certain functions with
	 * invalid chess positions may result in undefined behaviour.
	 */
	class Position
	{
	private:
		bool white_ks_castle = false;
		bool white_qs_castle = false;
		bool black_ks_castle = false;
		bool black_qs_castle = false;
		Color side_to_move = Color::WHITE;
		std::optional<Square> en_passant = std::nullopt;
		uint16_t halfmove = 0;
		uint16_t fullmove = 1;
		Board board;

		constexpr void swap_side_to_move()
		{
			switch (get_side_to_move())
			{
				case Color::WHITE:
					set_side_to_move(Color::BLACK);
					break;
				case Color::BLACK:
					set_side_to_move(Color::WHITE);
					break;
				default:
					throw std::invalid_argument(
						"Could not swap side to move: Invalid side to move");
			}
		}

		constexpr void remove_white_castling_rights(const Square from_square)
		{
			if (from_square == squares::E1)
			{
				set_white_ks_castle(false);
				set_white_qs_castle(false);
			} else if (from_square == squares::A1)
			{
				set_white_qs_castle(false);
			} else if (from_square == squares::H1)
			{
				set_white_ks_castle(false);
			}
		}

		constexpr void remove_black_castling_rights(const Square from_square)
		{
			if (from_square == squares::E8)
			{
				set_black_ks_castle(false);
				set_black_qs_castle(false);
			} else if (from_square == squares::A8)
			{
				set_black_qs_castle(false);
			} else if (from_square == squares::H8)
			{
				set_black_ks_castle(false);
			}
		}

		/**
		 * @brief Removes any castling rights that need to be removed after a piece is moved from
		 * the from-square.
		 */
		constexpr void remove_castling_rights(const Square from_square)
		{
			if (get_side_to_move() == Color::WHITE)
			{
				remove_white_castling_rights(from_square);
			} else if (get_side_to_move() == Color::BLACK)
			{
				remove_black_castling_rights(from_square);
			}
		}

		/**
		 * @brief Increments move counters, updates castling rights, swaps side to move.
		 *
		 * Does not update castling rights or en-passant.
		 */
		constexpr void perform_end_of_move(const Move move)
		{
			set_halfmove(get_halfmove() + 1);
			if (side_to_move == Color::BLACK)
			{
				set_fullmove(get_fullmove() + 1);
			}

			remove_castling_rights(move.from_square);
			swap_side_to_move();
		}

		/**
		 * @brief Returns true if moving the piece to the square results in an en-passant capture.
		 *
		 * The piece must be the same color as the side to move and be a pawn. The to-square
		 * must match the en-passant to-square, and the from_square must be 1 spot diagonal
		 * from the to-square.
		 */
		constexpr bool is_en_passant_move(const Piece piece_to_move, const Move move) const
		{
			if (piece_to_move.type == PieceType::PAWN &&
				piece_to_move.color == side_to_move &&
				move.to_square == get_en_passant())
			{
				return detail::is_from_square_en_passant_valid(move.from_square,
					get_en_passant().value(), get_side_to_move());
			} else
			{
				return false;
			}
		}

		/**
		 * @brief Performs an en-passant capture.
		 *
		 * @param move Should be validated by is_en_passant_move.
		 */
		constexpr void perform_en_passant_move(const Move move)
		{
			clear_piece(move.from_square);

			const Square square_to_clear{
				move.to_square.file,
				static_cast<Rank>(static_cast<uint8_t>(move.to_square.rank) + (
									get_side_to_move() == Color::WHITE ? -1 : 1))
			};

			clear_piece(square_to_clear);

			const Piece new_piece{get_side_to_move(), move.promotion.value_or(PieceType::PAWN)};
			board.set_piece(new_piece, move.to_square);
		}

		constexpr bool is_white_ks_castle(const Move move) const
		{
			return get_white_ks_castle() &&
					move.from_square == squares::E1 &&
					move.to_square == squares::G1 &&
					get_piece(move.from_square) == pieces::WHITE_KING &&
					get_piece(squares::H1) == pieces::WHITE_ROOK &&
					!move.promotion.has_value();
		}

		constexpr bool is_white_qs_castle(const Move move) const
		{
			return get_white_qs_castle() &&
					move.from_square == squares::E1 &&
					move.to_square == squares::C1 &&
					get_piece(move.from_square) == pieces::WHITE_KING &&
					get_piece(squares::A1) == pieces::WHITE_ROOK &&
					!move.promotion.has_value();
		}

		constexpr bool is_black_ks_castle(const Move move) const
		{
			return get_black_ks_castle() &&
					move.from_square == squares::E8 &&
					move.to_square == squares::G8 &&
					get_piece(move.from_square) == pieces::BLACK_KING &&
					get_piece(squares::H8) == pieces::BLACK_ROOK &&
					!move.promotion.has_value();
		}

		constexpr bool is_black_qs_castle(const Move move) const
		{
			return get_black_qs_castle() &&
					move.from_square == squares::E8 &&
					move.to_square == squares::C8 &&
					get_piece(move.from_square) == pieces::BLACK_KING &&
					get_piece(squares::A8) == pieces::BLACK_ROOK &&
					!move.promotion.has_value();
		}

		/**
		 * @brief Returns true if the move could be a castling move in the current board state.
		 *
		 * The from-squares and to-squares must be valid castling locations, the castling rights
		 * must still be valid, and the king and rook must still be on their starting squares. There
		 * cannot be a promotion either.
		 */
		constexpr bool is_castling_move(const Move move) const
		{
			switch (get_side_to_move())
			{
				case Color::WHITE:
					return is_white_ks_castle(move) || is_white_qs_castle(move);
				case Color::BLACK:
					return is_black_ks_castle(move) || is_black_qs_castle(move);
				default:
					throw std::invalid_argument(
						"Could not check if move is castling move: Invalid side to move");
			}
		}

		/**
		 * @brief Performs a castling move.
		 *
		 * @param move Should be validated by is_castling_move.
		 */
		constexpr void perform_castling_move(const Move move)
		{
			board.clear_piece(move.from_square);
			board.set_piece(Piece{get_side_to_move(), PieceType::KING}, move.to_square);

			const Square rook_from_square = detail::gen_rook_from_square(move);
			const Square rook_to_square = detail::gen_rook_to_square(move);

			board.clear_piece(rook_from_square);
			board.set_piece(Piece{get_side_to_move(), PieceType::ROOK}, rook_to_square);
		}

		constexpr std::optional<Square> calculate_new_en_passant(const Piece piece_to_move,
			const Move move) const
		{
			if (piece_to_move != Piece{get_side_to_move(), PieceType::PAWN}
				|| move.from_square.file != move.to_square.file)
			{
				return std::nullopt;
			} else
			{
				if (get_side_to_move() == Color::WHITE &&
					move.from_square.rank == Rank::TWO &&
					move.to_square.rank == Rank::FOUR)
				{
					return Square{move.from_square.file, Rank::THREE};
				} else if (get_side_to_move() == Color::BLACK &&
							move.from_square.rank == Rank::SEVEN &&
							move.to_square.rank == Rank::FIVE)
				{
					return Square{move.from_square.file, Rank::SIX};
				} else
				{
					return std::nullopt;
				}
			}
		}

		/**
		 * @brief If a move is neither an en-passant, or castling move then standard move rules
		 * apply.
		 *
		 * En passant is set if a pawn moves forward two from its starting rank.
		 */
		constexpr void perform_standard_move(const Piece piece_to_move, const Move move)
		{
			board.clear_piece(move.from_square);
			board.set_piece(Piece{piece_to_move.color, move.promotion.value_or(piece_to_move.type)},
				move.to_square);
			const std::optional<Square> new_en_passant = calculate_new_en_passant(piece_to_move,
				move);
			if (new_en_passant.has_value())
			{
				set_en_passant(new_en_passant.value());
			} else
			{
				clear_en_passant();
			}
		}

		/**
		 * @brief Sets the castling rights based on a 1-4 letter string.
		 *
		 * @throws std::invalid_argument If the castling cannot be parsed or are invalid.
		 */
		void set_castling_rights(std::string_view castling_rights);

	public:
		/**
		 * @brief The starting chess position in Forsyth-Edwards Notation.
		 */
		static constexpr std::string_view DEFAULT_FEN =
				"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

		/**
		 * @brief Creates an empty position.
		 *
		 * There are no castling rights, no en-passant, and no pieces on the board. The side to move
		 * is white, the half-move counter is set to 0, and the full move counter is set to 1.
		 */
		[[nodiscard]] constexpr Position() = default;

		/**
		 * @brief Creates a position from an FEN string.
		 *
		 * @param fen Forsyth–Edwards Notation formatted board position.
		 * @throws std::invalid_argument When fen is invalid.
		 */
		[[nodiscard]] explicit Position(std::string_view fen);

		/**
        * @brief Gets White's kingside castling rights.
        */
		[[nodiscard]] constexpr bool get_white_ks_castle() const
		{
			return white_ks_castle;
		}

		/**
		* @brief Gets White's queenside castling rights.
		*/
		[[nodiscard]] constexpr bool get_white_qs_castle() const
		{
			return white_qs_castle;
		}

		/**
		* @brief Gets Black's kingside castling rights.
		*/
		[[nodiscard]] constexpr bool get_black_ks_castle() const
		{
			return black_ks_castle;
		}

		/**
		* @brief Gets Black's queenside castling rights.
		*/
		[[nodiscard]] constexpr bool get_black_qs_castle() const
		{
			return black_qs_castle;
		}

		/**
		* @brief Gets the color of the side currently to move.
		*/
		[[nodiscard]] constexpr Color get_side_to_move() const
		{
			return side_to_move;
		}

		/**
		* @brief Gets the current en passant target square.
		*/
		[[nodiscard]] constexpr std::optional<Square> get_en_passant() const
		{
			return en_passant;
		}

		/**
		* @brief Gets the current halfmove clock value.
		*/
		[[nodiscard]] constexpr uint16_t get_halfmove() const
		{
			return halfmove;
		}

		/**
		* @brief Gets the current fullmove counter.
		*/
		[[nodiscard]] constexpr uint16_t get_fullmove() const
		{
			return fullmove;
		}

		/**
		* @brief Sets White's kingside castling rights.
		*/
		constexpr void set_white_ks_castle(const bool value)
		{
			white_ks_castle = value;
		}

		/**
		* @brief Sets White's queenside castling rights.
		*/
		constexpr void set_white_qs_castle(const bool value)
		{
			white_qs_castle = value;
		}

		/**
		* @brief Sets Black's kingside castling rights.
		*/
		constexpr void set_black_ks_castle(const bool value)
		{
			black_ks_castle = value;
		}

		/**
		* @brief Sets Black's queenside castling rights.
		*/
		constexpr void set_black_qs_castle(const bool value)
		{
			black_qs_castle = value;
		}

		/**
		* @brief Sets the side currently to move.
		*/
		constexpr void set_side_to_move(const Color value)
		{
			this->side_to_move = value;
		}

		/**
		* @brief Sets the en passant target square.
		*/
		constexpr void set_en_passant(const Square square)
		{
			en_passant = square;
		}

		/**
		* @brief Clears the en passant target square.
		*/
		constexpr void clear_en_passant()
		{
			en_passant = std::nullopt;
		}

		/**
		* @brief Sets the half-move clock value.
		*/
		constexpr void set_halfmove(const uint16_t value)
		{
			halfmove = value;
		}

		/**
		* @brief Sets the full-move counter value.
		*/
		constexpr void set_fullmove(const uint16_t value)
		{
			fullmove = value;
		}

		/**
		 * @brief Gets the piece at the given square in the position.
		 */
		[[nodiscard]] constexpr std::optional<Piece> get_piece(const Square square) const
		{
			return board.get_piece(square);
		}

		/**
		 * @brief Sets a piece on the board at the given square.
		 */
		constexpr void set_piece(const Piece piece, const Square square)
		{
			board.set_piece(piece, square);
		}

		/**
		 * @brief Clears the piece (if any) on the given square.
		 */
		constexpr void clear_piece(const Square square)
		{
			board.clear_piece(square);
		}

		/**
		 * @brief Returns a reference to the internal piece representation of the position.
		 */
		[[nodiscard]] constexpr const Board& get_board() const
		{
			return board;
		}

		/**
		 * @brief Returns true if the side to move has a king in check.
		 */
		[[nodiscard]] bool is_check() const
		{
			const bitboard kings = get_board().get_bitboard(Piece{
				get_side_to_move(), PieceType::KING
			});
			const bitboard occupied = get_board().get_occupied();
			const Color opposing_color = get_side_to_move() == Color::WHITE
											? Color::BLACK
											: Color::WHITE;

			return (kings & bitboards::queen_attacks(
						get_board().get_bitboard(Piece{opposing_color, PieceType::QUEEN}),
						occupied)) != 0 ||
					(kings & bitboards::bishop_attacks(
						get_board().get_bitboard(Piece{opposing_color, PieceType::BISHOP}),
						occupied)) != 0 ||
					(kings & bitboards::rook_attacks(
						get_board().get_bitboard(Piece{opposing_color, PieceType::ROOK}),
						occupied)) != 0 ||
					(kings & bitboards::knight_attacks(get_board().get_bitboard(Piece{
						opposing_color, PieceType::KNIGHT
					}))) != 0 ||
					(kings & (opposing_color == Color::WHITE
								? bitboards::white_pawn_attacks(get_board().get_bitboard(Piece{
									opposing_color, PieceType::PAWN
								}))
								: bitboards::black_pawn_attacks(get_board().get_bitboard(Piece{
									opposing_color, PieceType::PAWN
								})))) != 0 ||
					(kings & bitboards::king_attacks(get_board().get_bitboard(Piece{
						opposing_color, PieceType::KING
					}))) != 0;
		}

		/**
		 * @brief Performs the given move whether or not it is legal.
		 *
		 * All legal chess moves given to this function are performed as they would be in a normal
		 * game of chess. While <b>it is recommended to only pass legal moves into this
		 * function,</b> fields are updated as follows regardless of the legality of the move:
		 *
		 *  1. Side to move is swapped.
		 *  2. If a piece is moved off a square that a rook or king normally starts on, the
		 *  corresponding castling right for the side to move is set to false.
		 *  3. If a pawn moves from its starting rank forward two squares, the en-passant square
		 *  is set to the square it jumped over. Otherwise, the en-passant square is cleared.
		 *  4. The half-move counter increments by one.
		 *  5. If the side to move was black and swapped to white, the full-move counter increments
		 *  by one.
		 *
		 * En-passant captures are only performed if the piece moving has the same color as the side
		 * to move, is a pawn, and is attacking 1-square diagonal from the to-square. In which case
		 * the square directly above or below the en-passant square is cleared, depending on
		 * the side that is moving.
		 *
		 * Castle moves are only performed if the piece moving has the same color as the side to
		 * move, and the king and rook are on their starting squares. The space in between the king
		 * and rook is not checked. The king and rook squares are simply cleared, and their
		 * destinations are filled in with a king and rook.
		 *
		 * If a move is not legal, the default logic is to clear the piece on the from-square and
		 * replace the piece on the to square with the cleared piece. If there is a promotion, the
		 * piece is promoted. If the piece that was moved was a pawn on its starting rank, and it
		 * moved forward two spaced, en-passant is set even if the pawn was promoted.
		 *
		 * If there is no piece on the from-square, then the board is not changed but, move
		 * counters, the side to move, castling rights, and en_passant are all updated.
		 *
		 * Skipping the check for legality gives big savings in chess engine development. It is on
		 * the user of this library to make sure moves are legal before passing them to this
		 * function.
		 */
		constexpr void perform_move(const Move move)
		{
			const std::optional<Piece> optional_piece_to_move = get_piece(move.from_square);
			// Do nothing if there is no piece to move.
			if (!optional_piece_to_move.has_value())
			{
				clear_en_passant();
				perform_end_of_move(move);
				return;
			}

			const Piece piece_to_move = optional_piece_to_move.value();

			// Perform en passant moves
			if (is_en_passant_move(piece_to_move, move))
			{
				perform_en_passant_move(move);
				clear_en_passant();
			}
			// Perform castling moves
			else if (is_castling_move(move))
			{
				perform_castling_move(move);
				clear_en_passant();
			} else
			{
				perform_standard_move(piece_to_move, move);
			}

			perform_end_of_move(move);
		}
	};
}

#endif //CHESS_AGAIN_POSITION_H
