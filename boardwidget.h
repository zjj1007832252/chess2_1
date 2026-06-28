#pragma once
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include "chessgame.h"

class BoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit BoardWidget(ChessGame* game, QWidget* parent = nullptr);

    QPoint lastMoveFrom;
    QPoint lastMoveTo;

    // These need to be accessible from MainWindow for reset
    QPoint selected_;
    QVector<Move> legalMoves_;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    QSize sizeHint() const override;

private:
    ChessGame* game_;
    int cellSize_;
    int margin_;

    void drawBoard(QPainter& painter);
    void drawPieces(QPainter& painter);
    void drawPiece(QPainter& painter, int row, int col, const ChessPiece& piece);
    void drawSelection(QPainter& painter);
    void drawLegalHints(QPainter& painter);
    void drawLastMoveHighlight(QPainter& painter);
    void drawCheckIndicator(QPainter& painter);
    QPoint rowColToPixel(int row, int col) const;
    QPoint pixelToRowCol(int x, int y) const;
    void updateLegalHints();
    void updateSize();

signals:
    void moveMade(const Move& move);
};
