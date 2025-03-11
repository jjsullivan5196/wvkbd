#ifndef config_def_h_INCLUDED
#define config_def_h_INCLUDED

#define DEFAULT_FONT "Sans 14"
static const int transparency = 255;

struct clr_scheme schemes[] = {
{
  /* colors */
  .bg = {.bgra = {54, 42, 40, transparency}},
  .fg = {.bgra = {164, 114, 98, transparency}},
  .high = {.bgra = {100, 100, 100, transparency}},
  .swipe = {.bgra = {100, 255, 100, 64}},
  .text = {.color = UINT32_MAX},
  .font = DEFAULT_FONT,
},
{
  /* colors */
  .bg = {.bgra = {54, 42, 40, transparency}},
  .fg = {.bgra = {90, 71, 68, transparency}},
  .high = {.bgra = {100, 100, 100, transparency}},
  .swipe = {.bgra = {100, 255, 100, 64}},
  .text = {.color = UINT32_MAX},
  .font = DEFAULT_FONT,
}
};

/* layers is an ordered list of layouts, used to cycle through */
static enum layout_id layers[] = {
  Full, // First layout is the default layout on startup
  Special, 
  NumLayouts // signals the last item, may not be omitted
};

/* layers is an ordered list of layouts, used to cycle through */
static enum layout_id landscape_layers[] = {
  Landscape, // First layout is the default layout on startup
  LandscapeSpecial,
  NumLayouts // signals the last item, may not be omitted
};

#endif // config_def_h_INCLUDED
