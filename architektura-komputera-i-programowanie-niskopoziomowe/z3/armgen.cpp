#include "armcpu.h"
#include "todo.h"

using namespace std;

int main(void) {

	TODO arm_code;
	arm_code.load_from_stdin();
	arm_code.do_todo();
	
	return 0;
}
