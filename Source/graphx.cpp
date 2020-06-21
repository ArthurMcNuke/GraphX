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

unsigned long RGB (unsigned short int R, unsigned short int G, unsigned short int B){
    unsigned long color = 0;
    color += R << 16;
    color += G << 8;
    color += B;
    return color;
}

int ConnectGraphX(int width, int height){
    int err = 0;
    int fd;

    unlink(MAIN_FIFO);
    unlink(KEY_FIFO);
    unlink(KEY_SEM);
    unlink(FIFO_SEM);

    key_access = sem_open(KEY_SEM, O_CREAT, 0666, 0);
    if (key_access == SEM_FAILED){
        return 5;
    }

    fifo_access = sem_open(FIFO_SEM, O_CREAT, 0666, 1);
    if (fifo_access == SEM_FAILED){
        return 5;
    }

    if((mkfifo(MAIN_FIFO, 0666)) == -1){
        return 1;
    }

    if((MAIN_FD = open(MAIN_FIFO, O_RDWR)) == - 1){
        return 2;
    }

    if((mkfifo(KEY_FIFO, 0666)) == -1){
        return 1;
    }

    if((KEY_FD = open(KEY_FIFO, O_RDWR)) == - 1){
        return 2;
    }

    w = new graphxWindow(width, height, err);
    if (err == 1){
        delete w;
        return 3;
    }

    pid_t pid = fork();

    if (pid == -1){
        return 4;
    }

    if (pid == 0){
        w->Recieve(MAIN_FD, KEY_FD);
        delete w;
        exit(0);
    }

    return 0;
}

void CloseGraphX(){
    msg m;
    m.type = closegraphx;

    write(MAIN_FD,(char*) &m, sizeof(m)); 
    wait(0);

    sem_close(key_access);
    unlink(KEY_SEM);
    sem_close(fifo_access);
    unlink(FIFO_SEM);

    close(MAIN_FD);
    unlink(MAIN_FIFO);

    close(KEY_FD);
    unlink(KEY_FIFO);
}

void Pixel(int x, int y, long int color = -1){
    msg m;
    m.type = pixel;
    m.x = x;
    m.y = y;
    m.color = color;

    write(MAIN_FD,(char*) &m, sizeof(m));
}

void Line(int x1, int y1, int x2, int y2, long int color = -1){
    msg m;
    m.type = line;
    m.x = x1;
    m.y = y1;
    m.width = x2;
    m.height = y2;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void Polyline(XPoint *points, int n, long int color = -1){
    msg m;
    m.type = polyline;
    m.npoints = n;
    m.color = color;
    
    short int coord = 0;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    for (int i = 0; i < n; i++){
        coord = points[i].x;
        write(MAIN_FD, (char*) &coord, sizeof(short int));
        coord = points[i].y;
        write(MAIN_FD, (char*) &coord, sizeof(short int));
    }
    sem_post(fifo_access);
}

void Rectangle(int x, int y, int width, int height, long int color = -1){
    msg m;
    m.type = rectangle;
    m.x = x;
    m.y = y;
    m.width = width;
    m.height = height;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void Ellipse(int x, int y, int width, int height, long int color = -1){
    msg m;
    m.type = ellipse;
    m.x = x;
    m.y = y;
    m.width = width;
    m.height = height;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void Arc(int x, int y, int width, int height, int angle1, int angle2, long int color = -1){
    msg m;
    m.type = arc;
    m.x = x;
    m.y = y;
    m.width = width;
    m.height = height;
    m.angle1 = angle1;
    m.angle2 = angle2;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void Text(int x, int y, char *string, int length, long int color = -1){
    msg m;
    m.type = text;
    m.x = x;
    m.y = y;
    m.text = string;
    m.length = length;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void SetForeground(long int color){
    msg m;
    m.type = setforeground;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void SetBackground(long int color){
    msg m;
    m.type = setbackground;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void FillArc(int x, int y, int width, int height, int angle1, int angle2, long int color){
    msg m;
    m.type = fillarc;
    m.x = x;
    m.y = y;
    m.width = width;
    m.height = height;
    m.angle1 = angle1;
    m.angle2 = angle2;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void FillEllipse(int x, int y, int width, int height, long int color){
    msg m;
    m.type = fillellipse;
    m.x = x;
    m.y = y;
    m.width = width;
    m.height = height;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void FillRectangle(int x, int y, int width, int height, long int color){
    msg m;
    m.type = fillrectangle;
    m.x = x;
    m.y = y;
    m.width = width;
    m.height = height;
    m.color = color;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

void FillPolygon(XPoint *points, int n, long int color){
    msg m;
    m.type = fillpolygon;
    m.npoints = n;
    m.color = color;
    
    short int coord = 0;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    for (int i = 0; i < n; i++){
        coord = points[i].x;
        write(MAIN_FD, (char*) &coord, sizeof(short int));
        coord = points[i].y;
        write(MAIN_FD, (char*) &coord, sizeof(short int));
    }
    sem_post(fifo_access);
}

void Clear(){
    msg m;
    m.type = clear;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);
}

int InputChar(){
    msg m;
    m.type = inputchar;
    int key = 0;

    sem_wait(fifo_access);
    write(MAIN_FD,(char*) &m, sizeof(m));
    sem_post(fifo_access);

    sem_wait(key_access);
    read(KEY_FD, (char*) &key, sizeof(int));

    return key;
}
