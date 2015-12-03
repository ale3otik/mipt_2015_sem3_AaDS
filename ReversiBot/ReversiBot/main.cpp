//
//  main.cpp
//  ReversiBot
//
//  Created by Alex on 03.12.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <time.h>
using namespace std;
typedef long long ll;
#define WHITE 1
#define BLACK 0
#define EMPTY 2
bool my_color;
int table[8][8]; // [y][x]

bool checkMoveValid(int xs, int ys, int color);
string myTurn();
void recalcTable(int xs, int ys, int color);

char chy(int y) {return 'a' + y;}
int indy(char y) {return (int) (y - 'a');}
char chx(int x) {return '1' + x;}
int indx(int x) {return x - 1;}

int direction[8][2] = {{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};
bool isValidPos(int x, int y) {
    return x < 8 && x >= 0 && y < 8 && y >= 0;
}

void init() {
    srand((int)time(NULL));
    string tmp;
    string clr;
    cin >> tmp >> clr;
    assert(tmp == "init");

    if(clr == "white") my_color = WHITE;
    else my_color = BLACK;
    
    for(int x = 0; x < 8; ++x) {
        for(int y = 0; y < 8; ++y) {
            table[y][x] = EMPTY;
        }
    }
    table[indy('d')][indx(5)] = BLACK;
    table[indy('e')][indx(4)] = BLACK;
    
    table[indy('d')][indx(4)] = WHITE;
    table[indy('e')][indx(5)] = WHITE;
}

string myTurn() {
    string ans;
    for(int y = 0; y < 8; ++y) {
        for(int x = 0; x < 8; ++x) {
            if(checkMoveValid(x, y, my_color)){
                recalcTable(x, y, my_color);
                ans.push_back(chy(y));
                ans.push_back(' ');
                ans.push_back(chx(x));
                return ans;
            }
        }
    }
    
    assert(1);
    return NULL;
}

bool checkMoveValid(int xs, int ys, int color) {
    if(table[ys][xs] != EMPTY) return false;
    
    bool isValidMove = false;
    for(int dind = 0; dind < 8; ++dind) {
        int y_dir = direction[dind][0];
        int x_dir = direction[dind][1];
        int x = xs + x_dir,y = ys + y_dir;
        bool another_color = false;
        while(isValidPos(x,y))
        {
            if(table[y][x] == !color) {
                another_color = true;
                x += x_dir; y+= y_dir;
                continue;
            }
            else if(table[y][x] == color && another_color == true) {
                isValidMove = true;
            }
            break;
        }
    }
    return isValidMove;
}

void recalcTable(int xs, int ys, int color) {
    assert(table[ys][xs] == EMPTY);
    
    table[ys][xs] = color;
    bool isValidMove = false;
    for(int dind = 0; dind < 8; ++dind) {
        int y_dir = direction[dind][0];
        int x_dir = direction[dind][1];
        int x = xs + x_dir,y = ys + y_dir;
        bool another_color = false;
        while(isValidPos(x,y))
        {
            if(table[y][x] == !color) {
                another_color = true;
                x += x_dir; y += y_dir;
                continue;
            }
            else if(table[y][x] == color && another_color == true) {
                isValidMove = true;
              
                for(int ix = xs + x_dir, iy = ys + y_dir; iy != y || ix != x; ix += x_dir,iy += y_dir) {
                    table[iy][ix] = color;
                }
                break;
            }
            break;
        }
    }
    
    assert(isValidMove);
}
void out_db() {
    for(int y = 0; y < 8; ++y) {
        for(int x = 0; x < 8; ++x) {
            cout<<table[y][x];
        }
        cout << endl;
    }
}
int main()
{
    init();
    string cmd;
//    out_db();
    while(true) {
        cin >> cmd;
        if(cmd == "turn") {
            cout << "move " + myTurn() << endl;
//            out_db();
        } else if(cmd == "move") {
            int x; char y;
            cin >> y >> x;
            recalcTable(indx(x),indy(y),!my_color);
        }
        else {
            return 0;
        }
    }
    return 0;
}
