#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <memory>
#include "chessgame.h"
#include "boardwidget.h"
#include "aiworker.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onLocalPlay();
    void onAiPlay();
    void onNewGame();
    void onUndo();
    void onExitToMenu();
    void onAiThink();
    void onAiResult(const Move& move);
    void onMoveMade(const Move& move);

private:
    QWidget* createMainMenu();
    QWidget* createGameUI();
    void updateStatus();
    void startAiGame();

    std::unique_ptr<ChessGame> game_;
    BoardWidget* boardWidget_ = nullptr;   // parented via setCentralWidget → stacked_
    std::unique_ptr<QStackedWidget> stacked_;

    std::unique_ptr<AIThread> aiThread_;
    PieceColor aiColor_ = PieceColor::Black;
    int aiDepth_ = 3;

    enum GameMode { MainMenu, LocalTwoPlayer, PlayerVsAI };
    GameMode mode_ = MainMenu;

    QLabel* statusLabel_ = nullptr;        // parented via createGameUI layout
    QLabel* turnLabel_ = nullptr;          // parented via createGameUI layout
};
