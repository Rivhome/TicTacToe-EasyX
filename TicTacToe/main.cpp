#include <graphics.h> // EasyX 图形库
#include <tchar.h>    // 用于处理 Unicode 字符串
#include <stdio.h>    // 字符串格式化

// ============================================================================
// 【全局常量与数据定义】
// ============================================================================
const int BOARD_SIZE = 600;           // 整个窗口的宽高
const int CELL_SIZE = BOARD_SIZE / 3; // 每个格子的尺寸

// 棋盘数据结构：使用 3x3 数组。'_' 代表空格，'O' 和 'X' 代表棋子
char board_data[3][3] = {
    {'_', '_', '_'},
    {'_', '_', '_'},
    {'_', '_', '_'}
};

char current_piece = 'O'; // 当前执子方，开局默认为 'O'

// ============================================================================
// 【逻辑判定模块】
// ============================================================================

// 判定某一方棋子是否获胜
bool CheckWin(char c) {
    // 检查行与列：利用循环减少重复的 if 语句，代码更优雅，易于维护
    for (int i = 0; i < 3; i++) {
        if (board_data[i][0] == c && board_data[i][1] == c && board_data[i][2] == c) return true; // 行
        if (board_data[0][i] == c && board_data[1][i] == c && board_data[2][i] == c) return true; // 列
    }
    // 检查两条对角线
    if (board_data[0][0] == c && board_data[1][1] == c && board_data[2][2] == c) return true;
    if (board_data[0][2] == c && board_data[1][1] == c && board_data[2][0] == c) return true;

    return false;
}

// 判定平局：棋盘填满且没有人赢
bool CheckDraw() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board_data[i][j] == '_') return false; // 只要有空位，游戏就没结束
        }
    }
    // 此时棋盘已满，如果没有人赢，那就是平局
    return !CheckWin('X') && !CheckWin('O');
}

// ============================================================================
// 【图形渲染模块】
// ============================================================================

// 将内存中的 board_data 数据同步到屏幕画面上
void RenderAll() {
    cleardevice(); // 清屏，防止上一帧内容重叠

    // 1. 绘制棋盘网络
    setlinecolor(WHITE);
    setlinestyle(PS_SOLID, 4);
    for (int i = 1; i < 3; i++) {
        line(i * CELL_SIZE, 20, i * CELL_SIZE, BOARD_SIZE - 20); // 纵线
        line(20, i * CELL_SIZE, BOARD_SIZE - 20, i * CELL_SIZE); // 横线
    }

    // 2. 绘制棋子
    setlinestyle(PS_SOLID, 8); // 线条加粗，增加视觉冲击力
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // 将数组下标转回屏幕中心坐标 (x, y)
            int x = j * CELL_SIZE + CELL_SIZE / 2;
            int y = i * CELL_SIZE + CELL_SIZE / 2;
            int off = CELL_SIZE / 4; // X 棋子的线条偏移量

            if (board_data[i][j] == 'O') {
                setcolor(LIGHTBLUE);
                circle(x, y, CELL_SIZE / 3);
            }
            else if (board_data[i][j] == 'X') {
                setcolor(LIGHTRED);
                line(x - off, y - off, x + off, y + off);
                line(x + off, y - off, x - off, y + off);
            }
        }
    }

    // 3. 绘制文字信息
    settextcolor(YELLOW);
    settextstyle(24, 0, _T("微软雅黑"));
    TCHAR s[30];
    _stprintf_s(s, _T("当前轮次: %c"), current_piece);
    outtextxy(10, 10, s);

    // 关键点：FlushBatchDraw 配合双缓冲，一次性推送画面，避免闪烁
    FlushBatchDraw();
}

// ============================================================================
// 【主控制模块：性能优化的关键】
// ============================================================================

int main() {
    // 初始化 600x600 窗口
    initgraph(BOARD_SIZE, BOARD_SIZE);
    BeginBatchDraw(); // 开启双缓冲模式

    // 首次运行先渲染一遍，否则界面初始是黑色的
    RenderAll();

    bool running = true;
    ExMessage msg;

    /**
     * 【性能优化说明】
     * 为什么不使用 while(peekmessage)？
     * peekmessage 是“看一眼就走”，如果不加控制，CPU 会在死循环里空转。
     * * 这里使用 getmessage(EM_MOUSE)，它会阻塞（挂起）程序。
     * 只有当你移动或点击鼠标时，操作系统才会唤醒程序进行处理。
     * 这对于井字棋这种“静态”游戏来说，是 CPU 占用降到 0 的终极方案。
     */
    while (running) {
        // 等待并获取一个鼠标消息
        msg = getmessage(EM_MOUSE);

        if (msg.message == WM_LBUTTONDOWN) {
            // 将鼠标像素坐标通过“整数除法”直接映射为数组下标
            int col = msg.x / CELL_SIZE;
            int row = msg.y / CELL_SIZE;

            // 只有点在棋盘格内，且该格为空时才有效
            if (row < 3 && col < 3 && board_data[row][col] == '_') {

                // 1. 数据更新
                board_data[row][col] = current_piece;

                // 2. 状态同步：弹出对话框前必须重绘一次，让玩家看到刚落下的棋子
                RenderAll();

                // 3. 胜负检测
                if (CheckWin(current_piece)) {
                    TCHAR end_text[30];
                    _stprintf_s(end_text, _T("玩家 %c 获胜！"), current_piece);
                    MessageBox(GetHWnd(), end_text, _T("游戏结束"), MB_OK);
                    running = false; // 退出循环
                }
                else if (CheckDraw()) {
                    MessageBox(GetHWnd(), _T("棋逢对手，平局！"), _T("游戏结束"), MB_OK);
                    running = false;
                }
                else {
                    // 如果游戏继续，切换玩家并再次刷新界面提示
                    current_piece = (current_piece == 'O') ? 'X' : 'O';
                    RenderAll();
                }
            }
        }
    }

    EndBatchDraw();
    closegraph();
    return 0;
}