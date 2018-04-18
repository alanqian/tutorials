#include <iostream>
#include <string.h>

// 迷宫 【问题描述】
// 给定一个N*M方格的迷宫，迷宫里有T处障碍，障碍处不可通过。给定起点坐标和
// 终点坐标，问: 每个方格最多经过1次，有多少种从起点坐标到终点坐标的方案。在迷宫
// 中移动有上下左右四种方式，每次只能移动一个方格。数据保证起点上没有障碍。
// 输入样例 输出样例
//
// 【数据规模】
// 1≤N,M≤5
//
// 题目描述
// 输入输出格式
// 输入格式：
//
// 【输入】
// 第一行N、M和T，N为行，M为列，T为障碍总数。第二行起点坐标SX,SY，终点
// 坐标FX,FY。接下来T行，每行为障碍点的坐标。
// 输出格式：
//
// 【输出】
// 给定起点坐标和终点坐标，问每个方格最多经过1次，从起点坐标到终点坐标的方
// 案总数。
// 输入输出样例
// 输入样例#1： 复制
// 2 2 1
// 1 1 2 2
// 1 2
// 输出样例#1： 复制
// 1

#ifdef DEBUG
#define DEBUG_LOG(fmt, args...) \
      do { \
          fprintf(stderr, "DEBUG: %s():%d: " fmt "\n",  __func__, __LINE__, ##args); \
      } while(0)
#else
#define DEBUG_LOG(fmt, args...)    void(0)/* Don't do anything in release builds */
#endif

using namespace std;

// Graph: class for a depth-first search graph
class Graph
{
  public:
    enum {
        kUpper = 0,
        kBelow,
        kLeft,
        kRight,
        kMaxSibling = 4,
        kMaxVertex = 5 * 5,
    };
    // ctor
    Graph();
    // create graph by width/height/stop_xy
    bool create(const int width, const int height, const int* stop_xy, const int stopper_cnt);
    // dfs search graph from node to stop_node
    void dfs(const int node, const int stop_node, int& count, bool* visited) const;
    // debug dump
    void dump(ostream& os) const;

  public:
    // (x,y) to index helper
    static int xyToIndex(int x, int y, int width);

  private:
    int edge_[kMaxVertex][kMaxSibling];  // node index for upper/below/left/right
    int num_vertex_; // count of vertex
};

// ctor
Graph::Graph()
{
    num_vertex_ = 0;
}

// (x,y) to index helper
inline int Graph::xyToIndex(int row, int col, int width)
{
    return row * width + col;
}

// create graph
bool Graph::create(const int width, const int height, const int* stop_xy, const int stopper_cnt)
{
    // no edge
    memset(edge_, -1, sizeof(edge_));
    if (width <= 0 || height <= 0 || width * height > kMaxVertex) {
        return false;
    }
    num_vertex_ = width * height;

    // connect each grid with upper/below/left/right
    int index = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++index, ++j) {
            // 0: upper, 1: below, 2: left, 3: right
            if (i > 0) {
                // connect to upper index
                edge_[index][kUpper] = index - width;
            }
            if (i < height - 1)
            {
                // connect to below index
                edge_[index][kBelow] = index + width;
            }
            if (j > 0) {
                // connect to left index
                edge_[index][kLeft] = index - 1;
            }
            if (j < width - 1) {
                // connect to right index
                edge_[index][kRight] = index + 1;
            }
        }
    }

    // remove edges with stop index
    for (int k = 0; k < stopper_cnt; ++k, stop_xy += 2) {
        int i = stop_xy[0];
        int j = stop_xy[1];
        if (i < 0 || i >= width ||
            j < 0 || j >= height) {
            // ignore invalid stop index
            continue;
        }
        index = xyToIndex(i, j, width);
        DEBUG_LOG("stop:%d", index);

        if (i > 0) {
            // disconnect from upper index
            edge_[index][kUpper] = -1;
            edge_[index - width][kBelow] = -1;
        }
        if (i < height - 1)
        {
            // disconnect from below index
            edge_[index][kBelow] = -1;
            edge_[index + width][kUpper] = -1;
        }
        if (j > 0) {
            // disconnect from left index
            edge_[index][kLeft] = -1;
            edge_[index - 1][kRight] = -1;
        }
        if (j < width - 1) {
            // disconnect from right index
            edge_[index][kRight] = -1;
            edge_[index + 1][kLeft] = -1;
        }
    }
    // created successfully
    return true;
}

// dfs search graph from node to stop_node
void Graph::dfs(const int node, const int stop_node, int& count, bool* visited) const
{
    if (node == stop_node) {
        // found then exit
        ++count;
        return;
    }
    for (int i = 0; i < kMaxSibling; i++) {
        int index = edge_[node][i];
        if (index >= 0 && index < num_vertex_ && !visited[index]) {
            visited[index] = true;  // set to avoid re-entering
            dfs(index, stop_node, count, visited);
            visited[index] = false; // clear for other paths
        }
    }
}

// debug dump
void Graph::dump(ostream& os) const
{
    os << "num_vertex_: " << num_vertex_ << endl;
    for (int i = 0; i < num_vertex_; ++i) {
        char buffer[128];
        sprintf(buffer, "[%d] %d, %d, %d, %d\n",
                i, edge_[i][0], edge_[i][1], edge_[i][2], edge_[i][3]);
        os << buffer;
    }
}

// main
int main(int argc, const char *argv[])
{
    Graph graph;
    int n = 0;
    int m = 0;
    int t = 0;
    int sx = 0;
    int sy = 0;
    int fx = 0;
    int fy = 0;
    int stop_xy[Graph::kMaxVertex * 2];

    // get input from stdin
    std::ios::sync_with_stdio(false);
    cin >> n >> m >> t;
    cin >> sx >> sy >> fx >> fy;
    int* p = stop_xy;
    for (int i = 0; i < t && i < Graph::kMaxVertex; ++i) {
        int x, y;
        cin >> x >> y;
        DEBUG_LOG("stop:(%d,%d)", x - 1, y - 1);
        *p++ = x - 1;
        *p++ = y - 1;
    }

    int start = Graph::xyToIndex(sx - 1, sy - 1, n);
    int stop = Graph::xyToIndex(fx - 1, fy - 1, n);
    DEBUG_LOG("N:%d, M:%d, T:%d", n, m, t);
    DEBUG_LOG("sx:%d, sy:%d, fx:%d, fy:%d", sx, sy, fx, fy);
    DEBUG_LOG("start:%d, stop:%d", start, stop);

    // create graph
    if (!graph.create(n, m, stop_xy, t)) {
        cerr << "failed to create graph, check data!" << endl;
        return -1;
    }
#ifdef DEBUG
    DEBUG_LOG("graph:");
    graph.dump(cout);
    DEBUG_LOG(">>");
#endif
    DEBUG_LOG("---");

    int count = 0;
    bool visited[Graph::kMaxVertex];
    memset(visited, 0, sizeof(visited));
    graph.dfs(start, stop, count, visited);
    cout << count << endl;
    return 0;
}
