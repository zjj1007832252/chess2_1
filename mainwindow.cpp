#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSpacerItem>
#include <QFont>
#include <QApplication>
#include <QScreen>
#include <QInputDialog>
#include <QTimer>
#include <QMessageBox>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      game_(std::make_unique<ChessGame>()),
      stacked_(std::make_unique<QStackedWidget>()),
      aiThread_(std::make_unique<AIThread>()) {
    setCentralWidget(stacked_.get());
    setWindowTitle("中国象棋");
    setMinimumSize(600, 680);

    connect(aiThread_.get(), &AIThread::resultReady, this, &MainWindow::onAiResult);

    stacked_->addWidget(createMainMenu());
    stacked_->addWidget(createGameUI());
    stacked_->setCurrentIndex(0);
}

MainWindow::~MainWindow() = default;

QWidget* MainWindow::createMainMenu() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    // 标题
    QLabel* title = new QLabel("中 国 象 棋");
    QFont titleFont("KaiTi, SimSun", 36, QFont::Bold);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #8B0000; padding: 20px;");
    layout->addWidget(title);

    // 副标题
    QLabel* subtitle = new QLabel("XIANGQI CHESS");
    subtitle->setFont(QFont("Arial", 14));
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #666; padding-bottom: 10px;");
    layout->addWidget(subtitle);

    layout->addSpacing(20);

    // 开始游戏按钮组
    auto* btnGroup = new QGroupBox("游戏模式");
    btnGroup->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; padding: 15px; }");
    auto* btnLayout = new QVBoxLayout(btnGroup);

    auto* localBtn = new QPushButton("双人对战（本地联机）");
    localBtn->setMinimumHeight(50);
    localBtn->setStyleSheet(
        "QPushButton { "
        "  font-size: 16px; "
        "  padding: 12px 30px; "
        "  background-color: #f5f0e8; "
        "  border: 2px solid #8B7355; "
        "  border-radius: 8px; "
        "}"
        "QPushButton:hover { "
        "  background-color: #e8dcc8; "
        "  border-color: #8B0000; "
        "}"
        "QPushButton:pressed { "
        "  background-color: #d4c4a8; "
        "}"
    );
    connect(localBtn, &QPushButton::clicked, this, &MainWindow::onLocalPlay);
    btnLayout->addWidget(localBtn);

    auto* aiBtn = new QPushButton("人机对战");
    aiBtn->setMinimumHeight(50);
    aiBtn->setStyleSheet(
        "QPushButton { "
        "  font-size: 16px; "
        "  padding: 12px 30px; "
        "  background-color: #f5f0e8; "
        "  border: 2px solid #8B7355; "
        "  border-radius: 8px; "
        "}"
        "QPushButton:hover { "
        "  background-color: #e8dcc8; "
        "  border-color: #8B0000; "
        "}"
        "QPushButton:pressed { "
        "  background-color: #d4c4a8; "
        "}"
    );
    connect(aiBtn, &QPushButton::clicked, this, &MainWindow::onAiPlay);
    btnLayout->addWidget(aiBtn);

    layout->addWidget(btnGroup);
    layout->addSpacing(30);

    // 退出按钮
    auto* exitBtn = new QPushButton("退出游戏");
    exitBtn->setMinimumHeight(40);
    exitBtn->setStyleSheet(
        "QPushButton { "
        "  font-size: 14px; "
        "  padding: 8px 20px; "
        "  background-color: #f0f0f0; "
        "  border: 1px solid #ccc; "
        "  border-radius: 5px; "
        "}"
        "QPushButton:hover { background-color: #e0e0e0; }"
    );
    connect(exitBtn, &QPushButton::clicked, qApp, &QApplication::quit);
    layout->addWidget(exitBtn);

    layout->addStretch();

    // 底部说明
    QLabel* footer = new QLabel("基于 Qt6 + C++ 开发 | 支持双人对战与人机对战");
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet("color: #999; font-size: 11px; padding: 10px;");
    layout->addWidget(footer);

    return page;
}

void MainWindow::onLocalPlay() {
    mode_ = LocalTwoPlayer;
    game_->init();
    stacked_->setCurrentIndex(1);
    boardWidget_->selected_ = {-1, -1};
    boardWidget_->legalMoves_.clear();
    boardWidget_->lastMoveFrom = {-1, -1};
    boardWidget_->lastMoveTo = {-1, -1};
    boardWidget_->update();
    updateStatus();
}

void MainWindow::onAiPlay() {
    // 创建模式选择对话框
    auto* dialog = new QDialog(this);
    dialog->setWindowTitle("人机对战 - 选择模式");
    dialog->setMinimumWidth(400);

    auto* layout = new QVBoxLayout(dialog);

    auto* group = new QGroupBox("选择你的棋子");
    auto* colorLayout = new QHBoxLayout(group);

    auto* redBtn = new QPushButton("红方（先手）");
    redBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: #ffcccc; "
        "  border: 2px solid #cc0000; "
        "  border-radius: 6px; "
        "  padding: 12px; "
        "  font-size: 15px; "
        "  font-weight: bold; "
        "}"
        "QPushButton:hover { background-color: #ffaaaa; }"
    );
    connect(redBtn, &QPushButton::clicked, [dialog, this]() {
        dialog->close();
        aiColor_ = PieceColor::Black;
        startAiGame();
    });
    colorLayout->addWidget(redBtn);

    auto* blackBtn = new QPushButton("黑方（后手）");
    blackBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: #dddddd; "
        "  border: 2px solid #333; "
        "  border-radius: 6px; "
        "  padding: 12px; "
        "  font-size: 15px; "
        "  font-weight: bold; "
        "}"
        "QPushButton:hover { background-color: #bbbbbb; }"
    );
    connect(blackBtn, &QPushButton::clicked, [dialog, this]() {
        dialog->close();
        aiColor_ = PieceColor::Red;
        startAiGame();
    });
    colorLayout->addWidget(blackBtn);

    layout->addWidget(group);

    auto* diffGroup = new QGroupBox("难度选择");
    auto* diffLayout = new QHBoxLayout(diffGroup);

    auto* easyBtn = new QPushButton("简单 (深度2)");
    easyBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: #ccffcc; "
        "  border: 2px solid #00aa00; "
        "  border-radius: 6px; "
        "  padding: 10px; "
        "  font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #aaffaa; }"
    );
    connect(easyBtn, &QPushButton::clicked, [this]() { aiDepth_ = 2; });
    diffLayout->addWidget(easyBtn);

    auto* mediumBtn = new QPushButton("中等 (深度3)");
    mediumBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: #ffffcc; "
        "  border: 2px solid #aaaa00; "
        "  border-radius: 6px; "
        "  padding: 10px; "
        "  font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #ffffaa; }"
    );
    connect(mediumBtn, &QPushButton::clicked, [this]() { aiDepth_ = 3; });
    diffLayout->addWidget(mediumBtn);

    auto* hardBtn = new QPushButton("困难 (深度4)");
    hardBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: #ffcccc; "
        "  border: 2px solid #cc0000; "
        "  border-radius: 6px; "
        "  padding: 10px; "
        "  font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #ffaaaa; }"
    );
    connect(hardBtn, &QPushButton::clicked, [this]() { aiDepth_ = 4; });
    diffLayout->addWidget(hardBtn);

    layout->addWidget(diffGroup);

    auto* cancelBtn = new QPushButton("取消");
    cancelBtn->setStyleSheet(
        "QPushButton { "
        "  padding: 10px; "
        "  font-size: 14px; "
        "  border-radius: 6px; "
        "  background-color: #eee; "
        "}"
    );
    connect(cancelBtn, &QPushButton::clicked, dialog, &QDialog::reject);
    layout->addWidget(cancelBtn);

    layout->addStretch();
    dialog->setLayout(layout);
    dialog->exec();
}

void MainWindow::startAiGame() {
    mode_ = PlayerVsAI;
    game_->init();
    stacked_->setCurrentIndex(1);
    boardWidget_->selected_ = {-1, -1};
    boardWidget_->legalMoves_.clear();
    boardWidget_->lastMoveFrom = {-1, -1};
    boardWidget_->lastMoveTo = {-1, -1};
    boardWidget_->update();
    updateStatus();

    // 如果AI先手，自动走棋
    if (game_->getCurrentTurn() == aiColor_) {
        QTimer::singleShot(500, this, &MainWindow::onAiThink);
    }
}

QWidget* MainWindow::createGameUI() {
    // 这个只创建一次，返回stacked_中的第二个页面
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);

    // 顶部栏
    auto* topBar = new QHBoxLayout();

    auto* backBtn = new QPushButton("← 返回");
    backBtn->setStyleSheet(
        "QPushButton { "
        "  padding: 6px 15px; "
        "  background-color: #f0ebe3; "
        "  border: 1px solid #bbb; "
        "  border-radius: 4px; "
        "  font-size: 13px; "
        "}"
        "QPushButton:hover { background-color: #e0d8c8; }"
    );
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::onExitToMenu);
    topBar->addWidget(backBtn);

    turnLabel_ = new QLabel("红方走棋");
    turnLabel_->setFont(QFont("KaiTi", 14, QFont::Bold));
    turnLabel_->setStyleSheet("color: #cc0000; padding: 5px 15px;");
    topBar->addWidget(turnLabel_);

    statusLabel_ = new QLabel("");
    statusLabel_->setFont(QFont("KaiTi", 12));
    statusLabel_->setStyleSheet("color: #666; padding: 5px 10px;");
    topBar->addWidget(statusLabel_);
    topBar->addStretch();

    auto* newBtn = new QPushButton("新游戏");
    newBtn->setStyleSheet(
        "QPushButton { "
        "  padding: 6px 15px; "
        "  background-color: #f0ebe3; "
        "  border: 1px solid #bbb; "
        "  border-radius: 4px; "
        "  font-size: 13px; "
        "}"
        "QPushButton:hover { background-color: #e0d8c8; }"
    );
    connect(newBtn, &QPushButton::clicked, this, &MainWindow::onNewGame);
    topBar->addWidget(newBtn);

    auto* undoBtn = new QPushButton("悔棋");
    undoBtn->setStyleSheet(
        "QPushButton { "
        "  padding: 6px 15px; "
        "  background-color: #f0ebe3; "
        "  border: 1px solid #bbb; "
        "  border-radius: 4px; "
        "  font-size: 13px; "
        "}"
        "QPushButton:hover { background-color: #e0d8c8; }"
    );
    connect(undoBtn, &QPushButton::clicked, this, &MainWindow::onUndo);
    topBar->addWidget(undoBtn);

    layout->addLayout(topBar);

    // 棋盘
    boardWidget_ = new BoardWidget(game_.get(), this);
    boardWidget_->setMinimumSize(560, 620);
    layout->addWidget(boardWidget_);

    // 连接信号
    connect(boardWidget_, &BoardWidget::moveMade, this, &MainWindow::onMoveMade);

    return page;
}

void MainWindow::updateStatus() {
    if (!game_) return;
    auto turn = game_->getCurrentTurn();
    if (turn == PieceColor::Red) {
        turnLabel_->setText("红方走棋");
        turnLabel_->setStyleSheet("color: #cc0000; padding: 5px 15px; font-size: 14pt; font-weight: bold;");
    } else {
        turnLabel_->setText("黑方走棋");
        turnLabel_->setStyleSheet("color: #222; padding: 5px 15px; font-size: 14pt; font-weight: bold;");
    }

    if (game_->isInCheck(turn)) {
        statusLabel_->setText("⚠ 将军！");
        statusLabel_->setStyleSheet("color: #cc0000; padding: 5px 10px; font-weight: bold;");
    } else {
        statusLabel_->setText("");
    }

    if (mode_ == PlayerVsAI) {
        if (turn == aiColor_) {
            statusLabel_->setText("AI思考中...");
            statusLabel_->setStyleSheet("color: #666; padding: 5px 10px;");
        }
    }
}

void MainWindow::onMoveMade(const Move& move) {
    Q_UNUSED(move);
    updateStatus();

    // 检查游戏是否结束
    if (game_->isGameOver()) {
        PieceColor winner = game_->getWinner();
        QString msg;
        if (mode_ == PlayerVsAI) {
            if (winner == aiColor_) {
                msg = "AI获胜！";
            } else {
                msg = "恭喜你，你赢了！";
            }
        } else {
            msg = (winner == PieceColor::Red) ? "红方获胜！" : "黑方获胜！";
        }
        statusLabel_->setText(msg);
        QMessageBox::information(this, "游戏结束", msg);
        return;
    }

    // AI模式：轮到AI时启动后台计算
    if (mode_ == PlayerVsAI && game_->getCurrentTurn() == aiColor_) {
        onAiThink();
    }
}

void MainWindow::onAiThink() {
    if (game_->isGameOver()) return;
    if (game_->getCurrentTurn() != aiColor_) return;

    statusLabel_->setText("AI思考中...");
    statusLabel_->setStyleSheet("color: #666; padding: 5px 10px;");
    boardWidget_->setEnabled(false);

    // 发送到后台线程计算
    emit aiThread_->requestMove(*game_, aiColor_, aiDepth_);
}

void MainWindow::onAiResult(const Move& aiMove) {
    boardWidget_->setEnabled(true);

    if (!aiMove.isValid()) {
        updateStatus();
        return;
    }

    game_->makeMove(aiMove);
    boardWidget_->lastMoveFrom = QPoint(aiMove.fromCol, aiMove.fromRow);
    boardWidget_->lastMoveTo = QPoint(aiMove.toCol, aiMove.toRow);
    boardWidget_->update();

    updateStatus();

    // 检查游戏是否结束
    if (game_->isGameOver()) {
        PieceColor winner = game_->getWinner();
        QString msg;
        if (winner == aiColor_) {
            msg = "AI获胜！";
        } else {
            msg = "恭喜你，你赢了！";
        }
        statusLabel_->setText(msg);
        QMessageBox::information(this, "游戏结束", msg);
    }
}

void MainWindow::onNewGame() {
    game_->reset();
    boardWidget_->selected_ = {-1, -1};
    boardWidget_->legalMoves_.clear();
    boardWidget_->lastMoveFrom = {-1, -1};
    boardWidget_->lastMoveTo = {-1, -1};
    boardWidget_->update();
    updateStatus();

    if (mode_ == PlayerVsAI && game_->getCurrentTurn() == aiColor_) {
        QTimer::singleShot(500, this, &MainWindow::onAiThink);
    }
}

void MainWindow::onUndo() {
    auto history = game_->getMoveHistory();
    if (history.isEmpty()) return;

    // AI模式下先取消正在进行的AI计算
    if (mode_ == PlayerVsAI) {
        boardWidget_->setEnabled(true);
    }

    if (mode_ == LocalTwoPlayer) {
        // 双人模式：撤销一步
        game_->undoMove();
    } else if (mode_ == PlayerVsAI) {
        // AI模式：撤销两步（玩家一步 + AI一步）
        if (history.size() >= 2) {
            game_->undoMove();
            game_->undoMove();
        } else {
            game_->undoMove();
        }
    }

    boardWidget_->lastMoveFrom = {-1, -1};
    boardWidget_->lastMoveTo = {-1, -1};
    boardWidget_->update();
    updateStatus();
}

void MainWindow::onExitToMenu() {
    mode_ = MainMenu;
    stacked_->setCurrentIndex(0);
}
