#include "chessboard.h"
#include <QDebug>

ChessBoard::ChessBoard() {
    board_.resize(10);
    for (int i = 0; i < 10; ++i) {
        board_[i].resize(9);
    }
}

void ChessBoard::initBoard() {
    // 清空棋盘
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 9; ++j)
            board_[i][j] = ChessPiece();

    // 黑方（上方）
    place(0, 0, PieceType::Chariot,  PieceColor::Black);
    place(0, 1, PieceType::Horse,    PieceColor::Black);
    place(0, 2, PieceType::Elephant, PieceColor::Black);
    place(0, 3, PieceType::Advisor,  PieceColor::Black);
    place(0, 4, PieceType::General,  PieceColor::Black);
    place(0, 5, PieceType::Advisor,  PieceColor::Black);
    place(0, 6, PieceType::Elephant, PieceColor::Black);
    place(0, 7, PieceType::Horse,    PieceColor::Black);
    place(0, 8, PieceType::Chariot,  PieceColor::Black);
    place(2, 1, PieceType::Cannon,   PieceColor::Black);
    place(2, 7, PieceType::Cannon,   PieceColor::Black);
    place(3, 0, PieceType::Soldier,  PieceColor::Black);
    place(3, 2, PieceType::Soldier,  PieceColor::Black);
    place(3, 4, PieceType::Soldier,  PieceColor::Black);
    place(3, 6, PieceType::Soldier,  PieceColor::Black);
    place(3, 8, PieceType::Soldier,  PieceColor::Black);

    // 红方（下方）
    place(9, 0, PieceType::Chariot,  PieceColor::Red);
    place(9, 1, PieceType::Horse,    PieceColor::Red);
    place(9, 2, PieceType::Elephant, PieceColor::Red);
    place(9, 3, PieceType::Advisor,  PieceColor::Red);
    place(9, 4, PieceType::General,  PieceColor::Red);
    place(9, 5, PieceType::Advisor,  PieceColor::Red);
    place(9, 6, PieceType::Elephant, PieceColor::Red);
    place(9, 7, PieceType::Horse,    PieceColor::Red);
    place(9, 8, PieceType::Chariot,  PieceColor::Red);
    place(7, 1, PieceType::Cannon,   PieceColor::Red);
    place(7, 7, PieceType::Cannon,   PieceColor::Red);
    place(6, 0, PieceType::Soldier,  PieceColor::Red);
    place(6, 2, PieceType::Soldier,  PieceColor::Red);
    place(6, 4, PieceType::Soldier,  PieceColor::Red);
    place(6, 6, PieceType::Soldier,  PieceColor::Red);
    place(6, 8, PieceType::Soldier,  PieceColor::Red);
}

ChessPiece ChessBoard::getPiece(int row, int col) const {
    if (!isValidPos(row, col)) return ChessPiece();
    return board_[row][col];
}

void ChessBoard::setPiece(int row, int col, const ChessPiece& piece) {
    if (isValidPos(row, col)) {
        board_[row][col] = piece;
    }
}

void ChessBoard::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidPos(fromRow, fromCol) || !isValidPos(toRow, toCol)) return;
    board_[toRow][toCol] = board_[fromRow][fromCol];
    board_[fromRow][fromCol] = ChessPiece();
}

void ChessBoard::clearPiece(int row, int col) {
    if (isValidPos(row, col)) {
        board_[row][col] = ChessPiece();
    }
}

bool ChessBoard::isValidPos(int row, int col) {
    return row >= 0 && row < 10 && col >= 0 && col < 9;
}

QPoint ChessBoard::findGeneral(PieceColor color) const {
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (board_[r][c].getType() == PieceType::General &&
                board_[r][c].getColor() == color) {
                return QPoint(r, c);
            }
        }
    }
    return QPoint(-1, -1);
}

void ChessBoard::place(int row, int col, PieceType type, PieceColor color) {
    if (isValidPos(row, col)) {
        board_[row][col] = ChessPiece(type, color);
    }
}
