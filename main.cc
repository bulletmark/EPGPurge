#include <tap.h>
#include "version.h"

// Return the current time in MJDHM format
static dword getCurrentTime(void)
{
    word mjd;
    byte hour, min, sec;
    TAP_GetTime(&mjd, &hour, &min, &sec);
    return (mjd << 16) | (hour << 8) | min;
}

// Rebias current program for given channel
static void updateChannelEPG(dword now, int chan)
{
    int num_events;
    TYPE_TapEvent *epg = TAP_GetEvent(SVC_TYPE_Tv, chan, &num_events);

    // Ensure we got the data
    if (!epg)
	return;

    TYPE_TapEvent *event = epg;
    TYPE_TapEvent *event_end = &epg[num_events];
    TYPE_TapEvent *prev = NULL;

    // For each program on this channel ..
    for (; event < event_end; event++) {
	if (event->endTime <= now) {

	    // Delete any program older than current and previous
	    if (prev)
		TAP_EPG_DeleteEvent(SVC_TYPE_Tv, chan, prev->evtId);

	    prev = event;
	}
	else {

	    // Update the previous program if still marked as current
	    if (prev && prev->runningStatus != 1)
		TAP_EPG_UpdateEvent(SVC_TYPE_Tv, chan, prev->evtId, 1);

	    // Update currently running program
	    if (event->startTime <= now && event->runningStatus != 4)
		TAP_EPG_UpdateEvent(SVC_TYPE_Tv, chan, event->evtId, 4);

	    // No need to process future events
	    break;
	}
    }

    TAP_MemFree(epg);
}

// Run through the entire EPG and rebias current program for all channels
static void updateAllEPG(dword now)
{
    // Get total number of TV channels
    int num_tv, num_radio;
    TAP_Channel_GetTotalNum(&num_tv, &num_radio);

    // For each TV channel ..
    int chan;
    for (chan = 0; chan < num_tv; chan++)
	updateChannelEPG(now, chan);
}

// TMS TAP event handler
dword TAP_EventHandler(word event, dword param1, dword /*param2*/)
{
    static dword last;
    static int last_chan;

    dword state, subState;
    TAP_GetState(&state, &subState);

    // Just return if not in normal view mode
    if (state != STATE_Normal || subState != SUBSTATE_Normal)
	return param1;

    TYPE_PlayInfo info;
    TAP_Hdd_GetPlayInfo(&info);

    // Just return if not in normal TV playback mode
    if (info.playMode != PLAYMODE_None || info.svcType != SVC_TYPE_Tv)
	return param1;

    int type, chan;
    TAP_Channel_GetCurrent(&type, &chan);

    // Force update single new channel if channel change or return from
    // playback
    if (last_chan != chan || (event == EVT_RBACK && param1 == 1)) {
	last_chan = chan;
	updateChannelEPG(getCurrentTime(), chan);
	return param1;
    }

    // Force update immediately for any key which may cause display of EPG
    // data
    if (event == EVT_KEY) {
	switch (param1) {
	// case RKEY_F4: //P+
	// case RKEY_F5: //P-
	case RKEY_Right:
	case RKEY_Left:
	case RKEY_Up:
	case RKEY_Down:
	case RKEY_Info:
	case RKEY_Guide:
	    break;
	default:
	    return param1;
	}

	last = 0;
    }

    // Get current date+time
    dword now = getCurrentTime();

    // Update EPG at every minute boundary or if forced by key
    if (last != now) {
	updateAllEPG(now);
	last = now;
    }

    return param1;
}

// TMS Main
int TAP_Main(void)
{
    // TSR
    return 1;
}
