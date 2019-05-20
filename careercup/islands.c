#include <stdio.h>

void explore(int **map, size_t m, size_t n, int i, int j) {
	if (i > m || i < 0 || j > n || j < 0)
		return;
	if (map[i][j] == 0)
		return
	map[i][j] = 2;
	explore(map, m, n, i-1, j);
	explore(map, m, n, i+1, j);
	explore(map, m, n, i, j-1);
	explore(map, m, n, i, j+1);
}

int islands(int **map, size_t m, size_t n) {
	int i, j;
	int islands = 1;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			if (map[i][j] == 1) {
				islands++;
				explore(map, m, n, i, j);
			}
		}
	}
	return islands;
}
	
