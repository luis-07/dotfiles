/* Indicator properties, you can override these in your config.h if you want. */
#ifndef TAGSINDICATOR
#define TAGSINDICATOR 1 // 0 = off, 1 = on if >1 client/view tag, 2 = always on
#endif
#ifndef TAGSPX
#define TAGSPX 5        // # pixels for tag grid boxes
#endif
#ifndef TAGSROWS
#define TAGSROWS 3      // # rows in tag grid (9 tags, e.g. 3x3)
#endif

void
drawindicator(Monitor *m, Client *c, unsigned int occ, int x, int y, int w, int h, unsigned int tag, int filled, int invert, int type)
{
	int i, boxw, boxs, indn = 0;
	if (!(occ & 1 << tag) || type == INDICATOR_NONE)
		return;

	boxs = drw->fonts->h / 9;
	boxw = drw->fonts->h / 6 + 2;
	if (filled == -1)
		filled = m == selmon && m->sel && m->sel->tags & 1 << tag;

	switch (type) {
	default:
	case INDICATOR_TOP_LEFT_SQUARE:
		drw_rect(drw, x + boxs, y + boxs, boxw, boxw, filled, invert);
		break;
	case INDICATOR_TOP_LEFT_LARGER_SQUARE:
		drw_rect(drw, x + boxs + 2, y + boxs+1, boxw+1, boxw+1, filled, invert);
		break;
	case INDICATOR_TOP_BAR:
		drw_rect(drw, x + boxw, y, w - ( 2 * boxw + 1), boxw/2, filled, invert);
		break;
	case INDICATOR_TOP_BAR_SLIM:
		drw_rect(drw, x + boxw, y, w - ( 2 * boxw + 1), 1, 0, invert);
		break;
	case INDICATOR_BOTTOM_BAR:
		drw_rect(drw, x + boxw, y + h - boxw/2, w - ( 2 * boxw + 1), boxw/2, filled, invert);
		break;
	case INDICATOR_BOTTOM_BAR_SLIM:
		drw_rect(drw, x + boxw, y + h - 1, w - ( 2 * boxw + 1), 1, 0, invert);
		break;
	case INDICATOR_BOX:
		drw_rect(drw, x + boxw, y, w - 2 * boxw, h, 0, invert);
		break;
	case INDICATOR_BOX_WIDER:
		drw_rect(drw, x + boxw/2, y, w - boxw, h, 0, invert);
		break;
	case INDICATOR_BOX_FULL:
		drw_rect(drw, x, y, w - 2, h, 0, invert);
		break;
	case INDICATOR_CLIENT_DOTS:
		for (c = m->clients; c; c = c->next) {
			if (c->tags & (1 << tag)) {
				drw_rect(drw, x, 1 + (indn * 2), m->sel == c ? 6 : 1, 1, 1, invert);
				indn++;
			}
			if (h <= 1 + (indn * 2)) {
				indn = 0;
				x += 2;
			}
		}
		break;
	case INDICATOR_RIGHT_TAGS:
		if (!c)
			break;
		for (i = 0; i < NUMTAGS; i++) {
			drw_rect(drw,
				( x + w - 2 - ((NUMTAGS / TAGSROWS) * TAGSPX)
					- (i % (NUMTAGS/TAGSROWS)) + ((i % (NUMTAGS / TAGSROWS)) * TAGSPX)
				),
				( y + 2 + ((i / (NUMTAGS/TAGSROWS)) * TAGSPX)
					- ((i / (NUMTAGS/TAGSROWS)))
				),
				TAGSPX, TAGSPX, (c->tags >> i) & 1, 0
			);
		}
		break;
	case INDICATOR_PLUS_AND_LARGER_SQUARE:
		boxs += 2;
		boxw += 2;
		/* falls through */
	case INDICATOR_PLUS_AND_SQUARE:
		drw_rect(drw, x + boxs, y + boxs, boxw % 2 ? boxw : boxw + 1, boxw % 2 ? boxw : boxw + 1, filled, invert);
		/* falls through */
	case INDICATOR_PLUS:
		if (!(boxw % 2))
			boxw += 1;
		drw_rect(drw, x + boxs + boxw / 2, y + boxs, 1, boxw, filled, invert); // |
		drw_rect(drw, x + boxs, y + boxs + boxw / 2, boxw + 1, 1, filled, invert); // ‒
		break;
	}
}

void
drawstateindicator(Monitor *m, Client *c, unsigned int occ, int x, int y, int w, int h, unsigned int tag, int filled, int invert)
{
	if (c->isfloating)
		drawindicator(m, c, occ, x, y, w, h, tag, filled, invert, floatindicatortype);
	else
		drawindicator(m, c, occ, x, y, w, h, tag, filled, invert, tiledindicatortype);
}

char *
tagicon(Monitor *m, int tag)
{
	int tagindex = tag + NUMTAGS * m->index;
	if (tagindex >= LENGTH(tagicons[DEFAULT_TAGS]))
		tagindex = tagindex % LENGTH(tagicons[DEFAULT_TAGS]);
	return tagicons[DEFAULT_TAGS][tagindex];
}

static int statussig;
pid_t statuspid = -1;

pid_t
getstatusbarpid()
{
	char buf[32], *str = buf, *c;
	FILE *fp;

	if (statuspid > 0) {
		snprintf(buf, sizeof(buf), "/proc/%u/cmdline", statuspid);
		if ((fp = fopen(buf, "r"))) {
			fgets(buf, sizeof(buf), fp);
			while ((c = strchr(str, '/')))
				str = c + 1;
			fclose(fp);
			if (!strcmp(str, STATUSBAR))
				return statuspid;
		}
	}
	if (!(fp = popen("pidof -s "STATUSBAR, "r")))
		return -1;
	fgets(buf, sizeof(buf), fp);
	pclose(fp);
	return strtol(buf, NULL, 10);
}

void
sigstatusbar(const Arg *arg)
{
	union sigval sv;

	if (!statussig)
		return;
	if ((statuspid = getstatusbarpid()) <= 0)
		return;

	sv.sival_int = arg->i;
	sigqueue(statuspid, SIGRTMIN+statussig, sv);
}

static int combo = 0;

void
keyrelease(XEvent *e)
{
	combo = 0;
}

void
combotag(const Arg *arg)
{
	if (selmon->sel && arg->ui & TAGMASK) {
		if (combo) {
			selmon->sel->tags |= arg->ui & TAGMASK;
		} else {
			combo = 1;
			selmon->sel->tags = arg->ui & TAGMASK;
		}
		focus(NULL);
		arrange(selmon);
	}
}

void
comboview(const Arg *arg)
{
	unsigned newtags = arg->ui & TAGMASK;
	if (combo) {
		selmon->tagset[selmon->seltags] |= newtags;
	} else {
		selmon->seltags ^= 1;	/*toggle tagset*/
		combo = 1;
		if (newtags) {
			pertagview(&((Arg) { .ui = newtags }));
		}
	}
	focus(NULL);
	arrange(selmon);
}

int
width_ltsymbol(Bar *bar, BarArg *a)
{
	return TEXTW(bar->mon->ltsymbol);
}

int
draw_ltsymbol(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, bar->mon->ltsymbol, 0, False);
}

int
click_ltsymbol(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkLtSymbol;
}

int
width_status(Bar *bar, BarArg *a)
{
	return TEXTWM(stext);
}


int
draw_status(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, stext, 0, True);
}


int
click_status(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkStatusText;
}


int
width_status2d(Bar *bar, BarArg *a)
{
	int width;
	width = status2dtextlength(rawstext);
	return width ? width + lrpad : 0;
}


int
draw_status2d(Bar *bar, BarArg *a)
{
	return drawstatusbar(a, rawstext);
}



int
drawstatusbar(BarArg *a, char* stext)
{
	int i, w, len;
	int x = a->x;
	int y = a->y;
	short isCode = 0;
	char *text;
	char *p;
	Clr oldbg, oldfg;
	len = strlen(stext);
	if (!(text = (char*) malloc(sizeof(char)*(len + 1))))
		die("malloc");
	p = text;
	copyvalidchars(text, stext);

	x += lrpad / 2;
	drw_setscheme(drw, scheme[LENGTH(colors)]);
	drw->scheme[ColFg] = scheme[SchemeNorm][ColFg];
	drw->scheme[ColBg] = scheme[SchemeNorm][ColBg];

	/* process status text */
	i = -1;
	while (text[++i]) {
		if (text[i] == '^' && !isCode) {
			isCode = 1;

			text[i] = '\0';
			w = TEXTWM(text) - lrpad;
			drw_text(drw, x, y, w, bh, 0, text, 0, True);

			x += w;

			/* process code */
			while (text[++i] != '^') {
				if (text[i] == 'c') {
					char buf[8];
					if (i + 7 >= len) {
						i += 7;
						len = 0;
						break;
					}
					memcpy(buf, (char*)text+i+1, 7);
					buf[7] = '\0';
					drw_clr_create(drw, &drw->scheme[ColFg], buf);
					i += 7;
				} else if (text[i] == 'b') {
					char buf[8];
					if (i + 7 >= len) {
						i += 7;
						len = 0;
						break;
					}
					memcpy(buf, (char*)text+i+1, 7);
					buf[7] = '\0';
					drw_clr_create(drw, &drw->scheme[ColBg], buf);
					i += 7;
				} else if (text[i] == 'd') {
					drw->scheme[ColFg] = scheme[SchemeNorm][ColFg];
					drw->scheme[ColBg] = scheme[SchemeNorm][ColBg];
				} else if (text[i] == 'w') {
					Clr swp;
					swp = drw->scheme[ColFg];
					drw->scheme[ColFg] = drw->scheme[ColBg];
					drw->scheme[ColBg] = swp;
				} else if (text[i] == 'v') {
					oldfg = drw->scheme[ColFg];
					oldbg = drw->scheme[ColBg];
				} else if (text[i] == 't') {
					drw->scheme[ColFg] = oldfg;
					drw->scheme[ColBg] = oldbg;
				} else if (text[i] == 'r') {
					int rx = atoi(text + ++i);
					while (text[++i] != ',');
					int ry = atoi(text + ++i);
					while (text[++i] != ',');
					int rw = atoi(text + ++i);
					while (text[++i] != ',');
					int rh = atoi(text + ++i);

					if (ry < 0)
						ry = 0;
					if (rx < 0)
						rx = 0;

					drw_rect(drw, rx + x, y + ry, rw, rh, 1, 0);
				} else if (text[i] == 'f') {
					x += atoi(text + ++i);
				}
			}

			text = text + i + 1;
			len -= i + 1;
			i = -1;
			isCode = 0;
			if (len <= 0)
				break;
		}
	}
	if (!isCode && len > 0) {
		w = TEXTWM(text) - lrpad;
		drw_text(drw, x, y, w, bh, 0, text, 0, True);
		x += w;
	}
	free(p);

	drw_setscheme(drw, scheme[SchemeNorm]);
	return 1;
}

int
status2dtextlength(char* stext)
{
	int i, w, len;
	short isCode = 0;
	char *text;
	char *p;

	len = strlen(stext) + 1;
	if (!(text = (char*) malloc(sizeof(char)*len)))
		die("malloc");
	p = text;
	copyvalidchars(text, stext);

	/* compute width of the status text */
	w = 0;
	i = -1;
	while (text[++i]) {
		if (text[i] == '^') {
			if (!isCode) {
				isCode = 1;
				text[i] = '\0';
				w += TEXTWM(text) - lrpad;
				text[i] = '^';
				if (text[++i] == 'f')
					w += atoi(text + ++i);
			} else {
				isCode = 0;
				text = text + i + 1;
				i = -1;
			}
		}
	}
	if (!isCode)
		w += TEXTWM(text) - lrpad;
	free(p);
	return w;
}

int
width_stbutton(Bar *bar, BarArg *a)
{
	return TEXTW(buttonbar);
}

int
draw_stbutton(Bar *bar, BarArg *a)
{
	return drw_text(drw, a->x, a->y, a->w, a->h, lrpad / 2, buttonbar, 0, False);
}

int
click_stbutton(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkButton;
}


int
click_statuscmd(Bar *bar, Arg *arg, BarArg *a)
{
	return click_statuscmd_text(arg, a->x, rawstext);
}


int
click_statuscmd_text(Arg *arg, int rel_x, char *text)
{
	int i = -1;
	int x = 0;
	char ch;
	statussig = -1;
	while (text[++i]) {
		if ((unsigned char)text[i] < ' ') {
			ch = text[i];
			text[i] = '\0';
			x += status2dtextlength(text);
			text[i] = ch;
			text += i+1;
			i = -1;
			if (x >= rel_x && statussig != -1)
				break;
			statussig = ch;
		}
	}
	if (statussig == -1)
		statussig = 0;
	return ClkStatusText;
}

void
copyvalidchars(char *text, char *rawtext)
{
	int i = -1, j = 0;

	while (rawtext[++i]) {
		if ((unsigned char)rawtext[i] >= ' ') {
			text[j++] = rawtext[i];
		}
	}
	text[j] = '\0';
}

int
width_tags(Bar *bar, BarArg *a)
{
	int w, i;

	for (w = 0, i = 0; i < NUMTAGS; i++) {
		w += TEXTW(tagicon(bar->mon, i));
	}
	return w;
}

int
draw_tags(Bar *bar, BarArg *a)
{
	int invert;
	int w, x = a->x;
	unsigned int i, occ = 0, urg = 0;
	char *icon;
	Client *c;
	Monitor *m = bar->mon;

	for (c = m->clients; c; c = c->next) {
		occ |= c->tags;
		if (c->isurgent)
			urg |= c->tags;
	}
	for (i = 0; i < NUMTAGS; i++) {

		icon = tagicon(bar->mon, i);
		invert = 0;
		w = TEXTW(icon);
		drw_setscheme(drw, scheme[
			m->tagset[m->seltags] & 1 << i
			? SchemeTagsSel
			: urg & 1 << i
			? SchemeUrg
			: SchemeTagsNorm
		]);
		drw_text(drw, x, a->y, w, a->h, lrpad / 2, icon, invert, False);
		drawindicator(m, NULL, occ, x, a->y, w, a->h, i, -1, invert, tagindicatortype);
		x += w;
	}

	return 1;
}

int
click_tags(Bar *bar, Arg *arg, BarArg *a)
{
	int i = 0, x = lrpad / 2;

	do {
		x += TEXTW(tagicon(bar->mon, i));
	} while (a->x >= x && ++i < NUMTAGS);
	if (i < NUMTAGS) {
		arg->ui = 1 << i;
	}
	return ClkTagBar;
}

int
width_wintitle(Bar *bar, BarArg *a)
{
	return a->w;
}

int
draw_wintitle(Bar *bar, BarArg *a)
{
	int x = a->x + lrpad / 2, w = a->w - lrpad / 2;
	Monitor *m = bar->mon;
	int pad = lrpad / 2;

	if (!m->sel) {
		drw_setscheme(drw, scheme[SchemeTitleNorm]);
		drw_rect(drw, x, a->y, w, a->h, 1, 1);
		return 0;
	}

	drw_setscheme(drw, scheme[m == selmon ? SchemeTitleSel : SchemeTitleNorm]);
	drw_text(drw, x, a->y, w, a->h, pad, m->sel->name, 0, False);
	drawstateindicator(m, m->sel, 1, x, a->y, w, a->h, 0, 0, m->sel->isfixed);
	return 1;
}

int
click_wintitle(Bar *bar, Arg *arg, BarArg *a)
{
	return ClkWinTitle;
}

int
width_awesomebar(Bar *bar, BarArg *a)
{
	return a->w;
}

int
draw_awesomebar(Bar *bar, BarArg *a)
{
	int n = 0, scm, remainder = 0, tabw, pad;
	unsigned int i;
	int x = a->x + lrpad / 2, w = a->w - lrpad / 2;

	Client *c;
	for (c = bar->mon->clients; c; c = c->next)
		if (ISVISIBLE(c))
			n++;

	if (n > 0) {
		remainder = w % n;
		tabw = w / n;
		for (i = 0, c = bar->mon->clients; c; c = c->next, i++) {
			if (!ISVISIBLE(c))
				continue;
			if (bar->mon->sel == c && HIDDEN(c))
				scm = SchemeHidSel;
			else if (HIDDEN(c))
				scm = SchemeHidNorm;
			else if (bar->mon->sel == c)
				scm = SchemeTitleSel;
			else
				scm = SchemeTitleNorm;

			pad = lrpad / 2;

			drw_setscheme(drw, scheme[scm]);
			drw_text(drw, x, a->y, tabw + (i < remainder ? 1 : 0), a->h, pad, c->name, 0, False);
			drawstateindicator(c->mon, c, 1, x, a->y, tabw + (i < remainder ? 1 : 0), a->h, 0, 0, c->isfixed);
			x += tabw + (i < remainder ? 1 : 0);
		}
	}
	return n;
}

int
click_awesomebar(Bar *bar, Arg *arg, BarArg *a)
{
	int x = 0, n = 0;
	Client *c;

	for (c = bar->mon->clients; c; c = c->next)
		if (ISVISIBLE(c))
			n++;

	c = bar->mon->clients;

	do {
		if (!c || !ISVISIBLE(c))
			continue;
		else
			x += (1.0 / (double)n) * a->w;
	} while (c && a->x > x && (c = c->next));

	if (c) {
		arg->v = c;
		return ClkWinTitle;
	}
	return -1;
}

static Systray *systray = NULL;
static unsigned long systrayorientation = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;

int
width_systray(Bar *bar, BarArg *a)
{
	unsigned int w = 0;
	Client *i;
	if (!systray)
		return 1;
	if (showsystray)
		for (i = systray->icons; i; w += i->w + systrayspacing, i = i->next);
	return w ? w + lrpad - systrayspacing : 0;
}

int
draw_systray(Bar *bar, BarArg *a)
{
	if (!showsystray)
		return 0;

	XSetWindowAttributes wa;
	XWindowChanges wc;
	Client *i;
	unsigned int w;

	if (!systray) {
		/* init systray */
		if (!(systray = (Systray *)calloc(1, sizeof(Systray))))
			die("fatal: could not malloc() %u bytes\n", sizeof(Systray));

		wa.override_redirect = True;
		wa.event_mask = ButtonPressMask|ExposureMask;
		wa.border_pixel = 0;
		systray->h = MIN(a->h, drw->fonts->h);
		wa.background_pixel = scheme[SchemeNorm][ColBg].pixel;
		systray->win = XCreateSimpleWindow(dpy, root, bar->bx + a->x + lrpad / 2, bar->by + a->y + (a->h - systray->h) / 2, MIN(a->w, 1), systray->h, 0, 0, scheme[SchemeNorm][ColBg].pixel);
		XChangeWindowAttributes(dpy, systray->win, CWOverrideRedirect|CWBackPixel|CWBorderPixel|CWEventMask, &wa);

		XSelectInput(dpy, systray->win, SubstructureNotifyMask);
		XChangeProperty(dpy, systray->win, netatom[NetSystemTrayOrientation], XA_CARDINAL, 32,
				PropModeReplace, (unsigned char *)&systrayorientation, 1);
		XChangeProperty(dpy, systray->win, netatom[NetWMWindowType], XA_ATOM, 32,
				PropModeReplace, (unsigned char *)&netatom[NetWMWindowTypeDock], 1);
		XMapRaised(dpy, systray->win);
		XSetSelectionOwner(dpy, netatom[NetSystemTray], systray->win, CurrentTime);
		if (XGetSelectionOwner(dpy, netatom[NetSystemTray]) == systray->win) {
			sendevent(root, xatom[Manager], StructureNotifyMask, CurrentTime, netatom[NetSystemTray], systray->win, 0, 0);
			XSync(dpy, False);
		} else {
			fprintf(stderr, "dwm: unable to obtain system tray.\n");
			free(systray);
			systray = NULL;
			return 0;
		}
	}

	systray->bar = bar;

	wc.stack_mode = Above;
	wc.sibling = bar->win;
	XConfigureWindow(dpy, systray->win, CWSibling|CWStackMode, &wc);

	drw_setscheme(drw, scheme[SchemeNorm]);
	for (w = 0, i = systray->icons; i; i = i->next) {
		wa.background_pixel = scheme[SchemeNorm][ColBg].pixel;
		XChangeWindowAttributes(dpy, i->win, CWBackPixel, &wa);
		XMapRaised(dpy, i->win);
		i->x = w;
		XMoveResizeWindow(dpy, i->win, i->x, 0, i->w, i->h);
		w += i->w;
		if (i->next)
			w += systrayspacing;
		if (i->mon != bar->mon)
			i->mon = bar->mon;
	}

	XMoveResizeWindow(dpy, systray->win, bar->bx + a->x + lrpad / 2, (w ? bar->by + a->y + (a->h - systray->h) / 2: -bar->by - a->y), MAX(w, 1), systray->h);
	return w;
}

int
click_systray(Bar *bar, Arg *arg, BarArg *a)
{
	return -1;
}

void
removesystrayicon(Client *i)
{
	Client **ii;

	if (!showsystray || !i)
		return;
	for (ii = &systray->icons; *ii && *ii != i; ii = &(*ii)->next);
	if (ii)
		*ii = i->next;
	free(i);
	drawbarwin(systray->bar);
}

void
resizerequest(XEvent *e)
{
	XResizeRequestEvent *ev = &e->xresizerequest;
	Client *i;

	if ((i = wintosystrayicon(ev->window))) {
		updatesystrayicongeom(i, ev->width, ev->height);
		drawbarwin(systray->bar);
	}
}

void
updatesystrayicongeom(Client *i, int w, int h)
{
	if (!systray)
		return;

	int icon_height = systray->h;
	if (i) {
		i->h = icon_height;
		if (w == h)
			i->w = icon_height;
		else if (h == icon_height)
			i->w = w;
		else
			i->w = (int) ((float)icon_height * ((float)w / (float)h));
		applysizehints(i, &(i->x), &(i->y), &(i->w), &(i->h), False);
		/* force icons into the systray dimensions if they don't want to */
		if (i->h > icon_height) {
			if (i->w == i->h)
				i->w = icon_height;
			else
				i->w = (int) ((float)icon_height * ((float)i->w / (float)i->h));
			i->h = icon_height;
		}
		if (i->w > 2 * icon_height)
			i->w = icon_height;
	}
}

void
updatesystrayiconstate(Client *i, XPropertyEvent *ev)
{
	long flags;
	int code = 0;

	if (!showsystray || !systray || !i || ev->atom != xatom[XembedInfo] ||
			!(flags = getatomprop(i, xatom[XembedInfo])))
		return;

	if (flags & XEMBED_MAPPED && !i->tags) {
		i->tags = 1;
		code = XEMBED_WINDOW_ACTIVATE;
		XMapRaised(dpy, i->win);
		setclientstate(i, NormalState);
	}
	else if (!(flags & XEMBED_MAPPED) && i->tags) {
		i->tags = 0;
		code = XEMBED_WINDOW_DEACTIVATE;
		XUnmapWindow(dpy, i->win);
		setclientstate(i, WithdrawnState);
	}
	else
		return;
	sendevent(i->win, xatom[Xembed], StructureNotifyMask, CurrentTime, code, 0,
			systray->win, XEMBED_EMBEDDED_VERSION);
}

Client *
wintosystrayicon(Window w)
{
	if (!systray)
		return NULL;
	Client *i = NULL;
	if (!showsystray || !w)
		return i;
	for (i = systray->icons; i && i->win != w; i = i->next);
	return i;
}

void
hide(Client *c) {

	Client *n;
	if (!c || HIDDEN(c))
		return;

	Window w = c->win;
	static XWindowAttributes ra, ca;

	// more or less taken directly from blackbox's hide() function
	XGrabServer(dpy);
	XGetWindowAttributes(dpy, root, &ra);
	XGetWindowAttributes(dpy, w, &ca);
	// prevent UnmapNotify events
	XSelectInput(dpy, root, ra.your_event_mask & ~SubstructureNotifyMask);
	XSelectInput(dpy, w, ca.your_event_mask & ~StructureNotifyMask);
	XUnmapWindow(dpy, w);
	setclientstate(c, IconicState);
	XSelectInput(dpy, root, ra.your_event_mask);
	XSelectInput(dpy, w, ca.your_event_mask);
	XUngrabServer(dpy);

	if (c->isfloating || !c->mon->lt[c->mon->sellt]->arrange) {
		for (n = c->snext; n && (!ISVISIBLE(n) || HIDDEN(n)); n = n->snext);
		if (!n)
			for (n = c->mon->stack; n && (!ISVISIBLE(n) || HIDDEN(n)); n = n->snext);
	} else {
		n = nexttiled(c);
		if (!n)
			n = prevtiled(c);
	}
	focus(n);
	arrange(c->mon);
}

void
show(Client *c)
{
	if (!c || !HIDDEN(c))
		return;

	XMapWindow(dpy, c->win);
	setclientstate(c, NormalState);
	arrange(c->mon);
}

void
togglewin(const Arg *arg)
{
	Client *c = (Client*)arg->v;
	if (!c)
		return;
	if (c == selmon->sel)
		hide(c);
	else {
		if (HIDDEN(c))
			show(c);
		focus(c);
		restack(c->mon);
	}
}

Client *
prevtiled(Client *c)
{
	Client *p, *i;
	for (p = NULL, i = c->mon->clients; c && i != c; i = i->next)
		if (ISVISIBLE(i) && !HIDDEN(i))
			p = i;
	return p;
}

void
showhideclient(const Arg *arg)
{
	Client *c = (Client*)arg->v;
	if (!c)
		c = selmon->sel;
	if (!c)
		return;

	if (HIDDEN(c)) {
		show(c);
		focus(c);
		restack(c->mon);
	} else {
		hide(c);
	}
}

/* dwm will keep pid's of processes from autostart array and kill them at quit */
static pid_t *autostart_pids;
static size_t autostart_len;

/* execute command from autostart array */
static void
autostart_exec()
{
	const char *const *p;
	size_t i = 0;

	/* count entries */
	for (p = autostart; *p; autostart_len++, p++)
		while (*++p);

	autostart_pids = malloc(autostart_len * sizeof(pid_t));
	for (p = autostart; *p; i++, p++) {
		if ((autostart_pids[i] = fork()) == 0) {
			setsid();
			execvp(*p, (char *const *)p);
			fprintf(stderr, "dwm: execvp %s\n", *p);
			perror(" failed");
			_exit(EXIT_FAILURE);
		}
		/* skip arguments */
		while (*++p);
	}
}

void
cyclelayout(const Arg *arg)
{
	Layout *l;
	for (l = (Layout *)layouts; l != selmon->lt[selmon->sellt]; l++);
	if (arg->i > 0) {
		if (l->symbol && (l + 1)->symbol)
			setlayout(&((Arg) { .v = (l + 1) }));
		else
			setlayout(&((Arg) { .v = layouts }));
	} else {
		if (l != layouts && (l - 1)->symbol)
			setlayout(&((Arg) { .v = (l - 1) }));
		else
			setlayout(&((Arg) { .v = &layouts[LENGTH(layouts) - 2] }));
	}
}

struct Pertag {
	unsigned int curtag, prevtag; /* current and previous tag */
	int nmasters[NUMTAGS + 1]; /* number of windows in master area */
	int nstacks[NUMTAGS + 1]; /* number of windows in primary stack area */
	int ltaxis[NUMTAGS + 1][LTAXIS_LAST];
	const Layout *ltidxs[NUMTAGS + 1][3]; /* matrix of tags and layouts indexes  */
	float mfacts[NUMTAGS + 1]; /* mfacts per tag */
	unsigned int sellts[NUMTAGS + 1]; /* selected layouts */
};

void
pertagview(const Arg *arg)
{
	int i;
	unsigned int tmptag;
	if (arg->ui & TAGMASK) {
		selmon->pertag->prevtag = selmon->pertag->curtag;
		selmon->tagset[selmon->seltags] = arg->ui & TAGMASK;
		if (arg->ui == ~SPTAGMASK)
			selmon->pertag->curtag = 0;
		else {
			for (i = 0; !(arg->ui & 1 << i); i++) ;
			selmon->pertag->curtag = i + 1;
		}
	} else {
		tmptag = selmon->pertag->prevtag;
		selmon->pertag->prevtag = selmon->pertag->curtag;
		selmon->pertag->curtag = tmptag;
	}
	selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag];
	selmon->nstack = selmon->pertag->nstacks[selmon->pertag->curtag];
	selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag];
	selmon->sellt = selmon->pertag->sellts[selmon->pertag->curtag];
	selmon->lt[selmon->sellt] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt];
	selmon->lt[selmon->sellt^1] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt^1];


	selmon->ltaxis[LAYOUT] = selmon->pertag->ltaxis[selmon->pertag->curtag][LAYOUT];
	selmon->ltaxis[MASTER] = selmon->pertag->ltaxis[selmon->pertag->curtag][MASTER];
	selmon->ltaxis[STACK]  = selmon->pertag->ltaxis[selmon->pertag->curtag][STACK];
	selmon->ltaxis[STACK2] = selmon->pertag->ltaxis[selmon->pertag->curtag][STACK2];
}

static int restart = 0;

void
sighup(int unused)
{
	Arg a = {.i = 1};
	quit(&a);
}

void
sigterm(int unused)
{
	Arg a = {.i = 0};
	quit(&a);
}

void
removescratch(const Arg *arg)
{
	Client *c = selmon->sel;
	if (!c)
		return;
	unsigned int scratchtag = SPTAG(arg->ui);
	c->tags = c->mon->tagset[c->mon->seltags] ^ scratchtag;
	arrange(c->mon);
}

void
setscratch(const Arg *arg)
{
	Client *c = selmon->sel;
	if (!c)
		return;
	unsigned int scratchtag = SPTAG(arg->ui);
	c->tags = scratchtag;
	arrange(c->mon);
}

void
togglescratch(const Arg *arg)
{
	Client *c = NULL, *next = NULL, *found = NULL;
	Monitor *mon;
	unsigned int scratchtag = SPTAG(arg->ui);
	unsigned int newtagset = 0;
	int nh = 0, nw = 0;
	Arg sparg = {.v = scratchpads[arg->ui].cmd};

	for (mon = mons; mon; mon = mon->next) {
		for (c = mon->clients; c; c = next) {
			next = c->next;
			if (!(c->tags & scratchtag))
				continue;

			found = c;

			if (HIDDEN(c)) {
				XMapWindow(dpy, c->win);
				setclientstate(c, NormalState);
				newtagset = 0;
			} else
				newtagset = selmon->tagset[selmon->seltags] ^ scratchtag;

			if (c->mon != selmon) {
				if (c->mon->tagset[c->mon->seltags] & SPTAGMASK)
					c->mon->tagset[c->mon->seltags] ^= scratchtag;
				if (c->w > selmon->ww)
					nw = selmon->ww - c->bw * 2;
				if (c->h > selmon->wh)
					nh = selmon->wh - c->bw * 2;
				if (nw > 0 || nh > 0)
					resizeclient(c, c->x, c->y, nw ? nw : c->w, nh ? nh : c->h);
				sendmon(c, selmon);
			}
		}
	}

	if (found) {
		if (newtagset) {
			selmon->tagset[selmon->seltags] = newtagset;
			focus(NULL);
			arrange(selmon);
		}
		if (ISVISIBLE(found)) {
			focus(found);
			restack(selmon);
		}
	} else {
		selmon->tagset[selmon->seltags] |= scratchtag;
		spawn(&sparg);
	}
}

/* Settings */
static int enablegaps = 1;

static void
setgaps(int oh, int ov, int ih, int iv)
{
	if (oh < 0) oh = 0;
	if (ov < 0) ov = 0;
	if (ih < 0) ih = 0;
	if (iv < 0) iv = 0;

	selmon->gappoh = oh;
	selmon->gappov = ov;
	selmon->gappih = ih;
	selmon->gappiv = iv;


	arrange(selmon);
}


static void
togglegaps(const Arg *arg)
{
	enablegaps = !enablegaps;
	arrange(NULL);
}

static void
defaultgaps(const Arg *arg)
{
	setgaps(gappoh, gappov, gappih, gappiv);
}

static void
incrgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh + arg->i,
		selmon->gappov + arg->i,
		selmon->gappih + arg->i,
		selmon->gappiv + arg->i
	);
}

static void
incrigaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh,
		selmon->gappov,
		selmon->gappih + arg->i,
		selmon->gappiv + arg->i
	);
}

static void
incrogaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh + arg->i,
		selmon->gappov + arg->i,
		selmon->gappih,
		selmon->gappiv
	);
}

static void
incrohgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh + arg->i,
		selmon->gappov,
		selmon->gappih,
		selmon->gappiv
	);
}

static void
incrovgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh,
		selmon->gappov + arg->i,
		selmon->gappih,
		selmon->gappiv
	);
}

static void
incrihgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh,
		selmon->gappov,
		selmon->gappih + arg->i,
		selmon->gappiv
	);
}

static void
incrivgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh,
		selmon->gappov,
		selmon->gappih,
		selmon->gappiv + arg->i
	);
}

static void
getgaps(Monitor *m, int *oh, int *ov, int *ih, int *iv, unsigned int *nc)
{
	unsigned int n, oe, ie;
	oe = ie = enablegaps;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 1) {
		oe *= smartgaps_fact; // outer gaps disabled or multiplied when only one client
	}

	*oh = m->gappoh*oe; // outer horizontal gap
	*ov = m->gappov*oe; // outer vertical gap
	*ih = m->gappih*ie; // inner horizontal gap
	*iv = m->gappiv*ie; // inner vertical gap
	*nc = n;            // number of clients
}

void
getfacts(Monitor *m, int msize, int ssize, float *mf, float *sf, int *mr, int *sr)
{
	unsigned int n;
	float mfacts, sfacts;
	int mtotal = 0, stotal = 0;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	mfacts = MIN(n, m->nmaster);
	sfacts = n - m->nmaster;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++)
		if (n < m->nmaster)
			mtotal += msize / mfacts;
		else
			stotal += ssize / sfacts;

	*mf = mfacts; // total factor of master area
	*sf = sfacts; // total factor of stack area
	*mr = msize - mtotal; // the remainder (rest) of pixels after an even master split
	*sr = ssize - stotal; // the remainder (rest) of pixels after an even stack split
}

static void
col(Monitor *m)
{
	unsigned int i, n;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int sx = 0, sy = 0, sh = 0, sw = 0;
	float mfacts, sfacts;
	int mrest, srest;
	Client *c;

	int oh, ov, ih, iv;
	getgaps(m, &oh, &ov, &ih, &iv, &n);

	if (n == 0)
		return;

	sx = mx = m->wx + ov;
	sy = my = m->wy + oh;
	mh = m->wh - 2*oh;
	sh = m->wh - 2*oh - ih * (n - m->nmaster - 1);
	mw = m->ww - 2*ov - iv * (MIN(n, m->nmaster) - 1);
	sw = m->ww - 2*ov;

	if (m->nmaster && n > m->nmaster) {
		sw = (mw - iv) * (1 - m->mfact);
		mw = (mw - iv) * m->mfact;
		sx = mx + mw + iv * m->nmaster;
	}

	getfacts(m, mw, sh, &mfacts, &sfacts, &mrest, &srest);

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i < m->nmaster) {
			resize(c, mx, my, (mw / mfacts) + (i < mrest ? 1 : 0) - (2*c->bw), mh - (2*c->bw), 0);
			mx += WIDTH(c) + iv;
		} else {
			resize(c, sx, sy, sw - (2*c->bw), (sh / sfacts) + ((i - m->nmaster) < srest ? 1 : 0) - (2*c->bw), 0);
			sy += HEIGHT(c) + ih;
		}
}

static void
deck(Monitor *m)
{
	unsigned int i, n;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int sx = 0, sy = 0, sh = 0, sw = 0;
	float mfacts, sfacts;
	int mrest, srest;
	Client *c;

	int oh, ov, ih, iv;
	getgaps(m, &oh, &ov, &ih, &iv, &n);

	if (n == 0)
		return;

	sx = mx = m->wx + ov;
	sy = my = m->wy + oh;
	sh = mh = m->wh - 2*oh - ih * (MIN(n, m->nmaster) - 1);
	sw = mw = m->ww - 2*ov;

	if (m->nmaster && n > m->nmaster) {
		sw = (mw - iv) * (1 - m->mfact);
		mw = (mw - iv) * m->mfact;
		sx = mx + mw + iv;
		sh = m->wh - 2*oh;
	}

	getfacts(m, mh, sh, &mfacts, &sfacts, &mrest, &srest);

	if (n - m->nmaster > 0) /* override layout symbol */
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "D %d", n - m->nmaster);

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i < m->nmaster) {
			resize(c, mx, my, mw - (2*c->bw), (mh / mfacts) + (i < mrest ? 1 : 0) - (2*c->bw), 0);
			my += HEIGHT(c) + ih;
		} else {
			resize(c, sx, sy, sw - (2*c->bw), sh - (2*c->bw), 0);
		}
}

typedef struct {
	void (*arrange)(Monitor *, int, int, int, int, int, int, int);
} LayoutArranger;

typedef struct {
	void (*arrange)(Monitor *, int, int, int, int, int, int, int, int, int);
} TileArranger;

static const LayoutArranger flexlayouts[] = {
	{ layout_no_split },
	{ layout_split_vertical },
	{ layout_split_horizontal },
	{ layout_split_centered_vertical },
	{ layout_split_centered_horizontal },
	{ layout_split_vertical_dual_stack },
	{ layout_split_horizontal_dual_stack },
	{ layout_floating_master },
	{ layout_split_vertical_fixed },
	{ layout_split_horizontal_fixed },
	{ layout_split_centered_vertical_fixed },
	{ layout_split_centered_horizontal_fixed },
	{ layout_split_vertical_dual_stack_fixed },
	{ layout_split_horizontal_dual_stack_fixed },
	{ layout_floating_master_fixed },
};

static const TileArranger flextiles[] = {
	{ arrange_top_to_bottom },
	{ arrange_left_to_right },
	{ arrange_monocle },
	{ arrange_gapplessgrid },
	{ arrange_gapplessgrid_alt1 },
	{ arrange_gapplessgrid_alt2 },
	{ arrange_gridmode },
	{ arrange_horizgrid },
	{ arrange_dwindle },
	{ arrange_spiral },
	{ arrange_tatami },
};

static void
getfactsforrange(Monitor *m, int an, int ai, int size, int *rest, float *fact)
{
	int i;
	float facts;
	Client *c;
	int total = 0;

	facts = 0;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i >= ai && i < (ai + an))
			facts += 1;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i >= ai && i < (ai + an))
			total += size / facts;

	*rest = size - total;
	*fact = facts;
}


static void
layout_no_split(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	(&flextiles[m->ltaxis[m->nmaster >= n ? MASTER : STACK]])->arrange(m, x, y, h, w, ih, iv, n, n, 0);
}

static void
layout_split_vertical(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (m->nmaster && n > m->nmaster) {
		layout_split_vertical_fixed(m, x, y, h, w, ih, iv, n);
	} else {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_vertical_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sw, sx;

	sw = (w - iv) * (1 - m->mfact);
	w = (w - iv) * m->mfact;
	if (m->ltaxis[LAYOUT] < 0) { // mirror
		sx = x;
		x += sw + iv;
	} else {
		sx = x + w + iv;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, sx, y, h, sw, ih, iv, n, n - m->nmaster, m->nmaster);
}

static void
layout_split_vertical_dual_stack(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else if (n <= m->nmaster + (m->nstack ? m->nstack : 1)) {
		layout_split_vertical(m, x, y, h, w, ih, iv, n);
	} else {
		layout_split_vertical_dual_stack_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_vertical_dual_stack_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sh, sw, sx, oy, sc;

	if (m->nstack)
		sc = m->nstack;
	else
		sc = (n - m->nmaster) / 2 + ((n - m->nmaster) % 2 > 0 ? 1 : 0);

	sw = (w - iv) * (1 - m->mfact);
	sh = (h - ih) / 2;
	w = (w - iv) * m->mfact;
	oy = y + sh + ih;
	if (m->ltaxis[LAYOUT] < 0) { // mirror
		sx = x;
		x += sw + iv;
	} else {
		sx = x + w + iv;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, sx, y, sh, sw, ih, iv, n, sc, m->nmaster);
	(&flextiles[m->ltaxis[STACK2]])->arrange(m, sx, oy, sh, sw, ih, iv, n, n - m->nmaster - sc, m->nmaster + sc);
}

static void
layout_split_horizontal(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (m->nmaster && n > m->nmaster) {
		layout_split_horizontal_fixed(m, x, y, h, w, ih, iv, n);
	} else {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_horizontal_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sh, sy;

	sh = (h - ih) * (1 - m->mfact);
	h = (h - ih) * m->mfact;
	if (m->ltaxis[LAYOUT] < 0) { // mirror
		sy = y;
		y += sh + ih;
	} else {
		sy = y + h + ih;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, x, sy, sh, w, ih, iv, n, n - m->nmaster, m->nmaster);
}

static void
layout_split_horizontal_dual_stack(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else if (n <= m->nmaster + (m->nstack ? m->nstack : 1)) {
		layout_split_horizontal(m, x, y, h, w, ih, iv, n);
	} else {
		layout_split_horizontal_dual_stack_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_horizontal_dual_stack_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sh, sy, ox, sc;

	if (m->nstack)
		sc = m->nstack;
	else
		sc = (n - m->nmaster) / 2 + ((n - m->nmaster) % 2 > 0 ? 1 : 0);

	sh = (h - ih) * (1 - m->mfact);
	h = (h - ih) * m->mfact;
	sw = (w - iv) / 2;
	ox = x + sw + iv;
	if (m->ltaxis[LAYOUT] < 0) { // mirror
		sy = y;
		y += sh + ih;
	} else {
		sy = y + h + ih;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, x, sy, sh, sw, ih, iv, n, sc, m->nmaster);
	(&flextiles[m->ltaxis[STACK2]])->arrange(m, ox, sy, sh, sw, ih, iv, n, n - m->nmaster - sc, m->nmaster + sc);
}

static void
layout_split_centered_vertical(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else if (n <= m->nmaster + (m->nstack ? m->nstack : 1)) {
		layout_split_vertical(m, x, y, h, w, ih, iv, n);
	} else {
		layout_split_centered_vertical_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_centered_vertical_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sw, sx, ox, sc;

	if (m->nstack)
		sc = m->nstack;
	else
		sc = (n - m->nmaster) / 2 + ((n - m->nmaster) % 2 > 0 ? 1 : 0);

	sw = (w - 2*iv) * (1 - m->mfact) / 2;
	w = (w - 2*iv) * m->mfact;
	if (m->ltaxis[LAYOUT] < 0)  { // mirror
		sx = x;
		x += sw + iv;
		ox = x + w + iv;
	} else {
		ox = x;
		x += sw + iv;
		sx = x + w + iv;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, sx, y, h, sw, ih, iv, n, sc, m->nmaster);
	(&flextiles[m->ltaxis[STACK2]])->arrange(m, ox, y, h, sw, ih, iv, n, n - m->nmaster - sc, m->nmaster + sc);
}

static void
layout_split_centered_horizontal(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else if (n <= m->nmaster + (m->nstack ? m->nstack : 1)) {
		layout_split_horizontal(m, x, y, h, w, ih, iv, n);
	} else {
		layout_split_centered_horizontal_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_split_centered_horizontal_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int sh, sy, oy, sc;

	if (m->nstack)
		sc = m->nstack;
	else
		sc = (n - m->nmaster) / 2 + ((n - m->nmaster) % 2 > 0 ? 1 : 0);

	sh = (h - 2*ih) * (1 - m->mfact) / 2;
	h = (h - 2*ih) * m->mfact;
	if (m->ltaxis[LAYOUT] < 0)  { // mirror
		sy = y;
		y += sh + ih;
		oy = y + h + ih;
	} else {
		oy = y;
		y += sh + ih;
		sy = y + h + ih;
	}

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, h, w, ih, iv, n, m->nmaster, 0);
	(&flextiles[m->ltaxis[STACK]])->arrange(m, x, sy, sh, w, ih, iv, n, sc, m->nmaster);
	(&flextiles[m->ltaxis[STACK2]])->arrange(m, x, oy, sh, w, ih, iv, n, n - m->nmaster - sc, m->nmaster + sc);
}

static void
layout_floating_master(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	/* Split master into master + stack if we have enough clients */
	if (!m->nmaster || n <= m->nmaster) {
		layout_no_split(m, x, y, h, w, ih, iv, n);
	} else {
		layout_floating_master_fixed(m, x, y, h, w, ih, iv, n);
	}
}

static void
layout_floating_master_fixed(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n)
{
	int mh, mw;

	/* Draw stack area first */
	(&flextiles[m->ltaxis[STACK]])->arrange(m, x, y, h, w, ih, iv, n, n - m->nmaster, m->nmaster);

	if (w > h) {
		mw = w * m->mfact;
		mh = h * 0.9;
	} else {
		mw = w * 0.9;
		mh = h * m->mfact;
	}
	x = x + (w - mw) / 2;
	y = y + (h - mh) / 2;

	(&flextiles[m->ltaxis[MASTER]])->arrange(m, x, y, mh, mw, ih, iv, n, m->nmaster, 0);
}

static void
arrange_left_to_right(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, rest;
	float facts, fact = 1;
	Client *c;

	if (ai + an > n)
		an = n - ai;

	w -= iv * (an - 1);
	getfactsforrange(m, an, ai, w, &rest, &facts);
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i >= ai && i < (ai + an)) {
			resize(c, x, y, w * (fact / facts) + ((i - ai) < rest ? 1 : 0) - (2*c->bw), h - (2*c->bw), 0);
			x += WIDTH(c) + iv;
		}
	}
}

static void
arrange_top_to_bottom(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, rest;
	float facts, fact = 1;
	Client *c;

	if (ai + an > n)
		an = n - ai;

	h -= ih * (an - 1);
	getfactsforrange(m, an, ai, h, &rest, &facts);
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i >= ai && i < (ai + an)) {
			resize(c, x, y, w - (2*c->bw), h * (fact / facts) + ((i - ai) < rest ? 1 : 0) - (2*c->bw), 0);
			y += HEIGHT(c) + ih;
		}
	}
}

static void
arrange_monocle(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i;
	Client *c;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i >= ai && i < (ai + an))
			resize(c, x, y, w - (2*c->bw), h - (2*c->bw), 0);
}

static void
arrange_gridmode(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, cols, rows, ch, cw, cx, cy, cc, cr, chrest, cwrest; // counters
	Client *c;

	/* grid dimensions */
	for (rows = 0; rows <= an/2; rows++)
		if (rows*rows >= an)
			break;
	cols = (rows && (rows - 1) * rows >= an) ? rows - 1 : rows;

	/* window geoms (cell height/width) */
	ch = (h - ih * (rows - 1)) / (rows ? rows : 1);
	cw = (w - iv * (cols - 1)) / (cols ? cols : 1);
	chrest = h - ih * (rows - 1) - ch * rows;
	cwrest = w - iv * (cols - 1) - cw * cols;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i >= ai && i < (ai + an)) {
			cc = ((i - ai) / rows); // client column number
			cr = ((i - ai) % rows); // client row number
			cx = x + cc * (cw + iv) + MIN(cc, cwrest);
			cy = y + cr * (ch + ih) + MIN(cr, chrest);
			resize(c, cx, cy, cw + (cc < cwrest ? 1 : 0) - 2*c->bw, ch + (cr < chrest ? 1 : 0) - 2*c->bw, False);
		}
	}
}

static void
arrange_horizgrid(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int ntop, nbottom, rh, rest;

	/* Exception when there is only one client; don't split into two rows */
	if (an == 1) {
		arrange_monocle(m, x, y, h, w, ih, iv, n, an, ai);
		return;
	}

	ntop = an / 2;
	nbottom = an - ntop;
	rh = (h - ih) / 2;
	rest = h - ih - rh * 2;
	arrange_left_to_right(m, x, y, rh + rest, w, ih, iv, n, ntop, ai);
	arrange_left_to_right(m, x, y + rh + ih + rest, rh, w, ih, iv, n, nbottom, ai + ntop);
}

static void
arrange_gapplessgrid(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, cols, rows, ch, cw, cn, rn, cc, rrest, crest; // counters
	Client *c;

	/* grid dimensions */
	for (cols = 1; cols <= an/2; cols++)
		if (cols*cols >= an)
			break;
	if (an == 5) /* set layout against the general calculation: not 1:2:2, but 2:3 */
		cols = 2;
	rows = an/cols;
	cn = rn = cc = 0; // reset column no, row no, client count

	ch = (h - ih * (rows - 1)) / rows;
	rrest = (h - ih * (rows - 1)) - ch * rows;
	cw = (w - iv * (cols - 1)) / cols;
	crest = (w - iv * (cols - 1)) - cw * cols;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i >= ai && i < (ai + an)) {
			if (cc/rows + 1 > cols - an%cols) {
				rows = an/cols + 1;
				ch = (h - ih * (rows - 1)) / rows;
				rrest = (h - ih * (rows - 1)) - ch * rows;
			}
			resize(c,
				x,
				y + rn*(ch + ih) + MIN(rn, rrest),
				cw + (cn < crest ? 1 : 0) - 2*c->bw,
				ch + (rn < rrest ? 1 : 0) - 2*c->bw,
				0);
			rn++;
			cc++;
			if (rn >= rows) {
				rn = 0;
				x += cw + ih + (cn < crest ? 1 : 0);
				cn++;
			}
		}
	}
}

/* This version of gappless grid fills rows first */
static void
arrange_gapplessgrid_alt1(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, cols, rows, rest, ch;

	/* grid dimensions */
	for (cols = 1; cols <= an/2; cols++)
		if (cols*cols >= an)
			break;
	rows = (cols && (cols - 1) * cols >= an) ? cols - 1 : cols;
	ch = (h - ih * (rows - 1)) / (rows ? rows : 1);
	rest = (h - ih * (rows - 1)) - ch * rows;

	for (i = 0; i < rows; i++) {
		arrange_left_to_right(m, x, y, ch + (i < rest ? 1 : 0), w, ih, iv, n, MIN(cols, an - i*cols), ai + i*cols);
		y += ch + (i < rest ? 1 : 0) + ih;
	}
}

/* This version of gappless grid fills columns first */
static void
arrange_gapplessgrid_alt2(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	int i, cols, rows, rest, cw;

	/* grid dimensions */
	for (rows = 0; rows <= an/2; rows++)
		if (rows*rows >= an)
			break;
	cols = (rows && (rows - 1) * rows >= an) ? rows - 1 : rows;
	cw = (w - iv * (cols - 1)) / (cols ? cols : 1);
	rest = (w - iv * (cols - 1)) - cw * cols;

	for (i = 0; i < cols; i++) {
		arrange_top_to_bottom(m, x, y, h, cw + (i < rest ? 1 : 0), ih, iv, n, MIN(rows, an - i*rows), ai + i*rows);
		x += cw + (i < rest ? 1 : 0) + iv;
	}
}

static void
arrange_fibonacci(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai, int s)
{
	int i, j, nv, hrest = 0, wrest = 0, nx = x, ny = y, nw = w, nh = h, r = 1;
	Client *c;

	for (i = 0, j = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), j++) {
		if (j >= ai && j < (ai + an)) {
			if (r) {
				if ((i % 2 && ((nh - ih) / 2) <= (bh + 2*c->bw)) || (!(i % 2) && ((nw - iv) / 2) <= (bh + 2*c->bw))) {
					r = 0;
				}
				if (r && i < an - 1) {
					if (i % 2) {
						nv = (nh - ih) / 2;
						hrest = nh - 2*nv - ih;
						nh = nv;
					} else {
						nv = (nw - iv) / 2;
						wrest = nw - 2*nv - iv;
						nw = nv;
					}

					if ((i % 4) == 2 && !s)
						nx += nw + iv;
					else if ((i % 4) == 3 && !s)
						ny += nh + ih;
				}
				if ((i % 4) == 0) {
					if (s) {
						ny += nh + ih;
						nh += hrest;
					} else {
						nh -= hrest;
						ny -= nh + ih;
					}
				} else if ((i % 4) == 1) {
					nx += nw + iv;
					nw += wrest;
				} else if ((i % 4) == 2) {
					ny += nh + ih;
					nh += hrest;
					if (i < n - 1)
						nw += wrest;
				} else if ((i % 4) == 3) {
					if (s) {
						nx += nw + iv;
						nw -= wrest;
					} else {
						nw -= wrest;
						nx -= nw + iv;
						nh += hrest;
					}
				}
				if (i == 0)	{
					if (an != 1) {
						nw = (w - iv) - (w - iv) * (1 - m->mfact);
						wrest = 0;
					}
					ny = y;
				} else if (i == 1)
					nw = w - nw - iv;
				i++;
			}

			resize(c, nx, ny, nw - 2 * c->bw, nh - 2*c->bw, False);
		}
	}
}

static void
arrange_dwindle(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	arrange_fibonacci(m, x, y, h, w, ih, iv, n, an, ai, 1);
}

static void
arrange_spiral(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	arrange_fibonacci(m, x, y, h, w, ih, iv, n, an, ai, 0);
}

static void
arrange_tatami(Monitor *m, int x, int y, int h, int w, int ih, int iv, int n, int an, int ai)
{
	unsigned int i, j, nx, ny, nw, nh, tnx, tny, tnw, tnh, nhrest, hrest, wrest, areas, mats, cats;
	Client *c;

	nx = x;
	ny = y;
	nw = w;
	nh = h;

	mats = an / 5;
	cats = an % 5;
	hrest = 0;
	wrest = 0;

	areas = mats + (cats > 0);
	nh = (h - ih * (areas - 1)) / areas;
	nhrest = (h - ih * (areas - 1)) % areas;

	for (i = 0, j = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), j++) {
		if (j >= ai && j < (ai + an)) {

			tnw = nw;
			tnx = nx;
			tnh = nh;
			tny = ny;

			if (j < ai + cats) {
				/* Arrange cats (all excess clients that can't be tiled as mats). Cats sleep on mats. */

	 			switch (cats) {
				case 1: // fill
					break;
				case 2: // up and down
					if ((i % 5) == 0) //up
						tnh = (nh - ih) / 2 + (nh - ih) % 2;
					else if ((i % 5) == 1) { //down
						tny += (nh - ih) / 2 + (nh - ih) % 2 + ih;
						tnh = (nh - ih) / 2;
					}
					break;
				case 3: //bottom, up-left and up-right
					if ((i % 5) == 0) { // up-left
						tnw = (nw - iv) / 2 + (nw - iv) % 2;
						tnh = (nh - ih) * 2 / 3 + (nh - ih) * 2 % 3;
					} else if ((i % 5) == 1) { // up-right
						tnx += (nw - iv) / 2 + (nw - iv) % 2 + iv;
						tnw = (nw - iv) / 2;
						tnh = (nh - ih) * 2 / 3 + (nh - ih) * 2 % 3;
					} else if ((i % 5) == 2) { //bottom
						tnh = (nh - ih) / 3;
						tny += (nh - ih) * 2 / 3 + (nh - ih) * 2 % 3 + ih;
					}
					break;
				case 4: // bottom, left, right and top
					if ((i % 5) == 0) { //top
						hrest = (nh - 2 * ih) % 4;
						tnh = (nh - 2 * ih) / 4 + (hrest ? 1 : 0);
					} else if ((i % 5) == 1) { // left
						tnw = (nw - iv) / 2 + (nw - iv) % 2;
						tny += (nh - 2 * ih) / 4 + (hrest ? 1 : 0) + ih;
						tnh = (nh - 2 * ih) * 2 / 4 + (hrest > 1 ? 1 : 0);
					} else if ((i % 5) == 2) { // right
						tnx += (nw - iv) / 2 + (nw - iv) % 2 + iv;
						tnw = (nw - iv) / 2;
						tny += (nh - 2 * ih) / 4 + (hrest ? 1 : 0) + ih;
						tnh = (nh - 2 * ih) * 2 / 4 + (hrest > 1 ? 1 : 0);
					} else if ((i % 5) == 3) { // bottom
						tny += (nh - 2 * ih) / 4 + (hrest ? 1 : 0) + (nh - 2 * ih) * 2 / 4 + (hrest > 1 ? 1 : 0) + 2 * ih;
						tnh = (nh - 2 * ih) / 4 + (hrest > 2 ? 1 : 0);
					}
					break;
				}

			} else {
				/* Arrange mats. One mat is a collection of five clients arranged tatami style */

				if (((i - cats) % 5) == 0) {
					if ((cats > 0) || ((i - cats) >= 5)) {
						tny = ny = ny + nh + (nhrest > 0 ? 1 : 0) + ih;
						--nhrest;
					}
				}

				switch ((i - cats) % 5) {
				case 0: // top-left-vert
					wrest = (nw - 2 * iv) % 3;
					hrest = (nh - 2 * ih) % 3;
					tnw = (nw - 2 * iv) / 3 + (wrest ? 1 : 0);
					tnh = (nh - 2 * ih) * 2 / 3 + hrest + iv;
					break;
				case 1: // top-right-hor
					tnx += (nw - 2 * iv) / 3 + (wrest ? 1 : 0) + iv;
					tnw = (nw - 2 * iv) * 2 / 3 + (wrest > 1 ? 1 : 0) + iv;
					tnh = (nh - 2 * ih) / 3 + (hrest ? 1 : 0);
					break;
				case 2: // center
					tnx += (nw - 2 * iv) / 3 + (wrest ? 1 : 0) + iv;
					tnw = (nw - 2 * iv) / 3 + (wrest > 1 ? 1 : 0);
					tny += (nh - 2 * ih) / 3 + (hrest ? 1 : 0) + ih;
					tnh = (nh - 2 * ih) / 3 + (hrest > 1 ? 1 : 0);
					break;
				case 3: // bottom-right-vert
					tnx += (nw - 2 * iv) * 2 / 3 + wrest + 2 * iv;
					tnw = (nw - 2 * iv) / 3;
					tny += (nh - 2 * ih) / 3 + (hrest ? 1 : 0) + ih;
					tnh = (nh - 2 * ih) * 2 / 3 + hrest + iv;
					break;
				case 4: // (oldest) bottom-left-hor
					tnw = (nw - 2 * iv) * 2 / 3 + wrest + iv;
					tny += (nh - 2 * ih) * 2 / 3 + hrest + 2 * iv;
					tnh = (nh - 2 * ih) / 3;
					break;
				}

			}

			resize(c, tnx, tny, tnw - 2 * c->bw, tnh - 2 * c->bw, False);
			++i;
		}
	}
}

static void
flextile(Monitor *m)
{
	unsigned int n;
	int oh = 0, ov = 0, ih = 0, iv = 0; // gaps outer/inner horizontal/vertical

	getgaps(m, &oh, &ov, &ih, &iv, &n);

	if (m->lt[m->sellt]->preset.layout != m->ltaxis[LAYOUT] ||
			m->lt[m->sellt]->preset.masteraxis != m->ltaxis[MASTER] ||
			m->lt[m->sellt]->preset.stack1axis != m->ltaxis[STACK] ||
			m->lt[m->sellt]->preset.stack2axis != m->ltaxis[STACK2])
		setflexsymbols(m, n);
	else if (m->lt[m->sellt]->preset.symbolfunc != NULL)
		m->lt[m->sellt]->preset.symbolfunc(m, n);

	if (n == 0)
		return;

	/* No outer gap if full screen monocle */
	if (abs(m->ltaxis[MASTER]) == MONOCLE && (abs(m->ltaxis[LAYOUT]) == NO_SPLIT || n <= m->nmaster)) {
		oh = 0;
		ov = 0;
	}

	(&flexlayouts[abs(m->ltaxis[LAYOUT])])->arrange(m, m->wx + ov, m->wy + oh, m->wh - 2*oh, m->ww - 2*ov, ih, iv, n);
	return;
}

static void
setflexsymbols(Monitor *m, unsigned int n)
{
	int l;
	char sym1, sym2, sym3;
	Client *c;

	if (n == 0)
		for (c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	l = abs(m->ltaxis[LAYOUT]);
	if (m->ltaxis[MASTER] == MONOCLE && (l == NO_SPLIT || !m->nmaster || n <= m->nmaster)) {
		monoclesymbols(m, n);
		return;
	}

	if (m->ltaxis[STACK] == MONOCLE && (l == SPLIT_VERTICAL || l == SPLIT_HORIZONTAL_FIXED)) {
		decksymbols(m, n);
		return;
	}

	/* Layout symbols */
	if (l == NO_SPLIT || !m->nmaster) {
		sym1 = sym2 = sym3 = (int)tilesymb[m->ltaxis[MASTER]];
	} else {
		sym2 = layoutsymb[l];
		if (m->ltaxis[LAYOUT] < 0) {
			sym1 = tilesymb[m->ltaxis[STACK]];
			sym3 = tilesymb[m->ltaxis[MASTER]];
		} else {
			sym1 = tilesymb[m->ltaxis[MASTER]];
			sym3 = tilesymb[m->ltaxis[STACK]];
		}
	}

	snprintf(m->ltsymbol, sizeof m->ltsymbol, "%c%c%c", sym1, sym2, sym3);
}

static void
monoclesymbols(Monitor *m, unsigned int n)
{
	if (n > 0)
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[%d]", n);
	else
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[M]");
}

static void
decksymbols(Monitor *m, unsigned int n)
{
	if (n > m->nmaster)
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[]%d", n);
	else
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[D]");
}

/* Mirror layout axis for flextile */
void
mirrorlayout(const Arg *arg)
{
	if (!selmon->lt[selmon->sellt]->arrange)
		return;
	selmon->ltaxis[LAYOUT] *= -1;
	selmon->pertag->ltaxis[selmon->pertag->curtag][0] = selmon->ltaxis[LAYOUT];
	arrange(selmon);
}

/* Rotate layout axis for flextile */
void
rotatelayoutaxis(const Arg *arg)
{
	int incr = (arg->i > 0 ? 1 : -1);
	int axis = abs(arg->i) - 1;

	if (!selmon->lt[selmon->sellt]->arrange)
		return;
	if (axis == LAYOUT) {
		if (selmon->ltaxis[LAYOUT] >= 0) {
			selmon->ltaxis[LAYOUT] += incr;
			if (selmon->ltaxis[LAYOUT] >= LAYOUT_LAST)
				selmon->ltaxis[LAYOUT] = 0;
			else if (selmon->ltaxis[LAYOUT] < 0)
				selmon->ltaxis[LAYOUT] = LAYOUT_LAST - 1;
		} else {
			selmon->ltaxis[LAYOUT] -= incr;
			if (selmon->ltaxis[LAYOUT] <= -LAYOUT_LAST)
				selmon->ltaxis[LAYOUT] = 0;
			else if (selmon->ltaxis[LAYOUT] > 0)
				selmon->ltaxis[LAYOUT] = -LAYOUT_LAST + 1;
		}
	} else {
		selmon->ltaxis[axis] += incr;
		if (selmon->ltaxis[axis] >= AXIS_LAST)
			selmon->ltaxis[axis] = 0;
		else if (selmon->ltaxis[axis] < 0)
			selmon->ltaxis[axis] = AXIS_LAST - 1;
	}
	selmon->pertag->ltaxis[selmon->pertag->curtag][axis] = selmon->ltaxis[axis];
	arrange(selmon);
	setflexsymbols(selmon, 0);
}

void
incnstack(const Arg *arg)
{
	selmon->nstack = selmon->pertag->nstacks[selmon->pertag->curtag] = MAX(selmon->nstack + arg->i, 0);
	arrange(selmon);
}

void
monocle(Monitor *m)
{
	unsigned int n = 0;
	Client *c;

	for (c = m->clients; c; c = c->next)
		if (ISVISIBLE(c))
			n++;
	if (n > 0) /* override layout symbol */
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[%d]", n);
	for (c = nexttiled(m->clients); c; c = nexttiled(c->next))
		resize(c, m->wx, m->wy, m->ww - 2 * c->bw, m->wh - 2 * c->bw, 0);
}

static void
tile(Monitor *m)
{
	unsigned int i, n;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int sx = 0, sy = 0, sh = 0, sw = 0;
	float mfacts, sfacts;
	int mrest, srest;
	Client *c;


	int oh, ov, ih, iv;
	getgaps(m, &oh, &ov, &ih, &iv, &n);

	if (n == 0)
		return;

	sx = mx = m->wx + ov;
	sy = my = m->wy + oh;
	mh = m->wh - 2*oh - ih * (MIN(n, m->nmaster) - 1);
	sh = m->wh - 2*oh - ih * (n - m->nmaster - 1);
	sw = mw = m->ww - 2*ov;

	if (m->nmaster && n > m->nmaster) {
		sw = (mw - iv) * (1 - m->mfact);
		mw = (mw - iv) * m->mfact;
		sx = mx + mw + iv;
	}

	getfacts(m, mh, sh, &mfacts, &sfacts, &mrest, &srest);

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i < m->nmaster) {
			resize(c, mx, my, mw - (2*c->bw), (mh / mfacts) + (i < mrest ? 1 : 0) - (2*c->bw), 0);
			my += HEIGHT(c) + ih;
		} else {
			resize(c, sx, sy, sw - (2*c->bw), (sh / sfacts) + ((i - m->nmaster) < srest ? 1 : 0) - (2*c->bw), 0);
			sy += HEIGHT(c) + ih;
		}
}

