#include<atomic>
#include<climits>
#include<assert.h>

using namespace std;
#define CUBE_SIZE (3)
#define MAX_DEPTH (5)

enum Color {
    MAX_COLORS
};

enum Axis {
    x = 0,
    y,
    z,
    MAX_AXIS
};

enum Direction {
    clockwise = 0,
    anticlockwise = 1,
    MAX_DIRS
};

struct Surface {
    Color c[CUBE_SIZE][CUBE_SIZE];
};

class Cube{
private:
    Surface xy[2];
    Surface yz[2];
    Surface zx[2];
    
    bool IsSurfaceSameColor(Surface &s){
        auto color = s.c[0][0];
        for (int i = 0; i < CUBE_SIZE; i++) {
            for (int j = 0; j < CUBE_SIZE; j++) {
                if (s.c[i][j] != color)
                    return false;
            }
        }
        return true;
    }
    
    void RotateSurface(Surface &s, Direction d) {
        auto & a = s.c;
        if (d == clockwise) {
            for (int i = 0; i < CUBE_SIZE -1; i++) {
                for (int j = i; j < CUBE_SIZE/2; j++) {
                    auto tmp = a[i][j];
                    a[i][j] = a[j][CUBE_SIZE-1-i];
                    a[j][CUBE_SIZE-1-i] = a[CUBE_SIZE-1-i][CUBE_SIZE-1-j];
                    a[CUBE_SIZE-1-i][CUBE_SIZE-1-j] = a[CUBE_SIZE-1-j][i];
                    a[CUBE_SIZE-1-j][i] = tmp;
                }
            }
        } else {
            for (int i = 0; i < CUBE_SIZE -1; i++) {
                for (int j = i; j < CUBE_SIZE/2; j++) {
                    auto tmp = a[i][j];
                    a[i][j] = a[CUBE_SIZE-1-j][i];
                    a[CUBE_SIZE-1-j][i] = a[CUBE_SIZE-1-i][CUBE_SIZE-1-j];
                    a[CUBE_SIZE-1-i][CUBE_SIZE-1-j] = a[j][CUBE_SIZE-1-i];
                    a[j][CUBE_SIZE-1-i] = tmp;
                }
            }
        }
    }
    
    void RotateMeshAroundZ(int offset, Direction d) {
        // zx[1] <= yz[0] <= zx[0] <= yz[1]
        if (clockwise == d) {
            for (int i = 0; i < CUBE_SIZE; i++) {
                auto tmp = zx[1].c[offset][i];
                zx[1].c[offset][i] = yz[0].c[i][offset];
                yz[0].c[i][offset] = zx[0].c[offset][CUBE_SIZE-1-i];
                zx[0].c[offset][CUBE_SIZE-1-i] = yz[1].c[CUBE_SIZE-1-i][offset];
                yz[1].c[CUBE_SIZE-1-i][offset] = tmp;
            }
        } else {
            // zx[1] => yz[0] => zx[0] => yz[1]
            for (int i = 0; i < CUBE_SIZE; i++) {
                auto tmp = yz[1].c[CUBE_SIZE-1-i][offset];
                yz[1].c[CUBE_SIZE-1-i][offset] = zx[0].c[offset][CUBE_SIZE-1-i];
                zx[0].c[offset][CUBE_SIZE-1-i] = yz[0].c[i][offset];
                yz[0].c[i][offset] =  zx[1].c[offset][i];
                zx[1].c[offset][i] = tmp;
            }
        }
    }

    void RotateMeshAroundY(int offset, Direction d) {
        // yz[1] <= xy[0] <= yz[0] <= xy[1]
        if (clockwise == d) {
            for (int i = 0; i < CUBE_SIZE; i++) {
                auto tmp = yz[1].c[offset][i];
                yz[1].c[offset][i] = xy[0].c[i][offset];
                xy[0].c[i][offset] = yz[0].c[offset][CUBE_SIZE-1-i];
                yz[0].c[offset][CUBE_SIZE-1-i] = xy[1].c[CUBE_SIZE-1-i][offset];
                xy[1].c[CUBE_SIZE-1-i][offset] = tmp;
            }
        } else {
            // yz[1] => xy[0] => yz[0] => xy[1]
            for (int i = 0; i < CUBE_SIZE; i++) {
                auto tmp = xy[1].c[CUBE_SIZE-1-i][offset];
                xy[1].c[CUBE_SIZE-1-i][offset] = yz[0].c[offset][CUBE_SIZE-1-i];
                yz[0].c[offset][CUBE_SIZE-1-i] = xy[0].c[i][offset];
                xy[0].c[i][offset] = yz[1].c[offset][i];
                yz[1].c[offset][i] = tmp;
            }
        }
    }
    
    void RotateMeshAroundX(int offset, Direction d) {
        // zx[1] <= xy[1] <= zx[0] <= xy[0]
        if (clockwise == d) {
            for (int i = 0; i < CUBE_SIZE; i++) {
                auto tmp = zx[1].c[i][offset];
                zx[1].c[i][offset] = xy[1].c[offset][CUBE_SIZE-1-i];
                xy[1].c[offset][CUBE_SIZE-1-i] = zx[0].c[CUBE_SIZE-1-i][offset];
                zx[0].c[CUBE_SIZE-1-i][offset] = xy[0].c[offset][i];
                xy[0].c[offset][i] = tmp;
            }
        } else {
            // zx[1] => xy[1] => zx[0] => xy[0]
            for (int i = 0; i < CUBE_SIZE; i++) {
                auto tmp = xy[0].c[offset][i];
                xy[0].c[offset][i] = zx[0].c[CUBE_SIZE-1-i][offset];
                zx[0].c[CUBE_SIZE-1-i][offset] = xy[1].c[offset][CUBE_SIZE-1-i];
                xy[1].c[offset][CUBE_SIZE-1-i] = zx[1].c[i][offset];
                zx[1].c[i][offset] = tmp;
            }
        }
    }
    
public:
    bool IsDone() {
        return IsSurfaceSameColor(xy[0]) &&
        IsSurfaceSameColor(xy[1]) &&
        IsSurfaceSameColor(yz[0]) &&
        IsSurfaceSameColor(yz[1]) &&
        IsSurfaceSameColor(zx[0]) &&
        IsSurfaceSameColor(zx[1]);
    }
    
    void RotateAroundAxis(Axis a, int offset, Direction d){
        switch (a) {
            case x:
                if (0 == offset){
                    RotateSurface(yz[0], d);
                } else if (CUBE_SIZE-1 == d){
                    RotateSurface(yz[1], d);
                }
                RotateMeshAroundX(offset, d);
                break;
            case y:
                if (0 == offset){
                    RotateSurface(zx[0], d);
                } else if (CUBE_SIZE-1 == d) {
                    RotateSurface(zx[1], d);
                }
                RotateMeshAroundY(offset, d);
                break;
            case z:
                if (0 == offset){
                    RotateSurface(xy[0], d);
                } else if (CUBE_SIZE-1 == d) {
                    RotateSurface(xy[1], d);
                }
                RotateMeshAroundZ(offset, d);
                break;
            default:
                assert(0 && "Should not reach here");
        }
    }
};


atomic<int> bestFoundDepth(INT_MAX);


void Explore(const Cube &c, int depth) {
    // cutoff
    if (depth >= MAX_DEPTH) {
        return;
    }

    for(int dir = 0; dir < MAX_DIRS; dir++) {
        for(int axis = 0 ; axis < MAX_AXIS; axis++) {
            for(int offset = 0; offset < CUBE_SIZE; offset++) {
                Cube tmp = c;
                tmp.RotateAroundAxis(static_cast<Axis>(axis), offset, static_cast<Direction>(dir));
                if (tmp.IsDone()) {
                    
                    while (1) {
                        auto old = bestFoundDepth.load();
                        if (old < depth)
                            break;
                        if (bestFoundDepth.compare_exchange_strong(old, depth)){
                            break;
                        }
                    }
                    // TODO: report the steps in a stack
                    return;
                }
                Explore(tmp, depth+1);
            }
        }
    }
}

int main() {
    Cube c;//(/* initial config*/);
    Explore(c, 0);
    return 0;
}
