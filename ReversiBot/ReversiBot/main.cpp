
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

bool my_color;
map<vvc,pib> game_preproc;
map<vvc,pii> pre_crd;

char chy(int y) {return 'a' + y;}
int indy(char y) {return (int) (y - 'a');}
char chx(int x) {return '1' + x;}
int indx(int x) {return x - 1;}
int direction[8][2] = {{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};

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
        assert(tmp == "init");
        
        
        if(clr == "white") my_color = WHITE;
        else my_color = BLACK;
        
        //
        for(int x = 0; x < 7; ++x) {
            for(int y = 0; y < 8; ++y) {
                data[y][x] = rand()%2;
            }
        }
        data[7][0] = rand()%2;
        data[7][1] = rand()%2;
        data[7][2] = rand()%2;
        data[7][3] = rand()%2;
        //
        
        data[indy('d')][indx(5)] = BLACK;
        data[indy('e')][indx(4)] = BLACK;
        
        data[indy('d')][indx(4)] = WHITE;
        data[indy('e')][indx(5)] = WHITE;
        free_cells = 8;
    }
    
    string getTurnWriting(int x, int y) {
        string ans;
        recalcTable(x, y, my_color);
        ans.push_back(chy(y));
        ans.push_back(' ');
        ans.push_back(chx(x));
        return ans;
    }
};

pib calcPreproc(Table & table, bool color) {
    if(game_preproc.find(table.data) != game_preproc.end()) return game_preproc[table.data];
    pib cur_res = pib(0, true);
    pii crd = pii(0,0);
    for(int y = 0; y < 8; ++y) {
        for(int x = 0; x < 8; ++x) {
            if(table.checkMoveValid(x, y, color)) {
                Table tmp = table;
                tmp.recalcTable(x, y, color);
                pib result = calcPreproc(tmp, !color);
                if(cur_res.second == true) // now this loose pos
                {
                    if(result.second == true) {
                        crd = pii(y,x);
                        cur_res = make_pair(result.first + 1,false); // min way to win
                    } else {
                        cur_res = make_pair(std::max(result.first + 1,cur_res.first), true); // long way too lose
                        crd = cur_res.first == result.first + 1 ? pii(y,x) : crd;
                    }
                }
                else {
                    if(result.second == true) {
                        cur_res = make_pair(std::min(cur_res.first,result.first + 1),false); // min way to win
                        crd = cur_res.first == result.first + 1 ? pii(y,x) : crd;
                    }
                }
            }
        }
    }
    game_preproc[table.data] = cur_res;
    pre_crd[table.data] = crd;
    return cur_res;
    // game_preproc[tmp] = 0;
}

string myTurn(Table & table) {
    string ans;
    int r_max = 0, x_max = -1, y_max = -1;
    if(table.free_cells <= 30) {
        calcPreproc(table, my_color);
        pii crd = pre_crd[table.data];
        x_max = crd.second; y_max = crd.first;
    }
    else
    {
        if(table.free_cells > 20) r_max = 1e6;
        for(int y = 0; y < 8; ++y) {
            for(int x = 0; x < 8; ++x) {
                int reversed = table.checkMoveValid(x, y, my_color);
                if(reversed == 0) continue;
                if(table.free_cells <= 20){
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
    }
    return table.getTurnWriting(x_max, y_max);
}

/***********************************************************************/
void out_db(const Table & table) {
    for(int y = 0; y < 8; ++y) {
        for(int x = 0; x < 8; ++x) {
            cout<<table.data[y][x];
        }
        cout << endl;
    }
}

int main()
{
    Table table;
    table.init();
    string cmd;
    //    out_db();
    while(true) {
        cin >> cmd;
        if(cmd == "turn") {
            cout << "move " + myTurn(table) << endl;
            //            out_db();
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
