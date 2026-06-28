#pragma once
#include "chesspiece.h"
#include <QVector>
#include <QPoint>

// 棋盘：10行 x 9列
// row 0-9 (0是黑方底线, 9是红方底线)
// col 0-8

class ChessBoard {
public:
    ChessBoard();

    // 初始化标准开局布局
    void initBoard();

    // 获取指定位置的棋子
    ChessPiece getPiece(int row, int col) const;

    // 放置棋子
    void setPiece(int row, int col, const ChessPiece& piece);

    // 移动棋子（从 from 到 to）
    void movePiece(int fromRow, int fromCol, int toRow, int toCol);

    // 清空指定位置
    void clearPiece(int row, int col);

    // 交换两个位置
    void swapPieces(int r1, int c1, int r2, int c2);

    // 检查坐标是否在棋盘内
    static bool isValidPos(int row, int col);

    // 获取红方帅的位置
    QPoint getRedGeneralPos() const;

    // 获取黑方将的位置
    QPoint getBlackGeneralPos() const;

    // 获取棋盘副本
    ChessBoard clone() const;

    // 恢复棋盘状态
    void restore(const ChessBoard& other);

private:
    QVector<QVector<ChessPiece>> board_; // board_[row][col]

    // 设置棋子到棋盘
    void place(int row, int col, PieceType type, PieceColor color);
};
