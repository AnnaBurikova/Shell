#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <fcntl.h>
#include "tree.h"
#include "exec.h"


extern char *environ;

// поля next pipe psubcmd взаимоисключающие, поэтому если одна не NULL, то остальные NULL

static int k=1;

void analystree(tree t)
{
	if (t==NULL) return;
	while ((t->next!=NULL)||(t->pipe!=NULL))
	{
		if (t->next!=NULL)
		{
			if (fork()==0) { execvp(t->argv[0], &(t->argv[0])); error('e'); return; }
			wait(NULL);
			t=t->next;
			continue;
		}
		else if (t->pipe!=NULL)
		{
			int fd[2], in, out, next_in, i;
			pipe(fd); out=fd[1]; next_in=fd[0];
			if (fork()==0)
			{
				int f1;
				if (t->infile!=NULL)
				{
					if ((f1=open(t->infile+1, O_RDWR))==-1) error('f');
					dup2(f1, 0);
					close(f1);
				}
				close(next_in);
				dup2(out,1);
				close(out);
				execvp(t->argv[0], &(t->argv[0])); error('e'); return;
			}
			in=next_in;
			t=t->pipe;
			while (t->pipe!=NULL)
			{
				close(out);
				pipe(fd);
				out=fd[1];
				next_in=fd[0];
				if(fork()==0)
				{
					dup2(in,0);
					close(in);
					dup2(out,1);
					close(out);
					execvp(t->argv[0], &(t->argv[0])); error('e'); return;
				}
				close(in);
				in=next_in;
				t=t->pipe;
				++k;
			}
			close(out);
			while(wait(NULL)>0);
			++k;
			if (fork()==0)
			{
				if (t->backgrnd==1)
				{
					printf("pid %d\n", getpid());
					fflush(stdout);
				}
				int f;
				if (t->outfile!=NULL)
				{
					if (t->outfile[0]=='0')
					{
						if ((f=open(t->outfile+1, O_WRONLY|O_APPEND))==-1) error('f');
						dup2(f, 1);
						close(f);
					}
					else
					{
						if ((f=open(t->outfile+1, O_RDWR|O_TRUNC|O_CREAT))==-1) error('f');
						dup2(f, 1);
						close(f);
					}
					
				}
				dup2(in,0);
				close(in);
				execvp(t->argv[0], &(t->argv[0])); error('e'); return;
			}
			close(in);
			close(next_in);
			if (t->backgrnd==0) wait(NULL);
			//for( int i=0; i<k+2; ++i) wait(NULL);
			//while(wait(NULL)>0);
			if ((t->next==NULL)&&(t->pipe==NULL)) return;
			t=t->next;
			continue;
		}
		return;
	}
	int f, f1;
	if (fork()==0)
	{
		if (t->backgrnd==1)
		{
			printf("pid %d\n", getpid());
			fflush(stdout);
			sleep(0.05);
		}
		if (t->outfile!=NULL)
		{
			if (t->outfile[0]=='0')
			{
				if ((f=open(t->outfile+1, O_WRONLY|O_APPEND))==-1) error('f');
				dup2(f, 1);
				close(f);
			}
			else
			{
				if ((f=open(t->outfile+1, O_RDWR|O_TRUNC|O_CREAT))==-1) error('f');
				dup2(f, 1);
				close(f);
			}
			
		}
		if (t->infile!=NULL)
		{
			if ((f1=open(t->infile+1, O_RDWR))==-1) error('f');
			dup2(f1, 0);
			close(f1);
		}
		execvp(t->argv[0], &(t->argv[0]));
		error('e');
		return;
	}
	if (t->backgrnd==0) wait(NULL);
}
