#pragma once
#include "chessboard.h"
#include <QVector>
#include <QPoint>

class AI;  // forward declaration for friend declaration

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
    Move() : fromRow(-1), fromCol(-1), toRow(-1), toCol(-1) {}
    Move(int fr, int fc, int tr, int tc) : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc) {}
    bool isValid() const {
        return fromRow >= 0 && fromCol >= 0 && toRow >= 0 && toCol >= 0;
    }
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
    friend class AI;  // AI uses applyMove/undoMove for zero-allocation search
public:
    ChessGame();
    void init();
    PieceColor getCurrentTurn() const { return currentTurn_; }
    void switchTurn();
    const ChessBoard& getBoard() const { return board_; }
    ChessBoard& getBoard() { return board_; }

    QVector<Move> getAllLegalMoves() const;
    QVector<Move> getLegalMovesForPiece(int row, int col) const;

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
    QVector<Move> getPseudoLegalMovesAt(const ChessBoard& b, int row, int col) const;

    bool isMoveSafe(const Move& move) const;
    bool wouldBeInCheckAfterMove(const Move& move, PieceColor color) const;

    // Efficient make/unmake for AI search — avoids full board copy. Non-const.
    void applyMove(const Move& move);
    void undoMove(const Move& move, const ChessPiece& captured);

    // Internal: apply move in-place, run a predicate, undo. const-correct.
    template<typename F>
    bool probeMove(const Move& move, F&& predicate) const
    {
        auto& self = const_cast<ChessGame&>(*this);
        const ChessPiece mover = board_.getPiece(move.fromRow, move.fromCol);
        const ChessPiece captured = board_.getPiece(move.toRow, move.toCol);
        self.board_.setPiece(move.toRow, move.toCol, mover);
        self.board_.setPiece(move.fromRow, move.fromCol, ChessPiece());

        bool result = predicate(board_);

        self.board_.setPiece(move.fromRow, move.fromCol, mover);
        self.board_.setPiece(move.toRow, move.toCol, captured);
        return result;
    }

    int countPiecesBetween(const ChessBoard& b, int r1, int c1, int r2, int c2) const;
};
