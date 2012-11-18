#include <stdio.h>
#include <assert.h>
#include "tests.h"

#define BUFLEN 2000000
#define PARTS 10
#define TESTS 12
#define file1 "/root/testfile_4_1.test"
#define file2 "/mnt/hda6/testfile_4_2.test"
#define file3 "/mnt/hda6/testfile_4_3.test"
#define fromfile "/dev/urandom"

/* ext3 */
#define MAJORr 3
#define MINORr 2

/* fat32 */
#define MAJORw 3
#define MINORw 6

int main() {
	struct test_info info;
	int test, part, fd_random, fd_file1, fd_file2, fd_file3;
	char buf[BUFLEN];
	double speeds[PARTS];
	/*                     0       1       2       3       4       5        6        7        8        9       10       11 */
	int limitsr[] = {     10,     20,     50,     40,     20,    100,     150,     200,     500,     300,       0,       0 };
	int limitsw[] = {     15,     40,     30,     10,    100,     20,     150,     350,    1000,       0,     300,       0 };
	int sizes[]   = { 200000, 200000, 200000, 200000, 200000, 200000, 2000000, 2000000, 2000000, 2000000, 2000000, 2000000 };
	/* czas = ile_bajtow / limit */
	
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
		return 4;
	}
	if ((write(fd_file1, buf, BUFLEN)) < 0) {
		fprintf(stderr, "Error in read (%s)!\n", file1);
		return 5;
	}
	
	close(fd_random);
	close(fd_file1);
	
	iolimits(1);
	
	for (test = 0; test < TESTS; ++test) {
		int r, w, t, v;
		
		limit_read(MAJORr,MINORr,limitsr[test]);
		limit_write(MAJORw,MINORw,limitsw[test]);
	
		for (part = 0; part < PARTS; ++part) {
			int pos;
			t = 0; w = 0; r = 0; v = 0;
			
			if ((fd_file1 = open(file1, O_CREAT | O_RDONLY, 0666)) < 0) {
				fprintf(stderr, "Error in open (%s)!\n", file1);
				break;
			}	
			if ((fd_file2 = open(file2, O_CREAT | O_WRONLY, 0666)) < 0) {
				fprintf(stderr, "Error in open (%s)!\n", file2);
				break;
			}	
			if ((fd_file3 = open(file3, O_CREAT | O_WRONLY, 0666)) < 0) {
				fprintf(stderr, "Error in open (%s)!\n", file3);
				break;
			}	
			
			for (pos = 0; pos < sizes[test]; pos += 4096) {
				tic();
				if ((r += read(fd_file1, buf, (4096 < sizes[test] - pos ? 4096 : sizes[test] - pos))) < 0) {
					fprintf(stderr, "Error in read (%s)!\n", file1);
					break;
				}
				if ((w += write(fd_file2, buf, (4096 < sizes[test] - pos ? 4096 : sizes[test] - pos))) < 0) {
					fprintf(stderr, "Error in write (%s)!\n", file2);
					break;
				}
				if ((v += write(fd_file3, buf, (4096 < sizes[test] - pos ? 4096 : sizes[test] - pos))) < 0) {
					fprintf(stderr, "Error in write (%s)!\n", file3);
					break;
				}
				t += toc();
			}
			assert(w == sizes[test]);
			assert(v == sizes[test]);
			assert(r == sizes[test]);
			
			speeds[part] = (2000.0E0) * ((double) sizes[test]) / ((double) t); /*  (B / usec) = (10000000/1024) * (kB/s) */
			fprintf(stderr, "limit(read): %d, limit(write): %d, part: %d time: %d speed: %f\n", limitsr[test], limitsw[test], part, t, speeds[part]);
			
			close(fd_file1);
			close(fd_file2);
			close(fd_file3);
		}
		
		info = analyse(speeds, PARTS);
		
		fprintf(stdout, "=> TEST %d => limit(read): %dkB, limit(write): %dkB, data: %dB, avg: %fkB/s, dev: %fkB/s, opt: %dkB/s, len: %dparts\n", 
			test, limitsr[test], limitsw[test], sizes[test], info.avg, info.dev, 
			(int) ( limitsr[test] == 0 && limitsw[test] == 0 ? 0.0E0 :
				2.0E0/(
					( limitsr[test] ? 1.0E0/((double)limitsr[test]) : 0.0E0 )
					+
					( limitsw[test] ? 2.0E0/((double)limitsw[test]) : 0.0E0 ) 
				)
			), 
			info.len);
		fflush(stdout);
	}
		
	iolimits(0);
	
	return 0;
}
