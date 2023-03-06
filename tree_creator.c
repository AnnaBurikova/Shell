#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include "tree.h"
#include "task31.h"

extern char *environ;

static int i=0;

void error(char c)
{
	switch (c)
	{
		case 'a': printf("Not enough command\n"); break;
		case 'e': printf("Error in exec\n"); break;
		case 'f': printf("Error in open file\n"); break;
	}
}

char **addarg(char **s, char *buf, int *argc)
{
	if ((*argc)==0)
    {
        s=(char **)(malloc(sizeof(char *)*2));
        s[*argc]=buf;
        s[*argc+1]=NULL;
        (*argc)=1;
    }
    else
    {
        s=(char **)(realloc(s, sizeof(char *)*(*argc+2)));
        s[*argc]=buf;
        s[*argc+1]=NULL;
        (*argc)+=1;
    }
    return s;
}

char *addfile(char *str, char *buf, char f)
{
	// если > то ервый бит 1, если >> то первый бит 0
	int k=0;
	str=(char *)(malloc((strlen(buf)+2)*sizeof(char)));
	str[k]=f;
	++k;
	while (buf[k-1])
	{
		str[k]=buf[k-1];
		++k;
	}
	str[k]='\0';
	return str;
}

tree init(void)
{
	tree t=(tree)(malloc(sizeof(node)));
	t->argv=NULL;
	t->outfile=0;
	t->infile=0;
	t->next=NULL;
	t->pipe=NULL;
	t->backgrnd=0;
	t->psubcmd=NULL;
	return t;
}
	
tree build_tree(char **s)
{
	if (s==NULL) return NULL;
	tree t=init();
	int argc=0;
	while (s[i]!=NULL)
	{
	
	
		if (strcmp(s[i], ">")==0)
		{
			++i;
			int out=0;
			t->outfile=addfile(t->outfile, s[i], '1');
			//++i;
			if (s[i]==NULL) break;
		}
		else if (strcmp(s[i], ">>")==0)
		{
			++i;
			t->outfile=addfile(t->outfile, s[i], '0');
			if (s[i]==NULL) break;
		}
		else if (strcmp(s[i], "<")==0)
		{
			++i;
			int out=0;
			t->infile=addfile(t->infile, s[i], '1');
			if (s[i]==NULL) break;
		}
		else if (strcmp(s[i], ";")==0)
		{
			if (t->argv==NULL) { error('a'); break; }
			++i;
			t->next=build_tree(s);
			if (s[i]==NULL) break;
		}
		else if (strcmp(s[i], "|")==0)
		{
			if (t->argv==NULL) { error('a'); break; }
			++i;
			t->pipe=build_tree(s);
			if (s[i]==NULL) break;
		}
		else if (strcmp(s[i], "&")==0)
		{
			if (t->argv==NULL) { error('a'); break; }
			t->backgrnd=1;
		}
		else if (s[i]!=NULL) t->argv=addarg(t->argv, s[i], &argc);
		else break;
		++i;
	}
	return t;
}

void cleartree(tree t)
{
	int kwords=0;
	if (t==NULL) return;
	if (t->argv!=NULL) clears(t->argv, &kwords);
	if (t->outfile!=NULL) free(t->outfile);
	if (t->infile!=NULL) free(t->infile);
	cleartree(t->next);
	cleartree(t->psubcmd);
	cleartree(t->pipe);
	free(t);
}

void printtree(tree t)
{
	if (t==NULL) return;
	printf("\n");
	int i=0;
	printf("argv ");
	if (t->argv!=NULL)
		while (t->argv[i])
		{
			printf("%s ", t->argv[i]);
			++i;
		}
	else printf("NULL");
	printf("\n");
	if (t->outfile!=NULL) printf("outfile %s\n", t->outfile+1);
	else printf("outfile NULL\n");
	if (t->infile!=NULL) printf("infile %s\n", t->infile+1);
	else printf("infile NULL\n");
	printf("backgrnd %d\n", t->backgrnd);
	if (t->next!=NULL) printf("next %s\n", t->next->argv[0]);
	else printf("backgrnd NULL\n");
	//if (t->psubcmd!=NULL) printtree(t->psubcmd);
	//else printf("NULL\n");
	if (t->pipe!=NULL) printf("pipe %s\n", t->pipe->argv[0]);
	else printf("pipe NULL\n");
	if (t->pipe!=NULL)
	{
		printf("\n");
		printtree(t->pipe);
	}
	if (t->next!=NULL)
	{
		printf("\n");
		printtree(t->next);
	}
}

