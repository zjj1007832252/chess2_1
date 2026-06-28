#pragma once
#include <QString>

// 棋子类型枚举
enum class PieceType {
    None = 0,
    General,      // 将/帅
    Advisor,      // 士/仕
    Elephant,     // 象/相
    Horse,        // 马
    Chariot,      // 车
    Cannon,       // 炮
    Soldier       // 卒/兵
};

// 棋子颜色
enum class PieceColor {
    None = 0,
    Red,          // 红方
    Black         // 黑方
};

// 棋子类
class ChessPiece {
public:
    ChessPiece() : type(PieceType::None), color(PieceColor::None) {}
    ChessPiece(PieceType t, PieceColor c) : type(t), color(c) {}

    bool isEmpty() const { return type == PieceType::None; }
    PieceType getType() const { return type; }
    PieceColor getColor() const { return color; }

    // 获取棋子的汉字显示
    QString getChar() const;
    // 获取棋子名称（用于AI评估）
    QString getName() const;
    // 获取棋子基础价值
    int getBaseValue() const;

    static QString getPieceKey(PieceType t, PieceColor c);

private:
    PieceType type;
    PieceColor color;
};
