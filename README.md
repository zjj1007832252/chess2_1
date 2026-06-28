# 中国象棋 (Xiangqi Chess)

基于 Qt6 + C++17 开发的中国象棋桌面应用，支持本地双人对战和人机对战。

## 功能特性

- **双人对战** — 本地两人轮流下棋
- **人机对战** — 内置 AI 对手，支持简单/中等/困难三种难度
- **完整规则** — 将军/将杀检测、飞将规则、合法走法提示、悔棋
- **棋盘绘制** — 自定义 QWidget 渲染，棋子用汉字显示，带选中高亮和走法提示

## 构建

**依赖：** CMake 3.16+、Qt 6（Core / Gui / Widgets）、MinGW 工具链

```bash
cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/xiangqi.exe
```

Qt 路径自动从 PATH 中的 `qmake` 查找，无需手动指定 `CMAKE_PREFIX_PATH`。构建后 `windeployqt` 自动部署运行时 DLL。

## 架构

采用 MVC 模式：

```
┌─────────────┐     signal      ┌──────────────┐
│  BoardWidget │ ──moveMade──→  │  MainWindow   │
│  (View/Ctrl) │                │  (App State)  │
└──────┬───────┘                └──────┬────────┘
       │ reads                         │ owns
       ▼                               ▼
┌─────────────┐                ┌──────────────┐
│  ChessGame   │ ◄── threaded ──│  AIThread    │
│  (Model)     │                │  (AI Worker) │
└──────────────┘                └──────────────┘
```

| 文件 | 职责 |
|------|------|
| `chessboard.h/cpp` | 10×9 棋盘网格数据、将帅查找 |
| `chesspiece.h/cpp` | 棋子类型/颜色枚举、汉字显示、基础价值 |
| `chessgame.h/cpp` | 规则引擎：走法生成、合法性校验、将军/将杀检测、悔棋 |
| `ai.h/cpp` | Minimax + Alpha-Beta 剪枝 AI，带位置价值表 |
| `aiworker.h` | 后台线程 Worker，避免 AI 计算阻塞 UI |
| `boardwidget.h/cpp` | 棋盘渲染、鼠标交互 |
| `mainwindow.h/cpp` | 主菜单、游戏模式、状态栏、信号协调 |
| `main.cpp` | 入口点、日志处理器 |

## 项目结构

```
├── CMakeLists.txt
├── main.cpp
├── mainwindow.h / .cpp
├── boardwidget.h / .cpp
├── chessgame.h / .cpp
├── chessboard.h / .cpp
├── chesspiece.h / .cpp
├── ai.h / .cpp
├── aiworker.h
├── resources.qrc
└── images/
    ├── red_general.png
    ├── red_advisor.png
    ├── ...
    └── black_soldier.png
```
