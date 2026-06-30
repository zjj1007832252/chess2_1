#pragma once
#include <QThread>
#include <atomic>
#include <memory>
#include "ai.h"
#include "chessgame.h"

// Worker object that runs AI computation in a dedicated thread.
// Signals cross thread boundaries via Qt's queued connection.
class AIWorker : public QObject {
    Q_OBJECT
public:
    explicit AIWorker(QObject* parent = nullptr) : QObject(parent) {}

signals:
    void finished(const Move& move);

public slots:
    void requestMove(const ChessGame& game, PieceColor aiColor, int depth) {
        if (running_.exchange(true)) return;  // already running
        AI ai(aiColor);
        ai.setDepth(depth);
        ChessGame copy = game;
        Move best = ai.getBestMove(copy);
        running_.store(false);
        emit finished(best);
    }

private:
    // Atomic flag guarding against re-entrant computation on the worker thread.
    std::atomic<bool> running_{false};
};

// Thin wrapper that owns the thread and worker, auto-connects signals.
class AIThread : public QObject {
    Q_OBJECT
public:
    explicit AIThread(QObject* parent = nullptr)
        : QObject(parent),
          thread_(std::make_unique<QThread>()),
          worker_(std::make_unique<AIWorker>())
    {
        worker_->moveToThread(thread_.get());

        connect(this, &AIThread::requestMove, worker_.get(), &AIWorker::requestMove);
        connect(worker_.get(), &AIWorker::finished, this, &AIThread::resultReady);

        thread_->start();
    }

    ~AIThread() {
        thread_->quit();
        thread_->wait();
        // worker_ destroyed automatically after thread has stopped
    }

signals:
    void requestMove(const ChessGame& game, PieceColor aiColor, int depth);
    void resultReady(const Move& move);

private:
    std::unique_ptr<QThread> thread_;
    std::unique_ptr<AIWorker> worker_;
};
