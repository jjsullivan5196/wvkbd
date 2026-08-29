# Unreleased

[Emiel Kollof]
* Add `terminal` layout set: compact 60%-style layout for small UMPC/tablet touchscreens (GPD Pocket), traditional ISO-like arrangement with modifiers, arrows and shifted punctuation on the primary layer; identical primary layer for portrait and landscape

# v0.20 - 2026-07-24

This release implements automatic focus-based visibility toggling, thanks to Christian Duerr. Start wvkbd with the `--auto` parameter to opt-in for this feature (it is not the default yet).
It also implements the ability to define different text colours for pressed and swiped keys (thanks to Pachulke and to Christoph Butz for fixes).

There are also new flags that are using for users of wvkbd in privacy sensitive use cases (e.g. for as a lock screens): `--no-popup`, `--no-highlight`, `--supress-feedback`.

[Christoph Butz]
* Added flags to suppress key-press feedback
* Fixed issue on deskintl layout with text_press and text_swipe scheme colors

[Christian Duerr]
* Fix CLI option alignment
* Add automatic focus-based visibility toggling (pass the `--auto` parameter)

[Justin Lovinger]
* Fix keys with multi-character labels causing issues when swiping

[Pachulke]
* Add arguments to define different text color for pressed and swiped keys

[Maarten van Gompel]
* Documentation updates and added a changelog

# v0.19.4 - 2026-02-06

This fixes a regression introduced in v0.19 where composed keys (diacritics etc) no longer produced uppercase letters when Shift was pressed ([#115](https://github.com/jjsullivan5196/wvkbd/issues/115))

v0.19.3 was premature



# v0.19.3 - 2026-02-06

This fixes a regression introduced in v0.19 where composed keys (diacritics etc) no longer produced uppercase letters when Shift was pressed ([#115](https://github.com/jjsullivan5196/wvkbd/issues/115))



# v0.19.2 - 2026-01-22

Minor build fix only:

[Willow Barraco]
* Makefile: do not rebuild on install 



# v0.19.1 - 2026-01-22

[Willow Barraco]

* Improve the makefile to simplify building multiple layouts




# v0.19 - 2026-01-21

[Willow Barraco]
* Overhaul the output available dimension detection
* Send press and release events for modifier keys
 
[Armando DiCianno]

* Fixes for complying with wlr-layer-shell-unstable-v1 and preventing crashes
    * update touch/pointer event checks in order to sequence drawing to a surface before configured.
    * cancel pending frame callback before destroying surface and ignore old surfaces



# v0.18 - 2025-08-31

[Jun Aruga] ([@junaruga](https://github.com/junaruga))
* Added deskintl (desktop international) layout ([#103](https://github.com/jjsullivan5196/wvkbd/pull/103), https://github.com/jjsullivan5196/wvkbd/pull/67). This layout features a more full-sized keyboard modelled after hardware keyboards. It is based upon earlier work by Jordan Johnston ([@nine7nine](https://github.com/nine7nine)). 
   * The layout that is built is determined at compile time: to opt for this layout instead of the default mobile international (mobintl) layout, run `make LAYOUT=deskintl` instead of plain `make`.

[Maarten van Gompel]
* Added cyrillic layer (russian ЙЦУКЕН) for new desktop layout
* Added `SHIFT_SPACE_IS_TAB` as a compile-time parameter
* Fix landscape appearance bug in multi-monitor setup
* Added --non-exclusive parameter to skip requesting exclusive zone from compositor ([#102](https://github.com/jjsullivan5196/wvkbd/issues/102) , [#93](https://github.com/jjsullivan5196/wvkbd/issues/93))
 
## Note for distribution packagers

Packagers are recommended to provide a package `wvkbd` with subpackages `wvkbd-mobintl` and `wvkbd-deskintl`, allowing users to install both layouts at once or a specific layout independently.


# v0.17 - 2025-06-22

[Willow Barraco]
* Drawing performance improvements:
    * Implemented double buffering
    * Tie the daamge tracking to the drawing methods

[Viacheslav Gagara]
* Added missing cyrillic letter from the Ukrainian alphabet (ґ)

[Zach DeCook]
* Namespace change, now properly identifies as "wvkbd"



# v0.16 - 2024-11-25

[Maarten van Gompel]
* Use overlay mode so keyboard is visible over full-screen windows
* Fixes for capslock display
* Documentation update

[Willow Barraco]
* Fixes for visual delay on startup.
    * Skip the first resize when landscaped while starting. 
    * Fix the initial output geometry guessing 

[Frank Oltmanns]
* Restore Cairo also when rounding. This fixes some visual glitches.

[Jami Kettunen]
* Accommodate cross-build pkg-config

[Paul Rimmer]
* Make all modifiers except capslock one-shot and redraw keyboard when capslock is pressed



# v0.15 - 2024-05-04

## Features

[Amir Dahan]
* Add basic rounding

[Maarten van Gompel]
* Added -R parameter to configure rounding at runtime

## Bugfixes

[mojyack]
* Check if popup surf configured on callbacks

[rdbo]
* fixed malfunctioning theme at random



# v0.14.4 - 2024-03-03

[Willow Barraco]
* fix fractional scalled buffer missing one pixel
* minimise visual glitches when starting
* re-open the keyboard on the same output it was

[Zach DeCook]
* event loop: exit if the wayland socket disappears (prevents infinite loop when your compositor crashes)

[Maarten van Gompel]
* implemented a stub wl_surface_leave (addresses [#52](https://github.com/jjsullivan5196/wvkbd/issues/52))



# v0.14.3 - 2023-11-04

Bugfix release due to regression in yesterday's v0.14.2: [#50](https://github.com/jjsullivan5196/wvkbd/issues/50)

[mojyack]
* do not refresh on wl_surface_enter

[Maarten van Gompel]
* fixup: re-add flip_landscape() to wl_surface_enter (needed for landscape detection)



# v0.14.2 - 2023-11-03

[Frank Oltmanns]
* Add support for multiple schemes
* Make font selection scheme specific

[mojyack]

Bugfixes and code improvement:
* avoid using "wl_output" literal
* handle screen resize and redraw in layer_surface_configure
* resize keyboard only when entered to different output
* optimize output iteration
* fix hyprland crashes when creating a surface multiple times



# v0.14.1 - 2023-09-25

Bugfix release:

[Einar Arnason]
* Add missing include (failed to compile only on some systems) 

[Willow Barraco]
* Regression fix: keyboard didn't re-show on SIGUSR2 anymore

[Maarten van Gompel]
* set preferred scale default to 1 (fixes isssue [#43](https://github.com/jjsullivan5196/wvkbd/issues/43)) (thanks to [@flexibeast](https://github.com/flexibeast))



# v0.14 - 2023-09-16

[Maarten van Gompel]
* Revised various layouts
    * the `full` layout squeezes less keys in a row
    * the `landscape` layout was revised
    * added a dedicated symbols layout for landscape mode: `landscapespecial`
    * the tiny keys on the `simple` layout are removed
    * more consistency between layouts (relative row placement, key locations)
    * changed default font to Sans instead of Mono
    * changed some icons
* More consistent compose behaviour across layouts
* Compose layouts can be dismissed with a simple click/tap now
* Scroll wheel switches layers (as opposed to crashing as it did before)
* Added a layout index to quickly switch layouts (activate with cmp + space or cmp + keyboard)
     * Reduced the number of layers enabled by default, no need to cycle through all available layouts anymore. Users are encouraged to set their preferred layers via parameters or environment variables.
* Added hebrew layout (in collaboration with Shimon Jeduhah)
* Documentation update

[Mojyack]
* Support multiple outputs; start on currently active output
* fixed wayland seat_handle_capabilities

[Willow Barraco]
* Support for wayland fractional-scale-v1
* Crop text outside of keys
* Added a popup to display pressed keys
* Add --alpha parameter to configure transparency; disabled transparency by default
* Cleanup in some layouts
* Added non-breaking space (espace fine insécable) to compose layout for punctuation
* Updated wayland protocols
* Code formatting covnentions

[Zach De Cook]
* Added comma to simple layout
* Added specialpad layout
* Bugfix in layer switching



# v0.12 - 2022-11-16

[Stacy Harper]
* Fixed dialer numeric pound key

[Zach DeCook]
* Cleanup




# v0.11 - 2022-10-07

[Patrick Steinhardt]

Allow overriding layers in landscape mode



# v0.10 - 2022-08-25

[Emmanuel LE TRONG]
* Added a --list-layers option

[Ghassan Alduraibi]
* args to customize keyboard colors (takes rrggbb or rrggbbaa values) add: function to set keyboard colors from rrggbb or rrggbbaa values 
* fix: handle empty font arg



# v0.9 - 2022-07-26

[Ghassan Alduraibi]
* fix: arabic key layout

[Maarten van Gompel]
* allow modifiers with next layer button to switch to first/previous layer

[Zach DeCook]
* shift key: change icon when shifted
* border: display around all sides
* layout: Implement spacing better
* Keyboard: Avoid unnecessary resizes

[ArenM]
* only commit surface when it changed
* use signalfd instead of signal
* call show instead of duplicating in startshidden check
* reset layer index on rotation
* call kbd_resize when showing
* Use output dimensions to detect landscape mode

[Peter John Hartman]
* Add + sign in dialer.




# v0.8.2 - 2022-06-17

Minor bugfix release, man page installation sometimes failed, no functional changes


# v0.8.1 - 2022-06-17

* Added a persian layout ([#26](https://github.com/jjsullivan5196/wvkbd/issues/26))
* Correct version output ([#17](https://github.com/jjsullivan5196/wvkbd/issues/17) [#18](https://github.com/jjsullivan5196/wvkbd/issues/18) [#25](https://github.com/jjsullivan5196/wvkbd/issues/25))
* Added a greek layout ([#24](https://github.com/jjsullivan5196/wvkbd/issues/24))
* Fixed latin keyboard for hungarian diacritics ([#21](https://github.com/jjsullivan5196/wvkbd/issues/21))
* Some changes on the simple layout ([#15](https://github.com/jjsullivan5196/wvkbd/issues/15))
* Added a real-time signal for toggling keyboard visibility ([#16](https://github.com/jjsullivan5196/wvkbd/issues/16))
* Compilation fixes: Include LDFLAGS from environment ([#27](https://github.com/jjsullivan5196/wvkbd/issues/27))
* Prevent crash on sigpipe ([#20](https://github.com/jjsullivan5196/wvkbd/issues/20))
* Added manpage ([#22](https://github.com/jjsullivan5196/wvkbd/issues/22))
* Fixed segfault when starting hidden ([#23](https://github.com/jjsullivan5196/wvkbd/issues/23))



# v0.7 - 2022-01-10

- Fixes Issue [#8](https://github.com/jjsullivan5196/wvkbd/issues/8) and resolves [#12](https://github.com/jjsullivan5196/wvkbd/issues/12), the reset logic for compose keys will now be reflected in the currently shown layout, with modifiers updated accordingly



# v0.6 - 2022-01-10

- [@bolbishvili](https://github.com/bolbishvili) added Georgian layout
- Merged [@earboxer](https://github.com/earboxer)'s swipe-type patch
- [@proycon](https://github.com/proycon) fixed the 'i' diacritics in the Turkish layout



# v0.5 - 2021-12-01

- Add layer switch key to dialer layout


# v0.4 - 2021-11-16

- Added a dialer/DTMF keypad layout


# v0.3 - 2021-11-01

- Fixed some mappings in the provided emoji layer
- Added a navigation layer for keyboard arrow keys/page up&down

# v0.2 - 2021-10-20

