#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <fcntl.h>
#include "tree.h"
#include "task31.h"
#include "exec.h"

extern char *environ;

void printlist(char **s)
{
	int i=0;
	if (s!=NULL)
		while (s[i]!=NULL)
		{
			printf("%s ", s[i]);
			++i;
		}
}

int main()
{
	char **s;
	int kwords;
	tree t=NULL;
	s=NULL;
	s=MakeListWithWords(s);
	//printf("%d", kwords);
	//if (s==NULL) printf("Ok");
	//print(s, kwords);
	t=build_tree(s);
	printtree(t);
	printf("\n");
	analystree(t);
	cleartree(t);
}
