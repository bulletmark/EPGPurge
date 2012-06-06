## EPGPurge

A small TAP program to purge old EPG entries on a Topfield TMS PVR.

This fixes the following problems in the TMS PVR:

1. After each minute boundary, the tap purges the EPG of all program
   entries older than the current + previous program on each channel and
   also ensures the current program is updated to match the current time
   of day. This stops old program info being displayed on the info
   display, and old program info being stored in recordings.

2. Stops the guide display from jumping back a large period as you move
   the cursor up or down the display.

To install the TAP, upload it to the `ProgramFiles/AutoStart` folder on
your PVR using ftp. There is no configuration or user interaction
required for this tap. It merely runs in the background and fixes these
issues. Ensure you delete the FixCurrent tap if you had it previously
installed, as this tap replaces it.

Get the latest built TAP at
<http://markb.dreamhosters.com/EPGPurge/EPGPurge.zip>. See/get the
source code at <http://github.com/bulletmark/EPGPurge>. I developed this
on my Australian TF7100HDPlus model PVR but it should work on at least
all other Australian TMS models, i.e. TRF-2400, TRF-2460, TRF7170, and
TRF7160. I have tested the app using both ICE and FTA EPG data.

## Author

Mark Blakeney, <mailto:markb@berlios.de>. I am user "markb" on the
[Topfield Australia forums](http://www.itopfield.com.au/forum/).
See my other TAPs [here](http://markb.dreamhosters.com/web/).

## License

Copyright (C) 2012 Mark Blakeney. This program is distributed under the
terms of the GNU General Public License.

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or any later
version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License at <http://www.gnu.org/licenses/> for more details.

## Change History

v1.0: Original version for guide display. This original version by
victorhi hosted at <http://sourceforge.net/projects/topfieldtapmisc/>.

v1.1-MGB: Imported to github and added the following:

1. Also forces EPG purge after returning from playback or timeslip to
   realtime TV.

2. Made code slightly more efficient and made some minor cleanups.

3. Fixed typo bug in SUBSTATE check.

4. Fixed end time range bug. Programs start on the start time and end
   just before the end time, i.e. range = [start, end).

V2.0 - 2011-12-22: Rewrote and renamed my version EPGPurge to add the
following enhancements:

1. Deletes old EPG entries rather than marks them as expired. There is
   no need for old EPG data anyhow and deleting them subsequently makes
   the processing much more efficient as we then only have to process
   typically 2 events per channel each run (previous + current) rather
   than potentially hundreds of old events.

2. The FixCurrent tap converted each programs start and end date+times
   into an internal time number biased against an arbitary epoch.
   However this is unnecessary as we only need to compare date+times
   against current date+time and since the MJDHM format embeds the date
   and time in significant digit order that means we can compare them
   directly and thus save significant CPU overhead avoiding the time
   conversions.

3. Runs just after each minute boundary to purge and adjust the EPG.
   Since the TAP is now much more efficient (running typically only
   2 iterations per channel as per above comment) we can run it each
   minute and catch all "edge" conditions where EPG data is otherwise
   sometimes seen to be out of date.

V2.1 - 2012-04-13: Add check for no EPG data.

V2.2 - 2012-04-29: Always runs after each minute boundary now.
  Runs in all modes, to always ensure that the EPG is updated. Apart from the
  other problems addressed before this version, this should also
  now ensure that recordings capture the correct program info data.

v2.2.1 - 2012-06-20: Only do EPG purge on IDLE and KEY events.
  I do not know of any problems before this but FireBird recommends
  avoiding processing on the other TAP events so may as well be sure.

<!-- vim: se ai syn=markdown: -->
