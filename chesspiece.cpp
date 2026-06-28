#include "chesspiece.h"

QString ChessPiece::getChar() const {
    switch (type) {
    case PieceType::General:
        return color == PieceColor::Red ? "帅" : "将";
    case PieceType::Advisor:
        return color == PieceColor::Red ? "仕" : "士";
    case PieceType::Elephant:
        return color == PieceColor::Red ? "相" : "象";
    case PieceType::Horse:
        return color == PieceColor::Red ? "马" : "马";
    case PieceType::Chariot:
        return color == PieceColor::Red ? "车" : "车";
    case PieceType::Cannon:
        return color == PieceColor::Red ? "炮" : "炮";
    case PieceType::Soldier:
        return color == PieceColor::Red ? "兵" : "卒";
    default:
        return "";
    }
}

int ChessPiece::getBaseValue() const {
    switch (type) {
    case PieceType::General:  return 10000;
    case PieceType::Chariot:  return 900;
    case PieceType::Cannon:   return 450;
    case PieceType::Horse:    return 400;
    case PieceType::Elephant: return 200;
    case PieceType::Advisor:  return 200;
    case PieceType::Soldier:  return 100;
    default: return 0;
    }
}

QString ChessPiece::getPieceKey(PieceType t, PieceColor c) {
    QString col = (c == PieceColor::Red) ? "red" : "black";
    QString tp;
    switch (t) {
    case PieceType::General:  tp = "general"; break;
    case PieceType::Advisor:  tp = "advisor"; break;
    case PieceType::Elephant: tp = "elephant"; break;
    case PieceType::Horse:    tp = "horse"; break;
    case PieceType::Chariot:  tp = "chariot"; break;
    case PieceType::Cannon:   tp = "cannon"; break;
    case PieceType::Soldier:  tp = "soldier"; break;
    default: tp = "none"; break;
    }
    return col + "_" + tp;
}
