#include <stdio.h>
#include <string.h>

int main()
{
//	FILE* fp = popen("ls -l","r");
	FILE* fp = popen("ftp localhost","w");
	char buffer[100];
//	fprintf(fp,"\r");
//	fprintf(fp,"Uttara1986\r");
	fprintf(fp,"cd logs\n");
	strcpy(buffer,"put popen_test.cc");
	fprintf(fp,"%s",buffer);
//	fprintf(fp,"bye");
//	while(fgets(buffer, 100, fp)  != NULL) puts(buffer);
	pclose(fp);
	return 0;
}
