# wvkbd - On-screen keyboard for wlroots that sucks less

<img src="https://raw.githubusercontent.com/jjsullivan5196/wvkbd/master/contrib/grab.png" width=350 />

This project aims to deliver a minimum implementation of a wlroots on-screen
keyboard in legible C. This will **only** be a keyboard, not a feedback buzzer,
led blinker, or anything that requires more than what's needed to input text
quickly. The end product should be a static codebase that can be patched to add
new features.

At the moment work still needs to be done to make the keyboard fully functional
and determine a minimum feature set. As of now, the following works:

## Features

 - Typing, modifier locking, layout switching
 - Positive visual feedback on key presses
 - Custom layouts
 - Custom color schemes

There are some relatively critical areas that still need work:

 - Proper drawing of font glyphs/fontconfig alternatives (unknown glyphs for the
   configured font are not drawn)
 - Make sure the virtual input method in wayland is working as best as it can
 - Customize keyboard window docking
 - Nicer layout drawing/padding
 - Determine if some dependencies are really needed (fontconfig is VERY
   annoying, and wld may not be strictly necessary)

And some nice to haves:

 - Daemon mode (hide/show keyboard on signals)
 - Support for input method protocol in wayland, ability to respond to text
   fields
 - Alt input modes for things like emojis
 - Typical international layouts in the repository

Of course there's probably some more I'm forgetting, everything here is very
much early WIP so things will change very quickly.

## Install

You'll need the following developer packages

 - fontconfig
 - wayland-client
 - xkbcommon
 - pixman

After cloning this repo, run `git submodules update --init --recursive`

Make any customizations you would like in `config.h` and run `make`, then `./wvkbd`

## Contribute

Any contributions are welcome, please tell me what I did wrong in issues or
PRs. I could also use some nice branding if that tickles your fancy.

For code contributions, all I ask for now is you run `make format` (requires
`clang-format`) before opening a PR and include as much relevant detail as
possible.
