#ifndef __KERNEL_PT209223_IOLIMIT_TEST_H__
#define __KERNEL_PT209223_IOLIMIT_TEST_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>

int iolimits(int on_off) {
	int fd;
	if ((fd = open("/proc/iolimits", O_WRONLY)) < 0)
		return 1;
	if (write(fd, (on_off ? "1" : "0"), 1) < 0)
		return 2;
	if (close(fd))
		return 3;
	return 0;
}

int limit_pid(const char * type, int pid, int major, int minor, int limit) {
	int fd;
	char buf[256];
	snprintf(buf, 256, "/proc/%d/%slimit", pid, type);
	if ((fd = open(buf, O_WRONLY)) < 0)
		return 1;
	snprintf(buf, 256, "%d %d %d\n", major, minor, limit);		
	if (write(fd, buf, strlen(buf)) < 0)
		return 2;
	if (close(fd))
		return 3;
	return 0;
}

int limit_write(int major, int minor, int limit) {
	return limit_pid("write", getpid(), major, minor, limit);
}

int limit_read(int major, int minor, int limit) {
	return limit_pid("read", getpid(), major, minor, limit);
}

struct timeval tic_stamp;

int tic(void) {
	if (gettimeofday(&tic_stamp,NULL)) return 1;
	return 0;
}

int toc(void) {
	struct timeval t;
	if (gettimeofday(&t,NULL)) return -1;
	return (t.tv_sec - tic_stamp.tv_sec)*1000000 + (t.tv_usec - tic_stamp.tv_usec);
}

#define SQUARE(x) ((x)*(x))

struct test_info {
	double avg, dev;
	int len;
};

struct test_info analyse(double * vt, int n) {
	struct test_info out = { .avg = 0.0E0, .dev = 0.0E0, };
	int i;
	for (i = 0; i < n; ++i) 
		out.avg += vt[i];
	out.avg /= ((double) n);
	for (i = 0; i < n; ++i) 
		out.dev += SQUARE(vt[i] - out.avg);
	out.dev /= ((double) n);
	out.dev = sqrt(out.dev);
	out.len = n;
	return out;
}

#endif 
