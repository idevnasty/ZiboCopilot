#if APL
#if defined(__MACH__)
#include <Carbon/Carbon.h>
#endif
#endif
#include "XPLMMenus.h"
#include <cstdio>
#include <string>
#if IBM
#include <windows.h>
#endif
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMPlugin.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPLMScenery.h"
#include "XPLMDataAccess.h"
#include <cstdlib>

using namespace std;

#include "DataRefList.h"
#include "EngineStartProcedure.h"

int EngineStartProcedure::engineStartProcedure(int stage)
{
	if (stage == 0) {
		XPLMCommandOnce(XPLMFindCommand("laminar/B738/toggle_switch/eng_start_source_right"));
		XPLMCommandOnce(XPLMFindCommand("laminar/B738/toggle_switch/eng_start_source_right"));
		return 1;
	}
	else if (stage == 1) {
		XPLMCommandOnce(XPLMFindCommand("laminar/B738/rotary/eng2_start_grd"));
		return 1;
	}
	else if (stage == 2) {
		if (XPLMGetDatad(XPLMFindDataRef(DataRefList::dataRefList[26])) > 19.0) {
			XPLMCommandOnce(XPLMFindCommand("laminar/B738/engine/mixture2_idle"));
			return 1;
		}
	}
	else if (stage == 3) {
		if (XPLMGetDatad(XPLMFindDataRef(DataRefList::dataRefList[26])) > 55.0) {
			XPLMCommandOnce(XPLMFindCommand("laminar/B738/toggle_switch/eng_start_source_left"));
			XPLMCommandOnce(XPLMFindCommand("laminar/B738/toggle_switch/eng_start_source_left"));
			return 1;
		}
	}
	else if (stage == 4) {
		XPLMCommandOnce(XPLMFindCommand("laminar/B738/rotary/eng1_start_grd"));
		return 1;
	}

	else if (stage == 5) {
		if (XPLMGetDatad(XPLMFindDataRef(DataRefList::dataRefList[27])) > 19.0) {
			XPLMCommandOnce(XPLMFindCommand("laminar/B738/engine/mixture1_idle"));
			return 1;
		}
	}
	else if (stage == 6) {
		if (XPLMGetDatad(XPLMFindDataRef(DataRefList::dataRefList[26])) > 64.0 && XPLMGetDatad(XPLMFindDataRef(DataRefList::dataRefList[27])) > 64.0) {
			XPLMSpeakString("Engine Start Procedures Completed");
			return 2;
		}
	}

	return 0;
}
