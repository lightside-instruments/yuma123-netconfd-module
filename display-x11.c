#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/time.h>

Window create_window(Display * display, int width, int height, int x, int y)
{
    int screen_num = DefaultScreen(display);
    int win_border_width = 2;
    Window win;

    win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
			      x, y, width, height, win_border_width, BlackPixel(display, screen_num), WhitePixel(display, screen_num));

    XMapWindow(display, win);

    XFlush(display);

    return win;
}

#define NEXTEVENT_X11 0
#define NEXTEVENT_STDIN 1
#define NEXTEVENT_TIMEOUT 2

int nextevent(Display * dsp, Window win, int timeout, XEvent * evt)
{
    fd_set fds;
    int max, ret;
    struct timeval tv;
    int end, current;

    /* ending time */
    gettimeofday(&tv, NULL);
    end = tv.tv_sec * 1000 + tv.tv_usec / 1000 + timeout;

    while (!XCheckWindowEvent(dsp, win, KeyPressMask, evt)) {

	FD_ZERO(&fds);

	/* wait for the x11 socket */
	FD_SET(ConnectionNumber(dsp), &fds);
	max = ConnectionNumber(dsp);

	/* wait for standard input */
	FD_SET(STDIN_FILENO, &fds);
	max = max < STDIN_FILENO ? STDIN_FILENO : max;

	/* how long to wait */
	gettimeofday(&tv, NULL);
	current = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	if (current >= end)
	    current = end;
	tv.tv_sec = (end - current) / 1000;
	tv.tv_usec = ((end - current) % 1000) * 1000;

	/* select */
	ret = select(max + 1, &fds, NULL, NULL, &tv);
	if (ret == -1)
	    continue;		/* error, maybe a signal */

	if (FD_ISSET(ConnectionNumber(dsp), &fds))
	    return NEXTEVENT_X11;	/* X11 activity, maybe an event */

	if (FD_ISSET(STDIN_FILENO, &fds))
	    return NEXTEVENT_STDIN;	/* data from stdin */

	return NEXTEVENT_TIMEOUT;	/* timeout */
    }
    return NEXTEVENT_X11;	/* X11 event */
}

int main(int argc, char** argv)
{
    Display* display = XOpenDisplay(NULL);
    int screen_num = DefaultScreen(display);
    Window root = RootWindow(display, screen_num);
    Visual* visual = DefaultVisual(display, screen_num);

    int display_width = DisplayWidth(display, screen_num);
    int display_height = DisplayHeight(display, screen_num);
    unsigned char* data = (char *) malloc(display_width * display_height * 4);

    Window win = create_window(display, display_width, display_height, 0, 0);

    XSelectInput(display, win, ExposureMask);
    XMapWindow(display, win);

    XEvent event;
    int type;
    unsigned int offset = 0;
    size_t res;
    int k = 0;

    for (int i = 0; i < display_height; i++) {
	for (int j = 0; j < display_width; j++) {
	    data[(i * display_width + j) * 4 + 0] = 0;
	    data[(i * display_width + j) * 4 + 1] = 0;
	    data[(i * display_width + j) * 4 + 2] = 0;
	    data[(i * display_width + j) * 4 + 3] = 0;
	}
    }

    XImage* img = XCreateImage(display, visual, DefaultDepth(display, screen_num),
			       ZPixmap,
			       0, data, display_width, display_height, 32, 0);
    while (1) {
	type = nextevent(display, win, 1000, &event);
	if ((type == NEXTEVENT_X11) && (event.type == Expose)) {
	    fprintf(stderr, "X11 Expose event.\n");
	    XPutImage(display, win, DefaultGC(display, screen_num), img, 0, 0, 0, 0, display_width, display_height);

	} else if (type == NEXTEVENT_STDIN) {
	    res = read(STDIN_FILENO, (void *) (data + offset), display_height * display_width * 4 - offset);
	    if (res < 0) {
		perror("read failed.\n");
		return -1;
	    }
	    if (res == 0) {
		fprintf(stderr, "stdin closed\n");
		break;
	    }
	    fprintf(stderr, "read %u total %u from %u\n", res, res + offset, display_height * display_width * 4);
	    if ((res + offset) == (display_height * display_width * 4)) {
		offset = 0;
		fprintf(stderr, "X11 image update.\n");
		XPutImage(display, win, DefaultGC(display, screen_num), img, 0, 0, 0, 0, display_width, display_height);
	    } else {
		offset += res;
	    }
	} else {
	    fprintf(stderr, "other event\n");
	    XPutImage(display, win, DefaultGC(display, screen_num), img, 0, 0, 0, 0, display_width, display_height);
	}

    }

    XCloseDisplay(display);
    return 0;
}
