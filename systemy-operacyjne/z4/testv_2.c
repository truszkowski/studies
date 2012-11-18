#include <stdio.h>
#include <assert.h>
#include <sys/uio.h>
#include "tests.h"

#define VECLEN 100
#define BUFLEN 60000
#define PARTS 10
#define TESTS 12
#define testfile "test_1.txt"
#define fromfile "/dev/urandom"

#define MAJOR 3
#define MINOR 2

char buf[BUFLEN*VECLEN];
int main() {
	struct test_info info;
	int test, part, fd_random, fd, segs, i;	
	double speeds[PARTS], speeds2[PARTS];
	int limits[] = {   10,   20,   30,   40,   50,  100,  150,  200,   500,  1000,  2000,     0 };
	int sizes[] =  { 2000, 2000, 2000, 2000, 2000, 5000, 5000, 5000, 20000, 20000, 30000, 60000 };
	int parts[] =  {  100,  500,  500,  200,  200,  200,  200,  100,   200,   200,   500,   500 };

	struct iovec v[VECLEN];
	
	if ((fd_random = open(fromfile, O_RDONLY)) < 0 ) {
		fprintf(stderr, "Error in open (%s)!\n", fromfile);
		return 1;
	}
	
	iolimits(1);
	
	for (test = 0; test < TESTS; ++test) {
		int r, w, t, s;
		limit_write(MAJOR,MINOR,limits[test]); /* <- ten limit jest tak naprawde w 1024B/ 1024ms */

		if ((fd = open(testfile, O_CREAT | O_WRONLY, 0666)) < 0) {
			fprintf(stderr, "Error in open (%s)!\n", testfile);
			break;
		}
		
		if ((r = read(fd_random, buf, BUFLEN)) < 0) {
			fprintf(stderr, "Error in read (%s)!\n", fromfile);
			break;
		}
		
		for (i = 0; i < VECLEN; ++i) {
			v[i].iov_base = buf + i*sizes[test]/parts[test];
			v[i].iov_len  = sizes[test]/parts[test];
		}
		
		for (part = 0; part < PARTS; ++part) {
			
			t = 0; w = 0; s = 0;
			for (segs = 0; segs < parts[test]; ++segs) {
				tic();
				if ((w += writev(fd, v, VECLEN)) < 0) {
					fprintf(stderr, "Error in write (%s)!\n", testfile);
					break;
				}
				t += toc();
				usleep(20000);
				s += toc();
			}
			assert(w == sizes[test]*VECLEN);
			
			speeds[part] = (1000.0E0) * ((double) VECLEN*sizes[test]) / ((double) t); /*  (B / usec) = (10000000/1024) * (kB/s) */
			speeds2[part] = (1000.0E0) * ((double) VECLEN*sizes[test]) / ((double) s); /*  (B / usec) = (10000000/1024) * (kB/s) */
			fprintf(stderr, "limit: %d, part: %d time: %d speed: %f speed2: %f count: %d\n", 
				limits[test], part, t, speeds[part], speeds2[part], sizes[test]/parts[test]);
		}
		
		info = analyse(speeds, PARTS);
		
		fprintf(stdout, "=> TEST %d.1 => limit: %dkB/s, data: %dB, avg: %fkB/s, dev: %fkB/s, count: %d len: %dparts\n", 
			test, limits[test], sizes[test], info.avg, info.dev, sizes[test]/parts[test], info.len);
		
		info = analyse(speeds2, PARTS);
		fprintf(stdout, "=> TEST %d.2 => limit: %dkB/s, data: %dB, avg: %fkB/s, dev: %fkB/s, count: %d len: %dparts\n", 
			test, limits[test], sizes[test], info.avg, info.dev, sizes[test]/parts[test], info.len);

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
