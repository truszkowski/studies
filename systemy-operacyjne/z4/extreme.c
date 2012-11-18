#include <stdio.h>
#include <assert.h>
#include "tests.h"

#define BUFLEN 10000000
#define PARTS 10
#define TESTS 5
#define file1 "/root/testfile_e_1.test"
#define file2 "/mnt/hda6/testfile_e_2.test"
#define fromfile "/dev/urandom"

/* ext3 */
#define MAJORr 3
#define MINORr 2

/* fat32 */
#define MAJORw 3
#define MINORw 6

char buf[BUFLEN];
int main() {
	struct test_info info;
	int test, part, fd_random, fd_file1, fd_file2;

	double speeds[PARTS];
	
	int limitsr[] = {    0,    0,       0,     2000,     3000 };
	int limitsw[] = {    0,    1,    2000,     5000,     1000 };
	int sizes[]   = { 1000, 1000, 5000000, 10000000, 10000000 };
	int counts[]  = {    1,    5,      10,     1000,   100000 };
	
	iolimits(0);
	
	if ((fd_random = open(fromfile, O_RDONLY)) < 0 ) {
		fprintf(stderr, "Error in open (%s)!\n", fromfile);
		return 1;
	}
	if ((fd_file1 = open(file1, O_WRONLY | O_CREAT, 0666)) < 0 ) {
		fprintf(stderr, "Error in open (%s)!\n", file1);
		return 2;
	}
	if ((read(fd_random, buf, BUFLEN)) < 0) {
		fprintf(stderr, "Error in read (%s)!\n", fromfile);
		return 3;
	}
	if ((write(fd_file1, buf, BUFLEN)) < 0) {
		fprintf(stderr, "Error in read (%s)!\n", file1);
		return 4;
	}
	
	close(fd_random);
	close(fd_file1);
	
	iolimits(1);
	
	for (test = 0; test < TESTS; ++test) {
		int r, w, t;
		
		limit_read(MAJORr,MINORr,limitsr[test]);
		limit_write(MAJORw,MINORw,limitsw[test]);
	
		for (part = 0; part < PARTS; ++part) {
			int pos;
			t = 0; w = 0; r = 0;
			
			if ((fd_file1 = open(file1, O_CREAT | O_RDONLY, 0666)) < 0) {
				fprintf(stderr, "Error in open (%s)!\n", file1);
				break;
			}	
			if ((fd_file2 = open(file2, O_CREAT | O_WRONLY, 0666)) < 0) {
				fprintf(stderr, "Error in open (%s)!\n", file2);
				break;
			}	
			
			for (pos = 0; pos < sizes[test]; pos += counts[test]) {
				tic();
				if ((r += read(fd_file1, buf, (counts[test] < sizes[test] - pos ? counts[test] : sizes[test] - pos))) < 0) {
					fprintf(stderr, "Error in read (%s)!\n", file1);
					break;
				}
				if ((w += write(fd_file2, buf, (counts[test] < sizes[test] - pos ? counts[test] : sizes[test] - pos))) < 0) {
					fprintf(stderr, "Error in write (%s)!\n", file2);
					break;
				}
				t += toc();
			}
			assert(w == sizes[test]);
			assert(r == sizes[test]);
			
			speeds[part] = (1000.0E0) * ((double) sizes[test]) / ((double) t); /*  (B / usec) = (10000000/1024) * (kB/s) */
			fprintf(stderr, "limit(read): %d, limit(write): %d, count: %dB, part: %d time: %d speed1: %f\n", 
				limitsr[test], limitsw[test], counts[test], part, t,  speeds[part]);
			
			close(fd_file1);
			close(fd_file2);
		}
		
		info = analyse(speeds, PARTS);
		
		fprintf(stdout, "=> TEST %d.1 => limit(read): %dkB, limit(write): %dkB, data: %dB, count: %dB, avg: %fs, dev: %fs, len: %dparts\n", 
			test, limitsr[test], limitsw[test], sizes[test], counts[test], info.avg, info.dev, info.len);
		
		
		fflush(stdout);
	}
		
	iolimits(0);
	
	return 0;
}
