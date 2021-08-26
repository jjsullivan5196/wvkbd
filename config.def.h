#ifndef config_def_h_INCLUDED
#define config_def_h_INCLUDED

static const char *default_font = "Monospace 14";

struct clr_scheme scheme = {
	/* colors */
	.bg = {.bgra = {15, 15, 15, 225}},
	.fg = {.bgra = {45, 45, 45, 225}},
	.high = {.bgra = {100, 100, 100, 225}},
	.text = {.color = UINT32_MAX},
};
struct clr_scheme scheme1 = {
	/* colors */
	.bg = {.bgra = {15, 15, 15, 225}},
	.fg = {.bgra = {32, 32, 32, 225}},
	.high = {.bgra = {100, 100, 100, 225}},
	.text = {.color = UINT32_MAX},
};

/* layers is an ordered list of layouts, used to cycle through */
static enum layout_id layers[] = {
	Full, //First layout is the default layout on startup
	Special,
	Emoji,
	Simple,
	SimpleGrid,
	Cyrillic,
	Arabic,
	NumLayouts //signals the last item, may not be omitted
};

/* layers is an ordered list of layouts, used to cycle through */
static enum layout_id landscape_layers[] = {
	Landscape, //First layout is the default layout on startup
	Special,
	Emoji,
	NumLayouts //signals the last item, may not be omitted
};

#endif // config_def_h_INCLUDED

