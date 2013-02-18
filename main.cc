#include <tap.h>

// Return the current time in MJDHM format
static dword getCurrentTime(void)
{
    word mjd;
    byte hour, min, sec;
    TAP_GetTime(&mjd, &hour, &min, &sec);
    return (mjd << 16) | (hour << 8) | min;
}

// Run through the entire EPG and rebias current program for all channels
static void updateAllEPG(dword now)
{
    // Get total number of TV channels
    int num_tv, num_radio;
    TAP_Channel_GetTotalNum(&num_tv, &num_radio);

    // For each TV channel ..
    int chan;
    for (chan = 0; chan < num_tv; chan++) {
	int num_events;
	TYPE_TapEvent *epg = TAP_GetEvent(SVC_TYPE_Tv, chan, &num_events);

	// Ensure we got data for this channel
	if (!epg)
	    continue;

	TYPE_TapEvent *event = epg;
	TYPE_TapEvent *event_end = &epg[num_events];
	TYPE_TapEvent *prev = NULL;

	// For each program on this channel ..
	for (; event < event_end; event++) {
	    if (event->endTime > now) {

		// Update the previous program if still marked as current
		if (prev && prev->runningStatus != 1)
		    TAP_EPG_UpdateEvent(SVC_TYPE_Tv, chan, prev->evtId, 1);

		// Update currently running program
		if (event->startTime <= now && event->runningStatus != 4)
		    TAP_EPG_UpdateEvent(SVC_TYPE_Tv, chan, event->evtId, 4);

		// No need to process future events for this channel
		break;
	    }

	    // Delete any program older than current and previous
	    if (prev)
		TAP_EPG_DeleteEvent(SVC_TYPE_Tv, chan, prev->evtId);

	    prev = event;
	}

	TAP_MemFree(epg);
    }
}

// TMS TAP event handler
dword TAP_EventHandler(word event, dword param1, dword /*param2*/)
{
    static dword last;

    // Only action idle and key events
    if (event != EVT_IDLE && event != EVT_KEY)
	return param1;

    // Get current date+time
    dword now = getCurrentTime();

    // Update EPG at system startup and then every minute boundary
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
