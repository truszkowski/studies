#include <stdio.h>
#include <assert.h>
#include <sys/uio.h>
#include <errno.h>
#include "tests.h"

#define VECLEN 50
#define BUFLEN 100000
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

char buf[BUFLEN*VECLEN];
struct iovec v[VECLEN];
int main() {
	struct test_info info;
	int test, part, fd_random, fd_file1, fd_file2;

	double speeds[PARTS];
	
	int limitsr[] = {   0,   0,    0, 2000, 3000 };
	int limitsw[] = {   0,  80, 2000, 5000, 1000 };
	int sizes[]   = { 100, 100,  700,  500,  200 };
	int counts[]  = {  10,   5,  100,  200,  500 };
	
	iolimits(0);
	
	if ((fd_random = open(fromfile, O_RDONLY)) < 0 ) {
		fprintf(stderr, "Error in open (%s)!\n", fromfile);
		return 1;
	}
	if ((fd_file1 = open(file1, O_WRONLY | O_CREAT, 0666)) < 0 ) {
		fprintf(stderr, "Error in open (%s)!\n", file1);
		return 2;
	}
	if ((read(fd_random, buf, BUFLEN*VECLEN)) < 0) {
		fprintf(stderr, "Error in read (%s)!\n", fromfile);
		return 3;
	}
	if ((write(fd_file1, buf, BUFLEN*VECLEN)) < 0) {
		fprintf(stderr, "Error in read (%s)!\n", file1);
		return 4;
	}
	
	close(fd_random);
	close(fd_file1);
	
	iolimits(1);
	
	for (test = 0; test < TESTS; ++test) {
		int r, w, t, i;
		
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
			
			for (i = 0; i < VECLEN; ++i) {
				v[i].iov_base = buf + i*counts[test];
				v[i].iov_len = counts[test];
			}

			tic();			
			for (i = 0; i < sizes[test]; ++i) {
				if ((r += readv(fd_file1, v, VECLEN)) < 0) {
					fprintf(stderr, "Error in read (%s,%d,%d,%s)!\n", file1, i, errno, strerror(errno));
					break;
				}
				if ((w += writev(fd_file2, v, VECLEN)) < 0) {
					fprintf(stderr, "Error in write (%s,%d,%d,%s)!\n", file2,i, errno, strerror(errno));
					break;
				}
			}
			t = toc();
			if (r != w)
				fprintf(stderr, "w= %d r= %d ok= %d\n", w, r, sizes[test]*counts[test]*VECLEN); 
			assert(r == w);
			if (r != sizes[test]*counts[test]*VECLEN)
				fprintf(stderr, "r= %d ok= %d\n", r, sizes[test]*counts[test]*VECLEN); 
			
			speeds[part] = (1000.0E0) * ((double) w) / ((double) t); /*  (B / usec) = (10000000/1024) * (kB/s) */
			fprintf(stderr, "limit(read): %d, limit(write): %d, count: %dB, part: %d time: %d speed1: %f\n", 
				limitsr[test], limitsw[test], counts[test], part, t, speeds[part]);
			
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
