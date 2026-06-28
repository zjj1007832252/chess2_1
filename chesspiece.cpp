#include "chesspiece.h"
#include <QMap>

static QMap<QString, PieceType> parseMap() {
    QMap<QString, PieceType> m;
    m["red_general"] = PieceType::General;
    m["red_advisor"] = PieceType::Advisor;
    m["red_elephant"] = PieceType::Elephant;
    m["red_horse"] = PieceType::Horse;
    m["red_chariot"] = PieceType::Chariot;
    m["red_cannon"] = PieceType::Cannon;
    m["red_soldier"] = PieceType::Soldier;
    m["black_general"] = PieceType::General;
    m["black_advisor"] = PieceType::Advisor;
    m["black_elephant"] = PieceType::Elephant;
    m["black_horse"] = PieceType::Horse;
    m["black_chariot"] = PieceType::Chariot;
    m["black_cannon"] = PieceType::Cannon;
    m["black_soldier"] = PieceType::Soldier;
    return m;
}

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

QString ChessPiece::getName() const {
    switch (type) {
    case PieceType::General:  return "general";
    case PieceType::Advisor:  return "advisor";
    case PieceType::Elephant: return "elephant";
    case PieceType::Horse:    return "horse";
    case PieceType::Chariot:  return "chariot";
    case PieceType::Cannon:   return "cannon";
    case PieceType::Soldier:  return "soldier";
    default: return "none";
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
