# AGENTS.md

## Build & Run

Requires: CMake 3.16+, Qt 6 (auto-detected from PATH via qmake), MinGW toolchain.

```bash
cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/xiangqi.exe
```

- No `-DCMAKE_PREFIX_PATH` needed — CMake auto-finds Qt via `qmake -query QT_INSTALL_PREFIX`.
- `windeployqt` runs post-build automatically; no manual DLL copying.
- Debug build: adds `-mconsole` flag, writes `qDebug` to `debug_log.txt`.

## Architecture

MVC: `ChessBoard`/`ChessGame` (model) ↔ `BoardWidget` (view/controller) ↔ `MainWindow` (app state).

| File | Role |
|------|------|
| `chessboard.cpp/.h` | 10×9 grid data model |
| `chesspiece.cpp/.h` | Piece type/color enums, movement logic |
| `chessgame.cpp/.h` | Rules engine: move validation, check/checkmate, history |
| `ai.cpp/.h` | Minimax + alpha-beta pruning AI |
| `boardwidget.cpp/.h` | Board rendering, mouse interaction (custom QWidget) |
| `mainwindow.cpp/.h` | UI coordinator, game state management |
| `main.cpp` | Entry point, debug message handler |

Signal flow: `BoardWidget::moveMade` → `MainWindow::onMoveMade`.

## Key Conventions

- C++17, Qt 6 (Core + Gui + Widgets).
- `CMAKE_AUTOMOC`/`CMAKE_AUTORCC`/`CMAKE_AUTOUIC` handle Qt meta compilation.
- Resources embedded via `resources.qrc` (14 PNGs in `images/`).
- `BoardWidget::clone/restore` pattern for undo (memento).
- No test suite — verify changes by running the app.
