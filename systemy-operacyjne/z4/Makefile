all : test_1 test_2 test_3 test_4 testv_1 testv_2 testv_3 testv_4 extreme extremev
ALL = test_1 test_2 test_3 test_4 testv_1 testv_2 testv_3 testv_4 extreme extremev

test_1: test_1.c tests.h
	gcc -lm -o test_1 test_1.c

test_2: test_2.c tests.h
	gcc -lm -o test_2 test_2.c

test_3: test_3.c tests.h
	gcc -lm -o test_3 test_3.c

test_4: test_4.c tests.h
	gcc -lm -o test_4 test_4.c

testv_1: testv_1.c tests.h
	gcc -lm -o testv_1 testv_1.c

testv_2: testv_2.c tests.h
	gcc -lm -o testv_2 testv_2.c

testv_3: testv_3.c tests.h
	gcc -lm -o testv_3 testv_3.c

testv_4: testv_4.c tests.h
	gcc -lm -o testv_4 testv_4.c

extreme: extreme.c tests.h
	gcc -lm -o extreme extreme.c

extremev: extremev.c tests.h
	gcc -lm -o extremev extremev.c

clean:
	rm -f $(ALL)
