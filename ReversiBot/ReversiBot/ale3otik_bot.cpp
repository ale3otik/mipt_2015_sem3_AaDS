
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <time.h>
#include <map>
using namespace std;
typedef long long ll;
typedef pair<int, bool> pib; // min stages to loose ; is loose
typedef pair<int, int> pii;
typedef vector<vector<char> > vvc;
#define WHITE 1
#define BLACK 0
#define EMPTY 2
#define START_COUNT 12
#define mp make_pair

bool my_color;
map<pair<vvc,bool>,int> game_end_preproc; // max reachable end
map<pair<vvc,bool>,pii> pre_end_crd;

vector<pii> free_cells;
vector<bool> used;
typedef long long ll;

char chy(int y) {return 'a' + y;}
int indy(char y) {return (int) (y - 'a');}
char chx(int x) {return '1' + x;}
int indx(int x) {return x - 1;}
int direction[8][2] = {{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};
//int priority1[8][8] = {
//    { 50, -50, 25,  10,  10, 25, -50,  50},
//    {-50, -50, 15, -10, -10, 15, -50, -50},
//    { 25,  15, 35,   5,   5, 35,  15,  25},
//    { 10, -10,  5,   0,   0,  5, -10,  10},
//    { 10, -10,  5,   0,   0,  5, -10,  10},
//    { 25,  15,  35,  5,   5,  35, 15,  25},
//    {-50, -50,  15,-10, -10,  15,-50, -50},
//    { 50, -50,  25, 10,  10,  25,-50,  50}
//};

int priority[8][8] = {
    { 800, -100, 25,  10,  10, 25, -100,  800},
    {-100, -100, 15, -10, -10, 15, -100, -100},
    { 25,  15, 35,   5,   5, 35,  15,  25},
    { 10, -10,  5,   5,   5,  5, -10,  10},
    { 10, -10,  5,   5,   5,  5, -10,  10},
    { 25,  15,  35,  5,   5,  35, 15,  25},
    {-100, -100,  15,-10, -10,  15,-100, -100},
    { 800, -100,  25, 10,  10,  25,-100,  800}
};


ll start_time;
class Table {
public:
    vvc data;
    int free_cells;
    Table() {
        free_cells = 60;
    }
    
    Table(const Table & parent) {
        data = parent.data;
        free_cells = parent.free_cells;
    }
    
    static bool isValidPos(int x, int y) {
        return x < 8 && x >= 0 && y < 8 && y >= 0;
    }
    
    void recalcTable(int xs, int ys, int color) {
        assert(data[ys][xs] == EMPTY);
        int reversed = 0;
        
        data[ys][xs] = color;
        bool isValidMove = false;
        for(int dind = 0; dind < 8; ++dind) {
            int y_dir = direction[dind][0];
            int x_dir = direction[dind][1];
            int x = xs + x_dir,y = ys + y_dir;
            bool another_color = false;
            while(isValidPos(x,y))
            {
                if(data[y][x] == !color) {
                    another_color = true;
                    x += x_dir; y += y_dir;
                    continue;
                }
                else if(data[y][x] == color && another_color == true) {
                    isValidMove = true;
                    reversed += max(abs(x - xs)-1, abs(y - ys) - 1);
                    for(int ix = xs + x_dir, iy = ys + y_dir; iy != y || ix != x; ix += x_dir,iy += y_dir) {
                        data[iy][ix] = color;
                    }
                    break;
                }
                break;
            }
        }
        
        assert(isValidMove);
        this->free_cells -= 1;
        // return reversed;
    }
    
    int checkMoveValid(int xs, int ys, int color) {
        int reversed = 0;
        if(data[ys][xs] != EMPTY) return false;
        
        for(int dind = 0; dind < 8; ++dind) {
            int y_dir = direction[dind][0];
            int x_dir = direction[dind][1];
            int x = xs + x_dir,y = ys + y_dir;
            bool another_color = false;
            while(isValidPos(x,y))
            {
                if(data[y][x] == !color) {
                    another_color = true;
                    x += x_dir; y+= y_dir;
                    continue;
                }
                else if(data[y][x] == color && another_color == true) {
                    reversed += max(abs(x - xs)-1, abs(y - ys) - 1);
                }
                break;
            }
        }
        return reversed;
    }
    
    void init() {
        data = vvc(8,vector<char>(8,EMPTY));
        srand((int)time(NULL));
        string tmp;
        string clr;
        cin >> tmp >> clr;
        
        if(clr == "white") my_color = WHITE;
        else my_color = BLACK;

        data[indy('d')][indx(5)] = BLACK;
        data[indy('e')][indx(4)] = BLACK;
        
        data[indy('d')][indx(4)] = WHITE;
        data[indy('e')][indx(5)] = WHITE;
    }
    
    string getTurnWriting(int x, int y) {
        string ans;
        recalcTable(x, y, my_color);
        ans.push_back(chy(y));
        ans.push_back(' ');
        ans.push_back(chx(x));
        return ans;
    }
    ll getPriority(bool color) {
        ll ans = 0;
        for(int x = 0; x < 8; ++x) {
            for(int y = 0; y < 8; ++y) {
                if(data[y][x] == color) ans += priority[y][x];
//                if(data[y][x] == !color) ans -= priority[y][x];
//                if(checkMoveValid(x, y, color)) ans += 150;
                if(checkMoveValid(x, y, !color)) ans -= 150;
            }
        }
        
        int start_pos[4][2] = {{7,0},{7,7},{0,0},{0,7}};
        int diag_dir[4][2] = {{1,1},{-1,1},{1,-1},{-1,-1}};
        int diag_ortho[4][2] = {{-1,0},{0,-1},{0,1},{1,0}};
            for(int k = 0; k < 4; ++k) {
                int x_dir = diag_dir[k][1];
                int y_dir = diag_dir[k][0];
                int lvl;
                bool is_ok = true;
                int count = 0;
                for(lvl = 0; lvl < 5; ++lvl) {
                    if(!is_ok) break;
                    int s_x = start_pos[k][1] + diag_ortho[k][1] * lvl;
                    int s_y = start_pos[k][0] + diag_ortho[k][0] * lvl;
                    while(isValidPos(s_x, s_y)) {
                        if(data[s_y][s_x] != color){
                            is_ok = false;
                            break;
                        }
                        else {
                            s_x += x_dir;
                            s_y += y_dir;
                            ++count;
                        }
                    }
                }
             // ans += (bool)clr == color ? (lvl-1) * 150 : -(lvl-1)*150;
                ans += count * 90;
            }

        int lng_dir[8][2] = {{0,1},{0,-1},{0,1},{0,-1},{1,0},{-1,0},{1,0},{-1,0}};
        int startyx[8][2] = {{0,0},{0,7},{7,0},{7,7},{0,0},{7,0},{0,7},{7,7}};
//        for(int clr = 0; clr < 2; ++clr) {
            for(int i = 0; i < 4; ++i) {
                for(int k = 0; k < 2; ++k) {
                    int dir_y = lng_dir[i * 2 + k][0];
                    int dir_x = lng_dir[i * 2 + k][1];
                    int x = startyx[i*2 + k][1], y =startyx[i*2 + k][0];
                    int count = 0;
                    while(isValidPos(x, y) && data[y][x] == color) {
                        ++count;
                        x += dir_x;
                        y += dir_y;
                    }
                    ans += count * 90 ;
                }
//            }
        }
        
        int strt[4][2] = {{0,0},{7,7},{7,0},{0,7}};
        int diag[4][2] = {{1,1},{-1,-1},{-1,1},{1,-1}};
        
            for(int i = 0; i < 4; ++i) {
                int x = strt[i][1];
                int y = strt[i][0];
                int x_dir = diag[i][1];
                int y_dir = diag[i][0];
                if(data[y][x] == EMPTY && data[y + y_dir][x + x_dir] == color) {
                    y += 2 * y_dir;
                    x += 2 * x_dir;
                    int coef = 0;
                    while(isValidPos(x, y)) {
                        if(data[y][x] == EMPTY){
                            coef = -900;
                            break;
                        } else {
                            if(data[y][x] == !color) {
                                coef = -1800;
                                break;
                            }
                        }
                        x += x_dir; y += y_dir;
                    }
                    ans += coef;
                }
            }
//
        return ans;
    }
};

int countGameResult(Table & table, bool mcolor) {
    int diff = 0;
        
        for(int i = 0; i < 8; ++i) {
            for(int j = 0;j < 8; ++j) {
                if(table.data[i][j] == mcolor) ++diff;
                else if(table.data[i][j] == !mcolor) --diff;
            }
        }
    return diff;

}

pii getMaxPriority(Table & table, bool color) {
        ll curans = -1e18;
        pii crd(-1,-1);
        for(int i = 0; i < free_cells.size(); ++i) {
            int x = free_cells[i].second;
            int y = free_cells[i].first;
            if(table.checkMoveValid(x, y, color)) {
                Table tmp = table;
                tmp.recalcTable(x, y, color);
                ll next = tmp.getPriority(color);
                if(next > curans) {
                    curans = next;
                    crd = pii(y,x);
                }
            }
        }
        return crd;
}

int calcPreproc(Table & table, bool color, bool is_last_turn_empty,int deep) {
    if(game_end_preproc.find(mp(table.data,color)) != game_end_preproc.end()) return game_end_preproc[mp(table.data,color)];
    int cur_diff = -1e8;
    pii crd = pii(-1,-1);
    for(int i = 0; i < free_cells.size(); ++i) {
        if((clock() - start_time)/(1.0*CLOCKS_PER_SEC) > 2.7) return cur_diff;
        if(used[i]) continue;
        int x = free_cells[i].second;
        int y = free_cells[i].first;
        if(table.checkMoveValid(x, y, color)) {
            Table tmp = table;
            tmp.recalcTable(x, y, color);
            used[i] = true;
            int diff = -calcPreproc(tmp, !color, false,deep + 1);
            used[i] = false;
            if(diff == 1e8) diff = -1e8;
            if(diff > cur_diff){
                cur_diff = diff;
                crd = pii(y,x);
            }
        }
    }

    // don't have any turn
    if(crd.first == -1) {
        if(!is_last_turn_empty) {
            cur_diff = -calcPreproc(table, !color, true,deep + 1);
        } else {
            cur_diff = countGameResult(table,color);
        }
    }

    game_end_preproc[mp(table.data,color)] = cur_diff;
    pre_end_crd[mp(table.data,color)] = crd;
    return cur_diff;
    // game_end_preproc[tmp] = 0;
}
pii simpleTurn(Table & table) {
    int r_max = 0, x_max = -1, y_max = -1;
    if(table.free_cells > 65) r_max = 1e6;
        for(int y = 0; y < 8; ++y) {
            for(int x = 0; x < 8; ++x) {
                int reversed = table.checkMoveValid(x, y, my_color);
                if(reversed == 0) continue;
                if(table.free_cells <= 65){
                    if(reversed > r_max) {
                        x_max = x;
                        y_max = y;
                        r_max = reversed;
                    }
                }
                else {
                    if(reversed < r_max) {
                        x_max = x;
                        y_max = y;
                        r_max = reversed;
                    }
                }
            }
        }
        return pii(y_max,x_max);
}

void countFree(Table & table) {
    free_cells.clear();
    for(int y = 0; y < 8; ++y) {
        for(int x = 0; x < 8; ++x) {
            if(table.data[y][x] == EMPTY) {
                free_cells.push_back(pii(y,x));
            }
        }
    }
    used.assign(free_cells.size(), false);
}

string myTurn(Table & table) {
    string ans;
    pii crd(-1,-1);
    pre_end_crd.clear();
    game_end_preproc.clear();
    countFree(table);
    if(table.free_cells <= START_COUNT) {
        calcPreproc(table, my_color, false,1);
    } else {
            crd = getMaxPriority(table,my_color);
    }
    
    if(table.free_cells <= START_COUNT && game_end_preproc.find(mp(table.data,my_color)) != game_end_preproc.end()){

        if(game_end_preproc[mp(table.data,my_color)] < 0 && table.free_cells >= 9) {
            used.assign(free_cells.size(), false);
            crd = getMaxPriority(table,my_color);
        }
        else {
            crd = pre_end_crd[mp(table.data,my_color)];
        }
    }
    else
    {
        if(crd.first == -1) crd = simpleTurn(table);
    }
    return table.getTurnWriting(crd.second,crd.first);
}

/***********************************************************************/
void out_db(const Table & table) {
    for(int y = 0; y < 8; ++y) {
        for(int x = 0; x < 8; ++x) {
            if(table.data[y][x] == BLACK) cout<<'B';
            if(table.data[y][x] == WHITE) cout<<'W';
            if(table.data[y][x] == EMPTY) cout<<'O';
        }
        cout << endl;
    }
}

int main()
{
    free_cells.reserve(60);
    used.reserve(60);
    Table table = Table();
    table.init();
//    out_db(table);
    string cmd;
    //    out_db();
    while(true) {
        cin >> cmd;
        if(cmd == "turn") {
            start_time = clock();
            cout << "move " + myTurn(table) << endl;
//            out_db(table);
        } else if(cmd == "move") {
            int x; char y;
            cin >> y >> x;
            table.recalcTable(indx(x),indy(y),!my_color);
        }
        else {
            return 0;
        }
    }
    return 0;
}
