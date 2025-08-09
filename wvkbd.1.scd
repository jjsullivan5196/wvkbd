wvkbd(1)

# NAME

wvkbd - on-screen virtual keyboard for wayland compositors using wlroots

# SYNOPSIS

wvkbd-mobintl [OPTIONS]...
wvkbd-deskintl [OPTIONS]...

*NOTE*: Your binary may have a different suffix depending on which layout you compiled.

# DESCRIPTION

This project aims to deliver a minimal but practically usable implementation of
a wlroots on-screen keyboard in legible C. This will _only_ be a keyboard, not
a feedback buzzer, led blinker, or anything that requires more than what's
needed to input text quickly. The end product should be a static codebase that
can be patched to add new features.

## OPTIONS

*-D*
	enable debug mode.

*-o*
	print pressed keys to standard output.

*-O*
	print intersected keys to standard output.

*-l* _layers_
	comma separated list of layers in vertical/portrait mode.

*--landscape-layers* _layers_
	comma separated list of layers used in horizontal/landscape mode.

*--list-layers*
	prints a list of all available layers.

*-H* _pixels_
	Height of the keyboard in pixels, for vertical/portrait mode.

*-L* _pixels_
	Height of the keyboard in pixels, for horizontal/landscape mode

*--fn* _font_
	set font and size (e.g. DejaVu Sans 20)

*--hidden*
	Start hidden (send SIGUSR2 to show).

*--non-exclusive*
	Allow keyboard to overlap existing windows, do not request an
	exclusive zone from the compositor.

*--alpha* _int_
	Set alpha value (i.e. transparency) for all colors [0-255]
	
*--bg* _rrggbb|aa_ 
	Set color of background

*--fg* _rrggbb|aa_       
	Set color of keys

*--fg-sp* _rrggbb|aa_       
	Set color of special keys

*--press* _rrggbb|aa_       
	Set color of pressed keys

*--press-sp* _rrggbb|aa_       
	Set color of pressed special keys

*--swipe* _rrggbb|aa_       
	Set color of swiped keys

*--swipe-sp* _rrggbb|aa_       
	Set color of swiped special keys

*--text* _rrggbb|aa_       
	Set color text on keys

*--text-sp* _rrggbb|aa_       
	Set color text on special keys

*--version*
	Print version information

*-h*, *--help*
	Print usage help 


# SIGNALS

You can send signals to wvkbd to hide/show it (e.g. using _kill_(1) with _-s_):

*SIGUSR1*
	Hide the keyboard.

*SIGUSR2*
	Show the keyboard

*SIGRTMIN*
	Toggle visibility

# COMPOSE BUTTON

The default mobile international layout features a Compose button (*Cmp*)
which, when combined with another key, opens up a layout that offers variants
for that key. This is similar to functionality that other keyboards implemented
using a *long press* (wvkbd has no such notion, holding a key will repeat
it like on a physical keyboard).

For example, press Cmp + a to access variants with diacritics like á,à,â,ä, etc.. 

Most layouts also feature the following that are less obvious:

- Press Cmp and . to access more punctuation
- Press Cmp and - or , to access 'mathematical' symbols (+,-,=,etc)
- Press Cmp and ' or 0 or 9 to access more brackets and quotes
- Press Cmp and q to access emojis

Last, but not least, pressing Cmp + space or Cmp + ⌨  or Cmp + Abc opens up an
index that allows you to immediately jump to any layout by name, even layouts
not explicitly added to your layers on startup.

# AUTHORS

Created by John Sullivan <jsullivan@csumb.edu>, maintained by the Sxmo project
<https://sxmo.org> in collaboration with other open source contributors. For
more information about wvkbd development, see <https://git.sr.ht/~proycon/wvkbd>
or <https://github.com/jjsullivan5196/wvkbd>.
