# wvkbd - On-screen keyboard for wlroots that sucks less

<img src="https://raw.githubusercontent.com/proycon/wvkbd/master/contrib/wvkbd-mobintl.jpg" width=300 /> <img src="https://raw.githubusercontent.com/proycon/wvkbd/master/contrib/wvkbd-mobintl-cyrillic.jpg" width=300 />

This project aims to deliver a minimal but practically usable implementation of a wlroots on-screen
keyboard in legible C. This will **only** be a keyboard, not a feedback buzzer,
led blinker, or anything that requires more than what's needed to input text
quickly. The end product should be a static codebase that can be patched to add
new features.

## Features

 - Typing, modifier locking, layout switching
 - Positive visual feedback on key presses
 - Custom layouts and underlying keymaps
 - On-the-fly layout and keymap switching
 - Custom color schemes
 - Proper font drawing
 - Intuitive layouts
 - International layouts (cyrillic, arabic)
 - Support for 'Copy' keys which are not on the keymap
 - Emoji support
 - Compose key for character variants (e.g. diacritics)
 - Show/hide keyboard on signals (SIGUSR1 = hide, SIGUSR2 = show)
 - Automatic portrait/landscape detection and subsequent layout switching


<img src="https://raw.githubusercontent.com/proycon/wvkbd/master/contrib/wvkbd-mobintl-landscape.jpg" width=640 />

There are some areas that still need work:

 - Make sure the virtual input method in wayland is working as best as it can
 - Support for input method protocol in wayland, ability to respond to text
   fields

## Install

You'll need the following developer packages

 - pangocairo
 - wayland-client
 - xkbcommon

Make any customizations you would like in `config.def.h` and run `make`

The default set of layouts is called `mobintl` *(mobile international)*, which groups various layouts aimed at mobile devices
and also attempts to accommodate various international users. The resulting binary is called `wvkbd-mobintl`.

You can, however, define your own layouts by copying and and modifying `layout.mobintl.h` and `keymap.mobintl.h`
(replace `mobintl` for something like `yourlayout`). Then make your layout set using `make LAYOUT=yourlayout`, and
the resulting binary will be `wvkbd-yourlayout`

## Usage

Run `wvkbd-mobintl` (or the binary for your custom layout set).

You can switch between the layouts/layers of the keyboard by pressing the Abc/Sym key in the bottom-left. If you only
want a subset of the available layers, you can define which wants you want and in what order you want to cycle through
them using the `-l` parameter. This takes takes a ordered comma separated list of
layout names that are defined in your layout set.

The keyboard can be hidden by sending it a `SIGUSR1` signal and shown again by sending it `SIGUSR2`. This saves some
start up time and may be appropriate in some low-resource environments.

Wvkbd has an output mode `-o` that will echo its output to standard output. This facility can be used if users want
audio/haptic feedback, a feature explicitly out of scope for wvkbd. To achieve this, simply pipe wvkbd's output through the external tool
[clickclack](https://git.sr.ht/~proycon/clickclack):

`$ wvkbd-mobintl -l simple,special,emoji -o | clickclack -V -f keypress.wav`

Another output mode, `-O` will let the keyboard output keys which are swiped over. It can be used by an external program, such as [swipeGuess](https://git.sr.ht/~earboxer/swipeGuess) to get swipe-typing support.

`$ wvkbd-mobintl -O | swipeGuess.sh words.txt | completelyTypeWord.sh`


## Contribute

Any contributions are welcome, please tell me what I did wrong in issues or
PRs. I could also use some nice branding if that tickles your fancy.

For code contributions, all I ask for now is you run `make format` (requires
`clang-format`) before opening a PR and include as much relevant detail as
possible.

## Related projects

* [clickclack](https://git.sr.ht/~proycon/clickclack) - Audio/haptic feedback (standalone)
* [swipeGuess](https://git.sr.ht/~earboxer/swipeGuess) - Word-completion for swipe-typing
* [Sxmo](https://sxmo.org) - A hackable mobile interface environment for Linux phones that adopted wvkbd as its keyboard
* [svkbd](https://tools.suckless.org/x/svkbd/) - A similar project as wvkbd but for X11 rather than Wayland
* [squeekboard](https://gitlab.gnome.org/World/Phosh/squeekboard) - The virtual keyboard developed for the Librem5 (used
	by Phosh)
