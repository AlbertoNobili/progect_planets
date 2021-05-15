#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <allegro.h>
#include <time.h>

#include "ptask.h"
#include "kbfunc.h"
#include "constant.h"


//planet structure
struct status {
	int		c;		// color [1,15]
	float	r;		// radius (m)
	float	m;		// mass (kg)
	float	x;		// x coordinate (m)
	float	y;		// y coordinate (m)
	float	vx;		// horizontal velocity (m/s)
	float	vy;		// vertical velocity (m/s)
};
struct status planet[MAX_PLANETS]; // planet status buffer

//Circular buffer structure
struct cbuf {
	int	top;
	int	x[WLEN];
	int	y[WLEN];
};
struct cbuf wake[MAX_PLANETS]; // trail array

//Other global variables
int		nap = 0;			// number of active planets
int		wflag = 0;			// trail flag
int		wl = 10;			// actual trail length
int		end = 0;			// end flag
float	g = G0;				// gravitational constant
float	sscale = SSCALE;	// space scaling factor
float	tscale = TSCALE;	// time scaling factor
BITMAP	*buffer;			//bitmap for planets

//-----------------------------------------------
//Trail related functions
//-----------------------------------------------

void store_wake(int i) // Insert value of planet i
{
int k;

	if (i>= MAX_PLANETS) return;
	k = wake[i].top;
	k = (k + 1)% WLEN;
	wake[i].x[k] = planet[i].x;
	wake[i].y[k] = planet[i].y;
	wake[i].top = k;
}

void draw_wake(int i, int w) // draw w past values
{
int	j, k;
int	x, y;
	
	for (j=0; j<w; j++){
		k = (wake[i].top + WLEN - j) % WLEN;
		x = XBOX + 1 + wake[i].x[k];
		y = YWIN - wake[i].y[k];
		putpixel(screen, x, y, TCOL);
	}
}

//	FRAND: returns a random float in [xmi, xma)
//---------------------------------------------
float frand (float xmi, float xma)
{
float	r;
	r = rand()/(float)RAND_MAX;	// rand in [0, 1)
	return xmi + (xma - xmi)*r;
}
//--------------------------------------------


void draw_planet(int i)
{
int	x, y;
	
	x = LBOX/2 + sscale*planet[i].x/(DIV);
	y = HBOX/2 + sscale*planet[i].y/(DIV);
	circlefill(buffer, x, y, sscale*planet[i].r/(DIV), planet[i].c);
}

void init_planet(int i)
{
float phase, dist, vel;

	planet[i].c = 2 + i%14;
	planet[i].r = frand(RMIN, RMAX);
	
	phase = frand(0, 6.28);
	dist = frand(DMIN, DMAX);
	printf("la mia dist è %f\n", dist);
	planet[i].x = dist*cos(phase);
	planet[i].y = dist*sin(phase);
	
	phase -= 1.57;
	vel = sqrt(MSUN*g/dist);
	planet[i].vx = vel*cos(phase);
	planet[i].vy = vel*sin(phase);
	
	planet[i].m = frand(MMIN, MMAX);
}

void* planettask(void *arg)
{
int		i, j;
float	x, y, ax, ay, mass, dist, force, fx, fy;
float	dt;

	i = get_task_index(arg);
	set_activation(i);
	//printf("pianeta %d: inizio\n", i);
	
	init_planet(i);
	dt = TSCALE*(float)get_task_period(i)/PER/1000;
	mass = planet[i].m;
	while(!end){
		x = planet[i].x;
		printf("la mia x è %f\n", x);
		y = planet[i].y;
		printf("la mia y è %f\n", y);
		dist = sqrt(pow(x, 2)+pow(y, 2));
		printf("la mia distanza è %f\n", dist);
		force = g*MSUN*mass/pow(dist, 2);
		printf("la mia forza è %f\n", force);
		fx = - force*dist/x;
		fy = - force*dist/y;
		/*for(j=0; j++; j<nap){
			dist = sqrt(pow(x-planet[j].x, 2)+pow(y-planet[j].y, 2));
			force = 
		}	*/
		ax = fx/mass;
		printf("la mia ax è %f\n", ax);
		ay = fy/mass;
		printf("la mia ay è %f\n", ay);
		
		planet[i].x += planet[i].vx*dt;
		planet[i].y += planet[i].vy*dt;
		planet[i].vx +=ax*dt;
		planet[i].vy +=ay*dt;

		
		store_wake(i);
		deadline_miss(i);
		wait_for_period(i);
	}
	
	return NULL;
}

void show_dmiss(int i)
{
int		dmiss;
char	s[20];

	dmiss = get_task_dmiss(i);
	sprintf(s, "%d", dmiss);
	textout_ex(screen, font, s, XSTATUS + 50, 70 + i*10, NCOL, BKG);
}

void aggiorna_status()
{
char	s[20];
int		j;

	sprintf(s, "planets: %d", nap);
	textout_ex(screen, font, s, XSTATUS+10, YSTATUS+10, NCOL, BKG);
	sprintf(s, "wake: %d", wl); 
	textout_ex(screen, font, s, XSTATUS+10, YSTATUS+20, NCOL, BKG);
	sprintf(s, "grav:  %3.1f", g);
	textout_ex(screen, font, s, XSTATUS+10, YSTATUS+30, NCOL, BKG);
	sprintf(s, "tscale: %5.3f", tscale/1e4);
	textout_ex(screen, font, s, XSTATUS+10, YSTATUS+20, NCOL, BKG);
	sprintf(s, "sscale:  %4.2f", sscale);
	textout_ex(screen, font, s, XSTATUS+10, YSTATUS+30, NCOL, BKG);
	
	textout_ex(screen, font, "dm", XSTATUS+45, YSTATUS+50, MCOL, BKG);
	
	for (j=0; j<nap; j++){
		sprintf(s, "T%d:", j+1);
		textout_ex(screen, font, s, XSTATUS + 10, 70 + j*10, MCOL, BKG);
		show_dmiss(j);
	}
}

void* display(void* arg)
{
int i, j;

	i = get_task_index(arg);
	set_activation(MAX_PLANETS);
	//printf("display: inzio (task %d)\n", i);
	
	while (!end){
		clear_bitmap(buffer);
		circlefill(buffer, LBOX/2, HBOX/2, sscale*RSUN/(DIV), CSUN);
		for (j=0; j<nap; j++){
			draw_planet(j);
			if(wflag)
				draw_wake(j, wl);
		}
		blit(buffer, screen, 0, 0, XBOX+1, YBOX+1, buffer->w, buffer->h);
		aggiorna_status();
		deadline_miss(i);	
		wait_for_period(i);
	}
	
	return NULL;
}

void* interpr (void *arg)
{
int i;
char scan;
	
	i = get_task_index(arg);
	set_activation(i);	
	//printf("interprete: inizio (task %d)\n", i);

	do{
		scan = get_scancode_nb();
		switch (scan){
			case KEY_SPACE:
				if (nap < MAX_PLANETS)
					task_create(planettask, nap++, PER, DL, PRI);
				break;
			case KEY_RIGHT:
					if (sscale <= 1.95)
						sscale += 0.05;
				break;
			case KEY_LEFT:
				if (sscale > 0.1)
					sscale -= 0.05;
				break;
			case KEY_UP:
				if (tscale < 1e4)
					tscale += 1e3;
				break;
			case KEY_DOWN:
				if(tscale > -1e4)
					tscale -= 1e3;
				break;
			/*case KEY_RIGHT:
				if (wl < 30)
					wl++;
				break;
			case KEY_LEFT:
				if(wl > 0)
					wl--;
				break;
			case KEY_UP:
				g = g + 1;		// increase gravity
				break;
			case KEY_DOWN:
				if (g > 1)
					g = g - 1;
			case KEY_A:
				for (i=0; i<nap; i++)
					planet[i].vy = planet[i].v0;
				break;*/
			case KEY_T:
				printf("cambio wake\n");
				if (wflag ==0)	wflag = 1;
				else 			wflag = 0;
				break; 
			default: break;
		}
		deadline_miss(i);
		wait_for_period(i);
	}while (scan != KEY_ESC);
	
	end = 1;
	printf("interprete: ho finito\n");
	return NULL;
}

void disegna_menu()
{
char	s[30];

	rect(screen, XMENU, YMENU, RMENU, BMENU, MCOL);
	
	sprintf(s, "SPACE:   create planet");
	textout_ex(screen, font, s, 15, 15, MCOL, BKG);
	//sprintf(s, "A:       jump");
	//textout_ex(screen, font, s, 15, 25, MCOL, BKG);
	sprintf(s, "T:       show wake");
	textout_ex(screen, font, s, 15, 35, MCOL, BKG);
	sprintf(s, "ESC:     exit");
	textout_ex(screen, font, s, 15, 45, MCOL, BKG);
	
	sprintf(s, "RIGHT:   zoom +");
	textout_ex(screen, font, s, 250, 15, MCOL, BKG);
	sprintf(s, "LEFT:    zoom -");
	textout_ex(screen, font, s, 250, 25, MCOL, BKG);
	sprintf(s, "UP:      time +");
	textout_ex(screen, font, s, 250, 35, MCOL, BKG);
	sprintf(s, "DOWN:    time -");
	textout_ex(screen, font, s, 250, 45, MCOL, BKG);
}

void disegna_status()
{
char	s[20];

	rect(screen, XSTATUS, YSTATUS, RSTATUS, BSTATUS, MCOL);
	
	sprintf(s, "planets: 0");
	textout_ex(screen, font, s, RBOX+15, FLEV+10, NCOL, BKG);
	sprintf(s, "wake: 10");
	textout_ex(screen, font, s, RBOX+15, FLEV+20, NCOL, BKG);
	sprintf(s, "grav:  6.674");
	textout_ex(screen, font, s, RBOX+15, FLEV+30, NCOL, BKG);
	sprintf(s, "tscale: 1.000");
	textout_ex(screen, font, s, RBOX+15, FLEV+20, NCOL, BKG);
	sprintf(s, "sscale:  1.00");
	textout_ex(screen, font, s, RBOX+15, FLEV+30, NCOL, BKG);
	
	textout_ex(screen, font, "dm", RBOX+50, FLEV+50, MCOL, BKG);
}

void init()
{
int	ret1, ret2;

	//printf("init: inizio\n");
	
	allegro_init();
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
	clear_to_color(screen, BKG);
	install_keyboard();
	
	srand(time(NULL));
	
	rect(screen, XBOX, YBOX, RBOX, BBOX, MCOL);
	buffer = create_bitmap(LBOX, HBOX);	
	disegna_menu();
	disegna_status();
	
	ptask_init(SCHED_FIFO);
	ret1 = task_create(display, MAX_PLANETS, PER, DL, PRI1);
	ret2 = task_create(interpr, MAX_PLANETS+1, PER, DL, PRI1);
	
	//printf("creazione task display ha ritornato %d\n", ret1);
	//printf("creazione task interprete ha ritornato %d\n", ret2);
	
	return;
}

int main() 
{
int i;

	init();
	for(i=0; i<=MAX_PLANETS; i++){
		wait_for_task_end(i);
		//printf("fine ciclo %d\n", i);
	}
	allegro_exit();
	printf("Main: addio\n");
	return 0;
}

