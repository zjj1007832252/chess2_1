#include "chessgame.h"
#include <QSet>
#include <algorithm>

ChessGame::ChessGame()
    : currentTurn_(PieceColor::Red), winner_(PieceColor::None), gameOver_(false) {}

void ChessGame::init() {
    board_.initBoard();
    currentTurn_ = PieceColor::Red;
    winner_ = PieceColor::None;
    gameOver_ = false;
    moveHistory_.clear();
}

void ChessGame::switchTurn() {
    currentTurn_ = (currentTurn_ == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
}

void ChessGame::reset() {
    init();
}

// ==================== move generation ====================

QVector<Move> ChessGame::getPseudoLegalMoves(int row, int col) const {
    ChessPiece piece = board_.getPiece(row, col);
    if (piece.isEmpty() || piece.getColor() != currentTurn_)
        return {};
    return getPseudoLegalMovesAt(board_, row, col);
}

// ==================== legality checking ====================

QVector<Move> ChessGame::getPseudoLegalMovesAt(const ChessBoard& b, int row, int col) const {
    ChessPiece piece = b.getPiece(row, col);
    if (piece.isEmpty()) return {};

    QVector<Move> moves;
    switch (piece.getType()) {
    case PieceType::General: {
        int cMin=3, cMax=5, rMin, rMax;
        if (piece.getColor()==PieceColor::Red){rMin=7;rMax=9;}else{rMin=0;rMax=2;}
        int dr[]={-1,1,0,0}, dc[]={0,0,-1,1};
        for(int i=0;i<4;++i){
            int nr=row+dr[i],nc=col+dc[i];
            if(nr>=rMin&&nr<=rMax&&nc>=cMin&&nc<=cMax){
                ChessPiece t=b.getPiece(nr,nc);
                if(!t.isEmpty()&&t.getColor()==piece.getColor())continue;
                moves.append(Move(row,col,nr,nc));
            }
        }
        PieceColor enemyColor = (piece.getColor()==PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
        QPoint enemyGen = b.findGeneral(enemyColor);
        if (enemyGen.x() >= 0 && col == enemyGen.y()) {
            if (countPiecesBetween(b, row, col, enemyGen.x(), enemyGen.y()) == 0) {
                moves.append(Move(row, col, enemyGen.x(), enemyGen.y()));
            }
        }
        break;
    }
    case PieceType::Advisor: {
        int cMin=3,cMax=5,rMin,rMax;
        if(piece.getColor()==PieceColor::Red){rMin=7;rMax=9;}else{rMin=0;rMax=2;}
        int dr[]={-1,-1,1,1},dc[]={-1,1,-1,1};
        for(int i=0;i<4;++i){
            int nr=row+dr[i],nc=col+dc[i];
            if(nr>=rMin&&nr<=rMax&&nc>=cMin&&nc<=cMax){
                ChessPiece t=b.getPiece(nr,nc);
                if(!t.isEmpty()&&t.getColor()==piece.getColor())continue;
                moves.append(Move(row,col,nr,nc));
            }
        }
        break;
    }
    case PieceType::Elephant: {
        int rMin,rMax;
        if(piece.getColor()==PieceColor::Red){rMin=5;rMax=9;}else{rMin=0;rMax=4;}
        struct D{int dr,dc,br,bc;};
        D ds[]={{-2,-2,-1,-1},{-2,2,-1,1},{2,-2,1,-1},{2,2,1,1}};
        for(auto&d:ds){
            int nr=row+d.dr,nc=col+d.dc;
            if(nr<rMin||nr>rMax||nc<0||nc>8)continue;
            if(!b.getPiece(row+d.br,col+d.bc).isEmpty())continue;
            ChessPiece t=b.getPiece(nr,nc);
            if(!t.isEmpty()&&t.getColor()==piece.getColor())continue;
            moves.append(Move(row,col,nr,nc));
        }
        break;
    }
    case PieceType::Horse: {
        struct D{int dr,dc,lr,lc;};
        D ds[]={{-2,-1,-1,0},{-2,1,-1,0},{2,-1,1,0},{2,1,1,0},
                {-1,-2,0,-1},{1,-2,0,-1},{-1,2,0,1},{1,2,0,1}};
        for(auto&d:ds){
            int nr=row+d.dr,nc=col+d.dc;
            if(!ChessBoard::isValidPos(nr,nc))continue;
            if(!b.getPiece(row+d.lr,col+d.lc).isEmpty())continue;
            ChessPiece t=b.getPiece(nr,nc);
            if(!t.isEmpty()&&t.getColor()==piece.getColor())continue;
            moves.append(Move(row,col,nr,nc));
        }
        break;
    }
    case PieceType::Chariot: {
        int dr[]={-1,1,0,0},dc[]={0,0,-1,1};
        for(int dir=0;dir<4;++dir){
            int nr=row+dr[dir],nc=col+dc[dir];
            while(ChessBoard::isValidPos(nr,nc)){
                ChessPiece t=b.getPiece(nr,nc);
                if(t.isEmpty()){moves.append(Move(row,col,nr,nc));}
                else{if(t.getColor()!=piece.getColor())moves.append(Move(row,col,nr,nc));break;}
                nr+=dr[dir];nc+=dc[dir];
            }
        }
        break;
    }
    case PieceType::Cannon: {
        int dr[]={-1,1,0,0},dc[]={0,0,-1,1};
        for(int dir=0;dir<4;++dir){
            int nr=row+dr[dir],nc=col+dc[dir];
            bool jumped=false;
            while(ChessBoard::isValidPos(nr,nc)){
                ChessPiece t=b.getPiece(nr,nc);
                if(!jumped){
                    if(t.isEmpty())moves.append(Move(row,col,nr,nc));
                    else jumped=true;
                }else{
                    if(!t.isEmpty()){
                        if(t.getColor()!=piece.getColor())moves.append(Move(row,col,nr,nc));
                        break;
                    }
                }
                nr+=dr[dir];nc+=dc[dir];
            }
        }
        break;
    }
    case PieceType::Soldier: {
        if(piece.getColor()==PieceColor::Red){
            int nr=row-1;
            if(ChessBoard::isValidPos(nr,col)){
                ChessPiece t=b.getPiece(nr,col);
                if(t.isEmpty()||t.getColor()!=piece.getColor())
                    moves.append(Move(row,col,nr,col));
            }
            if(row<=4){
                for(int d=-1;d<=1;d+=2){
                    int nc=col+d;
                    if(ChessBoard::isValidPos(row,nc)){
                        ChessPiece t=b.getPiece(row,nc);
                        if(t.isEmpty()||t.getColor()!=piece.getColor())
                            moves.append(Move(row,col,row,nc));
                    }
                }
            }
        }else{
            int nr=row+1;
            if(ChessBoard::isValidPos(nr,col)){
                ChessPiece t=b.getPiece(nr,col);
                if(t.isEmpty()||t.getColor()!=piece.getColor())
                    moves.append(Move(row,col,nr,col));
            }
            if(row>=5){
                for(int d=-1;d<=1;d+=2){
                    int nc=col+d;
                    if(ChessBoard::isValidPos(row,nc)){
                        ChessPiece t=b.getPiece(row,nc);
                        if(t.isEmpty()||t.getColor()!=piece.getColor())
                            moves.append(Move(row,col,row,nc));
                    }
                }
            }
        }
        break;
    }
    default: break;
    }
    return moves;
}

bool ChessGame::isMoveSafe(const Move& move) const {
    return !wouldBeInCheckAfterMove(move, currentTurn_);
}

bool ChessGame::wouldBeInCheckAfterMove(const Move& move, PieceColor color) const {
    ChessBoard tempBoard = board_;
    tempBoard.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
    QPoint genPos = tempBoard.findGeneral(color);
    if (genPos.x() < 0) return true;

    // 检查将帅是否面对面（飞将）
    QPoint enemyGen = tempBoard.findGeneral((color == PieceColor::Red) ? PieceColor::Black : PieceColor::Red);
    if (enemyGen.x() >= 0 && genPos.y() == enemyGen.y()) {
        if (countPiecesBetween(tempBoard, genPos.x(), genPos.y(), enemyGen.x(), enemyGen.y()) == 0) {
            return true;
        }
    }

    PieceColor enemyColor = (color == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            auto p = tempBoard.getPiece(r, c);
            if (p.isEmpty() || p.getColor() != enemyColor) continue;
            auto pmoves = getPseudoLegalMovesAt(tempBoard, r, c);
            for (auto& m : pmoves) {
                if (m.toRow == genPos.x() && m.toCol == genPos.y()) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool ChessGame::generalsFacingEachOther() const {
    QPoint redG = board_.findGeneral(PieceColor::Red);
    QPoint blackG = board_.findGeneral(PieceColor::Black);
    if (redG.x()<0 || blackG.x()<0) return false;
    if (redG.y() != blackG.y()) return false;
    return countPiecesBetween(board_, redG.x(), redG.y(), blackG.x(), blackG.y()) == 0;
}

int ChessGame::countPiecesBetween(const ChessBoard& b, int r1, int c1, int r2, int c2) const {
    int count = 0;
    if (r1 == r2) {
        int mn = qMin(c1, c2), mx = qMax(c1, c2);
        for (int c = mn+1; c < mx; ++c) {
            if (!b.getPiece(r1, c).isEmpty()) count++;
        }
    } else if (c1 == c2) {
        int mn = qMin(r1, r2), mx = qMax(r1, r2);
        for (int r = mn+1; r < mx; ++r) {
            if (!b.getPiece(r, c1).isEmpty()) count++;
        }
    }
    return count;
}

bool ChessGame::isInCheck(PieceColor color) const {
    QPoint genPos = board_.findGeneral(color);
    if (genPos.x() < 0) return false;

    PieceColor enemyColor = (color == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            auto p = board_.getPiece(r, c);
            if (p.isEmpty() || p.getColor() != enemyColor) continue;
            auto pmoves = getPseudoLegalMovesAt(board_, r, c);
            for (auto& m : pmoves) {
                if (m.toRow == genPos.x() && m.toCol == genPos.y()) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool ChessGame::isCheckmate(PieceColor color) {
    if (!isInCheck(color)) return false;
    PieceColor savedTurn = currentTurn_;
    currentTurn_ = color;
    bool hasMove = !getAllLegalMoves().isEmpty();
    currentTurn_ = savedTurn;
    return !hasMove;
}

bool ChessGame::isGameOver() {
    if (gameOver_) return true;

    // 检查将/帅是否被吃
    QPoint redG = board_.findGeneral(PieceColor::Red);
    QPoint blackG = board_.findGeneral(PieceColor::Black);
    if (redG.x() < 0) {
        winner_ = PieceColor::Black;
        gameOver_ = true;
        return true;
    }
    if (blackG.x() < 0) {
        winner_ = PieceColor::Red;
        gameOver_ = true;
        return true;
    }

    // 将帅面对面：刚走棋的一方判负
    if (generalsFacingEachOther()) {
        PieceColor lastMover = (currentTurn_ == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
        winner_ = lastMover;
        gameOver_ = true;
        return true;
    }

    // 将杀检测
    PieceColor savedTurn = currentTurn_;
    for (auto c : {PieceColor::Red, PieceColor::Black}) {
        currentTurn_ = c;
        if (isInCheck(c) && getAllLegalMoves().isEmpty()) {
            winner_ = (c == PieceColor::Red) ? PieceColor::Black : PieceColor::Red;
            gameOver_ = true;
            currentTurn_ = savedTurn;
            return true;
        }
    }
    currentTurn_ = savedTurn;
    return false;
}

QVector<Move> ChessGame::getAllLegalMoves() const {
    QVector<Move> legalMoves;
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            auto p = board_.getPiece(r, c);
            if (p.isEmpty() || p.getColor() != currentTurn_) continue;
            auto moves = getPseudoLegalMoves(r, c);
            for (auto& m : moves) {
                if (isMoveSafe(m)) legalMoves.append(m);
            }
        }
    }
    return legalMoves;
}

QVector<Move> ChessGame::getLegalMovesForPiece(int row, int col) const {
    auto p = board_.getPiece(row, col);
    if (p.isEmpty() || p.getColor() != currentTurn_) return {};
    auto moves = getPseudoLegalMoves(row, col);
    QVector<Move> legal;
    for (auto& m : moves) {
        if (isMoveSafe(m)) legal.append(m);
    }
    return legal;
}

bool ChessGame::makeMove(const Move& move) {
    if (!isMoveSafe(move)) return false;
    ChessPiece captured = board_.getPiece(move.toRow, move.toCol);
    board_.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
    moveHistory_.append(RecordedMove(move, captured));
    switchTurn();
    isGameOver(); // update gameOver_ and winner_
    return true;
}

bool ChessGame::undoMove() {
    if (moveHistory_.isEmpty()) return false;
    RecordedMove rm = moveHistory_.takeLast();
    board_.movePiece(rm.move.toRow, rm.move.toCol, rm.move.fromRow, rm.move.fromCol);
    board_.setPiece(rm.move.toRow, rm.move.toCol, rm.captured);
    switchTurn();
    gameOver_ = false;
    winner_ = PieceColor::None;
    return true;
}

QString ChessGame::getMoveNotation(const Move& move) const {
    auto piece = board_.getPiece(move.fromRow, move.fromCol);
    QString ch = piece.getChar();
    QString dir;
    if (move.toRow > move.fromRow) dir = "jin";
    else if (move.toRow < move.fromRow) dir = "tui";
    else dir = "ping";
    QString targetStr;
    if (dir == "ping") targetStr = QString::number(move.toCol + 1);
    else targetStr = QString::number(move.toRow + 1);
    return ch + dir + targetStr;
}
