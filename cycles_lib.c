// cycles_lib.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* 判断 path（长度 k，path[0] 是起点）在正向和反向中是否为“正向更小” */
static int is_forward_canonical(const int *path, int k)
{
    /* 比较 (s, v1, ..., v_{k-1}) 和 (s, v_{k-1}, ..., v1) */
    for (int i = 1; i < k; ++i)
    {
        int f = path[i];
        int r = path[k - i];  /* 反向对应位置 */
        if (f < r) return 1;  /* 正向字典序更小，保留 */
        if (f > r) return 0;  /* 反向更小，丢弃 */
    }
    return 1; /* 极少见相等情况，随便保留 */
}

/* ========= 1. 只判断是否存在长度为 k 的圈 ========= */

static int exist_found;
static int exist_n, exist_k;
static const unsigned char *exist_adj;

static void dfs_exist(int start, int current, int depth,
                      int *path, char *visited)
{
    if (exist_found) return;

    if (depth == exist_k)
    {
        if (exist_adj[current * exist_n + start] &&
            is_forward_canonical(path, exist_k))
        {
            exist_found = 1;
        }
        return;
    }

    for (int v = 0; v < exist_n; ++v)
    {
        if (!exist_adj[current * exist_n + v]) continue;  /* 不是邻点 */
        if (v == start) continue;                         /* 只允许在 depth==k 时回起点 */
        if (visited[v]) continue;                         /* 不重复顶点 */
        if (v <= start) continue;                         /* start 为整圈最小顶点 */

        visited[v] = 1;
        path[depth] = v;
        dfs_exist(start, v, depth + 1, path, visited);
        visited[v] = 0;

        if (exist_found) return;
    }
}

/* 返回：1 存在长度为 k 的简单圈；0 不存在 */
int cycles_exists(int n, int k, const unsigned char *adj)
{
    if (k < 3 || k > n) return 0;

    exist_n   = n;
    exist_k   = k;
    exist_adj = adj;
    exist_found = 0;

    int  *path    = (int *)malloc(sizeof(int) * k);
    char *visited = (char *)malloc(sizeof(char) * n);
    if (!path || !visited)
    {
        fprintf(stderr, "memory error in cycles_exists\n");
        free(path);
        free(visited);
        return 0;
    }

    for (int s = 0; s < n && !exist_found; ++s)
    {
        memset(visited, 0, (size_t)n);
        path[0] = s;
        visited[s] = 1;
        dfs_exist(s, s, 1, path, visited);
    }

    free(path);
    free(visited);
    return exist_found ? 1 : 0;
}

/* ========= 2. 枚举长度为 k 的圈（可限制条数） ========= */

static int enum_n, enum_k;
static const unsigned char *enum_adj;
static int *enum_out;
static int enum_max_cycles;
static int enum_per_cycle;
static int enum_count;
static int enum_stop;

/* 内部 DFS，用于枚举圈 */
static void dfs_enum(int start, int current, int depth,
                     int *path, char *visited)
{
    if (enum_stop) return;

    if (depth == enum_k)
    {
        if (enum_adj[current * enum_n + start] &&
            is_forward_canonical(path, enum_k))
        {
            if (enum_count < enum_max_cycles)
            {
                int base = enum_count * enum_per_cycle;
                /* 写入路径顶点 */
                for (int i = 0; i < enum_k; ++i)
                    enum_out[base + i] = path[i];
                if (enum_per_cycle == enum_k + 1)
                    enum_out[base + enum_k] = start;
                enum_count++;
            }
            else
            {
                enum_stop = 1; /* 达到上限，停止搜索 */
            }
        }
        return;
    }

    for (int v = 0; v < enum_n; ++v)
    {
        if (!enum_adj[current * enum_n + v]) continue;  /* 不是邻点 */
        if (v == start) continue;                       /* 只在 depth==k 时回起点 */
        if (visited[v]) continue;                       /* 简单圈：不能重复顶点 */
        if (v <= start) continue;                       /* start 为整圈最小顶点，去旋转重复 */

        visited[v] = 1;
        path[depth] = v;
        dfs_enum(start, v, depth + 1, path, visited);
        visited[v] = 0;

        if (enum_stop) return;
    }
}

/*
 * 枚举长度为 k 的简单圈
 *
 * 参数：
 *   n           顶点数
 *   k           圈长度
 *   max_cycles  最多输出多少条圈
 *   adj         n×n 邻接矩阵（0/1），行优先
 *   out_cycles  输出缓冲区，长度至少 max_cycles * (store_closed ? (k+1) : k)
 *   store_closed !=0 表示每条圈存 k+1 个顶点（最后再重复起点），否则只存 k 个
 *   truncated   输出参数：若搜索因为达到 max_cycles 被截断，则写 1，否则写 0
 *
 * 返回：实际输出的圈数量（<= max_cycles）
 */
int cycles_enumerate(int n, int k, int max_cycles,
                     const unsigned char *adj,
                     int *out_cycles,
                     int store_closed,
                     int *truncated)
{
    if (truncated) *truncated = 0;
    if (k < 3 || k > n || max_cycles <= 0) return 0;

    enum_n   = n;
    enum_k   = k;
    enum_adj = adj;
    enum_out = out_cycles;
    enum_max_cycles = max_cycles;
    enum_per_cycle  = store_closed ? (k + 1) : k;
    enum_count = 0;
    enum_stop  = 0;

    int  *path    = (int *)malloc(sizeof(int) * k);
    char *visited = (char *)malloc(sizeof(char) * n);
    if (!path || !visited)
    {
        fprintf(stderr, "memory error in cycles_enumerate\n");
        free(path);
        free(visited);
        return 0;
    }

    for (int s = 0; s < n && !enum_stop; ++s)
    {
        memset(visited, 0, (size_t)n);
        path[0] = s;
        visited[s] = 1;
        dfs_enum(s, s, 1, path, visited);
    }

    free(path);
    free(visited);

    if (truncated && enum_stop)
        *truncated = 1;

    return enum_count;
}
