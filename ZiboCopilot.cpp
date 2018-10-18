//Created by Yohan Joo 18-06-21
/*
	IF YOU READ THIS. YOUR EYES WILL BLEED
	THIS IS HORRIBLE PROGRAMMING PRACTICE AND I KNOW IT.
	I SUCK AT PROGRAMMING HELP.

*/

#if APL
#if defined(__MACH__)
#include <Carbon/Carbon.h>
#endif
#endif
#include "XPLMMenus.h"
#include <cstdio>
#include <cstring>
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

#ifndef XPLM300
#error This is made to be compiled against the XPLM300 SDK
#endif

#include "SubHandler.h"
#include "Procedures/DataRefList.h"
SubHandler subHandler;
XPLMMenuID g_menu_id;
int g_menu_container_idx;
int current_procedure = 0;
char DataRefList::dataRefList[100][255] = {
	"laminar/B738/toggle_switch/irs_left",						//IRS LEFT 0
	"laminar/B738/toggle_switch/irs_right",						//IRS RIGHT 1
	"sim/cockpit/switches/yaw_damper_on",						//IS YAW DAMPER ON? 2
	"laminar/B738/toggle_switch/vhf_nav_source",				//VHF NAV SOURCE 3
	"laminar/B738/toggle_switch/irs_source",					//IRS SOURCE 4
	"laminar/B738/toggle_switch/fmc_source",					//FMC SOURCE 5
	"laminar/B738/fuel/fuel_tank_pos_lft1",						//FUEL PUMP 1 6
	"laminar/B738/fuel/fuel_tank_pos_lft2",						//FUEL PUMP 2 7
	"laminar/B738/fuel/fuel_tank_pos_rgt1",						//FUEL PUMP 3 8
	"laminar/B738/fuel/fuel_tank_pos_rgt2",						//FUEL PUMP 4 9
	"laminar/B738/toggle_switch/cab_util_pos",					//CAB UTIL 10
	"laminar/B738/toggle_switch/ife_pass_seat_pos",				//IFE PASS SEAT 11
	"laminar/B738/toggle_switch/emer_exit_lights",				//EMERGENCY EXIT LIGHTS 12
	"laminar/B738/toggle_switch/seatbelt_sign_pos",				//SEATBELT SIGNS 13
	"laminar/B738/ice/window_heat_l_side_pos",					//CHECK WINDOW HEAT 14
	"laminar/B738/toggle_switch/electric_hydro_pumps1_pos",		//ELEC HYD PUMPS LEFT 15
	"laminar/B738/toggle_switch/electric_hydro_pumps2_pos",		//ELEC HYD PUMPS RIGHT 16
	"laminar/B738/air/trim_air_pos",							//TRIM AIR 17
	"laminar/B738/toggle_switch/bleed_air_1_pos",				//ENGINE BLEED 1 18
	"laminar/B738/toggle_switch/bleed_air_2_pos",				//ENGINE BLEED 2 19
	"laminar/B738/toggle_switch/bleed_air_apu_pos",				//APU BLEED 20
	"sim/cockpit/electrical/beacon_lights_on",					//ANTI COLLISION LIGHTS 21
	"laminar/B738/autobrake/autobrake_RTO_arm",					//RTO arm 22
	"laminar/B738/toggle_switch/capt_probes_pos",				//PROBE HEAT L 23
	"laminar/B738/toggle_switch/fo_probes_pos",					//PROBE HEAT R 24
	"laminar/B738/electrical/apu_bus_enable",					//CHECK APU BUS LIT 25
	"laminar/B738/engine/indicators/N2_percent_2",              //CHECK ENGINE 2 26
	"laminar/B738/engine/indicators/N2_percent_1",              //CHECK ENGINE 1 27
	""
};


//Commands
XPLMCommandRef cmdpowerUpProcedures = nullptr;
XPLMCommandRef cmdpreflightProcedures = nullptr;
XPLMCommandRef cmdengineStartProcedures = nullptr;
XPLMCommandRef cmdbeforeTaxiProcedures = nullptr;
XPLMCommandRef cmdbeforeTakeoffProcedures = nullptr;
XPLMCommandRef cmdcleanUpProcedures = nullptr;
XPLMCommandRef cmdshutdownProcedures = nullptr;
XPLMCommandRef cmdnextStep = nullptr;


//-------------------------------------------------------- Initiate Functions ------------------------------------//
static float MyFlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void * inRefcon);
void startFunction(SubHandler::Procedures procedure);
void doNextProcedure();
bool canStartNewProcedure();
int funcpowerUpProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
int funcpreflightProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
int funcengineStartProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
int funcbeforeTaxiProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
int funcbeforeTakeoffProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
int funccleanUpProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
int funcshutdownProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
int funcnextProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon);
void menu_handler(void *, void *);


//-------------------------------------------------------- START OF PLUGIN --------------------------------------//
PLUGIN_API int XPluginStart(char * outName, char * outSig, char * outDesc)
{
	strcpy(outName, "ZiboCopilot");
	strcpy(outSig, "yohan.copilot.zibocopilot");
	strcpy(outDesc, "Copilot (like FS2Crew) for Zibo 737");

	g_menu_container_idx = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "Copilot Menu", 0, 0);
	g_menu_id = XPLMCreateMenu("Copilot Menu", XPLMFindPluginsMenu(), g_menu_container_idx, menu_handler, nullptr);
  
	XPLMAppendMenuItem(g_menu_id, "Powerup Procedure", (void *)"power_up", 1);
	XPLMAppendMenuItem(g_menu_id, "Preflight Procedures", (void *)"preflight", 1);
	XPLMAppendMenuItem(g_menu_id, "Engine Start Procedures", (void *)"engine_start", 1);
	XPLMAppendMenuItem(g_menu_id, "Before Taxi Procedures", (void *)"before_taxi", 1);
	XPLMAppendMenuItem(g_menu_id, "Before Takeoff Procedures", (void *)"before_takeoff", 1);
	XPLMAppendMenuItem(g_menu_id, "Clean Up Procedures", (void *)"clean_up", 1);
	XPLMAppendMenuItem(g_menu_id, "Shutdown Procedures", (void *)"shutdown", 1);
	XPLMAppendMenuSeparator(g_menu_id);
	XPLMAppendMenuItem(g_menu_id, "Next Procedure", (void *)"next_procedure", 1);

	const auto aircraft_menu = XPLMFindAircraftMenu();
	if (aircraft_menu) // This will be nullptrptr unless this plugin was loaded with an aircraft (i.e., it was located in the current aircraft's "plugins" subdirectory)
	{
		XPLMAppendMenuItemWithCommand(aircraft_menu, "Toggle Settings (Command-Based)", XPLMFindCommand("sim/operation/toggle_settings_window"));
	}

	//Commands
	cmdpowerUpProcedures = XPLMCreateCommand("737/Zibocopilot/powerUpProcedure", "Power Up Procedure");
	cmdpreflightProcedures = XPLMCreateCommand("737/Zibocopilot/preflightProcedure", "Preflight Procedure");
	cmdengineStartProcedures = XPLMCreateCommand("737/Zibocopilot/engineStartProcedure", "Engine Start Procedure");
	cmdbeforeTaxiProcedures = XPLMCreateCommand("737/Zibocopilot/beforeTaxiProcedure", "Before Taxi Procedure");
	cmdbeforeTakeoffProcedures = XPLMCreateCommand("737/Zibocopilot/beforeTakeoffProcedure", "Before Takeoff Procedure");
	cmdcleanUpProcedures = XPLMCreateCommand("737/Zibocopilot/cleanUpProcedure", "Clean Up Procedure");
	cmdshutdownProcedures = XPLMCreateCommand("737/Zibocopilot/shutdownProcedure", "Shutdown Procedure");
	cmdnextStep = XPLMCreateCommand("737/Zibocopilot/nextStep", "Next Procedure");
	XPLMRegisterCommandHandler(cmdpowerUpProcedures, funcpowerUpProcedures, 1, nullptr);
	XPLMRegisterCommandHandler(cmdpreflightProcedures, funcpreflightProcedures, 1, nullptr);
	XPLMRegisterCommandHandler(cmdengineStartProcedures, funcengineStartProcedures, 1, nullptr);
	XPLMRegisterCommandHandler(cmdbeforeTaxiProcedures, funcbeforeTaxiProcedures, 1, nullptr);
	XPLMRegisterCommandHandler(cmdbeforeTakeoffProcedures, funcbeforeTakeoffProcedures, 1, nullptr);
	XPLMRegisterCommandHandler(cmdcleanUpProcedures, funccleanUpProcedures, 1, nullptr);
	XPLMRegisterCommandHandler(cmdshutdownProcedures, funcshutdownProcedures, 1, nullptr);
	XPLMRegisterCommandHandler(cmdnextStep, funcnextProcedures, 1, nullptr);

	return 1;
}

//-------------------------------------------------------- DO NOT TOUCH -----------------------------------------//
PLUGIN_API void XPluginStop()
{
	// Since we created this menu, we'll be good citizens and clean it up as well
	XPLMDestroyMenu(g_menu_id);
	// If we were able to add a command to the aircraft menu, it will be automatically removed for us when we're unloaded
}

PLUGIN_API void XPluginDisable() { }
PLUGIN_API int XPluginEnable() { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }
//-------------------------------------------------------- END --------------------------------------------------//


//-------------------------------------------------------- SUB MENU HANDLER -------------------------------------//
void menu_handler(void * in_menu_ref, void * in_item_ref)
{
	if (!canStartNewProcedure())
		return;
	const auto compare_string = static_cast<const char *>(in_item_ref);
	if (!strcmp(compare_string, "power_up"))
	{
		startFunction(SubHandler::power_up);
	}
	else if (!strcmp(compare_string, "preflight"))
	{
		startFunction(SubHandler::pre_flight);
	}
	else if (!strcmp(compare_string, "engine_start"))
	{
		startFunction(SubHandler::engine_start);
	}
	else if (!strcmp(compare_string, "before_taxi"))
	{
		startFunction(SubHandler::before_taxi);
	}
	else if (!strcmp(compare_string, "before_takeoff"))
	{
		startFunction(SubHandler::before_take_off);
	}
	else if (!strcmp(compare_string, "clean_up"))
	{
		startFunction(SubHandler::clean_up);
	}
	else if (!strcmp(compare_string, "shutdown"))
	{
		startFunction(SubHandler::shutdown);
	}
	else if (!strcmp(compare_string, "next_procedure"))
	{
		doNextProcedure();
	}
}


//-------------------------------------------------------- Procedure Starter ------------------------------------//
void startFunction (SubHandler::Procedures procedure) {
	if (procedure == SubHandler::power_up)
	{
		if (!subHandler.procedures[SubHandler::Procedures::power_up]) {
			XPLMSpeakString("Beginning Powerup Procedures");
			subHandler.procedures[SubHandler::Procedures::power_up] = true;
			subHandler.timeElapsed = XPLMGetElapsedTime();
			XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 1.0, nullptr);
		}
		else {
			subHandler.ProcedureStage = 0;
			XPLMSpeakString("Stopping Powerup Procedures");
			subHandler.procedures[SubHandler::Procedures::power_up] = false;
			XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, nullptr);
		}
	}
	else if (procedure == SubHandler::pre_flight)
	{
		if (!subHandler.procedures[SubHandler::Procedures::pre_flight]) {
			XPLMSpeakString("Beginning Preflight Procedures");
			subHandler.procedures[SubHandler::Procedures::pre_flight] = true;
			subHandler.timeElapsed = XPLMGetElapsedTime();
			XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 1.0, nullptr);
		}
		else {
			subHandler.ProcedureStage = 0;
			XPLMSpeakString("Stopping Preflight Procedures");
			subHandler.procedures[SubHandler::Procedures::pre_flight] = false;
			XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, nullptr);
		}
	}
	else if (procedure == SubHandler::engine_start)
	{
		if (!subHandler.procedures[SubHandler::Procedures::engine_start]) {
			XPLMSpeakString("Beginning Engine Start Procedures");
			subHandler.procedures[SubHandler::Procedures::engine_start] = true;
			subHandler.timeElapsed = XPLMGetElapsedTime();
			XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 1.0, nullptr);
		}
		else {
			subHandler.ProcedureStage = 0;
			XPLMSpeakString("Stopping Engine Start Procedures");
			subHandler.procedures[SubHandler::Procedures::engine_start] = false;
			XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, nullptr);
		}
	}
	else if (procedure == SubHandler::before_taxi)
	{
		if (!subHandler.procedures[SubHandler::Procedures::before_taxi]) {
			XPLMSpeakString("Beginning Before Taxi Procedures");
			subHandler.procedures[SubHandler::Procedures::before_taxi] = true;
			subHandler.timeElapsed = XPLMGetElapsedTime();
			XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 1.0, nullptr);
		}
		else {
			subHandler.ProcedureStage = 0;
			XPLMSpeakString("Stopping Before Taxi Procedures");
			subHandler.procedures[SubHandler::Procedures::before_taxi] = false;
			XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, nullptr);
		}
	}
	else if (procedure == SubHandler::before_take_off)
	{
		if (!subHandler.procedures[SubHandler::Procedures::before_take_off]) {
			XPLMSpeakString("Beginning Before Takeoff Procedures");
			subHandler.procedures[SubHandler::Procedures::before_take_off] = true;
			subHandler.timeElapsed = XPLMGetElapsedTime();
			XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 1.0, nullptr);
		}
		else {
			subHandler.ProcedureStage = 0;
			XPLMSpeakString("Stopping Before Takeoff Procedures");
			subHandler.procedures[SubHandler::Procedures::before_take_off] = false;
			XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, nullptr);
		}
	}
	else if (procedure == SubHandler::clean_up)
	{
		if (!subHandler.procedures[SubHandler::Procedures::clean_up]) {
			XPLMSpeakString("Beginning Clean Up Procedures");
			subHandler.procedures[SubHandler::Procedures::clean_up] = true;
			subHandler.timeElapsed = XPLMGetElapsedTime();
			XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 1.0, nullptr);
		}
		else {
			subHandler.ProcedureStage = 0;
			XPLMSpeakString("Stopping Clean Up Procedures");
			subHandler.procedures[SubHandler::Procedures::clean_up] = false;
			XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, nullptr);
		}
	}
	else if (procedure == SubHandler::shutdown)
	{
		if (!subHandler.procedures[SubHandler::Procedures::shutdown]) {
			XPLMSpeakString("Beginning Shutdown Procedures");
			subHandler.procedures[SubHandler::Procedures::shutdown] = true;
			subHandler.timeElapsed = XPLMGetElapsedTime();
			XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 1.0, nullptr);
		}
		else {
			subHandler.ProcedureStage = 0;
			XPLMSpeakString("Stopping Shutdown Procedures");
			subHandler.procedures[SubHandler::Procedures::shutdown] = false;
			XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, nullptr);
		}
	}
}


//-------------------------------------------------------- Procedure Loop ---------------------------------------//
static float MyFlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void * inRefcon)
{
	/* Elapsed Time */
	const auto elapsed = XPLMGetElapsedTime();

	if (!subHandler.doneProcedures) {
		subHandler.doProcedures(elapsed);
	}
	else {
		XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, nullptr);
		subHandler.doneProcedures = false;
	}

	/* Return 1.0 to indicate that we want to be called again in 1 second. */
	return 1.0;
}


//-------------------------------------------------------- Command Functions ------------------------------------//
int funcpowerUpProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) { 
	if (!canStartNewProcedure())
		return 0;
	if (inPhase == xplm_CommandBegin)
	{
		startFunction(SubHandler::power_up);
	}
	return 0;
	
}
int funcpreflightProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) {
	if (!canStartNewProcedure())
		return 0;
	if (inPhase == xplm_CommandBegin)
	{
		startFunction(SubHandler::pre_flight);
	}
	return 0;
}
int funcengineStartProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) {
	if (!canStartNewProcedure())
		return 0;
	if (inPhase == xplm_CommandBegin)
	{
		startFunction(SubHandler::engine_start);
	}
	return 0;
}
int funcbeforeTaxiProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) {
	if (!canStartNewProcedure())
		return 0;
	if (inPhase == xplm_CommandBegin)
	{
		startFunction(SubHandler::before_taxi);
	}
	return 0;
}
int funcbeforeTakeoffProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) {
	if (!canStartNewProcedure())
		return 0;
	if (inPhase == xplm_CommandBegin)
	{
		startFunction(SubHandler::before_take_off);
	}
	return 0;
}
int funccleanUpProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) {
	if (!canStartNewProcedure())
		return 0;
	if (inPhase == xplm_CommandBegin)
	{
		startFunction(SubHandler::clean_up);
	}
	return 0;
}
int funcshutdownProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) {
	if (!canStartNewProcedure())
		return 0;
	if (inPhase == xplm_CommandBegin)
	{
		startFunction(SubHandler::shutdown);
	}
	return 0;
}
int funcnextProcedures(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void * inRefcon) {
	if (!canStartNewProcedure())
		return 0;
	if (inPhase == xplm_CommandBegin)
	{
		doNextProcedure();
	}
	return 0;
}


//-------------------------------------------------------- Next Procedure Function ------------------------------//
void doNextProcedure()
{
	if (subHandler.ProcedureType < SubHandler::Procedures::count)
		startFunction((SubHandler::Procedures)subHandler.ProcedureType);
}


bool canStartNewProcedure()
{
	for (auto procedure : subHandler.procedures)
	{
		if (procedure)
			return false;
	}
	return true;
}