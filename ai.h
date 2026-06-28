#pragma once
#include "chessgame.h"
#include <QVector>

class AI {
public:
    AI(PieceColor color);

    // 获取最佳走法
    Move getBestMove(ChessGame& game);

    // 设置AI难度（搜索深度）
    void setDepth(int d) { depth_ = d; }
    int getDepth() const { return depth_; }

private:
    PieceColor aiColor_;
    int depth_;

    // 评估当前局面分数（对AI有利为正）
    int evaluate(const ChessBoard& board) const;

    // Minimax with alpha-beta pruning
    int minimax(ChessGame& game, int depth, int alpha, int beta, bool maximizing);

    // 生成所有合法走法
    QVector<Move> generateAllLegalMoves(const ChessGame& game) const;

    // 走法排序（提高剪枝效率）
    void sortMoves(QVector<Move>& moves, const ChessBoard& board) const;

    // 位置价值表
    int getPositionValue(PieceType type, int row, int col, PieceColor color) const;
};
