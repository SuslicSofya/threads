#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct {
	int left;
	int right;
	int result;
}chiselky;

void* razdel (void* arg) {
	chiselky* copy = (chiselky*) arg;
	copy->result = 0;
	for (int i = copy->left; i <= copy->right; i++) {
		copy->result = copy->result + i;
	}
	return copy;
}

int main (int argc, char** argv) {
	int stock = 0;
	int n, leftt, rightt;
	if (argc != 4) {
		printf("left:");
		scanf("%d", &leftt);
		printf("right:");
		scanf("%d", &rightt);
		printf("n =");
		scanf("%d", &n);
	}
	else {
		leftt = atoi (argv[1]);
		rightt = atoi (argv[2]);
		n = atoi (argv[3]);
	}
	chiselky* memy = malloc (n*sizeof (chiselky));
	pthread_t* ids = malloc (n*sizeof (pthread_t));
	int zeliy = (rightt-leftt)/n;
	printf("∆ = %d\n", zeliy);
	int ostatok = (rightt-leftt)%n;
	printf("d = %d\n", ostatok);
	for (int j = 0; j < n; j++) {
		memy[j].left = leftt+j*zeliy;
		memy[j].right = memy[j].left+zeliy-1;
		if (j == n-1) {
			memy[j].right = leftt+((j+1)*zeliy)+ostatok;
		}
		printf("mem[%d] (%d <-> %d)\n", j, memy[j].left, memy[j].right);
		pthread_create (ids+j, NULL, razdel, &memy[j]);
	}
	for (int j = 0; j<n; j++) {
		pthread_join(ids[j], NULL);
		stock += memy[j].result;
	}
	printf ("Result: %d\n", stock);
	return 0;
}


