#pragma once
#include <QThread>
#include "ai.h"
#include "chessgame.h"

// Worker object that runs AI computation in a dedicated thread.
// Signals cross thread boundaries via Qt's queued connection.
class AIWorker : public QObject {
    Q_OBJECT
public:
    explicit AIWorker(QObject* parent = nullptr) : QObject(parent), running_(false) {}

signals:
    void resultReady(const Move& move);

public slots:
    void requestMove(const ChessGame& game, PieceColor aiColor, int depth) {
        if (running_) return;
        running_ = true;
        AI ai(aiColor);
        ai.setDepth(depth);
        ChessGame copy = game;
        Move best = ai.getBestMove(copy);
        running_ = false;
        emit resultReady(best);
    }

private:
    bool running_;
};

// Thin wrapper that owns the thread and worker, auto-connects signals.
class AIThread : public QObject {
    Q_OBJECT
public:
    explicit AIThread(QObject* parent = nullptr) : QObject(parent) {
        thread_ = new QThread(this);
        worker_ = new AIWorker();
        worker_->moveToThread(thread_);

        connect(this, &AIThread::requestMove, worker_, &AIWorker::requestMove);
        connect(worker_, &AIWorker::resultReady, this, &AIThread::resultReady);

        thread_->start();
    }

    ~AIThread() {
        thread_->quit();
        thread_->wait();
        delete worker_;
    }

signals:
    void requestMove(const ChessGame& game, PieceColor aiColor, int depth);
    void resultReady(const Move& move);

private:
    QThread* thread_;
    AIWorker* worker_;
};
