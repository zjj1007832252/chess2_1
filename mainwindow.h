#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include "chessgame.h"
#include "boardwidget.h"
#include "ai.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onLocalPlay();
    void onAiPlay();
    void onNewGame();
    void onUndo();
    void onExitToMenu();
    void onAiThink();
    void onMoveMade(const Move& move);

private:
    QWidget* createMainMenu();
    QWidget* createGameUI();
    void updateStatus();
    void startAiGame();

    ChessGame* game_;
    BoardWidget* boardWidget_;
    QStackedWidget* stacked_;

    AI* ai_;
    PieceColor aiColor_;
    bool aiThinking_;
    int aiDepth_;

    enum GameMode { MainMenu, LocalTwoPlayer, PlayerVsAI };
    GameMode mode_;

    QLabel* statusLabel_;
    QLabel* turnLabel_;
};
