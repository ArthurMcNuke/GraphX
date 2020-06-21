#ifndef GRAPHLIB_H
#define GRAPHLIB_H

#include "graphxwin.h"

#include <sys/wait.h>
#include <stdlib.h>

#define MAIN_FIFO "/tmp/graphxfifo"
#define KEY_FIFO "/tmp/keyfifo"

#define FIFO_SEM "/fifosem"
#define KEY_SEM "/keysem"

sem_t *fifo_access;
sem_t *key_access;

graphxWindow *w;

int MAIN_FD = 0;
int KEY_FD = 0;

unsigned long RGB (unsigned short int R, unsigned short int G, unsigned short int B);
int ConnectGraphX(int width, int height);
void CloseGraphX();
void Pixel(int x, int y, long int color = -1);
void Line(int x1, int y1, int x2, int y2, long int color = -1);
void Polyline(XPoint *points, int n, long int color = -1);
void Rectangle(int x, int y, int width, int height, long int color = -1);
void Ellipse(int x, int y, int width, int height, long int color = -1);
void Arc(int x, int y, int width, int height, int angle1, int angle2, long int color = -1);
void Text(int x, int y, char *string, int length, long int color = -1);
void SetForeground(long int color);
void SetBackground(long int color);
void FillArc(int x, int y, int width, int height, int angle1, int angle2, long int color);
void FillEllipse(int x, int y, int width, int height, long int color);
void FillRectangle(int x, int y, int width, int height, long int color);
void FillPolygon(XPoint *points, int n, long int color);
void Clear();
int InputChar();

#endif

