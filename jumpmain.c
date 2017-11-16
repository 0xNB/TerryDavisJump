#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

FILE *holytext;
char holy[500];
int sacred_counter = 0;
int holyc = 0;
int max_x = 0, max_y = 0;
char input[100];
long last = 0;
const char nigger[] = "NIGGER";
long current = 0;
long textcurrent = 0;
struct timespec spec;
struct Player{
  int x,y;
  double v;
};

struct Player *p;

struct Platform{
   int x, y, size;
};

struct ListElement{
     struct ListElement *prev;
     struct ListElement *next;
     struct Platform element;
}ListElement;

struct ListElement *platforms = NULL;

struct Map{
  int x,y;
  double a;
  struct Platform platforms[50];
};

struct Map *map;

// Returns the Difference of the last measured time frame and the current time frame
// Returns 1 if the difference was above one second 0 otherwise

int get_diff(){
     clock_gettime(CLOCK_REALTIME, &spec);
     time_t seconds = spec.tv_sec;
     long ms = round(spec.tv_nsec / 1.0e3);
     current = ms;
     if(current - last > 5 || last - current > 5){
         last = current;
	 return 1;
     }
     return 0;
}

void draw_platforms(){
    struct ListElement *i = platforms;
    while(i != NULL){
       int realx = i->element.x - map->x;
       if(p->x >= realx - 2 && p->x < realx + i->element.size
		       && p->y > i->element.y - 2 && p->y < i->element.y && p->v >= 0){
         p->v = 0;
       }
       for(int j = 0; j < i->element.size; j++){
	  char word[2];
	  word[0] = nigger[j%6];
	  word[1] = '\0';
          mvprintw(i->element.y, i->element.x + j - map->x, word);
       }
       i = i->next;
    }
}

void add_platform(int y, int x, int size){
     struct ListElement *j = platforms;
     while(j->next != NULL){
       j = j->next;
     }
     struct ListElement *insert = malloc(sizeof(ListElement));
     insert->prev = j;
     insert->next = NULL;
     insert->element.x = x;
     insert->element.y = y;
     insert->element.size = size;
     j->next = insert;
}

void generate_platforms(int count){
     for(int i = 0; i < count; i++){
         add_platform(rand() % max_y, rand() % 300, rand() % 9 + 3);
     }
}

void free_platforms(){
     struct ListElement *last = platforms;
     struct ListElement *next = platforms->next;
     while(next != NULL){
       free(last);
       last = next;
       if(next->next == NULL)
	       return;
       next = next->next;
     }
}

void displayholytext(){
         for(int i = 0; i < max_x; i++){
	       char w[2];
	       w[0] = (char)holy[(i+sacred_counter)%500];
	       w[1] = '\0';
               mvprintw(0,i,w);
	 }
	 if(current - textcurrent > 100000 || textcurrent - current > 100000){
	       textcurrent = current;
               sacred_counter++;
	 }
}

int main(int argc, char *argv[]){
	
   holytext = fopen("oursaviour", "r");
   fgets(holy,500,holytext);
   initscr();
   noecho();
   curs_set(FALSE);
   cbreak();
   int o = 1;
   getmaxyx(stdscr, max_y, max_x);
   nodelay(stdscr, o);
   srand(time(NULL));
   
   p = malloc(sizeof(struct Player));
   map = malloc(sizeof(struct Map));
   platforms = malloc(sizeof(struct ListElement));
   platforms->prev = NULL;
   platforms->next = NULL;
   platforms->element.x = 10;
   platforms->element.y = 10;
   platforms->element.size = 5;
   generate_platforms(50);
   p->x = 0;
   p->y = max_y - 10;
   p->v = 0;
   map->x = 0;
   map->y = 0;
   map->a = 0.09;

   char curinput = 'l';

   while(curinput != 'q'){
       curinput = getch();	   
       getmaxyx(stdscr, max_y, max_x);
       clear(); 
       if(curinput == 'a'){
	       p->x -= 2;
       }
       else if(curinput == 'd'){
               p->x += 2;
       }
       if(p->x < 0){
	    map->x--;
            p->x = 0;
       }
       else if(p->x > max_x){
	    map->x+=p->x - max_x;
            p->x = max_x;
       }
       if(curinput == ' '){
          p->v = -1;
	  p->y += p->v;
       }

       p->y += p->v;

       draw_platforms();
       displayholytext();

       mvprintw(max_y-1,0,"CURRENT: %lld LAST: %lld", current, last);

       if(get_diff()){
          p->v += map->a;
       }

       if(p->y > max_y){
          p->v = 0;
	  p->y = max_y - 1;
       }

       //mvprintw(0,0,"CHAR: %c",curinput);
       mvprintw(p->y, p->x, ":^)");
       refresh();
       usleep(20000);
   }

   clear();
   mvprintw((int)(max_y/2),(int)(max_x/2) - 5, "EXITING");
   refresh();
   sleep(1);

   free(p);
   free_platforms();
   free(map);
   fclose(holytext);

   endwin();
}
