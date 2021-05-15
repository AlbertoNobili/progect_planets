//-----------------------------------------------------
// GRAPHICAL CONSTANTS
//-----------------------------------------------------
#define XWIN	640		// windows x resolution
#define YWIN	480		// window y resolution
#define	FLEV	5		// spacing between boxes
#define BKG		0		// background color
#define MCOL	14		// menu color
#define NCOL	7		// numbers color
#define TCOL	6		// trail color
#define	CSUN	7		// sun color
//-----------------------------------------------------
#define LBOX	489 	// X length of the planet box
#define HBOX	399		// Y heightof the planet box
#define XBOX	5		// left box X coordinate
#define YBOX	75		// upper box Y coordinate
#define RBOX	495		// right box X coordinate
#define BBOX	475 	// bottom box Y coordinate
//-----------------------------------------------------
#define LMENU	489 	// X length of the planet box
#define HMENU	69		// Y heightof the planet box
#define XMENU	5		// left box X coordinate
#define YMENU	5		// upper box Y coordinate
#define RMENU	495		// right box X coordinate
#define BMENU	70 		// bottom box Y coordinate
//-----------------------------------------------------
#define LSTATUS		134 	// X length of the planet box
#define HSTATUS		469		// Y heightof the planet box
#define XSTATUS		500		// left box X coordinate
#define YSTATUS		5		// upper box Y coordinate
#define RSTATUS		635		// right box X coordinate
#define BSTATUS		475		// bottom box Y coordinate
//-----------------------------------------------------
// TASK CONSTANTS
//----------------------------------------------------
#define PER		20		// task period in ms 
#define DL		20		// relative deadline in ms
#define PRI		50		// task priority
#define PRI1	40		// display and interp priority
//---------------------------------------------------
// PLANET CONSTANTS
//----------------------------------------------------
#define MAX_PLANETS	20				// max number of planets
#define	G0			6.674e-11		// gravitational constant [m^3*kg^-1*s^-2]
#define	WLEN		30				// trail length
#define	MMAX		10e8			// max mass [kg]
#define MMIN		1e8				// min mass [kg]
#define DMIN		50				// min initial distance [m]
#define DMAX		200				// max initial distance [m]
#define VMIN		3				// min initial speed [m/s]
#define VMAX		5				// max initial speed [m/s]
#define TSCALE		10				// time scale factor (DT = TSCALE ms)
									// 1 secondo realtà = DT/PER secondi simulati
#define SSCALE		1				// space scale factor
#define RMAX		15				// max planet radius [m]
#define RMIN		10				// min planet radius [m]
#define	RSUN		30				// sun radius [m]
#define	MSUN		1e15			// sun mass [kg]
#define	DIV			1				// fattore di divisione
