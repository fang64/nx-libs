/* $Xorg: SendEvent.c,v 1.4 2001/02/09 02:03:36 xorgcvs Exp $ */
/*

Copyright 1986, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/
/* $XFree86: xc/lib/X11/SendEvent.c,v 1.4 2001/12/14 19:54:05 dawes Exp $ */

#define NEED_EVENTS
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "Xlibint.h"

/*
 * In order to avoid all images requiring _XEventToWire, we install the
 * event converter here if it has never been installed.
 */
Status
XSendEvent(dpy, w, propagate, event_mask, event)
    register Display *dpy;
    Window w;
    Bool propagate;
    long event_mask;
    XEvent *event;
{
    register xSendEventReq *req;
    xEvent ev;
    register Status (**fp)(
                Display *       /* dpy */,
                XEvent *        /* re */,
                xEvent *        /* event */);
    Status status;

    LockDisplay (dpy);

    /* call through display to find proper conversion routine */

    fp = &dpy->wire_vec[event->type & 0177];
    if (*fp == NULL) *fp = _XEventToWire;
    status = (**fp)(dpy, event, &ev);

    if (status) {
	GetReq(SendEvent, req);
	req->destination = w;
	req->propagate = propagate;
	req->eventMask = event_mask;
#ifdef WORD64
	/* avoid quad-alignment problems */
	memcpy ((char *) req->eventdata, (char *) &ev, SIZEOF(xEvent));
#else
	req->event = ev;
#endif /* WORD64 */
    }

    UnlockDisplay(dpy);
    SyncHandle();
    return(status);
}
