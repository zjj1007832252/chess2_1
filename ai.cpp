#include "ai.h"
#include <climits>
#include <algorithm>

AI::AI(PieceColor color) : aiColor_(color), depth_(3) {}

Move AI::getBestMove(ChessGame& game) {
    auto moves = generateAllLegalMoves(game);
    if (moves.isEmpty()) return Move();

    // 检查是否将军
    bool isCheck = game.isInCheck(aiColor_);

    // 如果将军，减少搜索深度以加快响应
    int effectiveDepth = depth_;
    if (isCheck) {
        effectiveDepth = qMax(1, depth_ - 1);  // 将军时减少一层深度
    }

    sortMoves(moves, game.getBoard());

    Move bestMove = moves.first();
    int bestScore = INT_MIN;

    for (auto& move : moves) {
        ChessGame savedGame = game;

        game.makeMove(move);
        int score = minimax(game, effectiveDepth - 1, INT_MIN, INT_MAX, false);

        game = savedGame;

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

int AI::minimax(ChessGame& game, int depth, int alpha, int beta, bool maximizing) {
    if (depth == 0) {
        return evaluate(game.getBoard());
    }

    auto moves = generateAllLegalMoves(game);
    if (moves.isEmpty()) {
        if (maximizing) return -100000 + depth;
        else return 100000 - depth;
    }

    sortMoves(moves, game.getBoard());

    if (maximizing) {
        int maxEval = INT_MIN;
        for (auto& move : moves) {
            ChessGame savedGame = game;
            game.makeMove(move);
            int eval = minimax(game, depth - 1, alpha, beta, false);
            game = savedGame;
            maxEval = qMax(maxEval, eval);
            alpha = qMax(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (auto& move : moves) {
            ChessGame savedGame = game;
            game.makeMove(move);
            int eval = minimax(game, depth - 1, alpha, beta, true);
            game = savedGame;
            minEval = qMin(minEval, eval);
            beta = qMin(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

QVector<Move> AI::generateAllLegalMoves(const ChessGame& game) const {
    QVector<Move> legalMoves;
    const auto& board = game.getBoard();
    PieceColor turnColor = game.getCurrentTurn();

    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            auto p = board.getPiece(r, c);
            if (p.isEmpty() || p.getColor() != turnColor) continue;

            auto pseudoMoves = game.getPseudoLegalMovesAt(board, r, c);
            for (auto& m : pseudoMoves) {
                // 模拟走法，检查是否安全
                ChessBoard tempBoard = board;
                tempBoard.movePiece(m.fromRow, m.fromCol, m.toRow, m.toCol);

                // 找到己方将/帅
                QPoint myGen(-1, -1);
                for (int rr = 0; rr < 10; ++rr) {
                    for (int cc = 0; cc < 9; ++cc) {
                        auto pp = tempBoard.getPiece(rr, cc);
                        if (pp.getType() == PieceType::General && pp.getColor() == turnColor) {
                            myGen = QPoint(cc, rr);
                        }
                    }
                }
                if (myGen.x() < 0) continue;

                // 检查对方是否能攻击到将/帅
                PieceColor enemyColor = (turnColor == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
                bool safe = true;
                for (int er = 0; er < 10 && safe; ++er) {
                    for (int ec = 0; ec < 9 && safe; ++ec) {
                        auto ep = tempBoard.getPiece(er, ec);
                        if (ep.isEmpty() || ep.getColor() != enemyColor) continue;

                        auto eMoves = game.getPseudoLegalMovesAt(tempBoard, er, ec);
                        for (auto& em : eMoves) {
                            if (em.toRow == myGen.x() && em.toCol == myGen.y()) {
                                safe = false;
                                break;
                            }
                        }
                    }
                }

                if (safe) {
                    legalMoves.append(m);
                }
            }
        }
    }

    return legalMoves;
}

void AI::sortMoves(QVector<Move>& moves, const ChessBoard& board) const {
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        // 优先吃子
        int va = board.getPiece(a.toRow, a.toCol).getBaseValue();
        int vb = board.getPiece(b.toRow, b.toCol).getBaseValue();

        // 如果是吃子，给额外加分
        if (va > 0) va += 1000;
        if (vb > 0) vb += 1000;

        // 保护重要棋子
        auto pieceA = board.getPiece(a.fromRow, a.fromCol);
        auto pieceB = board.getPiece(b.fromRow, b.fromCol);
        if (pieceA.getType() == PieceType::General) va -= 500;  // 不轻易移动将/帅
        if (pieceB.getType() == PieceType::General) vb -= 500;

        return va > vb;
    });
}

int AI::evaluate(const ChessBoard& board) const {
    int score = 0;
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            auto p = board.getPiece(r, c);
            if (p.isEmpty()) continue;

            int baseValue = p.getBaseValue();
            int posValue = getPositionValue(p.getType(), r, c, p.getColor());
            int total = baseValue + posValue;

            if (p.getColor() == aiColor_) score += total;
            else score -= total;
        }
    }
    return score;
}

int AI::getPositionValue(PieceType type, int row, int col, PieceColor color) const {
    int value = 0;

    if (type == PieceType::Soldier) {
        if (color == PieceColor::Red && row < 5) value += 50;
        if (color == PieceColor::Black && row > 4) value += 50;
        if (color == PieceColor::Red && row <= 2) value += 100;
        if (color == PieceColor::Black && row >= 7) value += 100;
        if (col == 4) value += 20;
    }

    if (type == PieceType::Horse) {
        if (col >= 3 && col <= 5) value += 30;
        if (color == PieceColor::Red && row < 5) value += 20;
        if (color == PieceColor::Black && row > 4) value += 20;
    }

    if (type == PieceType::Cannon) {
        if (col == 4) value += 20;
    }

    return value;
}
