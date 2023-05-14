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
#include "grlib/pushbutton.h"

/* Driver header files */
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"

/* Font header files */
#include "fonts/fontnf10.h"
#include "fonts/fontnf16.h"
#include "fonts/fontnf16i.h"

/* Global defines */
#define DISPLAY &g_sKentec320x240x16_SSD2119

/* Global variables */
tContext g_sContext;
tRectangle g_sScreenRect;

/* Main panel widgets */
tCanvasWidget g_sMainPanel;
tPushButtonWidget g_sMainStartBtn;
tPushButtonWidget g_sMainSettingsBtn;
tPushButtonWidget g_sMainGraphBtn;
tCanvasWidget g_sMainContent;

/* Settings panel widgets */
tCanvasWidget g_sSettingsPanel;

/* Graph panel widgets */
tCanvasWidget g_sGraphPanel;
tPushButtonWidget g_sGraphBackBtn;
tCheckBoxWidget g_sGraphSpeedChk;
tCheckBoxWidget g_sGraphPowerChk;
tCheckBoxWidget g_sGraphOther1Chk;
tCheckBoxWidget g_sGraphOther2Chk;
tCanvasWidget g_sGraphContent;

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
	"Start",										  // text
	NULL,											  // image pointer
	NULL,											  // press image pointer
	0,												  // auto repeat delay
	0,												  // auto repeat rate
	NULL											  // on-click function pointer
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
	ClrBlack,										  // fill color
	ClrGray,										  // press fill color
	ClrWhite,										  // outline color
	ClrWhite,										  // text color
	&g_sFontNf16,									  // font pointer
	"Settings",										  // text
	NULL,											  // image pointer
	NULL,											  // press image pointer
	0,												  // auto repeat delay
	0,												  // auto repeat rate
	NULL											  // on-click function pointer
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
	ClrBlack,										  // fill color
	ClrGray,										  // press fill color
	ClrWhite,										  // outline color
	ClrWhite,										  // text color
	&g_sFontNf16,									  // font pointer
	"Graph",										  // text
	NULL,											  // image pointer
	NULL,											  // press image pointer
	0,												  // auto repeat delay
	0,												  // auto repeat rate
	NULL											  // on-click function pointer
);
Canvas(
	g_sMainContent,																									  // struct name
	&g_sMainPanel,																									  // parent widget pointer
	NULL,																											  // sibling widget pointer
	NULL,																											  // child widget pointer
	DISPLAY,																										  // display device pointer
	6,																												  // x position
	6,																												  // y position
	308,																											  // width
	172,																											  // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_TOP, // style
	ClrBlack,																										  // fill color
	ClrWhite,																										  // outline color
	ClrWhite,																										  // text color
	&g_sFontNf16,																									  // font pointer
	"Current Time: 21:56 14/05/23",																					  // text
	NULL,																											  // image pointer
	NULL																											  // on-paint function pointer
);

/* Settings panel widget contructors */
Canvas(
	g_sSettingsPanel,  // struct name
	NULL,			   // parent widget pointer
	NULL,			   // sibling widget pointer
	NULL,			   // child widget pointer
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
	ClrBlack,										  // fill color
	ClrGray,										  // press fill color
	ClrWhite,										  // outline color
	ClrWhite,										  // text color
	&g_sFontNf16,									  // font pointer
	"Back",											  // text
	NULL,											  // image pointer
	NULL,											  // press image pointer
	0,												  // auto repeat delay
	0,												  // auto repeat rate
	NULL											  // on-click function pointer
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
 * @brief Initialize the GUI
 *
 * @param ui32SysClock The frequency of the system clock
 */
void InitGUI(uint32_t ui32SysClock) {
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
void HandleGUI() {
	while (1) {
		WidgetMessageQueueProcess();
	}
}

/**
 * @brief Draws the GUI
 *
 */
void DrawGUI() {
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sMainPanel);
	WidgetPaint(WIDGET_ROOT);
}
