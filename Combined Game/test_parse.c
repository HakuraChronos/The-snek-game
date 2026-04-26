#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 100
#define MAX_COLS 100
int map[MAX_ROWS][MAX_COLS];

int main() {
    FILE* file = fopen("The Ocean Map.csv", "r");
    int map_rows = 0;
    int map_cols = 0;
    memset(map, 0, sizeof(map));
    int r = 0, c = 0;
    char buffer[64];
    int b_idx = 0;
    int ch;

    while ((ch = fgetc(file)) != EOF && r < MAX_ROWS) {
        if (ch == ',' || ch == '\n' || ch == '\r') {
            if (b_idx > 0) {
                buffer[b_idx] = '\0';
                map[r][c] = atoi(buffer);
                b_idx = 0;
                c++;
            } else if (ch == ',') {
                map[r][c] = 0;
                c++;
            }
            if (ch == '\n' || ch == '\r') {
                if (c > map_cols) map_cols = c;
                if (c > 0) r++;
                c = 0;
                if (ch == '\r') {
                    int next = fgetc(file);
                    if (next != '\n' && next != EOF) ungetc(next, file);
                }
            }
        } else if (ch != ' ' && ch != '\t') {
            if (b_idx < (int)sizeof(buffer) - 1) {
                buffer[b_idx++] = (char)ch;
            }
        }
    }
    if (b_idx > 0 && r < MAX_ROWS) {
        buffer[b_idx] = '\0';
        map[r][c++] = atoi(buffer);
        if (c > map_cols) map_cols = c;
        r++;
    }
    map_rows = r;
    if (map_rows > 50) map_rows = 50;
    if (map_cols > 50) map_cols = 50;
    printf("[%d][%d] = %d\n", 0, 0, map[0][0]);
    printf("[%d][%d] = %d\n", 0, map_cols-1, map[0][map_cols-1]);
    printf("[%d][%d] = %d\n", map_rows-1, 0, map[map_rows-1][0]);
    printf("[%d][%d] = %d\n", map_rows-1, map_cols-1, map[map_rows-1][map_cols-1]);
    return 0;
}
