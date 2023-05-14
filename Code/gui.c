#include "gui.h"

/* Standard header files */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* GRLib header files */
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/checkbox.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"

/* Driver header files */
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"

/* Font header files */
#include "fonts/fontnf10.h"
#include "fonts/fontnf16.h"
#include "fonts/fontnf16i.h"
#include "fonts/fontnf36.h"

/* Global defines */
#define DISPLAY &g_sKentec320x240x16_SSD2119

/* Global variables */
tContext g_sContext;
tRectangle g_sScreenRect;
int16_t g_i16DesiredSpeed = 0;
int16_t g_i16CurrentSpeed = 0;
bool g_bIsRunning = false;

/* Callback function array */
tGUICallbackFxn g_pfnCallbacks[GUI_CALLBACK_COUNT];

/* Main panel widgets */
tCanvasWidget g_sMainPanel;
tPushButtonWidget g_sMainStartBtn;
tPushButtonWidget g_sMainSettingsBtn;
tPushButtonWidget g_sMainGraphBtn;
tCanvasWidget g_sMainContent;
tCanvasWidget g_sMainDesiredSpeed;
tCanvasWidget g_sMainCurrentSpeed;
tPushButtonWidget g_sMainDesiredSpeedUpBtn;
tPushButtonWidget g_sMainDesiredSpeedDownBtn;

/* Settings panel widgets */
tCanvasWidget g_sSettingsPanel;
tPushButtonWidget g_sSettingsBackBtn;

/* Graph panel widgets */
tCanvasWidget g_sGraphPanel;
tPushButtonWidget g_sGraphBackBtn;
tCheckBoxWidget g_sGraphSpeedChk;
tCheckBoxWidget g_sGraphPowerChk;
tCheckBoxWidget g_sGraphOther1Chk;
tCheckBoxWidget g_sGraphOther2Chk;
tCanvasWidget g_sGraphContent;

/* Forward function declerations */
void OnMainStartBtnClick(tWidget *psWidget);
void OnMainSpeedUpBtnClick(tWidget *psWidget);
void OnMainSpeedDownBtnClick(tWidget *psWidget);
void OnMainSettingsBtnClick(tWidget *psWidget);
void OnMainGraphBtnClick(tWidget *psWidget);
void OnMainDesiredSpeedPaint(tWidget *psWidget, tContext *psContext);
void OnMainCurrentSpeedPaint(tWidget *psWidget, tContext *psContext);
void OnSettingsBackBtnClick(tWidget *psWidget);
void OnGraphBackBtnClick(tWidget *psWidget);

/* Main panel widget contructors */
Canvas(
	g_sMainPanel,	   // struct name
	NULL,			   // parent widget pointer
	NULL,			   // sibling widget pointer
	&g_sMainStartBtn,  // child widget pointer
	DISPLAY,		   // display device pointer
	0,				   // x position
	0,				   // y position
	320,			   // width
	240,			   // height
	CANVAS_STYLE_FILL, // style
	ClrBlack,		   // fill color
	NULL,			   // outline color
	NULL,			   // text color
	NULL,			   // font pointer
	NULL,			   // text
	NULL,			   // image pointer
	NULL			   // on-paint function pointer
);
RectangularButton(
	g_sMainStartBtn,								  // struct name
	&g_sMainPanel,									  // parent widget pointer
	&g_sMainSettingsBtn,							  // sibling widget pointer
	NULL,											  // child widget pointer
	DISPLAY,										  // display device pointer
	6,												  // x position
	184,											  // y position
	98,												  // width
	50,												  // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL, // style
	ClrBlue,										  // fill color
	ClrDarkBlue,									  // press fill color
	ClrWhite,										  // outline color
	ClrWhite,										  // text color
	&g_sFontNf16,									  // font pointer
	"START",										  // text
	NULL,											  // image pointer
	NULL,											  // press image pointer
	0,												  // auto repeat delay
	0,												  // auto repeat rate
	OnMainStartBtnClick								  // on-click function pointer
);
RectangularButton(
	g_sMainSettingsBtn,								  // struct name
	&g_sMainPanel,									  // parent widget pointer
	&g_sMainGraphBtn,								  // sibling widget pointer
	NULL,											  // child widget pointer
	DISPLAY,										  // display device pointer
	110,											  // x position
	184,											  // y position
	98,												  // width
	50,												  // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL, // style
	ClrGray,										  // fill color
	ClrGray,										  // press fill color
	ClrWhite,										  // outline color
	ClrWhite,										  // text color
	&g_sFontNf16,									  // font pointer
	"Settings",										  // text
	NULL,											  // image pointer
	NULL,											  // press image pointer
	0,												  // auto repeat delay
	0,												  // auto repeat rate
	OnMainSettingsBtnClick							  // on-click function pointer
);
RectangularButton(
	g_sMainGraphBtn,								  // struct name
	&g_sMainPanel,									  // parent widget pointer
	&g_sMainContent,								  // sibling widget pointer
	NULL,											  // child widget pointer
	DISPLAY,										  // display device pointer
	214,											  // x position
	184,											  // y position
	98,												  // width
	50,												  // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL, // style
	ClrGray,										  // fill color
	ClrGray,										  // press fill color
	ClrWhite,										  // outline color
	ClrWhite,										  // text color
	&g_sFontNf16,									  // font pointer
	"Graph",										  // text
	NULL,											  // image pointer
	NULL,											  // press image pointer
	0,												  // auto repeat delay
	0,												  // auto repeat rate
	OnMainGraphBtnClick								  // on-click function pointer
);
Canvas(
	g_sMainContent,																			   // struct name
	&g_sMainPanel,																			   // parent widget pointer
	NULL,																					   // sibling widget pointer
	&g_sMainDesiredSpeed,																	   // child widget pointer
	DISPLAY,																				   // display device pointer
	6,																						   // x position
	6,																						   // y position
	308,																					   // width
	172,																					   // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_TOP, // style
	ClrBlack,																				   // fill color
	ClrWhite,																				   // outline color
	ClrWhite,																				   // text color
	&g_sFontNf16,																			   // font pointer
	"Current Time: 21:56 14/05/23",															   // text
	NULL,																					   // image pointer
	NULL																					   // on-paint function pointer
);
Canvas(
	g_sMainDesiredSpeed,																													   // struct name
	&g_sMainContent,																														   // parent widget pointer
	&g_sMainCurrentSpeed,																													   // sibling widget pointer
	NULL,																																	   // child widget pointer
	DISPLAY,																																   // display device pointer
	6,																																		   // x position
	28,																																		   // y position
	202,																																	   // width
	72,																																		   // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_TOP | CANVAS_STYLE_APP_DRAWN, // style
	ClrBlack,																																   // fill color
	ClrWhite,																																   // outline color
	ClrWhite,																																   // text color
	&g_sFontNf10,																															   // font pointer
	"Target Speed",																															   // text
	NULL,																																	   // image pointer
	OnMainDesiredSpeedPaint																													   // on-paint function pointer
);
Canvas(
	g_sMainCurrentSpeed,																													   // struct name
	&g_sMainContent,																														   // parent widget pointer
	&g_sMainDesiredSpeedUpBtn,																												   // sibling widget pointer
	NULL,																																	   // child widget pointer
	DISPLAY,																																   // display device pointer
	6,																																		   // x position
	106,																																	   // y position
	202,																																	   // width
	72,																																		   // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_TOP | CANVAS_STYLE_APP_DRAWN, // style
	ClrBlack,																																   // fill color
	ClrWhite,																																   // outline color
	ClrWhite,																																   // text color
	&g_sFontNf10,																															   // font pointer
	"Actual Speed",																															   // text
	NULL,																																	   // image pointer
	OnMainCurrentSpeedPaint																													   // on-paint function pointer
);
RectangularButton(
	g_sMainDesiredSpeedUpBtn,												 // struct name
	&g_sMainContent,														 // parent widget pointer
	&g_sMainDesiredSpeedDownBtn,											 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	214,																	 // x position
	28,																		 // y position
	98,																		 // width
	33,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf16,															 // font pointer
	"Up",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	250,																	 // auto repeat delay
	20,																		 // auto repeat rate
	OnMainSpeedUpBtnClick													 // on-click function pointer
);
RectangularButton(
	g_sMainDesiredSpeedDownBtn,												 // struct name
	&g_sMainContent,														 // parent widget pointer
	NULL,																	 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	214,																	 // x position
	67,																		 // y position
	98,																		 // width
	33,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf16,															 // font pointer
	"Down",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	250,																	 // auto repeat delay
	20,																		 // auto repeat rate
	OnMainSpeedDownBtnClick													 // on-click function pointer
);

/* Settings panel widget contructors */
Canvas(
	g_sSettingsPanel,	 // struct name
	NULL,				 // parent widget pointer
	NULL,				 // sibling widget pointer
	&g_sSettingsBackBtn, // child widget pointer
	DISPLAY,			 // display device pointer
	0,					 // x position
	0,					 // y position
	320,				 // width
	240,				 // height
	CANVAS_STYLE_FILL,	 // style
	ClrBlack,			 // fill color
	NULL,				 // outline color
	NULL,				 // text color
	NULL,				 // font pointer
	NULL,				 // text
	NULL,				 // image pointer
	NULL				 // on-paint function pointer
);
RectangularButton(
	g_sSettingsBackBtn,								  // struct name
	&g_sSettingsPanel,								  // parent widget pointer
	NULL,											  // sibling widget pointer
	NULL,											  // child widget pointer
	DISPLAY,										  // display device pointer
	6,												  // x position
	184,											  // y position
	70,												  // width
	50,												  // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL, // style
	ClrGray,										  // fill color
	ClrGray,										  // press fill color
	ClrWhite,										  // outline color
	ClrWhite,										  // text color
	&g_sFontNf16,									  // font pointer
	"Back",											  // text
	NULL,											  // image pointer
	NULL,											  // press image pointer
	0,												  // auto repeat delay
	0,												  // auto repeat rate
	OnSettingsBackBtnClick							  // on-click function pointer
);

/* Graph panel widget constructors */
Canvas(
	g_sGraphPanel,	   // struct name
	NULL,			   // parent widget pointer
	NULL,			   // sibling widget pointer
	&g_sGraphBackBtn,  // child widget pointer
	DISPLAY,		   // display device pointer
	0,				   // x position
	0,				   // y position
	320,			   // width
	240,			   // height
	CANVAS_STYLE_FILL, // style
	ClrBlack,		   // fill color
	NULL,			   // outline color
	NULL,			   // text color
	NULL,			   // font pointer
	NULL,			   // text
	NULL,			   // image pointer
	NULL			   // on-paint function pointer
);
RectangularButton(
	g_sGraphBackBtn,								  // struct name
	&g_sGraphPanel,									  // parent widget pointer
	&g_sGraphSpeedChk,								  // sibling widget pointer
	NULL,											  // child widget pointer
	DISPLAY,										  // display device pointer
	6,												  // x position
	184,											  // y position
	70,												  // width
	50,												  // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL, // style
	ClrGray,										  // fill color
	ClrGray,										  // press fill color
	ClrWhite,										  // outline color
	ClrWhite,										  // text color
	&g_sFontNf16,									  // font pointer
	"Back",											  // text
	NULL,											  // image pointer
	NULL,											  // press image pointer
	0,												  // auto repeat delay
	0,												  // auto repeat rate
	OnGraphBackBtnClick								  // on-click function pointer
);
CheckBox(
	g_sGraphSpeedChk,  // struct name
	&g_sGraphPanel,	   // parent widget pointer
	&g_sGraphPowerChk, // sibling widget pointer
	NULL,			   // child widget pointer
	DISPLAY,		   // display device pointer
	82,				   // x position
	184,			   // y position
	118,			   // width
	22,				   // height
	CB_STYLE_TEXT,	   // style
	22,				   // box size
	NULL,			   // fill color
	ClrWhite,		   // outline color
	ClrRed,			   // text color
	&g_sFontNf10,	   // font pointer
	" Speed (RPM)",	   // text
	NULL,			   // image pointer
	NULL			   // on-change function pointer
);
CheckBox(
	g_sGraphPowerChk,	// struct name
	&g_sGraphPanel,		// parent widget pointer
	&g_sGraphOther1Chk, // sibling widget pointer
	NULL,				// child widget pointer
	DISPLAY,			// display device pointer
	82,					// x position
	212,				// y position
	118,				// width
	22,					// height
	CB_STYLE_TEXT,		// style
	22,					// box size
	NULL,				// fill color
	ClrWhite,			// outline color
	ClrBlue,			// text color
	&g_sFontNf10,		// font pointer
	" Power (Watts)",	// text
	NULL,				// image pointer
	NULL				// on-change function pointer
);
CheckBox(
	g_sGraphOther1Chk,	// struct name
	&g_sGraphPanel,		// parent widget pointer
	&g_sGraphOther2Chk, // sibling widget pointer
	NULL,				// child widget pointer
	DISPLAY,			// display device pointer
	206,				// x position
	184,				// y position
	118,				// width
	22,					// height
	CB_STYLE_TEXT,		// style
	22,					// box size
	NULL,				// fill color
	ClrWhite,			// outline color
	ClrLime,			// text color
	&g_sFontNf10,		// font pointer
	" Other 1 (IDK)",	// text
	NULL,				// image pointer
	NULL				// on-change function pointer
);
CheckBox(
	g_sGraphOther2Chk, // struct name
	&g_sGraphPanel,	   // parent widget pointer
	&g_sGraphContent,  // sibling widget pointer
	NULL,			   // child widget pointer
	DISPLAY,		   // display device pointer
	206,			   // x position
	212,			   // y position
	118,			   // width
	22,				   // height
	CB_STYLE_TEXT,	   // style
	22,				   // box size
	NULL,			   // fill color
	ClrWhite,		   // outline color
	ClrYellow,		   // text color
	&g_sFontNf10,	   // font pointer
	" Other 2 (IDK)",  // text
	NULL,			   // image pointer
	NULL			   // on-change function pointer
);
Canvas(
	g_sGraphContent,																									  // struct name
	&g_sGraphPanel,																										  // parent widget pointer
	NULL,																												  // sibling widget pointer
	NULL,																												  // child widget pointer
	DISPLAY,																											  // display device pointer
	6,																													  // x position
	6,																													  // y position
	308,																												  // width
	172,																												  // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_VCENTER, // style
	ClrBlack,																											  // fill color
	ClrWhite,																											  // outline color
	ClrLightGrey,																										  // text color
	&g_sFontNf16i,																										  // font pointer
	"plot graph here...",																								  // text
	NULL,																												  // image pointer
	NULL																												  // on-paint function pointer
);

/**
 * @brief Function to handle the back button click event on the main panel
 *
 * @param pWidget Unused
 */
void OnMainStartBtnClick(tWidget *pWidget) {
	g_bIsRunning = !g_bIsRunning;

	if (g_bIsRunning) {
		PushButtonTextSet(&g_sMainStartBtn, "STOP");
		PushButtonFillColorSet(&g_sMainStartBtn, ClrRed);
		PushButtonFillColorPressedSet(&g_sMainStartBtn, ClrDarkRed);

		GUI_InvokeCallback(GUI_MOTOR_START, NULL, NULL);
	} else {
		PushButtonTextSet(&g_sMainStartBtn, "START");
		PushButtonFillColorSet(&g_sMainStartBtn, ClrBlue);
		PushButtonFillColorPressedSet(&g_sMainStartBtn, ClrDarkBlue);

		GUI_InvokeCallback(GUI_MOTOR_STOP, NULL, NULL);
	}

	GUI_InvokeCallback(GUI_MOTOR_STATE_CHANGE, g_bIsRunning, NULL);
}

/**
 * @brief Function to handle the speed up button click event on the main panel
 *
 * @param pWidget The widget that triggered the event
 */
void OnMainSpeedUpBtnClick(tWidget *pWidget) {
	g_i16DesiredSpeed++;
	WidgetPaint((tWidget *)&g_sMainDesiredSpeed);

	GUI_InvokeCallback(GUI_MOTOR_SPEED_CHANGE, g_i16DesiredSpeed, NULL);
}

/**
 * @brief Function to handle the speed down button click event on the main panel
 *
 * @param pWidget The widget that triggered the event
 */
void OnMainSpeedDownBtnClick(tWidget *pWidget) {
	g_i16DesiredSpeed--;
	WidgetPaint((tWidget *)&g_sMainDesiredSpeed);

	GUI_InvokeCallback(GUI_MOTOR_SPEED_CHANGE, g_i16DesiredSpeed, NULL);
}

/**
 * @brief Function to handle the settings button click event on the main panel
 *
 * @param pWidget The widget that triggered the event
 */
void OnMainSettingsBtnClick(tWidget *pWidget) {
	WidgetRemove((tWidget *)&g_sMainPanel);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sSettingsPanel);
	WidgetPaint(WIDGET_ROOT);
}

/**
 * @brief Function to handle the graph button click event on the main panel
 *
 * @param pWidget The widget that triggered the event
 */
void OnMainGraphBtnClick(tWidget *pWidget) {
	WidgetRemove((tWidget *)&g_sMainPanel);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sGraphPanel);
	WidgetPaint(WIDGET_ROOT);
}

/**
 * @brief Function to handle painting the desired speed widget on the main panel
 *
 * @param psWidget The widget that is being painted
 * @param psContext The graphics context
 */
void OnMainDesiredSpeedPaint(tWidget *psWidget, tContext *psContext) {
	GrContextForegroundSet(psContext, ClrRed);
	GrContextFontSet(psContext, &g_sFontNf36);
	char text[8];
	snprintf(text, 8, "%03d RPM\0", g_i16DesiredSpeed);
	GrStringDrawCentered(psContext, text, -1, 107, 64, false);
}

/**
 * @brief Function to handle painting the current speed widget on the main panel
 *
 * @param psWidget The widget that is being painted
 * @param psContext The graphics context
 */
void OnMainCurrentSpeedPaint(tWidget *psWidget, tContext *psContext) {
	GrContextForegroundSet(psContext, ClrRed);
	GrContextFontSet(psContext, &g_sFontNf36);
	char text[8];
	snprintf(text, 8, "%03d RPM\0", g_i16CurrentSpeed);
	GrStringDrawCentered(psContext, text, -1, 107, 142, false);
}

/**
 * @brief Function to handle the back button click event on the settings panel
 *
 * @param pWidget The widget that triggered the event
 */
void OnSettingsBackBtnClick(tWidget *pWidget) {
	WidgetRemove((tWidget *)&g_sSettingsPanel);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sMainPanel);
	WidgetPaint(WIDGET_ROOT);
}

/**
 * @brief Function to handle the back button click event on the graph panel
 *
 * @param pWidget The widget that triggered the event
 */
void OnGraphBackBtnClick(tWidget *pWidget) {
	WidgetRemove((tWidget *)&g_sGraphPanel);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sMainPanel);
	WidgetPaint(WIDGET_ROOT);
}

/**
 * @brief Initialize the GUI
 *
 * @param ui32SysClock The frequency of the system clock
 */
void GUI_Init(uint32_t ui32SysClock) {
	/* Initialize touch screen */
	Kentec320x240x16_SSD2119Init(ui32SysClock);
	GrContextInit(&g_sContext, &g_sKentec320x240x16_SSD2119);
	TouchScreenInit(ui32SysClock);
	TouchScreenCallbackSet(WidgetPointerMessage);

	/* Initialize screen rect */
	g_sScreenRect.i16XMin = 0;
	g_sScreenRect.i16YMin = 0;
	g_sScreenRect.i16XMax = GrContextDpyWidthGet(&g_sContext);
	g_sScreenRect.i16YMax = GrContextDpyHeightGet(&g_sContext);
}

/**
 * @brief Handles processing the messages for the widget message queue
 *
 * @note This function does not return and should be called in its own task
 *
 */
void GUI_Handle() {
	while (1) {
		WidgetMessageQueueProcess();
	}
}

/**
 * @brief Sets the callback function for a specific callback
 *
 * @param tCallbackOpt The callback to set
 * @param pfnCallbackFxn The function to call when the callback is triggered
 */
void GUI_SetCallback(tGUICallbackOption tCallbackOpt, tGUICallbackFxn pfnCallbackFxn) {
	if (tCallbackOpt >= GUI_CALLBACK_COUNT)
		return;

	g_pfnCallbacks[tCallbackOpt] = pfnCallbackFxn;
}

/**
 * @brief Invokes a callback
 *
 * @param tCallbackOpt The callback to invoke
 * @param arg1 The first argument to pass to the callback
 * @param arg2 The second argument to pass to the callback
 */
void GUI_InvokeCallback(tGUICallbackOption tCallbackOpt, uint32_t arg1, uint32_t arg2) {
	if (tCallbackOpt >= GUI_CALLBACK_COUNT)
		return;
	if (g_pfnCallbacks[tCallbackOpt] == NULL)
		return;

	g_pfnCallbacks[tCallbackOpt](arg1, arg2);
}

/**
 * @brief Starts the GUI
 *
 */
void GUI_Start() {
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sMainPanel);
	WidgetPaint(WIDGET_ROOT);
}
