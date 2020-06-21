#ifndef GRAPHXWIN_H
#define GRAPHXWIN_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>

enum msgtype {pixel, line, polyline, rectangle, ellipse, arc, text, setforeground, setbackground, fillellipse, fillarc, fillrectangle, fillpolygon, clear, inputchar, closegraphx, key};

struct msg {
    msgtype type;
    int x;
    int y;

    long int color;

    int height;
    int width;

    int angle1;
    int angle2;

    int npoints;

    int key;
    
    int length;
    char *text;
};

class graphxWindow {
    private:
        Display * display;
        int ScreenNumber;
        GC gc;
        Window window;
        int width;
        int height;
        Colormap RGBcmap;

    public:
        graphxWindow(int w, int h, int &err);
        ~graphxWindow();
        void Recieve(int fd, int fd2);

    public:
        void Pixel (int x, int y, long int color);
        void Line (int x1, int y1, int x2, int y2, long int color);
        void Polyline(XPoint *points, int n, long int color);
        void Rectangle(int x, int y, int width, int height, long int color);
        void Ellipse(int x, int y, int width, int height, long int color);
        void Arc(int x, int y, int width, int height, int angle1, int angle2, long int color);
        void Text(int x, int y, char *string, int length, long int color);

        void SetForeground(long int color);
        void SetBackground(long int color);

        void FillEllipse(int x, int y, int width, int height, long int color);
        void FillArc(int x, int y, int width, int height, int angle1, int angle2, long int color);
        void FillRectangle(int x, int y, int width, int height, long int color);
        void FillPolygon(XPoint *points, int n, long int color);

        void Clear();

        int InputChar();

    private:
    void AllocColor(long int color);       
};

#endif

