#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
#define FIL(s) (((int*)s)[-1])
#define CAP(s) (((int*)s)[-2])
typedef struct Str{int cap, fill; char s[];}Str;
typedef struct Vec{int cap, fill; long s[];}Vec;
char*mkstr(char*s){
  int len = strlen(s);
  Str*ss = malloc(sizeof(Str)+len+1);
  ss->fill = len; ss->cap = len+1;
  return strcpy(ss->s, s);}
char*mkvec(int cap){
  Vec*ss = malloc(sizeof(Vec)+cap*sizeof(long));
  ss->fill = 0; ss->cap = cap; return ss;}
FILE*_LOG=0;
#define LOG(...) fprintf(_LOG,__VA_ARGS__)
void*goodbye(){LOG("Goodbye!\n");exit(0);}
int fget(char*buf,int buf_sz, FILE*f){
  long p0 = ftell(f);
  return(!fgets(buf,buf_sz,f))?0:ftell(f)-p0;}
int main(int argc, char*argv[]){
  ++argv; --argc;
  time_t t = time(0);
  _LOG = fopen("log.log", "a");
  setbuf(_LOG, 0);
  LOG("Startup: %s", ctime(&t));
  printf("v0.0.1\n");
  FILE*f = fopen(argv[0], "r");
  char buffer[1024];
  while(1){
    long sz = fget(buffer,sizeof(buffer),f);
    if(!sz) break;
    if(buffer[sz-1]=='\n'){
      buffer[--sz] = '<';
      printf("Xn[%s]\n", buffer);
    }else{
      printf("Xy[%s]\n", buffer);
      break;}}
  char*s=mkstr("qwert");
  printf("[[%s]]\n", s);
  printf("[[%d]]\n", FIL(s));
  printf("[[%d]]\n", CAP(s));
  goodbye();
}
