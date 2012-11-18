#include <stdio.h>
#include <assert.h>
#include "tests.h"

#define BUFLEN 6000000
#define PARTS 10
#define TESTS 12
#define testfile "test_1.txt"
#define fromfile "/dev/urandom"

#define MAJOR 3
#define MINOR 2

int main() {
	struct test_info info;
	int test, part, fd_random, fd, segs;
	char buf[BUFLEN];
	double speeds[PARTS], speeds2[PARTS];
	/*                    0       1       2       3       4       5       6       7        8        9       10       11 */
	int limits[] = {     10,     20,     30,     40,     50,    100,    150,    200,     500,    1000,    2000,       0 };
	int sizes[] =  { 200000, 200000, 200000, 200000, 200000, 500000, 500000, 500000, 2000000, 2000000, 3000000, 6000000 };
	int parts[] =  {    100,    500,    500,    200,    200,    200,    200,    100,     200,     200,     500,     500 };
	/*   sleep(0.02sek); pokazdym write() daje maksymalna mozliwa przepustowosc: ( je¶liby write() = 0 )                */
	/* w kB/s :~        100,     20,     20,     50,     50,    125,    125,    250,     500,     500,     300,     600 */
	/* Jesliby kopiowaæ bez przerw to zajeloby to(+sleep):                                                              */
	/* w sek          20(2), 10(10),  7(10),   5(4),   4(4),   5(4),   3(4),   2(2),    4(4),    2(4),   2(10),   ?(10) */
	/* czas = ile_bajtow / limit */
	
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
		
		for (part = 0; part < PARTS; ++part) {
			
			t = 0; w = 0; s = 0;
			for (segs = 0; segs < parts[test]; ++segs) {
				tic();
				if ((w += write(fd, buf, sizes[test]/parts[test])) < 0) {
					fprintf(stderr, "Error in write (%s)!\n", testfile);
					break;
				}
				t += toc();
				usleep(20000);
				s += toc();
			}
			assert(w == sizes[test]);
			
			speeds[part] = (1000.0E0) * ((double) sizes[test]) / ((double) t); /*  (B / usec) = (10000000/1024) * (kB/s) */
			speeds2[part] = (1000.0E0) * ((double) sizes[test]) / ((double) s); /*  (B / usec) = (10000000/1024) * (kB/s) */
			fprintf(stderr, "limit: %d, part: %d time: %d speed: %f speed2: %f count: %d\n", 
				limits[test], part, t, speeds[part], speeds2[part], sizes[test]/parts[test]);
		}
		
		info = analyse(speeds, PARTS);
		
		fprintf(stdout, "=> TEST %d.1 => limit: %dkB/s, data: %dB, avg: %fkB/s, dev: %fkB/s, count: %d len: %dparts\n", 
			test, limits[test], sizes[test], info.avg, info.dev, sizes[test]/parts[test], info.len);
		
		info = analyse(speeds2, PARTS);
		fprintf(stdout, "=> TEST %d.2 => limit: %dkB/s, opt: %fkB/s, data: %dB, avg: %fkB/s, dev: %fkB/s, count: %d len: %dparts\n", 
			test, limits[test], ((double)(sizes[test]/parts[test]))/(2.0E-2), 
			sizes[test], info.avg, info.dev, sizes[test]/parts[test], info.len);

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
