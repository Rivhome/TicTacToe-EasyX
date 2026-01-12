# TicTacToe-EasyX
这是一个基于 C++ 和 EasyX 图形库开发的井字棋小游戏。

## 项目亮点
- **性能优化**：采用事件驱动模式（getmessage），闲置时 CPU 占用几乎为 0%。
- **渲染同步**：解决了 MessageBox 阻塞导致最后一手棋无法及时显示的 UI 问题。
- **逻辑解耦**：将绘图逻辑与胜负判定逻辑完全分离。

## 如何运行
1. 安装 Visual Studio。
2. 安装 EasyX 图形库。
3. 运行 `main.cpp` 即可。
