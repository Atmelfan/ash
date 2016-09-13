#include "walkpatterns.h"

int8_t walk_forw[] = {
//	 H, A, H, A
/*
	o--/|4^5|/---
		|  	|
	---\|2 3|\--o
    	|	|
	o--/|0_1|/---
*/
	-1,-1,1,1,//Höj ben 1,2,5 framåt, ben 0,3,4 sänkt bakåt
/*
	o--/|4^5|/--o
		|  	|
	o--\|2 3|\--o
    	|	|
	o--/|0_1|/--o
*/
	-1,-1,-1,1,//sänk ben 1,2,5 framåt
/*
	----|4^5|---o
		|  	|
	o---|2 3|----
    	|	|
	----|0_1|---o
*/
	1,0,-1,0,//Ben 1,2,5 neutral ned
/*
	o---|4^5|---o
		|  	|
	o---|2 3|---o
    	|	|
	o---|0_1|---o
*/
	-1,0,-1,0,//Ben 1,2,5 framåt höjd
};

int8_t walk_forw2[] = {
//	 H, A, H, A
	1,1,-1,-1,//Höj ben 1,2,5 framåt, ben 0,3,4 sänkt bakåt

	-1,1,-1,-1,//sänk ben 1,2,5 framåt

	-1,0,1,0,//Ben 1,2,5 neutral ned

	-1,0,-1,0,//Ben 1,2,5 framåt höjd
};

int8_t walk_revs[] = {
//	 H, A, H, A
/*
	o--/|4^5|/---
		|  	|
	---\|2 3|\--o
    	|	|
	o--/|0_1|/---
*/
	-1,1,1,-1,//Höj ben 1,2,5 framåt, ben 0,3,4 sänkt bakåt
/*
	o--/|4^5|/--o
		|  	|
	o--\|2 3|\--o
    	|	|
	o--/|0_1|/--o
*/
	-1,1,-1,-1,//sänk ben 1,2,5 framåt
/*
	----|4^5|---o
		|  	|
	o---|2 3|----
    	|	|
	----|0_1|---o
*/
	1,0,-1,0,//Ben 1,2,5 neutral ned
/*
	o---|4^5|---o
		|  	|
	o---|2 3|---o
    	|	|
	o---|0_1|---o
*/
	-1,0,-1,0//Ben 1,2,5 framåt höjd
};

int8_t walk_revs2[] = {
//	 H, A, H, A
	1,-1,-1,1,//Höj ben 1,2,5 framåt, ben 0,3,4 sänkt bakåt

	-1,-1,-1,1,//sänk ben 1,2,5 framåt

	-1,0,1,0,//Ben 1,2,5 neutral ned

	-1,0,-1,0//Ben 1,2,5 framåt höjd
};

int8_t walk_ccw[] = {
//	 H, A, H, A, H, A
	1, 1, -1, -1, 1, 1,

	0, 1, 0, -1, 0, 1,

	-1, 0, 1, 0, -1, 0,

	0, 0, 0, 0, 0, 0
};

int8_t walk_cw[] = {
//	 H, A, H, A, H, A
	1, -1, -1, 1, 1, -1,

	0, -1, 0, 1, 0, -1,

	-1, 0, 1, 0, -1, 0,

	0, 0, 0, 0, 0, 0
};
