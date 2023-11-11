#ifndef config_def_h_INCLUDED
#define config_def_h_INCLUDED

#define DEFAULT_FONT "Sans 14"
#define DEFAULT_ROUNDING 5
#define EDGE_CORNER_FONT "Sans 10"
static const int transparency = 255;
static const int text_visible = 255;
static const int text_invisible = 0;

/* dracula scheme */
struct clr_scheme schemes[] = {
{
  /* 0: special keys */
  .bg = {.bgra = {15, 15, 15, transparency}},
  .fg = {.bgra = {90, 71, 68, transparency}}, /* dracula "comment" */
  .high = {.bgra = {100, 100, 100, transparency}},
  .swipe = {.bgra = {255, 255, 255, 64}},
  .text = {.bgra = {249, 147, 189, text_visible}}, /* dracula "current line" */
  .font = DEFAULT_FONT,
  .rounding = DEFAULT_ROUNDING,
},
{
  /* 1: normal keys (letters etc.) */
  .bg = {.bgra = {15, 15, 15, transparency}},
  .fg = {.bgra = {54, 42, 40, transparency}},
  .high = {.bgra = {100, 100, 100, transparency}},
  .swipe = {.bgra = {100, 255, 100, 64}},
  .text = {.bgra = {249, 147, 189, text_visible}}, /* purple */
  .font = DEFAULT_FONT,
  .rounding = DEFAULT_ROUNDING,
},
{
  /* 2: primary edge and corner keys (letters, Shift) */
  .bg = {.bgra = {15, 15, 15, transparency}},
  .fg = {.bgra = {54, 42, 40, transparency}},
  .high = {.bgra = {100, 100, 100, transparency}},
  .swipe = {.bgra = {100, 255, 100, 64}},
  .text = {.bgra = {85, 85, 255, text_visible}}, /* red */
  .font = EDGE_CORNER_FONT,
  .rounding = DEFAULT_ROUNDING,
},
{
  /* 3: secondary edge and corner keys (important punctuation) */
  .bg = {.bgra = {15, 15, 15, transparency}},
  .fg = {.bgra = {54, 42, 40, transparency}},
  .high = {.bgra = {100, 100, 100, transparency}},
  .swipe = {.bgra = {100, 255, 100, 64}},
  .text = {.bgra = {164, 114, 98, text_visible}}, /* dracula "comment" */
  .font = EDGE_CORNER_FONT,
  .rounding = DEFAULT_ROUNDING,
},
{
  /* 4: tertiary (less common) edge and corner keys (invisible by default) */
  .bg = {.bgra = {15, 15, 15, transparency}},
  .fg = {.bgra = {54, 42, 40, transparency}},
  .high = {.bgra = {100, 100, 100, transparency}},
  .swipe = {.bgra = {100, 255, 100, 64}},
  .text = {.bgra = {164, 114, 98, text_invisible}}, /* dracula "comment", but transparent */
  .font = EDGE_CORNER_FONT,
  .rounding = DEFAULT_ROUNDING,
},
{
  /* 5: primary edge and corner special key */
  .bg = {.bgra = {15, 15, 15, transparency}},
  .fg = {.bgra = {164, 114, 98, transparency}}, /* dracula "comment" */
  .high = {.bgra = {255, 255, 255, transparency}},
  .swipe = {.bgra = {255, 255, 255, 64}},
  .text = {.bgra = {90, 71, 68, text_visible}}, /* dracula "current line" */
  .font = EDGE_CORNER_FONT,
  .rounding = DEFAULT_ROUNDING,
},
};

/* layers is an ordered list of layouts, used to cycle through */
static enum layout_id layers[] = {
  Anihortes, // First layout is the default layout on startup
  NumLayouts // signals the last item, may not be omitted
};

/* layers is an ordered list of layouts, used to cycle through */
static enum layout_id landscape_layers[] = {
  Anihortes, // First layout is the default layout on startup
  NumLayouts // signals the last item, may not be omitted
};

#endif // config_def_h_INCLUDED
