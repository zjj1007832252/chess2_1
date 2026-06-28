#include "boardwidget.h"
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QRect>

BoardWidget::BoardWidget(ChessGame* game, QWidget* parent)
    : QWidget(parent), lastMoveFrom(-1,-1), lastMoveTo(-1,-1),
      selected_(-1, -1), legalMoves_(), game_(game), cellSize_(60), margin_(40) {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    updateSize();
}

QSize BoardWidget::sizeHint() const {
    return {cellSize_ * 8 + margin_ * 2, cellSize_ * 9 + margin_ * 2};
}

void BoardWidget::updateSize() {
    QSize sz = sizeHint();
    setMinimumSize(sz);
    setMaximumSize(sz);
}

QPoint BoardWidget::rowColToPixel(int row, int col) const {
    return QPoint(margin_ + col * cellSize_, margin_ + row * cellSize_);
}

QPoint BoardWidget::pixelToRowCol(int x, int y) const {
    int col = qRound((x - margin_) / (double)cellSize_);
    int row = qRound((y - margin_) / (double)cellSize_);
    if (row < 0 || row > 9 || col < 0 || col > 8) return {-1, -1};
    return {row, col};
}

void BoardWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBoard(painter);
    drawLastMoveHighlight(painter);
    drawPieces(painter);
    drawSelection(painter);
    drawLegalHints(painter);
    drawCheckIndicator(painter);
}

void BoardWidget::drawBoard(QPainter& painter) {
    // 背景
    painter.fillRect(rect(), QColor(220, 180, 100));

    // 棋盘区域
    int boardLeft = margin_ - cellSize_/2;
    int boardTop = margin_ - cellSize_/2;
    int boardRight = margin_ + 8 * cellSize_ + cellSize_/2;
    int boardBottom = margin_ + 9 * cellSize_ + cellSize_/2;

    painter.fillRect(boardLeft, boardTop, boardRight - boardLeft, boardBottom - boardTop,
                     QColor(235, 200, 120));

    painter.setPen(QPen(QColor(80, 60, 30), 2));

    // 横线
    for (int r = 0; r < 10; ++r) {
        QPoint p1 = rowColToPixel(r, 0);
        QPoint p2 = rowColToPixel(r, 8);
        painter.drawLine(p1, p2);
    }

    // 竖线
    for (int c = 0; c < 9; ++c) {
        if (c == 0 || c == 8) {
            QPoint p1 = rowColToPixel(0, c);
            QPoint p2 = rowColToPixel(9, c);
            painter.drawLine(p1, p2);
        } else {
            QPoint p1 = rowColToPixel(0, c);
            QPoint p2 = rowColToPixel(4, c);
            painter.drawLine(p1, p2);
            p1 = rowColToPixel(5, c);
            p2 = rowColToPixel(9, c);
            painter.drawLine(p1, p2);
        }
    }

    // 九宫格斜线
    painter.setPen(QPen(QColor(80, 60, 30), 1.5));
    painter.drawLine(rowColToPixel(0, 3), rowColToPixel(2, 5));
    painter.drawLine(rowColToPixel(0, 5), rowColToPixel(2, 3));
    painter.drawLine(rowColToPixel(7, 3), rowColToPixel(9, 5));
    painter.drawLine(rowColToPixel(7, 5), rowColToPixel(9, 3));

    // 楚河汉界文字
    QFont font("KaiTi, SimSun, Microsoft YaHei", 22, QFont::Bold);
    painter.setFont(font);
    painter.setPen(QColor(80, 60, 30));
    painter.drawText(rowColToPixel(4, 1).x() - 10, rowColToPixel(4, 1).y() + 8, 120, cellSize_,
                     Qt::AlignCenter, "楚 河");
    painter.drawText(rowColToPixel(5, 5).x() - 10, rowColToPixel(5, 5).y() + 8, 120, cellSize_, Qt::AlignCenter, "汉 界");

    // 炮位标记
    int cannonPositions[][2] = {{2,1},{2,7},{7,1},{7,7}};
    for (auto& pos : cannonPositions) {
        QPoint center = rowColToPixel(pos[0], pos[1]);
        int offset = 6;
        int len = 10;
        painter.setPen(QPen(QColor(80, 60, 30), 1.5));
        // 四角标记
        if (pos[0] > 0) {
            painter.drawLine(center.x()-offset, center.y()-offset-len, center.x()-offset, center.y()-offset);
            painter.drawLine(center.x()-offset-len, center.y()-offset, center.x()-offset, center.y()-offset);
        }
        if (pos[1] < 8) {
            painter.drawLine(center.x()+offset, center.y()-offset-len, center.x()+offset, center.y()-offset);
            painter.drawLine(center.x()+offset+len, center.y()-offset, center.x()+offset, center.y()-offset);
        }
        if (pos[0] > 0) {
            painter.drawLine(center.x()-offset, center.y()+offset+len, center.x()-offset, center.y()+offset);
            painter.drawLine(center.x()-offset-len, center.y()+offset, center.x()-offset, center.y()+offset);
        }
        if (pos[1] < 8) {
            painter.drawLine(center.x()+offset, center.y()+offset+len, center.x()+offset, center.y()+offset);
            painter.drawLine(center.x()+offset+len, center.y()+offset, center.x()+offset, center.y()+offset);
        }
    }

    // 兵位标记
    int soldierRows[][2] = {{3,0},{3,2},{3,4},{3,6},{3,8},
                               {6,0},{6,2},{6,4},{6,6},{6,8}};
    for (auto& pos : soldierRows) {
        QPoint center = rowColToPixel(pos[0], pos[1]);
        int offset = 6;
        int len = 8;
        painter.setPen(QPen(QColor(80, 60, 30), 1.5));
        if (pos[0] == 3) {
            painter.drawLine(center.x()-offset, center.y()+offset+len, center.x()-offset, center.y()+offset);
            painter.drawLine(center.x()-offset-len, center.y()+offset, center.x()-offset, center.y()+offset);
            painter.drawLine(center.x()+offset, center.y()+offset+len, center.x()+offset, center.y()+offset);
            painter.drawLine(center.x()+offset+len, center.y()+offset, center.x()+offset, center.y()+offset);
        } else {
            painter.drawLine(center.x()-offset, center.y()-offset-len, center.x()-offset, center.y()-offset);
            painter.drawLine(center.x()-offset-len, center.y()-offset, center.x()-offset, center.y()-offset);
            painter.drawLine(center.x()+offset, center.y()-offset-len, center.x()+offset, center.y()-offset);
            painter.drawLine(center.x()+offset+len, center.y()-offset, center.x()+offset, center.y()-offset);
        }
    }

    // 外框加粗
    painter.setPen(QPen(QColor(60, 40, 20), 4));
    painter.drawRect(boardLeft, boardTop, boardRight - boardLeft, boardBottom - boardTop);
}

void BoardWidget::drawPieces(QPainter& painter) {
    const auto& board = game_->getBoard();
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            auto piece = board.getPiece(r, c);
            if (!piece.isEmpty()) {
                drawPiece(painter, r, c, piece);
            }
        }
    }
}

void BoardWidget::drawPiece(QPainter& painter, int row, int col, const ChessPiece& piece) {
    QPoint center = rowColToPixel(row, col);
    int radius = cellSize_ / 2 - 4;

    // 阴影
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 40));
    painter.drawEllipse(center.x() + 2, center.y() + 3, radius * 2, radius * 2);
    painter.restore();

    // 棋子底色
    QColor fillColor(255, 248, 230);
    painter.setBrush(fillColor);

    // 棋子边框
    QColor borderColor;
    if (piece.getColor() == PieceColor::Red) {
        borderColor = QColor(180, 20, 20);
    } else {
        borderColor = QColor(30, 30, 30);
    }
    painter.setPen(QPen(borderColor, 2.5));

    painter.drawEllipse(center.x() - radius, center.y() - radius, radius * 2, radius * 2);

    // 内圈装饰
    painter.setPen(QPen(borderColor, 1));
    painter.drawEllipse(center.x() - radius + 5, center.y() - radius + 5,
                        (radius - 5) * 2, (radius - 5) * 2);

    // 棋子文字
    QFont font("KaiTi, SimSun, Microsoft YaHei", 26, QFont::Bold);
    painter.setFont(font);
    painter.setPen(borderColor);
    QRect textRect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
    painter.drawText(textRect, Qt::AlignCenter, piece.getChar());
}

void BoardWidget::drawSelection(QPainter& painter) {
    if (selected_.x() < 0) return;
    QPoint center = rowColToPixel(selected_.x(), selected_.y());
    int radius = cellSize_ / 2 - 2;

    painter.setPen(QPen(QColor(0, 150, 255, 180), 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
}

void BoardWidget::drawLegalHints(QPainter& painter) {
    if (selected_.x() < 0) return;

    for (auto& move : legalMoves_) {
        if (move.fromRow == selected_.x() && move.fromCol == selected_.y()) {
            QPoint center = rowColToPixel(move.toRow, move.toCol);
            auto target = game_->getBoard().getPiece(move.toRow, move.toCol);

            if (target.isEmpty()) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(0, 180, 80, 120));
                painter.drawEllipse(center.x() - 8, center.y() - 8, 16, 16);
            } else {
                int radius = cellSize_ / 2 + 2;
                painter.setPen(QPen(QColor(0, 180, 80, 200), 3));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
            }
        }
    }
}

void BoardWidget::drawLastMoveHighlight(QPainter& painter) {
    if (lastMoveFrom.x() < 0 || lastMoveTo.x() < 0) return;

    painter.setPen(QPen(QColor(0, 100, 255, 60), 8));
    painter.setBrush(Qt::NoBrush);
    QPoint p1 = rowColToPixel(lastMoveFrom.y(), lastMoveFrom.x());
    QPoint p2 = rowColToPixel(lastMoveTo.y(), lastMoveTo.x());
    painter.drawEllipse(p1, cellSize_/2 - 2, cellSize_/2 - 2);
    painter.drawEllipse(p2, cellSize_/2 - 2, cellSize_/2 - 2);
}

void BoardWidget::drawCheckIndicator(QPainter& painter) {
    if (!game_->isInCheck(game_->getCurrentTurn())) return;

    auto turn = game_->getCurrentTurn();
    QPoint genPos = game_->getBoard().findGeneral(turn);

    if (genPos.x() < 0) return;
    QPoint center = rowColToPixel(genPos.x(), genPos.y());
    int radius = cellSize_ / 2 + 5;

    painter.setPen(QPen(QColor(255, 0, 0, 180), 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
}

void BoardWidget::mousePressEvent(QMouseEvent* event) {
    QPoint pos = pixelToRowCol(event->pos().x(), event->pos().y());

    if (pos.x() < 0 || pos.y() < 0) {
        selected_ = {-1, -1};
        legalMoves_.clear();
        update();
        return;
    }

    const auto& board = game_->getBoard();
    auto piece = board.getPiece(pos.x(), pos.y());

    // 如果已经选中了棋子
    if (selected_.x() >= 0) {
        // 点击的是合法走法的目标位置
        for (auto& m : legalMoves_) {
            if (m.toRow == pos.x() && m.toCol == pos.y()) {
                Move theMove(selected_.x(), selected_.y(), pos.x(), pos.y());
                game_->makeMove(theMove);
                lastMoveFrom = QPoint(theMove.fromCol, theMove.fromRow);
                lastMoveTo = QPoint(theMove.toCol, theMove.toRow);
                selected_ = {-1, -1};
                legalMoves_.clear();
                update();
                emit moveMade(theMove);
                return;
            }
        }
        // 点击的是另一个己方棋子，切换选择
        if (piece.getColor() == game_->getCurrentTurn()) {
            selected_ = pos;
            updateLegalHints();
            update();
            return;
        }
        // 点击非法位置，取消选择
        selected_ = {-1, -1};
        legalMoves_.clear();
        update();
        return;
    }

    // 没有选中棋子
    if (piece.isEmpty() || piece.getColor() != game_->getCurrentTurn()) {
        return;
    }

    // 选中棋子
    selected_ = pos;
    updateLegalHints();
    update();
}

void BoardWidget::updateLegalHints() {
    if (selected_.x() < 0) return;
    legalMoves_ = game_->getLegalMovesForPiece(selected_.x(), selected_.y());
}
