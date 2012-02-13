#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <process.h>

float pi;
int colors []={3,6,10,13,6,3,10,13,6,3,13,10},
    d []={640,350,1},
    i,
    k,
    x, y, x_min, y_min, x_max, y_max;
unsigned short random;

void main (void)
{
  float a,b,c,l0,l1,l2,ln,ln1,n0,n1,n2,p,q,r, s, t, v[12][3];
  int n;
  int graphdriver = DETECT, graphmode;
  struct palettetype palette;

   r = 128;  /* radius of ball */

/*put ega in hi-res mode & init palette */


   initgraph(&graphdriver, &graphmode, "");
   palette.size = MAXCOLORS;
   palette.colors [0] = 000;
   palette.colors [1] = 010;
   palette.colors [2] = 001;
   palette.colors [3] = 011;
   palette.colors [4] = 020;
   palette.colors [5] = 002;
   palette.colors [6] = 022;
   palette.colors [7] = 077;
   palette.colors [8] = 040;
   palette.colors [9] = 004;
   palette.colors [10] =044;
   palette.colors [11] = 060;
   palette.colors [12] = 006;
   palette.colors [13] = 066;
   palette.colors [14] = 007;
   palette.colors [15] = 077;
   setallpalette (&palette);

   a = 1.3;
   b = .5 * (d[0]-1);
   c = .5 * (d[1]-1);
   l0 = -1/sqrt (3.);
   l1 = l0;
   l2 = -l0;
   pi = 4*atan(1.);
   v[0][0] = 0;
   v[0][1] = 0;
   v[0][2] = 1;
   s = sqrt (5.);
   for (i = 1; i < 11; i++)
   {  p = pi * i/5;
      v[i][0] = 2 *cos(p)/s;
      v[i][1] = 2 *sin(p)/s;
      v[i][2] = (1.-i%2*2)/s;
   }
   v[11][0]= 0;
   v[11][1]= 0;
   v[11][2]= -1;

   y_max = c+r;
   y_min = 2*c-y_max;
   for (y = y_min; y<= y_max; y++)
   {  s = y-c;
      n1 = s/r;
      ln1 = l1*n1;
      s = r*r-s*s;
      x_max = b+a*sqrt(s);
      x_min = 2*b-x_max;
      for (x = x_min; x <= x_max; x++)
      {  t = (x-b)/a;
         n0 = t/r;
         t = sqrt (s-t*t);
         n2 = t/r;
         ln =l0*n0+ln1+l2*n2;
         if (ln <0) ln =0;
         t=ln*n2;
         t+=t-l2;
         t*=t*t;
         t*=t*t;
         t*=t*t;

         for (i=0, p=0; i<11;i++)
           if (p < (q=n0 * v[i][0] + n1 * v[i][1] + n2 * v[i][2]))
           {  p = q;
              k = colors [i];
           }

         i = k - 2.5 + 2.5 *ln + t + (random=37*random+1)/65536.;
         if (i < k-2) i=0;
         else if (i > k) i = 15;
         putpixel (x,y,i);
      }
   }
   (void) getchar ();
   restorecrtmode ();
}

