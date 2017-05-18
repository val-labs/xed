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
#define ERR(...) LOG(__VA_ARGS__),exit(1)
void*goodbye(){endwin();LOG("Goodbye!\n");exit(0);}
int fget(char*buf,int buf_sz, FILE*f){
  long p0 = ftell(f); return(!fgets(buf,buf_sz,f))?0:ftell(f)-p0;}
void**Buffer=0;
void push(void*arr,void*value){((void**)arr)[FIL(arr)++] = value;}
void*pop(void*arr){return((void**)arr)[FIL(arr)--];}
typedef void*(*FP)();
FP keymapDef[512];
FP keymapEsc[512];
FP keymapC_x[512];
FP keymapM_x[512];
FP*keymap=keymapDef;
int _inhibit_keymap_reset=0;
long c,minx=0,miny=0,maxy=0,maxx=0,row=0,col=0;
int usage(){ERR(">> Need filename.\n");}
void*up()   {if(row)--row;}
void*down() {if(row+1<FIL(Buffer))++row;}
void*left() {if(col)--col;}
void*right(){if(col+1<=FIL(Buffer[row]))++col;}
void*useKeymapM_x(){keymap=keymapM_x;_inhibit_keymap_reset=1;}
void*useKeymapC_x(){keymap=keymapC_x;_inhibit_keymap_reset=1;}
void*useKeymapEsc(){keymap=keymapEsc;_inhibit_keymap_reset=1;}
void*saveBuffer(){
  move(30,30); printw("SAVE ME");
  int n=0; FILE*f=fopen("OUT","w+");
  for(;n<FIL(Buffer);n++) fprintf(f, "%s", Buffer[n]);
  fclose(f);}
void*loadBuffer(){move(30,30); printw("LOAD ME");}
void*statusUpdate(){
  move(maxy-1, maxx/2);
  printw("| c = %3d  (%2d,%2d) ", c, col+1, row+1);
  printw("[%2d,%2d]", FIL(Buffer[row]), FIL(Buffer));
  move(row, col);}
void*refreshBuffer(){
  int n=0; clear();
  for(;n<FIL(Buffer);n++) addstr(Buffer[n]);
  statusUpdate();}
void step(){
  FP f = keymap[c=getch()];
  if(f) f();
  if(_inhibit_keymap_reset) _inhibit_keymap_reset=0;
  else keymap=keymapDef;
  statusUpdate();}
int main(int argc, char*argv[]){
  bzero(keymapDef,sizeof(keymapDef));
  bzero(keymapEsc,sizeof(keymapDef));
  bzero(keymapC_x,sizeof(keymapDef));
  bzero(keymapM_x,sizeof(keymapDef));
  keymapDef[24] = useKeymapC_x;
  keymapDef[27] = useKeymapEsc;
  keymapEsc['x']= useKeymapM_x;
  keymapEsc['X']= useKeymapM_x;
  keymapC_x['C'-'A'+1] = goodbye;
  keymapC_x['F'-'A'+1] = loadBuffer;
  keymapC_x['S'-'A'+1] = saveBuffer;
  keymapDef['L'-'A'+1] = refreshBuffer;
  keymapDef[KEY_UP   ] = up;
  keymapDef[KEY_DOWN ] = down;
  keymapDef[KEY_LEFT ] = left;
  keymapDef[KEY_RIGHT] = right;
  ++argv; --argc;
  time_t t = time(0);
  _LOG = fopen("log.log", "a");
  setbuf(_LOG, 0);
  LOG("Startup: %s", ctime(&t));
  printf("v0.0.1\n");
  Buffer=mkvec(1000);
  if(!argv[0]) usage();
  FILE*f = fopen(argv[0], "r");
  char buffer[1024];
  while(1){
    long sz = fget(buffer,sizeof(buffer),f);
    if(sz && buffer[sz-1]=='\n'){
      push(Buffer, mkstr(buffer, --sz));
    }else{
      push(Buffer, sz?mkstr(buffer, sz):mkstr("", 0));
      break;}}
  initscr();noecho();nonl();raw();keypad(stdscr,1);
  getmaxyx(stdscr, maxy, maxx);
  refreshBuffer();
  while(1)step();}
