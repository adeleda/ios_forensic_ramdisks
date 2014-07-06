#include <stdio.h>

main() {

	FILE *fp=NULL;
	int c=0;
	fp=fopen("/etc/passwd","r");
	c=fgetc(fp);
	while(c != EOF) {
		printf("%c",c);
		c=fgetc(fp);
	}
	fclose(fp);
}
