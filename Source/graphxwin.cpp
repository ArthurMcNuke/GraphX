#include "graphxwin.h"

extern sem_t *key_access;

graphxWindow::graphxWindow(int w, int h, int &err){
    if ((display = XOpenDisplay(NULL)) == NULL)
        err = -1;
    else {
        ScreenNumber = DefaultScreen (display);
        width = w;
        height = h;

        window = XCreateSimpleWindow (display,
        RootWindow(display, ScreenNumber),
        0, 0, width, height, 10,
        WhitePixel(display, ScreenNumber),
        BlackPixel(display, ScreenNumber));

        XSelectInput (display, window, ExposureMask | KeyPressMask | StructureNotifyMask);

        XMapWindow (display, window);

        gc = XCreateGC (display, window, 0, NULL);

        XSetForeground(display, gc, WhitePixel(display, ScreenNumber));

        while(1){
	        XEvent e;
	        XNextEvent(display, &e);
	        if (e.type == MapNotify) break;
        }

        RGBcmap = DefaultColormap(display, DefaultScreen(display));

        err = 0;
    }
}

graphxWindow::~graphxWindow(){
    XFlush(display);
    XFreeGC(display,gc);
    XFreeColormap(display, RGBcmap);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void graphxWindow::Pixel(int x, int y, long int color){
    if (color >= 0) 
        AllocColor(color);

    XDrawPoint(display, window, gc, x, y);
}

void graphxWindow::Line(int x1, int y1, int x2, int y2, long int color){
    if (color >= 0) 
        AllocColor(color);

    XDrawLine(display, window, gc, x1, y1, x2, y2);
}

void graphxWindow::Polyline(XPoint *points, int n, long int color){
    if (color >= 0)
        AllocColor(color);

    XDrawLines(display, window, gc, points, n, CoordModeOrigin);
}

void graphxWindow::Rectangle(int x, int y, int width, int height, long int color){
    if (color >= 0)
        AllocColor(color);

    XDrawRectangle(display, window, gc, x, y, width, height);
}

void graphxWindow::Ellipse(int x, int y, int height, int width, long int color){
    if (color >= 0)
        AllocColor(color);

    int angle1 = 0;
    int angle2 = 360 * 64;
    XDrawArc(display, window, gc, x, y, width, height, angle1, angle2);
}

void graphxWindow::Arc(int x, int y, int height, int width, int angle1, int angle2, long int color){
    if (color >= 0)
        AllocColor(color);

    angle1 *= 64;
    angle2 *= 64;
    XDrawArc(display, window, gc, x, y, width, height, angle1, angle2);
}

void graphxWindow::Text(int x, int y, char *string, int length, long int color){
    XDrawString(display, window, gc, x, y, string, length);
}

void graphxWindow::SetForeground(long int color){
    AllocColor(color);
}

void graphxWindow::SetBackground(long int color){
    XColor exact_color;

    exact_color.red = (color & 0xff0000) >> 8;
    exact_color.green = color & 0xff00;
    exact_color.blue = (color & 0xff) << 8;

    XAllocColor(display, RGBcmap, &exact_color);
    XSetWindowBackground(display, window, exact_color.pixel);
    XClearArea(display, window, 0, 0, width, height, false);

    XFreeColormap(display, RGBcmap);
}

void graphxWindow::FillEllipse(int x, int y, int width, int height, long int color){
    AllocColor(color);

    int angle1 = 0;
    int angle2 = 360 * 64;
    XFillArc(display, window, gc, x, y, width, height, angle1, angle2);
}

void graphxWindow::FillArc(int x, int y, int width, int height, int angle1, int angle2, long int color){
    AllocColor(color);

    angle1 *= 64;
    angle2 *= 64;
    XFillArc(display, window, gc, x, y, width, height, angle1, angle2);
}

void graphxWindow::FillRectangle(int x, int y, int width, int height, long int color){
    AllocColor(color);

    XFillRectangle(display, window, gc, x, y, width, height);
}

void graphxWindow::FillPolygon(XPoint *points, int n, long int color){
    AllocColor(color);

    XFillPolygon(display, window, gc, points, n, Complex, CoordModeOrigin);
}

void graphxWindow::Clear(){
    XClearWindow(display, window);
}

int graphxWindow::InputChar(){
    XEvent e;
    int key = 0;
    
    while (1){
        XNextEvent(display, &e);

        switch (e.type){
            case KeyPress :
                key = e.xkey.keycode;
                return key;
            default: break;
        }
    }

    return key;
}

void graphxWindow::AllocColor(long int color){
    XColor exact_color;

    exact_color.red = (color & 0xff0000) >> 8;
    exact_color.green = color & 0xff00;
    exact_color.blue = (color & 0xff) << 8;

    XAllocColor(display, RGBcmap, &exact_color);
    XSetForeground(display, gc, exact_color.pixel);

    XFreeColormap(display, RGBcmap);
}

void graphxWindow::Recieve(int fd, int fd2){
    int cond = 1;
    msg recvm;

    short int c = 0;
    int key = 0;

    XPoint *points;

    while(cond){
        XFlush(display);
        read(fd, (char *)&recvm, sizeof(recvm));
        switch(recvm.type){
            case pixel:
                Pixel(recvm.x, recvm.y, recvm.color);
                break;

            case line:
                Line(recvm.x, recvm.y, recvm.width, recvm.height, recvm.color);
                break;

            case polyline:
                points = new XPoint[recvm.npoints];

                for (int i = 0; i < recvm.npoints; i++){
                    read(fd, (char *) &c, sizeof(short int));
                    points[i].x = c;
                    read(fd, (char *) &c, sizeof(short int));
                    points[i].y = c;
                }

                Polyline(points, recvm.npoints, recvm.color);
                delete []points;
                break;

            case rectangle:
                Rectangle(recvm.x, recvm.y, recvm.width, recvm.height, recvm.color);
                break;

            case ellipse:
                Ellipse(recvm.x, recvm.y, recvm.width, recvm.height, recvm.color);
                break;

            case arc:
                Arc(recvm.x, recvm.y, recvm.width, recvm.height, recvm.angle1, recvm.angle2, recvm.color);
                break;

            case text:
                Text(recvm.x, recvm.y, recvm.text, recvm.length, recvm.color);
                break;

            case setforeground:
                SetForeground(recvm.color);
                break;

            case setbackground:
                SetBackground(recvm.color);
                break;

            case fillellipse:
                FillEllipse(recvm.x, recvm.y, recvm.width, recvm.height, recvm.color);
                break;

            case fillarc:
                FillArc(recvm.x, recvm.y, recvm.width, recvm.height, recvm.angle1, recvm.angle2, recvm.color);
                break;

            case fillrectangle:
                FillRectangle(recvm.x, recvm.y, recvm.width, recvm.height, recvm.color);
                break;

            case fillpolygon:
                points = new XPoint[recvm.npoints];

                for (int i = 0; i < recvm.npoints; i++){
                    read(fd, (char *) &c, sizeof(short int));
                    points[i].x = c;
                    read(fd, (char *) &c, sizeof(short int));
                    points[i].y = c;
                }

                FillPolygon(points, recvm.npoints, recvm.color);
                delete []points;
                break;

            case clear:
                Clear();
                break;

            case inputchar:
                key = InputChar();
                write(fd2, (char*) &key, sizeof(int));
                sem_post(key_access);
                break;

            case closegraphx:
                cond = 0;
                break;
            default:
                break;
        }
    }

    close(fd);
    close(fd2);
}
