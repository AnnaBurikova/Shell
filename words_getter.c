#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include "task31.h"

 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

 void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* записываем знак */
         n = -n;          /* делаем n положительным числом */
     i = 0;
     do {       /* генерируем цифры в обратном порядке */
         s[i++] = n % 10 + '0';   /* берем следующую цифру */
     } while ((n /= 10) > 0);     /* удаляем */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

char getsym(char *str, int i) /* возвращает каждый прочитанный символ */
{
   if (i==8) /* если str исчерпался, считываем следующие 8 символов */
   {
   	if (fscanf(stdin, "%8[' '!-~'\n''\t']", str)!=EOF)
   	{
   		i=0;
   		return getsym(str, i);
   	}
   	else return '\0';
   }
   else return str[i];
}

char **newword(char **str, char *buf, int *kwords) // записывает очередное слово из buf в список слов
{
    if ((*kwords)==0)
    {
        str=(char **)(malloc(sizeof(char *)));
        str[*kwords]=buf;
        (*kwords)=1;
    }
    else
    {
        str=(char **)(realloc(str, sizeof(char *)*(*kwords+1)));
        str[*kwords]=buf;
        (*kwords)+=1;
    }
    return str;
}

void print(char **s, int kwords)
{
    for (int i=0; i<kwords; ++i)
   	printf("%s ", s[i]);
}

char **newwordspecial(char **s, int *kwords, char cprev) // создаёт слово из одного специального символа и добавляет его в список слов
{
    char *buf1;
    buf1=(char *)(malloc(sizeof(char)*2));
    buf1[0]=cprev;
    buf1[1]='\0';
    if ((*kwords)==0)
    {
        s=(char **)(malloc(sizeof(char *)));
        s[*kwords]=buf1;
        (*kwords)=1;
    }
    else
    {
        s=(char **)(realloc(s, sizeof(char *)*(*kwords+1)));
        s[*kwords]=buf1;
        (*kwords)+=1;
    }
    return s;
}

char **sorts(char **s, int *kwords)
{
    /*char *tmp;
    for (int i=0; i<(*kwords-1); ++i)
        {
            for (int j=0; j<(*kwords-i-1); ++j)
            {
                if (strcmp(s[j],s[j+1])>0)
                {
                    tmp=s[j];
                    s[j]=s[j+1];
                    s[j+1]=tmp;
                }
            }
        } */
    return s;
}

char *newsymbol(char *buf, char c, int k) // запись в buf очередного символа слова
{
    buf=(char *)(realloc(buf, sizeof(char)*(k+1)));
    buf[k]=c;
    return buf;
}

char **clears(char **s, int *kwords)
{
    int i=0;
    while (s[i]!=NULL)
    {
        free(s[i]);
        ++i;
    }
    free(s);
    *kwords=0;
    return NULL;
}

void change(char **s)
{
	int i=0;
	while (s[i])
	{
		if (!strcmp(s[i], "$HOME"))
		{
			char *home;
			home=getenv("HOME");
			char *buf;
			buf=(char *)(malloc(strlen(home)*sizeof(char)));
			int j=0;
			while(home[j])
			{
				buf[j]=home[j];
				++j;
			}
			buf[j]='\0';
			free(s[i]);
			s[i]=buf;
		}
		if (!strcmp(s[i], "$SHELL"))
		{
			char *shell=NULL/*="/home/anna/Prac/task5\0"*/;
			size_t size=PATH_MAX;
			shell=getcwd(shell, size);
			char *buf;
			buf=(char *)(malloc(strlen(shell)*sizeof(char)));
			int j=0;
			while(shell[j])
			{
				buf[j]=shell[j];
				++j;
			}
			buf[j]='\0';
			free(s[i]);
			s[i]=buf;
		}
		if (!strcmp(s[i], "$EUID"))
		{
			int euid;
			struct stat s1;
			int stats=stat("main.c", &s1);
			euid=s1.st_uid;
			char euid1s[10000];
			itoa(euid, euid1s);
			char *buf;
			buf=(char *)(malloc(strlen(euid1s)*sizeof(char)));
			int j=0;
			while(euid1s[j])
			{
				buf[j]=euid1s[j];
				++j;
			}
			buf[j]='\0';
			free(s[i]);
			s[i]=buf;
		}
		if (!strcmp(s[i], "$USER"))
		{
			struct stat st;
			int stats=stat("main.c", &st);
			struct passwd *pws;
			pws = getpwuid(st.st_gid);
			char *user=NULL;
			user=pws->pw_name;
			char *buf;
			buf=(char *)(malloc(strlen(user)*sizeof(char)));
			int j=0;
			while(user[j])
			{
				buf[j]=user[j];
				++j;
			}
			buf[j]='\0';
			free(s[i]);
			s[i]=buf;
		}
		++i;
	}
}

char **MakeListWithWords(char **s)
{
   char c;
   char str[8];
   char *buf=NULL;
   //char **s=NULL;
   int i, k, kwords, f1=0, f2=0, f3=0; // f1, f2, f3 флаги того, что выделено слово из ||, && и >>
   char cprev;
   i=0;
   k=0;
   kwords=0;
   cprev='\0';
   if (fscanf(stdin, "%8[' '!-~'\n''\t']", str)==EOF) return NULL;
   while ((c=getsym(str, i))!='\0')
   {
        switch (c)
        {
            case ' ': case '\n': case '\t':
            // проверяем, был ли незаписанный одиночный специальный символ, и записываем слово, наколенное до него в buf, и специальный символ в список
                if (((cprev=='|')&&(f1==0))||((cprev=='&')&&(f2==0))||((cprev=='>')&&(f3==0)))
                {
                    if (buf!=NULL)
                    {
                        buf=newsymbol(buf, '\0', k);
                        k=0;
                        s=newword(s, buf, &kwords);
                    }
                    buf=NULL;
                    s=newwordspecial(s, &kwords, cprev);
                }
                // если пробел, перевод строки или табуляция, зписывем слово из buf в список
                if (buf!=NULL)
                {
                    buf=newsymbol(buf, '\0', k);
                    k=0;
                    s=newword(s, buf, &kwords);
                    buf=NULL;
                }
                break;
            case ';': case '<': case '(': case ')': case '#': case '\\': case '"':
            // проверяем, был ли незаписанный одиночный специальный символ, и записываем слово, наколенное до него в buf, и специальный символ в список
                if (((cprev=='|')&&(f1==0))||((cprev=='&')&&(f2==0))||((cprev=='>')&&(f3==0)))
                {
                    if (buf!=NULL)
                    {
                        buf=newsymbol(buf, '\0', k);
                        k=0;
                        s=newword(s, buf, &kwords);
                    }
                    buf=NULL;
                    s=newwordspecial(s, &kwords, cprev);
                }
                // специальные символы, которые всегда одиночные, записываем в спиок, записав до этого слово, накопившееся в buf
                if (buf!=NULL)
                {
                    buf=newsymbol(buf, '\0', k);
                    k=0;
                    s=newword(s, buf, &kwords);
                    buf=NULL;
                }
                buf=(char *)(malloc(sizeof(char)*2));
                buf[0]=c;
                buf[1]='\0';
                s=newword(s, buf, &kwords);
                buf=NULL;
                break;
            case '|':
            // проверяем, был ли незаписанный одиночный специальный символ, и записываем слово, наколенное до него в buf, и специальный символ в список
                if (((cprev=='&')&&(f2==0))||((cprev=='>')&&(f3==0)))
                {
                    if (buf!=NULL)
                    {
                        buf=newsymbol(buf, '\0', k);
                        k=0;
                        s=newword(s, buf, &kwords);
                    }
                    buf=NULL;
                    s=newwordspecial(s, &kwords, cprev);
                }
                // если предыдущий не  |, дописать в список накопившееся слово и перейти на следующую итерацию символа в поиске ||
                if (cprev!='|')
                {
                    if (buf!=NULL) /* save previous word */
                    {
                        buf=newsymbol(buf, '\0', k);
                        k=0;
                        s=newword(s, buf, &kwords);
                        buf=NULL;
                    }
                    cprev=c;
                    if (i==8) i=1;
                    else ++i;
                    continue;
                }
                // если предыдущий тот же сециальный символ, то смотри на флаг двойного специального символа и при возможности выдляем его
                else
                {
                   if (f1==0)
                   {
                        buf=(char *)(malloc(sizeof(char)*3));
                        buf[0]=c;
                        buf[1]=c;
                        buf[2]='\0';
                        s=newword(s, buf, &kwords);
                        buf=NULL;
                        f1=1;
                   }
                   // если двойной специальный символ был выделен до этого, то меняем флаг и переходим н следующую итерацию символа в поиске ||
                   else
                   {
                        f1=0;
                        cprev=c;
                        if (i==8) i=1;
                        else ++i;
                        continue;
                   }
                }
                break;
            case '&':
            // то же самое с другими специальными символами
                if (((cprev=='|')&&(f1==0))||((cprev=='>')&&(f3==0)))
                {
                    if (buf!=NULL)
                    {
                        buf=newsymbol(buf, '\0', k);
                        k=0;
                        s=newword(s, buf, &kwords);
                    }
                    buf=NULL;
                    s=newwordspecial(s, &kwords, cprev);
                }
                if (cprev!='&')
                {
                    if (buf!=NULL) /* save previous word */
                    {
                        buf=newsymbol(buf, '\0', k);
                        k=0;
                        s=newword(s, buf, &kwords);
                        buf=NULL;
                    }
                    cprev=c;
                    if (i==8) i=1;
                    else ++i;
                    continue;
                }
                else
                {
                   if (f2==0)
                   {
                        buf=(char *)(malloc(sizeof(char)*3));
                        buf[0]=c;
                        buf[1]=c;
                        buf[2]='\0';
                        s=newword(s, buf, &kwords);
                        buf=NULL;
                        f2=1;
                   }
                   else
                   {
                        f2=0;
                        cprev=c;
                        if (i==8) i=1;
                        else ++i;
                        continue;
                   }
                }
                break;
            case '>':
                if (((cprev=='|')&&(f1==0))||((cprev=='&')&&(f2==0)))
                {
                    if (buf!=NULL)
                    {
                        buf=newsymbol(buf, '\0', k);
                        k=0;
                        s=newword(s, buf, &kwords);
                    }
                    buf=NULL;
                    s=newwordspecial(s, &kwords, cprev);
                }
                if (cprev!='>')
                {
                    if (buf!=NULL) /* save previous word */
                    {
                        buf=newsymbol(buf, '\0', k);
                        k=0;
                        s=newword(s, buf, &kwords);
                        buf=NULL;
                    }
                    cprev=c;
                    if (i==8) i=1;
                    else ++i;
                    continue;
                }
                else
                {
                   if (f3==0)
                   {
                        buf=(char *)(malloc(sizeof(char)*3));
                        buf[0]=c;
                        buf[1]=c;
                        buf[2]='\0';
                        s=newword(s, buf, &kwords);
                        buf=NULL;
                        f3=1;
                   }
                   else
                   {
                        f3=0;
                        cprev=c;
                        if (i==8) i=1;
                        else ++i;
                        continue;
                   }
                }
                break;
            default:
            // проверяем, был ли незаписанный одиночный специальный символ, и записываем слово, наколенное до него в buf, и специальный символ в список
                if (((cprev=='|')&&(f1==0))||((cprev=='&')&&(f2==0))||((cprev=='>')&&(f3==0)))
                {
                    if (buf!=NULL)
                    {
                        buf=newsymbol(buf, '\0', k);
                        k=0;
                        s=newword(s, buf, &kwords);
                    }
                    buf=NULL;
                    s=newwordspecial(s, &kwords, cprev);
                }
                // если это буква, цифра или другой разрешённый символ, то записываем его в buf, если нет, не добавляем в слово неразрешённый символ и выводим ошибку
                /*if ((c=='$')||(c=='_')||(c=='/')||(c=='.')||((c>='a')&&(c<='z'))||((c>='A')&&(c<='Z'))||((c>='0')&&(c<='9'))) */
                    if (k==0)
                    {
                        buf=(char *)(malloc(sizeof(char)));
                        buf[k]=c;
                        k=1;
                    }
                    else
                    {
                        buf=newsymbol(buf, c, k);
                        ++k;
                    }
                /*else
                {
                    printf("Not correct input, not allowed symbols was skipped.\n");
                    break;
                } */
        }
        cprev=c;
        if (i==8) i=1;
        else ++i;
   }
   // проверяем, был ли незаписанный одиночный специальный символ, и записываем слово, наколенное до него в buf, и специальный символ в список
   if (((cprev=='|')&&(f1==0))||((cprev=='&')&&(f2==0))||((cprev=='>')&&(f3==0)))
    {
        if (buf!=NULL)
        {
            buf=newsymbol(buf, '\0', k);
            k=0;
            s=newword(s, buf, &kwords);
        }
        buf=NULL;
        s=newwordspecial(s, &kwords, cprev);
    }
    // дозаписываем накопленное слово в список
    if (buf!=NULL)
   {
       buf=newsymbol(buf, '\0', k);
       s=newword(s, buf, &kwords);
   }
   //print(s, kwords);
   //printf("\n");
   // сортируем список
   //s=sorts(s, &kwords);
   //print(s, kwords);
   buf=NULL;
   s=newword(s, buf, &kwords);
   // освобождаем память buf и списка
   //free(buf);
   change(s);
   return s;
   //s=clears(s, &kwords);
   //print(s, kwords);
}

