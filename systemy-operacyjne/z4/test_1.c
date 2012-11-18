#include <stdio.h>
#include <assert.h>
#include "tests.h"

#define BUFLEN 8000000
#define PARTS 10
#define TESTS 12
#define testfile "test_1.txt"
#define fromfile "/dev/urandom"

#define MAJOR 3
#define MINOR 2

char buf[BUFLEN];
int main() {
	struct test_info info;
	int test, part, fd_random, fd;
	double speeds[PARTS];
	/*                    0       1       2       3       4       5       6       7        8        9       10       11 */
	int limits[] = {     10,     20,     30,     40,     50,    100,    150,    200,     500,    1000,    2000,       0 };
	int sizes[]  = { 200000, 200000, 200000, 200000, 200000, 500000, 500000, 500000, 2000000, 5000000, 5000000, 5000000 };
	/* czas = ile_bajtow / limit */
	
	if ((fd_random = open(fromfile, O_RDONLY)) < 0 ) {
		fprintf(stderr, "Error in open (%s)!\n", fromfile);
		return 1;
	}
	
	iolimits(1);
	
	for (test = 0; test < TESTS; ++test) {
		int r, w, t;
		limit_write(MAJOR,MINOR,limits[test]); /* <- ten limit jest tak naprawde w 1024B/ 1024ms */

		if ((fd = open(testfile, O_CREAT | O_WRONLY, 0666)) < 0) {
			fprintf(stderr, "Error in open (%s)!\n", testfile);
			break;
		}
		
		if ((r = read(fd_random, buf, BUFLEN)) < 0) {
			fprintf(stderr, "Error in read (%s)!\n", fromfile);
			break;
		}
		
		for (part = 0; part < PARTS; ++part) {
			
			tic();
			if ((w = write(fd, buf, sizes[test])) < 0) {
				fprintf(stderr, "Error in write (%s)!\n", testfile);
				break;
			}
			t = toc();
			assert(w == sizes[test]);
			
			speeds[part] = (1000.0E0) * ((double) w) / ((double) t); /*  (B / usec) = (10000000/1024) * (kB/s) */
			fprintf(stderr, "limit: %d, part: %d time: %d speed: %f\n", limits[test], part, t, speeds[part]);
			sleep(2);
		}
		
		info = analyse(speeds, PARTS);
		
		fprintf(stdout, "=> TEST %d => limit: %dkB/s, data: %dB, avg: %fkB/s, dev: %fkB/s, len: %dparts\n", test, limits[test], sizes[test], info.avg, info.dev, info.len);
		fflush(stdout);
		
		if (close(fd)) {
			fprintf(stderr, "Error in close %s!\n", testfile);
			break;
		}
	}
		
	iolimits(0);

	close(fd_random);
	
	return 0;
}
