#include <stdio.h>
#include <unistd.h>

main() {

	execve("/bin/sh", NULL,NULL);

}

