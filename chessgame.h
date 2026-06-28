#pragma once
#include "chessboard.h"
#include <QVector>
#include <QPoint>

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
    Move() : fromRow(0), fromCol(0), toRow(0), toCol(0) {}
    Move(int fr, int fc, int tr, int tc) : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc) {}
    bool operator==(const Move& o) const {
        return fromRow==o.fromRow && fromCol==o.fromCol && toRow==o.toRow && toCol==o.toCol;
    }
};

struct RecordedMove {
    Move move;
    ChessPiece captured;
    RecordedMove() {}
    RecordedMove(const Move& m, const ChessPiece& c) : move(m), captured(c) {}
};

class ChessGame {
public:
    ChessGame();
    void init();
    PieceColor getCurrentTurn() const { return currentTurn_; }
    void switchTurn();
    const ChessBoard& getBoard() const { return board_; }
    ChessBoard& getBoard() { return board_; }

    QVector<Move> getAllLegalMoves() const;
    QVector<Move> getLegalMovesForPiece(int row, int col) const;
    QVector<Move> getPseudoLegalMovesAt(const ChessBoard& b, int row, int col) const;

    bool makeMove(const Move& move);
    bool undoMove();

    bool isInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color);
    bool isGameOver();
    PieceColor getWinner() const { return winner_; }
    QString getMoveNotation(const Move& move) const;
    QVector<RecordedMove> getMoveHistory() const { return moveHistory_; }
    void reset();
    bool generalsFacingEachOther() const;

private:
    ChessBoard board_;
    PieceColor currentTurn_;
    PieceColor winner_;
    bool gameOver_;
    QVector<RecordedMove> moveHistory_;

    QVector<Move> getPseudoLegalMoves(int row, int col) const;
    QVector<Move> getGeneralMoves(int row, int col) const;
    QVector<Move> getAdvisorMoves(int row, int col) const;
    QVector<Move> getElephantMoves(int row, int col) const;
    QVector<Move> getHorseMoves(int row, int col) const;
    QVector<Move> getChariotMoves(int row, int col) const;
    QVector<Move> getCannonMoves(int row, int col) const;
    QVector<Move> getSoldierMoves(int row, int col) const;

    bool isMoveSafe(const Move& move) const;
    bool wouldBeInCheckAfterMove(const Move& move, PieceColor color) const;
    QPoint findGeneral(const ChessBoard& b, PieceColor color) const;
    int countPiecesBetween(const ChessBoard& b, int r1, int c1, int r2, int c2) const;
};
