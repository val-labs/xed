#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <curses.h>
#define FIL(s) (((int*)s)[-1])
#define CAP(s) (((int*)s)[-2])
typedef struct Str{int cap, fill; char s[];}Str;
typedef struct Vec{int cap, fill; void*s[];}Vec;
char*mkstr(char*s, int len){
  Str*ss = malloc(sizeof(Str)+len+1);
  ss->fill = len; ss->cap = len+1;
  return strcpy(ss->s, s);}
void**mkvec(int cap){
  Vec*ss = malloc(sizeof(Vec)+cap*sizeof(long));
  ss->fill = 0; ss->cap = cap; return ss->s;}
FILE*_LOG=0;
#define LOG(...) fprintf(_LOG,__VA_ARGS__)
void*goodbye(){endwin();LOG("Goodbye!\n");exit(0);}
int fget(char*buf,int buf_sz, FILE*f){
  long p0 = ftell(f);
  return(!fgets(buf,buf_sz,f))?0:ftell(f)-p0;}
void**Buffer=0;
void push(void*arr,void*value){((void**)arr)[FIL(arr)++] = value;}
void*pop(void*arr){return((void**)arr)[FIL(arr)--];}
typedef void*(*FP)();
FP keymapDef[512];
FP keymapEsc[512];
FP keymapC_x[512];
FP*keymap=keymapDef;
int _inhibit_keymap_reset=0;
long c;
int main(int argc, char*argv[]){
  bzero(keymapDef,sizeof(keymapDef));
  bzero(keymapEsc,sizeof(keymapDef));
  bzero(keymapC_x,sizeof(keymapDef));
  keymapDef[3] = goodbye;
  keymapDef[4] = goodbye;
  ++argv; --argc;
  time_t t = time(0);
  _LOG = fopen("log.log", "a");
  setbuf(_LOG, 0);
  LOG("Startup: %s", ctime(&t));
  printf("v0.0.1\n");
  Buffer=mkvec(1000);
  FILE*f = fopen(argv[0], "r");
  char buffer[1024];
  while(1){
    if(_inhibit_keymap_reset) _inhibit_keymap_reset=0;
    else keymap=keymapDef;
    long sz = fget(buffer,sizeof(buffer),f);
    if(!sz){
      push(Buffer, mkstr("", 0));
      break;
    }else if(buffer[sz-1]=='\n'){
      buffer[--sz] = '<';
      printf("Xn[%s]\n", buffer);
      push(Buffer, mkstr(buffer, sz));
    }else{
      printf("Xy[%s]\n", buffer);
      push(Buffer, mkstr(buffer, sz));
      break;}}
  int n=0; for(;n<FIL(Buffer);n++){
    printf("[%s]\n", Buffer[n]);
  }
  initscr();noecho();nonl();raw();keypad(stdscr,1);
  while((c = getch())){
    printw("c = %d\n", c);
    FP f = keymapDef[c];
    if(f) f();
  }
}
