#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cstring>
#include <climits>
#include <algorithm>
#include <cmath>
#include <thread>
#include <atomic>
#include <chrono>

char board[9][3][3];
char winBoard[3][3];
std::string mode="air";
std::string gameRecord="^";
char nowPlayer[2];

void Welcome();
void InitMainUI();
void cleanBoard();
void printBoard();
void InitGameMode(char player);
bool issWin(int idx,char player);
bool isBoardFull(int b);
bool isbWin(char player);
bool isGameOver();
bool place(int boardIdx, int row, int col, char player);
void recordMove(int s_int);
void printAIEmotion(char aiPiece);
static int evaluate(char player, int forcedBoard = -1);
static int rootForcedBoard();

/**AI**/
void Random(char player);
void Heuristic(char player);
void Rule_Based(char player);
void Minimax(char player);
void Alpha_Beta(char player);
void MCTS(char player);

int lastBoard = -1;
int lastRow = -1;
int lastCol = -1;
int aiBoardIdx = -1;
int aiRow = -1;
int aiCol = -1;


class playerData {
public:
    char piece;   // 'X' 或 'O'
};

void RunGame(){
    Welcome();
    InitMainUI();
    if(mode=="Exit") return;

    cleanBoard();
    gameRecord="^";
    playerData play;
    play.piece = 'X';

    int row{0};
    int col{0};

    int Brow{0};
    int Bcol{0};

    int Idx{0};
    int cnt{1};

    char aiPiece = '.';
    if(mode != "friend"){
        aiPiece = (std::rand() % 2 == 0) ? 'X' : 'O';
    }

    while(!isGameOver()){

        if(mode != "friend") {
            printAIEmotion(aiPiece);
        }

        printBoard();
        std::cout << "now is " << cnt <<"th " << play.piece << " move." << std::endl;;
        row = 0;
        col = 0;
        Brow = 0;
        Bcol = 0;
        Idx = 0;

        if(play.piece == aiPiece){
            std::cout << "AI thinking..." << std::endl;
            InitGameMode(play.piece);
            std::cout << "Done!" << std::endl;

            Idx = aiBoardIdx;
            row = aiRow;
            col = aiCol;

            if(Idx < 0 || row < 1 || col < 1 || !place(Idx, row, col, play.piece)){
                std::cout << "AI failed to move! Falling back to random..." << std::endl;
                Random(play.piece);
                Idx = aiBoardIdx;
                row = aiRow;
                col = aiCol;
                if(Idx < 0 || row < 1 || col < 1 || !place(Idx, row, col, play.piece)){
                    std::cout << "Catastrophic failure. Exiting game." << std::endl;
                    return;
                }
            }   

            recordMove((row - 1) * 3 + (col - 1));
        }
        else{
            if(gameRecord.back() == '^'){
                std::cout << "Free move." << std::endl << "(You can place in any unfinished big board.)" << std::endl;
                std::cout << "Enter big board row col (1-3): ";
                std::cin >> Brow >> Bcol;
                if(Brow < 1 || Brow > 3 || Bcol < 1 || Bcol > 3) {
                    std::cout << ">>> Invalid input!" << std::endl << "Big board row and col must be 1-3." << std::endl;
                    continue;
                }
                Idx = (Brow -1) *3 + (Bcol -1);
                if(winBoard[Idx/3][Idx%3] != '.') {
                    std::cout << ">>> This big board is already finished (Win/Draw)." << std::endl <<"Choose another one!" << std::endl;
                    continue;
                }

                std::cout << "Enter small board row col (1-3): "<<std::endl;
                std::cin >> row >> col;
                std::system("cls");
                if(row < 1 || row > 3 || col < 1 || col > 3) {
                    std::cout << ">>> Invalid input!" << std::endl << "Small board row and col must be 1-3." << std::endl;
                    continue;
                }
                if(board[Idx][row-1][col-1] != '.') {
                    std::cout << ">>> This cell is already occupied." << std::endl << "Try again!" << std::endl;
                    continue;
                }

                if(place(Idx,row,col,play.piece)){
                    recordMove(((row-1) *3 + (col -1)));
                }
            }
            else{
                Idx = gameRecord.back() - '0';
                std::cout << ">>> You are forced to play in big board ["
                          << (Idx/3 + 1) << "][" << (Idx%3 + 1) << "]." << std::endl;
                std::cout << "Enter small board row col (1-3): "<<std::endl;
                std::cin >> row >> col;
            	std::system("cls");
                        
                if(row < 1 || row > 3 || col < 1 || col > 3) {
                    std::cout << ">>> Invalid input!" << std::endl << "Small board row and col must be 1-3." << std::endl;
                    continue;
                }
                if(board[Idx][row-1][col-1] != '.') {
                    std::cout << ">>> This cell is already occupied." << std::endl << "Try again!" << std::endl;
                    continue;
                }

                if(place(Idx,row,col,play.piece)){
                    recordMove(((row-1) *3 + col -1));
                }
            }
        }
        std::system("cls");
        if(issWin(Idx,play.piece)){
            winBoard[Idx/3][Idx%3] = play.piece;
        }
        else {
            if(isBoardFull(Idx)) winBoard[Idx/3][Idx%3] = 'D';
        }
        if(isbWin(play.piece)){
            break;
        }
        int nextBoard = (row-1)*3 + (col-1);
        if(winBoard[nextBoard/3][nextBoard%3] != '.'){
            gameRecord += '^';
        }


        play.piece = (play.piece == 'X') ? 'O' : 'X';
        cnt++;
    }
    if(isbWin('X'))      std::cout << "player X is Winner!" << std::endl;
    else if(isbWin('O')) std::cout << "player O is Winner!" << std::endl;
    else                 std::cout << "It's a DRAW! No winner." << std::endl;
    return;
    return;
}

int main(){
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::system("cls");
    while(1){
        RunGame();
        if(mode=="Exit") break;
    }
    return 0;
}


void InitMainUI(){
while(1){
    std::cout<<"[1]Play with AI"<<std::endl;
    std::cout<<"[2]play with friends"<<std::endl;
    std::cout<<"[3]Exit"<<std::endl;
    int l{0};
    std::cin>>l;
    if(l==1){
        l=0;
        std::cout<<"Please select an algorithm"<<std::endl;
        std::cout<<"[1]Random"<<std::endl;
        std::cout<<"[2]Heuristic"<<std::endl;
        std::cout<<"[3]Rule-Based"<<std::endl;
        std::cout<<"[4]Minimax"<<std::endl;
        std::cout<<"[5]Alpha-Beta"<<std::endl;
        std::cout<<"[6]MCTS"<<std::endl;
        std::cin>>l;
        if(l==1) mode="Random";
        else if(l==2) mode="Heuristic";
        else if(l==3) mode="Rule-Based";
        else if(l==4) mode="Minimax";
        else if(l==5) mode="Alpha-Beta";
        else if(l==6) mode="MCTS";
        else{
            std::cout<<"Please enter again"<<std::endl;
            std::system("cls");
            continue;
        }
            return;
        }//over
    if(l==2){
        mode="friend";
        return;
    }
    if(l==3){
        mode="Exit";
        return;//exit
    }
    std::cout<<"Please enter again"<<std::endl;
    }//while over;
    return;
}

void cleanBoard(){
    for(int i = 0; i < 9; i++)
        for(int h = 0; h < 3; h++)
            for(int l = 0; l < 3; l++)
                board[i][h][l] = '.';

    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            winBoard[i][j] = '.';
    return;
}

void printAIEmotion(char aiPiece) {
    char enemy = (aiPiece == 'X') ? 'O' : 'X';

    if (isbWin(aiPiece)) {
        std::cout << "( >w< )" << std::endl;
        return;
    }
    if (isbWin(enemy)) {
        std::cout << " ( T~T )" << std::endl;
        return;
    }

    int score = evaluate(aiPiece, rootForcedBoard());

    if (score > 800) {
        std::cout << " ( :D )" << std::endl;
    }
    else if (score < -800) {
        std::cout << " ( <O_O> )" << std::endl;
    }
    else {
        std::cout << "( awa )" << std::endl;
    }
}

void printBoard(){
    std::cout << std::endl << "   [ 1 ]  |  [ 2 ]  |  [ 3 ] " << std::endl;
    for(int bigRow = 0; bigRow < 3; bigRow++){
        for(int h = 0; h < 3; h++){
            std::cout<<(bigRow * 3) + (h + 1);
            for(int bigCol = 0; bigCol < 3; bigCol++){
                int idx = bigRow * 3 + bigCol;
                for(int l = 0; l < 3; l++){
                    if(idx == lastBoard && h == lastRow && l == lastCol){
                        std::cout << "[" << board[idx][h][l] << "]";
                    }
                    else{
                        std::cout << " " << board[idx][h][l] << " ";
                    }
                }
                if(bigCol < 2) std::cout << "|";
            }
            std::cout << std::endl;
        }
        if(bigRow != 2) std::cout << "----------+---------+---------" << std::endl;
    }
    std::cout << std::endl;
}

void Welcome(){
    std::cin.ignore();
    std::cout<<"                ##      ##  #######   ###         #######      #######    #### ####    #######"<<std::endl;
    std::cout<<"                ##  ##  ##  ##        ###        ###      #   ##     ##  ###  ##  ##   ##"<<std::endl;
    std::cout<<"                ##  ##  ##  #####     ###        ##           ##     ##  ###  ##  ###  #####"<<std::endl;
    std::cout<<"                ### ## ###  ##        ###        ###      #   ##     ##  ###      ###  ##"<<std::endl;
    std::cout<<"                 #### ###   ########  #########   #######      #######   ###      ###  #######"<<std::endl;
    std::cout<<std::endl<<std::endl<<std::endl<<std::endl;
    std::cout<<"######  ##   ##  ######   ######  ######      ######  ######  ######    ######   ####   ######    ######  ######  ######"<<std::endl;
    std::cout<<"##      ##   ##  ##   ##  ##      ##   ##       ##      ##    ##          ##    ##  ##  ##          ##    ##  ##  ##    "<<std::endl;
    std::cout<<"######  ##   ##  ######   ######  ######  ####  ##      ##    ##    ####  ##    ######  ##    ####  ##    ##  ##  ######"<<std::endl;
    std::cout<<"    ##  ##   ##  ##       ##      ## ##         ##      ##    ##          ##    ##  ##  ##          ##    ##  ##  ##    "<<std::endl;
    std::cout<<"######   ######  ##       ######  ##  ###       ##    ######  ######      ##    ##  ##  ######      ##    ######  ######"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"                                            Press Enter to continue..."<<std::endl;
    std::cin.get();
    std::cin.ignore();
    return;
}

void InitGameMode(char player){

    if(mode=="Random") Random(player);
    if(mode=="Heuristic") Heuristic(player);
    if(mode=="Rule-Based") Rule_Based(player);
    if(mode=="Minimax") Minimax(player);
    if(mode=="Alpha-Beta") Alpha_Beta(player);
    if(mode=="MCTS") MCTS(player);
    return;
}

bool isGameOver(){
    if(isbWin('X') || isbWin('O')) return true;
    for(int i = 0; i < 9; i++)
        if(winBoard[i/3][i%3] == '.') return false;
    return true;
}

bool place(int boardIdx, int row, int col, char player){
    if(boardIdx < 0 || boardIdx > 8) return false;
    if(row < 1 || row > 3 || col < 1 || col > 3) return false;
    if(winBoard[boardIdx/3][boardIdx%3] != '.') return false;
    if(board[boardIdx][row -1][col -1] != '.') return false;

    board[boardIdx][row -1][col -1] = player;
    lastBoard = boardIdx;
    lastRow = row - 1;
    lastCol = col - 1;
    return true;
}

void recordMove(int s_int){
    char s = s_int+'0';
    gameRecord += s;
}

bool issWin(int idx,char player){
    bool is{true};

    for(int i = 0; i < 3; i++){
        is = true;
        for(int j = 0; j< 3; j++){
            if(board[idx][i][j] != player) is = false;
        }
        if(is) return true;
    }


    for(int i = 0; i < 3; i++){
        is = true;
        for(int j = 0; j< 3; j++){
            if(board[idx][j][i] != player) is = false;
        }
        if(is) return true;
    }


    is = true;
    for(int i = 0; i < 3; i++){
        if(board[idx][i][i] != player) is = false;
    }
    if(is) return true;

    is = true;
    for(int i = 0; i < 3; i++){
        if(board[idx][i][2-i] != player) is = false;
    }
    if(is) return true;

    return false;
}

bool isBoardFull(int b) {
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (board[b][r][c] == '.') return false;
    return true;
}

bool isbWin(char player){
    bool is{true};

    for(int i = 0; i < 3; i++){
        is = true;
        for(int j = 0; j < 3; j++){
            if(winBoard[i][j] != player) is =false;
        }
        if(is) return true;
    }


    for(int i = 0; i < 3; i++){
        is = true;
        for(int j = 0; j < 3; j++){
            if(winBoard[j][i] != player) is =false;
        }
        if(is) return true;
    }

    is = true;
    for(int i = 0; i < 3; i++)
    if(winBoard[i][i] != player) is =false;
    if(is) return true;

    is = true;
    for(int i = 0; i < 3; i++)
    if(winBoard[i][2-i] != player) is = false;
    if(is) return true;

    return false;
}

// ============================================================
//                       屎山之王 · 优化版
// ============================================================
static std::clock_t gStart;
static int     gTimeLimitMs = 9000;
static bool    gTimeout     = false;
static long long gNodes     = 0;

// Zobrist 哈希
static unsigned long long ZOBRIST[9][3][3][3];
static unsigned long long ZOBRIST_WIN[3][3][3];
static bool gZobristReady = false;

static void zobristInit() {
    if (gZobristReady) return;
    unsigned long long s = 88172645463325252ULL;
    for (int i = 0; i < 100000; i++) {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17;
        if (i < 9 * 3 * 3 * 3) {
            int idx = i;
            ZOBRIST[idx / 27][(idx / 9) % 3][(idx / 3) % 3][idx % 3] = s;
        }
    }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++) {
                s ^= s << 13; s ^= s >> 7; s ^= s << 17;
                ZOBRIST_WIN[i][j][k] = s;
            }
    gZobristReady = true;
}

static unsigned long long hashBoard() {
    unsigned long long h = 0;
    for (int b = 0; b < 9; b++)
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++) {
                int v = (board[b][r][c] == 'X') ? 1 : (board[b][r][c] == 'O') ? 2 : 0;
                h ^= ZOBRIST[b][r][c][v];
            }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int v = (winBoard[i][j] == 'X') ? 1 : (winBoard[i][j] == 'O') ? 2 : 0;
            h ^= ZOBRIST_WIN[i][j][v];
        }
    return h;
}

// 置换表
struct TTEntry {
    unsigned long long hash;
    int depth;
    int score;
    int flag;
    int bestB, bestR, bestC;
};
static const int TT_BITS = 21;
static const int TT_SIZE = 1 << TT_BITS;
static const int TT_MASK = TT_SIZE - 1;
static TTEntry* gTT = nullptr;

static void ttInit() {
    if (!gTT) {
        gTT = new TTEntry[TT_SIZE];
    }
    std::memset(gTT, 0, sizeof(TTEntry) * TT_SIZE);
}

// 杀手与历史启发
struct KillerSlot { int b[3], r[3], c[3], cnt; };
static KillerSlot gKiller[64];
static int gHistory[9][3][3];

static void heurReset() {
    std::memset(gKiller, 0, sizeof(gKiller));
    std::memset(gHistory, 0, sizeof(gHistory));
    gNodes = 0;
    gTimeout = false;
}

static bool checkTime() {
    if (gTimeout) return true;
    if (((std::clock() - gStart) * 1000 / CLOCKS_PER_SEC) > gTimeLimitMs) {
        gTimeout = true;
        return true;
    }
    return false;
}

// ============================================================
//               评估函数 · 史诗级加强版
// ============================================================
static int evaluate(char player, int forcedBoard) {
    char enemy = (player == 'X') ? 'O' : 'X';
    int score = 0;

    // 1. 大棋盘得分
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (winBoard[i][j] == player) {
                score += 1500;
                if (i == 1 && j == 1) score += 500;
            }
            else if (winBoard[i][j] == enemy) {
                score -= 1500;
                if (i == 1 && j == 1) score -= 500;
            }
        }
    }

    const int lines[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };

    // 2. 大棋盘连线威胁 —— 第三个格子必须是 '.'
    for (int k = 0; k < 8; k++) {
        int p = 0, e = 0, empty = 0;
        for (int i = 0; i < 3; i++) {
            char w = winBoard[lines[k][i][0]][lines[k][i][1]];
            if      (w == player) p++;
            else if (w == enemy)  e++;
            else if (w == '.')    empty++;
            // 'D' 既不算 p 也不算 e
        }
        if (p == 2 && e == 0 && empty == 1) score += 100000;
        else if (e == 2 && p == 0 && empty == 1) score -= 100000;
    }

    // 3. 小棋盘内部打分
    for (int b = 0; b < 9; b++) {
        if (winBoard[b / 3][b % 3] != '.') continue;

        bool isCurrentBoard = (forcedBoard < 0) || (forcedBoard == b);

        int globalThreat = 0;
        for (int k = 0; k < 8; k++) {
            bool hasMe = false, hasEnemy = false;
            for (int i = 0; i < 3; i++) {
                if (lines[k][i][0] == b / 3 && lines[k][i][1] == b % 3) {
                    for (int j = 0; j < 3; j++) {
                        if (winBoard[lines[k][j][0]][lines[k][j][1]] == player) hasMe = true;
                        if (winBoard[lines[k][j][0]][lines[k][j][1]] == enemy) hasEnemy = true;
                    }
                }
            }
            if (hasMe && !hasEnemy) globalThreat += 20;
        }

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                if (board[b][r][c] == player) {
                    if (r == 1 && c == 1) score += 15 + globalThreat;
                    else if (r == 1 || c == 1) score += 8 + globalThreat;
                    else score += 4 + globalThreat;
                }
                else if (board[b][r][c] == enemy) {
                    if (r == 1 && c == 1) score -= 15 + globalThreat;
                    else if (r == 1 || c == 1) score -= 8 + globalThreat;
                    else score -= 4 + globalThreat;
                }
            }
        }

        for (int k = 0; k < 8; k++) {
            int p = 0, e = 0, empty = 0;
            for (int i = 0; i < 3; i++) {
                char v = board[b][lines[k][i][0]][lines[k][i][1]];
                if      (v == player) p++;
                else if (v == enemy)  e++;
                else if (v == '.')    empty++;
            }

            if (p == 2 && e == 0 && empty == 1) {
                if (isCurrentBoard) score += 8000 + globalThreat * 100;
                else                score += 800;

                if (isCurrentBoard) {
                    int threats = 0;
                    for (int kk = 0; kk < 8; kk++) {
                        int pp = 0, ee = 0, emp = 0;
                        for (int ii = 0; ii < 3; ii++) {
                            char vv = board[b][lines[kk][ii][0]][lines[kk][ii][1]];
                            if      (vv == player) pp++;
                            else if (vv == enemy)  ee++;
                            else if (vv == '.')    emp++;
                        }
                        if (pp == 2 && ee == 0 && emp == 1) threats++;
                    }
                    if (threats >= 2) score += 100000;
                }
            }
            else if (e == 2 && p == 0 && empty == 1) {
                if (isCurrentBoard) score -= 5000 + globalThreat * 50;
                else                score -= 500;
            }
            else if (p == 1 && e == 0) score += 10;
            else if (e == 1 && p == 0) score -= 10;
        }
    }

    return score;
}

// ============================================================
//                       走法生成
// ============================================================
static int genMoves(int forcedBoard, int* bs, int* rs, int* cs) {
    bool useForced = false;
    if (forcedBoard >= 0 && forcedBoard <= 8
        && winBoard[forcedBoard / 3][forcedBoard % 3] == '.') {
        for (int r = 0; r < 3 && !useForced; r++)
            for (int c = 0; c < 3; c++)
                if (board[forcedBoard][r][c] == '.') { useForced = true; break; }
    }
    int cnt = 0;
    if (useForced) {
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                if (board[forcedBoard][r][c] == '.') {
                    bs[cnt] = forcedBoard; rs[cnt] = r; cs[cnt] = c; cnt++;
                }
    }
    else {
        for (int b = 0; b < 9; b++) {
            if (winBoard[b / 3][b % 3] != '.') continue;
            for (int r = 0; r < 3; r++)
                for (int c = 0; c < 3; c++)
                    if (board[b][r][c] == '.') {
                        bs[cnt] = b; rs[cnt] = r; cs[cnt] = c; cnt++;
                    }
        }
    }
    return cnt;
}

static int nextForcedFrom(int r, int c) {
    int nf = r * 3 + c;
    if (winBoard[nf / 3][nf % 3] != '.') return -1;
    for (int rr = 0; rr < 3; rr++)
        for (int cc = 0; cc < 3; cc++)
            if (board[nf][rr][cc] == '.') return nf;
    return -1;
}

static int rootForcedBoard() {
    if (gameRecord.empty() || gameRecord.back() == '^') return -1;
    int fb = gameRecord.back() - '0';
    if (fb < 0 || fb > 8) return -1;
    if (winBoard[fb / 3][fb % 3] != '.') return -1;
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (board[fb][r][c] == '.') return fb;
    return -1;
}

// ============================================================
//           Alpha-Beta 搜索核心（PVS + TT + 杀手 + 历史）
// ============================================================
static int alphaBeta(char aiPlayer, char turn, int forcedBoard, int depth, int alpha, int beta, bool canNull) {
    (void)canNull;
    gNodes++;
    if ((gNodes & 1023) == 0 && checkTime()) return 0;

    char enemy = (aiPlayer == 'X') ? 'O' : 'X';

    if (isbWin(aiPlayer)) return 100000 + depth;
    if (isbWin(enemy))    return -100000 - depth;
    if (depth <= 0)       return evaluate(aiPlayer, forcedBoard);

    unsigned long long h = hashBoard();
    if (turn == 'X') h ^= 0x9E3779B97F4A7C15ULL;
    else             h ^= 0xC2B2AE3D27D4EB4FULL;
    h ^= (unsigned long long)(forcedBoard + 2) * 0x165667B19E3779F9ULL;

    int idx = (int)(h & TT_MASK);
    TTEntry& entry = gTT[idx];
    int ttB = -1, ttR = -1, ttC = -1;

    if (entry.hash == h) {
        if (entry.depth >= depth) {
            if (entry.flag == 0) return entry.score;
            if (entry.flag == 1 && entry.score >= beta) return entry.score;
            if (entry.flag == 2 && entry.score <= alpha) return entry.score;
        }
        ttB = entry.bestB; ttR = entry.bestR; ttC = entry.bestC;
    }

    int bs[81], rs[81], cs[81];
    int cnt = genMoves(forcedBoard, bs, rs, cs);
    if (cnt == 0) return evaluate(aiPlayer, forcedBoard);

    int scores[81];
    int dIdx = depth < 64 ? depth : 63;

    for (int i = 0; i < cnt; i++) {
        int b = bs[i], r = rs[i], c = cs[i];
        scores[i] = 0;
        if (b == ttB && r == ttR && c == ttC) scores[i] += 1 << 24;
        for (int k = 0; k < gKiller[dIdx].cnt; k++) {
            if (gKiller[dIdx].b[k] == b && gKiller[dIdx].r[k] == r && gKiller[dIdx].c[k] == c) {
                scores[i] += 1 << 16;
                break;
            }
        }
        scores[i] += gHistory[b][r][c];
    }

    for (int i = 0; i < cnt; i++)
        for (int j = i + 1; j < cnt; j++)
            if (scores[j] > scores[i]) {
                std::swap(scores[i], scores[j]);
                std::swap(bs[i], bs[j]); std::swap(rs[i], rs[j]); std::swap(cs[i], cs[j]);
            }

    int origAlpha = alpha;
    int bestScore = (turn == aiPlayer) ? -999999 : 999999;
    int bestB = bs[0], bestR = rs[0], bestC = cs[0];
    int moveIdx = 0;

    for (int i = 0; i < cnt; i++) {
        int b = bs[i], r = rs[i], c = cs[i];

        board[b][r][c] = turn;
        char ow = winBoard[b / 3][b % 3];
        bool wonThisBoard = issWin(b, turn);
        if (wonThisBoard) {
            winBoard[b / 3][b % 3] = turn;
        } else if (isBoardFull(b)) {
            winBoard[b / 3][b % 3] = 'D';
        }

        int nf = nextForcedFrom(r, c);
        int s;

        if (moveIdx == 0) {
            s = alphaBeta(aiPlayer, enemy, nf, depth - 1, alpha, beta, true);
        }
        else {
            int reduction = 0;
            if (depth >= 3 && moveIdx >= 4 && !wonThisBoard) {
                reduction = (moveIdx >= 8) ? 2 : 1;
                if (reduction >= depth - 1) reduction = depth - 2;
                if (reduction < 0) reduction = 0;
            }

            s = alphaBeta(aiPlayer, enemy, nf, depth - 1 - reduction, alpha, alpha + 1, true);

            if (!gTimeout && s > alpha && s < beta) {
                s = alphaBeta(aiPlayer, enemy, nf, depth - 1, alpha, beta, true);
            }
        }

        board[b][r][c] = '.';
        winBoard[b / 3][b % 3] = ow;

        if (gTimeout) return bestScore;
        moveIdx++;

        if (turn == aiPlayer) {
            if (s > bestScore) { bestScore = s; bestB = b; bestR = r; bestC = c; }
            if (s > alpha) alpha = s;
            if (alpha >= beta) {
                if (gKiller[dIdx].cnt < 3) {
                    gKiller[dIdx].b[gKiller[dIdx].cnt] = b;
                    gKiller[dIdx].r[gKiller[dIdx].cnt] = r;
                    gKiller[dIdx].c[gKiller[dIdx].cnt] = c;
                    gKiller[dIdx].cnt++;
                }
                gHistory[b][r][c] += depth * depth * 8;
                break;
            }
        }
        else {
            if (s < bestScore) { bestScore = s; bestB = b; bestR = r; bestC = c; }
            if (s < beta) beta = s;
            if (alpha >= beta) {
                gHistory[b][r][c] += depth * depth * 8;
                break;
            }
        }
    }

    entry.hash = h;
    entry.depth = depth;
    entry.score = bestScore;
    if (bestScore <= origAlpha) entry.flag = 2;
    else if (bestScore >= beta) entry.flag = 1;
    else entry.flag = 0;
    entry.bestB = bestB; entry.bestR = bestR; entry.bestC = bestC;

    return bestScore;
}

// ============================================================
//                        Random (最弱)
// ============================================================
void Random(char player) {
    (void)player;
    int bs[81], rs[81], cs[81];
    int cnt = genMoves(rootForcedBoard(), bs, rs, cs);
    if (cnt == 0) { aiBoardIdx = -1; return; }
    int pick = std::rand() % cnt;
    aiBoardIdx = bs[pick]; aiRow = rs[pick] + 1; aiCol = cs[pick] + 1;
}

// ============================================================
//                       Heuristic (弱)
// ============================================================
void Heuristic(char player) {
    int bs[81], rs[81], cs[81];
    int cnt = genMoves(rootForcedBoard(), bs, rs, cs);
    if (cnt == 0) { aiBoardIdx = -1; return; }

    int bestScore = -999999;
    int bestB = bs[0], bestR = rs[0], bestC = cs[0];

    for (int i = 0; i < cnt; i++) {
        int b = bs[i], r = rs[i], c = cs[i];
        board[b][r][c] = player;
        char ow = winBoard[b / 3][b % 3];
        if (issWin(b, player)) {
            winBoard[b / 3][b % 3] = player;
        } else if (isBoardFull(b)) {
            winBoard[b / 3][b % 3] = 'D';
        }
        int nf = nextForcedFrom(r, c);
        int s = evaluate(player, nf);
        board[b][r][c] = '.';
        winBoard[b / 3][b % 3] = ow;
        if (s > bestScore) { bestScore = s; bestB = b; bestR = r; bestC = c; }
    }
    aiBoardIdx = bestB; aiRow = bestR + 1; aiCol = bestC + 1;
}

// ============================================================
//                      Rule_Based (中)
// ============================================================
void Rule_Based(char player) {
    char enemy = (player == 'X') ? 'O' : 'X';
    int bs[81], rs[81], cs[81];
    int cnt = genMoves(rootForcedBoard(), bs, rs, cs);
    if (cnt == 0) { aiBoardIdx = -1; return; }

    // 规则1：能赢全局
    for (int i = 0; i < cnt; i++) {
        int b = bs[i], r = rs[i], c = cs[i];
        board[b][r][c] = player;
        char ow = winBoard[b / 3][b % 3];
        bool w = issWin(b, player);
        if (w) winBoard[b / 3][b % 3] = player;
        bool gw = isbWin(player);
        board[b][r][c] = '.';
        winBoard[b / 3][b % 3] = ow;
        if (gw) { aiBoardIdx = b; aiRow = r + 1; aiCol = c + 1; return; }
    }
    // 规则2：能赢小棋盘
    for (int i = 0; i < cnt; i++) {
        int b = bs[i], r = rs[i], c = cs[i];
        board[b][r][c] = player;
        bool w = issWin(b, player);
        board[b][r][c] = '.';
        if (w) { aiBoardIdx = b; aiRow = r + 1; aiCol = c + 1; return; }
    }
    // 规则3：堵对手小棋盘
    for (int i = 0; i < cnt; i++) {
        int b = bs[i], r = rs[i], c = cs[i];
        board[b][r][c] = enemy;
        bool w = issWin(b, enemy);
        board[b][r][c] = '.';
        if (w) { aiBoardIdx = b; aiRow = r + 1; aiCol = c + 1; return; }
    }
    // 规则4：抢中心
    for (int i = 0; i < cnt; i++) {
        if (rs[i] == 1 && cs[i] == 1) { aiBoardIdx = bs[i]; aiRow = 2; aiCol = 2; return; }
    }
    // 规则5：随机
    int pick = std::rand() % cnt;
    aiBoardIdx = bs[pick]; aiRow = rs[pick] + 1; aiCol = cs[pick] + 1;
}

// ============================================================
//                      Minimax (纯暴力版)
// ============================================================
static int pureMinimax(char aiPlayer, char turn, int forcedBoard, int depth) {
    char enemy = (aiPlayer == 'X') ? 'O' : 'X';
    if (isbWin(aiPlayer)) return 100000 + depth;
    if (isbWin(enemy))    return -100000 - depth;
    if (depth == 0)       return evaluate(aiPlayer, forcedBoard);

    int bs[81], rs[81], cs[81];
    int cnt = genMoves(forcedBoard, bs, rs, cs);
    if (cnt == 0) return evaluate(aiPlayer, forcedBoard);

    int best = (turn == aiPlayer) ? -999999 : 999999;
    for (int i = 0; i < cnt; i++) {
        int b = bs[i], r = rs[i], c = cs[i];
        board[b][r][c] = turn;
        char ow = winBoard[b / 3][b % 3];
        if (issWin(b, turn)) {
            winBoard[b / 3][b % 3] = turn;
        } else if (isBoardFull(b)) {
            winBoard[b / 3][b % 3] = 'D';
        }
        int nf = nextForcedFrom(r, c);
        int s = pureMinimax(aiPlayer, enemy, nf, depth - 1);
        board[b][r][c] = '.';
        winBoard[b / 3][b % 3] = ow;
        if (turn == aiPlayer) { if (s > best) best = s; }
        else                  { if (s < best) best = s; }
    }
    return best;
}

void Minimax(char player) {
    char enemy = (player == 'X') ? 'O' : 'X';
    int depth = 3;

    int bs[81], rs[81], cs[81];
    int cnt = genMoves(rootForcedBoard(), bs, rs, cs);
    if (cnt == 0) { aiBoardIdx = -1; return; }

    int bestScore = -999999;
    int bestB = bs[0], bestR = rs[0], bestC = cs[0];

    for (int i = 0; i < cnt; i++) {
        int b = bs[i], r = rs[i], c = cs[i];
        board[b][r][c] = player;
        char ow = winBoard[b / 3][b % 3];
        if (issWin(b, player)) {
            winBoard[b / 3][b % 3] = player;
        } else if (isBoardFull(b)) {
            winBoard[b / 3][b % 3] = 'D';
        }
        int nf = nextForcedFrom(r, c);
        int s;
        if (isbWin(player)) s = 100000;
        else s = pureMinimax(player, enemy, nf, depth - 1);
        board[b][r][c] = '.';
        winBoard[b / 3][b % 3] = ow;
        if (s > bestScore) { bestScore = s; bestB = b; bestR = r; bestC = c; }
    }
    aiBoardIdx = bestB; aiRow = bestR + 1; aiCol = bestC + 1;
}

// ============================================================
//               Alpha_Beta 满配版 (最强)
// ============================================================
void Alpha_Beta(char player) {
    zobristInit();
    ttInit();
    heurReset();

    gStart = std::clock();
    gTimeLimitMs = 9000;

    char enemy = (player == 'X') ? 'O' : 'X';
    if (gameRecord == "^") {
        if (board[4][1][1] == '.') {
            aiBoardIdx = 4; aiRow = 2; aiCol = 2;
            return;
        }
    }
    int rootForced = rootForcedBoard();

    int bs[81], rs[81], cs[81];
    int cnt = genMoves(rootForced, bs, rs, cs);
    if (cnt == 0) { aiBoardIdx = -1; return; }

    int bestB = bs[0], bestR = rs[0], bestC = cs[0];
    int prevB = bestB, prevR = bestR, prevC = bestC;

    // 迭代加深
    for (int depth = 1; depth <= 20; depth++) {
        if (checkTime()) break;

        int curBestScore = -999999;
        int curBestB = bs[0], curBestR = rs[0], curBestC = cs[0];
        int alpha = -999999, beta = 999999;

        int scores[81];
        for (int i = 0; i < cnt; i++) {
            int b = bs[i], r = rs[i], c = cs[i];
            scores[i] = 0;
            if (b == prevB && r == prevR && c == prevC) scores[i] += 1 << 25;
            scores[i] += gHistory[b][r][c];
        }
        for (int i = 0; i < cnt; i++)
            for (int j = i + 1; j < cnt; j++)
                if (scores[j] > scores[i]) {
                    std::swap(scores[i], scores[j]);
                    std::swap(bs[i], bs[j]); std::swap(rs[i], rs[j]); std::swap(cs[i], cs[j]);
                }

        bool completed = true;
        int moveIdx = 0;

        for (int i = 0; i < cnt; i++) {
            int b = bs[i], r = rs[i], c = cs[i];

            board[b][r][c] = player;
            char ow = winBoard[b / 3][b % 3];
            if (issWin(b, player)) {
                winBoard[b / 3][b % 3] = player;
            } else if (isBoardFull(b)) {
                winBoard[b / 3][b % 3] = 'D';
            }

            int nf = nextForcedFrom(r, c);
            int s;
            if (isbWin(player)) s = 100000 + depth;
            else {
                if (moveIdx == 0) {
                    s = alphaBeta(player, enemy, nf, depth - 1, alpha, beta, true);
                }
                else {
                    s = alphaBeta(player, enemy, nf, depth - 1, alpha, alpha + 1, true);
                    if (!gTimeout && s > alpha && s < beta) {
                        s = alphaBeta(player, enemy, nf, depth - 1, alpha, beta, true);
                    }
                }
            }

            board[b][r][c] = '.';
            winBoard[b / 3][b % 3] = ow;

            if (gTimeout) { completed = false; break; }

            if (s > curBestScore) {
                curBestScore = s;
                curBestB = b; curBestR = r; curBestC = c;
            }
            if (s > alpha) alpha = s;
            moveIdx++;
        }

        if (completed) {
            bestB = curBestB; bestR = curBestR; bestC = curBestC;
            prevB = curBestB; prevR = curBestR; prevC = curBestC;
            std::cout << "depth " << depth << "  score " << curBestScore << std::endl;
        }
        if (curBestScore >= 100000) break;
    }

    aiBoardIdx = bestB;
    aiRow = bestR + 1;
    aiCol = bestC + 1;

    long long elapsed = (std::clock() - gStart) * 1000 / CLOCKS_PER_SEC;
    std::cout << "[Alpha-Beta] nodes=" << gNodes << " time=" << elapsed << "ms" << std::endl;
}

// ============================================================
//                     MCTS · 启发式增强版
// ============================================================
static int mctsRollout(char firstPlayer, char enemy, int b0, int r0, int c0) {
    char backupB[9][3][3];
    char backupW[3][3];
    std::memcpy(backupB, board, sizeof(board));
    std::memcpy(backupW, winBoard, sizeof(winBoard));

    board[b0][r0][c0] = firstPlayer;
    if (issWin(b0, firstPlayer)) winBoard[b0 / 3][b0 % 3] = firstPlayer;
    else if (isBoardFull(b0))     winBoard[b0 / 3][b0 % 3] = 'D';

    char turn = enemy;
    int nf = nextForcedFrom(r0, c0);
    int safety = 0;

    while (!isbWin(firstPlayer) && !isbWin(enemy) && safety < 300) {
        safety++;
        int allB[81], allR[81], allC[81];
        int n = genMoves(nf, allB, allR, allC);
        if (n == 0) break;

        int pick = std::rand() % n;
        if (std::rand() % 100 < 30) {
            for (int i = 0; i < n; i++) {
                if (allR[i] == 1 && allC[i] == 1) { pick = i; break; }
            }
        }

        int bb = allB[pick], rr = allR[pick], cc = allC[pick];
        board[bb][rr][cc] = turn;
        if (issWin(bb, turn)) {
            winBoard[bb / 3][bb % 3] = turn;
        } else if (isBoardFull(bb)) {
            winBoard[bb / 3][bb % 3] = 'D';
        }
        nf = nextForcedFrom(rr, cc);
        turn = (turn == 'X') ? 'O' : 'X';
    }

    int result = 0;
    if (isbWin(firstPlayer)) result = 1;

    std::memcpy(board, backupB, sizeof(board));
    std::memcpy(winBoard, backupW, sizeof(winBoard));
    return result;
}

void MCTS(char player) {
    zobristInit();
    char enemy = (player == 'X') ? 'O' : 'X';
    int maxSim = 900;
    std::clock_t start = std::clock();
    int timeMs = 3900;

    int bs[81], rs[81], cs[81];
    int cnt = genMoves(rootForcedBoard(), bs, rs, cs);
    if (cnt == 0) { aiBoardIdx = -1; return; }

    int bestB = bs[0], bestR = rs[0], bestC = cs[0];
    int bestVisits = 0;

    int winsArr[81] = { 0 };
    int visitsArr[81] = { 0 };

    for (int sim = 0; sim < maxSim; sim++) {
        if (((std::clock() - start) * 1000 / CLOCKS_PER_SEC) > timeMs) break;

        int pick;
        if (sim < cnt) pick = sim;
        else {
            double bestUCB = -1e18;
            pick = 0;
            for (int i = 0; i < cnt; i++) {
                if (visitsArr[i] == 0) { pick = i; break; }
                double ucb = (double)winsArr[i] / visitsArr[i]
                    + 1.414 * std::sqrt(std::log((double)(sim + 1)) / visitsArr[i]);
                if (ucb > bestUCB) { bestUCB = ucb; pick = i; }
            }
        }

        int result = mctsRollout(player, enemy, bs[pick], rs[pick], cs[pick]);
        visitsArr[pick]++;
        winsArr[pick] += result;
    }

    for (int i = 0; i < cnt; i++) {
        if (visitsArr[i] > bestVisits) {
            bestVisits = visitsArr[i];
            bestB = bs[i]; bestR = rs[i]; bestC = cs[i];
        }
    }

    aiBoardIdx = bestB;
    aiRow = bestR + 1;
    aiCol = bestC + 1;

    long long elapsed = (std::clock() - start) * 1000 / CLOCKS_PER_SEC;
    std::cout << "[MCTS] sims=" << bestVisits << " total=" << maxSim
        << " time=" << elapsed << "ms" << std::endl;
}
