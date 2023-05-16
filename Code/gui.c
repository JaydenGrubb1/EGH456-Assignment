#pragma region Includes
#include "gui.h"

/* Standard header files */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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
#include "fonts/fontnf24.h"
#include "fonts/fontnf36.h"
#pragma endregion

#pragma region Variables and Defines
/* Global defines */
#define DISPLAY &g_sKentec320x240x16_SSD2119
#define AUTO_REPEAT_DELAY 250
#define AUTO_REPEAT_RATE 20
#define NIGHT_LIGHT_THRESHOLD 5

/* Global variables */
tContext g_sContext;
tRectangle g_sScreenRect;
tCurrentPanel g_eCurrentPanel = MAIN_PANEL;
int16_t g_i16DesiredSpeed = 0;
int16_t g_i16CurrentSpeed = 0;
uint8_t g_ui8MaxPower = 10;
uint8_t g_ui8MaxAccel = 10;
uint8_t g_ui8TimeHours = 0;
uint8_t g_ui8TimeMinutes = 0;
bool g_bIsRunning = false;
bool g_bGraphSpeed = true;
bool g_bGraphPower = true;
bool g_bGraphLight = false;
bool g_bGraphAccel = false;
volatile bool g_bDoUpdate = false;
uint32_t g_ui32PrevTime = UINT32_MAX;
uint32_t g_ui32PrevLight = UINT32_MAX;
char ga_cTimeText[20];

/* Callback function array */
tGUICallbackFxn g_pfnCallbacks[GUI_CALLBACK_COUNT];
#pragma endregion

#pragma region Forward widget and function declerations
/* Main panel widgets */
tCanvasWidget g_sMainPanel;
tPushButtonWidget g_sMainStartBtn;
tPushButtonWidget g_sMainSettingsBtn;
tPushButtonWidget g_sMainGraphBtn;
tCanvasWidget g_sMainContent;
tCanvasWidget g_sMainTime;
tCanvasWidget g_sMainStatus;
tCanvasWidget g_sMainDesiredSpeed;
tCanvasWidget g_sMainCurrentSpeed;
tPushButtonWidget g_sMainDesiredSpeedUpBtn;
tPushButtonWidget g_sMainDesiredSpeedDownBtn;

/* Settings panel widgets */
tCanvasWidget g_sSettingsPanel;
tPushButtonWidget g_sSettingsBackBtn;
tCanvasWidget g_sSettingsContent;
tCanvasWidget g_sSettingsOption1Panel;
tPushButtonWidget g_sSettingsOption1DownBtn;
tPushButtonWidget g_sSettingsOption1UpBtn;
tCanvasWidget g_sSettingsOption2Panel;
tPushButtonWidget g_sSettingsOption2DownBtn;
tPushButtonWidget g_sSettingsOption2UpBtn;
tCanvasWidget g_sSettingsOption3Panel;
tPushButtonWidget g_sSettingsOption3DownBtn;
tPushButtonWidget g_sSettingsOption3UpBtn;
tCanvasWidget g_sSettingsOption4Panel;
tPushButtonWidget g_sSettingsOption4DownBtn;
tPushButtonWidget g_sSettingsOption4UpBtn;
tCanvasWidget g_sSettingsLine;

/* Graph panel widgets */
tCanvasWidget g_sGraphPanel;
tPushButtonWidget g_sGraphBackBtn;
tCheckBoxWidget g_sGraphSpeedChk;
tCheckBoxWidget g_sGraphPowerChk;
tCheckBoxWidget g_sGraphLightChk;
tCheckBoxWidget g_sGraphAccelChk;
tCanvasWidget g_sGraphContent;

/* Forward button click function declerations */
void OnMainStartBtnClick(tWidget *psWidget);
void OnMainSpeedUpBtnClick(tWidget *psWidget);
void OnMainSpeedDownBtnClick(tWidget *psWidget);
void OnMainSettingsBtnClick(tWidget *psWidget);
void OnMainGraphBtnClick(tWidget *psWidget);
void OnSettingsBackBtnClick(tWidget *psWidget);
void OnSettingsOption1DownBtnClick(tWidget *psWidget);
void OnSettingsOption1UpBtnClick(tWidget *psWidget);
void OnSettingsOption2DownBtnClick(tWidget *psWidget);
void OnSettingsOption2UpBtnClick(tWidget *psWidget);
void OnSettingsOption3DownBtnClick(tWidget *psWidget);
void OnSettingsOption3UpBtnClick(tWidget *psWidget);
void OnSettingsOption4DownBtnClick(tWidget *psWidget);
void OnSettingsOption4UpBtnClick(tWidget *psWidget);
void OnGraphBackBtnClick(tWidget *psWidget);
void OnGraphSpeedChkClick(tWidget *psWidget, uint32_t bSelected);
void OnGraphPowerChkClick(tWidget *psWidget, uint32_t bSelected);
void OnGraphLightChkClick(tWidget *psWidget, uint32_t bSelected);
void OnGraphAccelChkClick(tWidget *psWidget, uint32_t bSelected);

/* Forward widget pain function declerations */
void OnMainDesiredSpeedPaint(tWidget *psWidget, tContext *psContext);
void OnMainCurrentSpeedPaint(tWidget *psWidget, tContext *psContext);
void OnSettingsOption1Paint(tWidget *psWidget, tContext *psContext);
void OnSettingsOption2Paint(tWidget *psWidget, tContext *psContext);
void OnSettingsOption3Paint(tWidget *psWidget, tContext *psContext);
void OnSettingsOption4Paint(tWidget *psWidget, tContext *psContext);
void OnGraphContentPaint(tWidget *psWidget, tContext *psContext);
#pragma endregion

#pragma region Main panel widget constructors
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
	g_sMainContent,		  // struct name
	&g_sMainPanel,		  // parent widget pointer
	&g_sMainTime,		  // sibling widget pointer
	&g_sMainDesiredSpeed, // child widget pointer
	DISPLAY,			  // display device pointer
	6,					  // x position
	6,					  // y position
	308,				  // width
	172,				  // height
	CANVAS_STYLE_FILL,	  // style
	ClrBlack,			  // fill color
	NULL,				  // outline color
	NULL,				  // text color
	NULL,				  // font pointer
	NULL,				  // text
	NULL,				  // image pointer
	NULL				  // on-paint function pointer
);
Canvas(
	g_sMainTime,																			// struct name
	&g_sMainPanel,																			// parent widget pointer
	&g_sMainStatus,																			// sibling widget pointer
	NULL,																					// child widget pointer
	DISPLAY,																				// display device pointer
	6,																						// x position
	6,																						// y position
	172,																					// width
	22,																						// height
	CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_LEFT | CANVAS_STYLE_TEXT_TOP, // style
	ClrBlack,																				// fill color
	NULL,																					// outline color
	ClrWhite,																				// text color
	&g_sFontNf16,																			// font pointer
	"Time: 21:56 (night)",																	// text
	NULL,																					// image pointer
	NULL																					// on-paint function pointer
);
Canvas(
	g_sMainStatus,																			 // struct name
	&g_sMainPanel,																			 // parent widget pointer
	NULL,																					 // sibling widget pointer
	NULL,																					 // child widget pointer
	DISPLAY,																				 // display device pointer
	178,																					 // x position
	6,																						 // y position
	136,																					 // width
	22,																						 // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_RIGHT | CANVAS_STYLE_TEXT_TOP, // style
	ClrBlack,																				 // fill color
	ClrWhite,																				 // outline color
	ClrWhite,																				 // text color
	&g_sFontNf16,																			 // font pointer
	"Status: Off",																			 // text
	NULL,																					 // image pointer
	NULL																					 // on-paint function pointer
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
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
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
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
	OnMainSpeedDownBtnClick													 // on-click function pointer
);
#pragma endregion

#pragma region Settings panel widget constructors
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
	&g_sSettingsContent,							  // sibling widget pointer
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
Canvas(
	g_sSettingsContent,		  // struct name
	&g_sSettingsPanel,		  // parent widget pointer
	NULL,					  // sibling widget pointer
	&g_sSettingsOption1Panel, // child widget pointer
	DISPLAY,				  // display device pointer
	82,						  // x position
	6,						  // y position
	232,					  // width
	228,					  // height
	CANVAS_STYLE_FILL,		  // style
	ClrBlack,				  // fill color
	NULL,					  // outline color
	NULL,					  // text color
	NULL,					  // font pointer
	NULL,					  // text
	NULL,					  // image pointer
	NULL					  // on-paint function pointer
);
Canvas(
	g_sSettingsOption1Panel,																												   // struct name
	&g_sSettingsContent,																													   // parent widget pointer
	&g_sSettingsOption2Panel,																												   // sibling widget pointer
	&g_sSettingsOption1DownBtn,																												   // child widget pointer
	DISPLAY,																																   // display device pointer
	138,																																	   // x position
	16,																																		   // y position
	120,																																	   // width
	50,																																		   // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_TOP | CANVAS_STYLE_APP_DRAWN, // style
	ClrBlack,																																   // fill color
	ClrWhite,																																   // outline color
	ClrWhite,																																   // text color
	&g_sFontNf10,																															   // font pointer
	"Max Power (W)",																														   // text
	NULL,																																	   // image pointer
	OnSettingsOption1Paint																													   // on-paint function pointer
);
RectangularButton(
	g_sSettingsOption1DownBtn,												 // struct name
	&g_sSettingsOption1Panel,												 // parent widget pointer
	&g_sSettingsOption1UpBtn,												 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	82,																		 // x position
	16,																		 // y position
	50,																		 // width
	50,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf36,															 // font pointer
	"-",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
	OnSettingsOption1DownBtnClick											 // on-click function pointer
);
RectangularButton(
	g_sSettingsOption1UpBtn,												 // struct name
	&g_sSettingsOption1Panel,												 // parent widget pointer
	NULL,																	 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	264,																	 // x position
	16,																		 // y position
	50,																		 // width
	50,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf36,															 // font pointer
	"+",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
	OnSettingsOption1UpBtnClick												 // on-click function pointer
);
Canvas(
	g_sSettingsOption2Panel,																												   // struct name
	&g_sSettingsContent,																													   // parent widget pointer
	&g_sSettingsOption3Panel,																												   // sibling widget pointer
	&g_sSettingsOption2DownBtn,																												   // child widget pointer
	DISPLAY,																																   // display device pointer
	138,																																	   // x position
	72,																																		   // y position
	120,																																	   // width
	50,																																		   // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_TOP | CANVAS_STYLE_APP_DRAWN, // style
	ClrBlack,																																   // fill color
	ClrWhite,																																   // outline color
	ClrWhite,																																   // text color
	&g_sFontNf10,																															   // font pointer
	"Max Accel (m/s/s)",																													   // text
	NULL,																																	   // image pointer
	OnSettingsOption2Paint																													   // on-paint function pointer
);
RectangularButton(
	g_sSettingsOption2DownBtn,												 // struct name
	&g_sSettingsOption2Panel,												 // parent widget pointer
	&g_sSettingsOption2UpBtn,												 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	82,																		 // x position
	72,																		 // y position
	50,																		 // width
	50,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf36,															 // font pointer
	"-",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
	OnSettingsOption2DownBtnClick											 // on-click function pointer
);
RectangularButton(
	g_sSettingsOption2UpBtn,												 // struct name
	&g_sSettingsOption2Panel,												 // parent widget pointer
	NULL,																	 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	264,																	 // x position
	72,																		 // y position
	50,																		 // width
	50,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf36,															 // font pointer
	"+",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
	OnSettingsOption2UpBtnClick												 // on-click function pointer
);
Canvas(
	g_sSettingsOption3Panel,																												   // struct name
	&g_sSettingsContent,																													   // parent widget pointer
	&g_sSettingsOption4Panel,																												   // sibling widget pointer
	&g_sSettingsOption3DownBtn,																												   // child widget pointer
	DISPLAY,																																   // display device pointer
	138,																																	   // x position
	128,																																	   // y position
	120,																																	   // width
	50,																																		   // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_TOP | CANVAS_STYLE_APP_DRAWN, // style
	ClrBlack,																																   // fill color
	ClrWhite,																																   // outline color
	ClrWhite,																																   // text color
	&g_sFontNf10,																															   // font pointer
	"Time Hour",																															   // text
	NULL,																																	   // image pointer
	OnSettingsOption3Paint																													   // on-paint function pointer
);
RectangularButton(
	g_sSettingsOption3DownBtn,												 // struct name
	&g_sSettingsOption3Panel,												 // parent widget pointer
	&g_sSettingsOption3UpBtn,												 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	82,																		 // x position
	128,																	 // y position
	50,																		 // width
	50,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf36,															 // font pointer
	"-",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
	OnSettingsOption3DownBtnClick											 // on-click function pointer
);
RectangularButton(
	g_sSettingsOption3UpBtn,												 // struct name
	&g_sSettingsOption3Panel,												 // parent widget pointer
	NULL,																	 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	264,																	 // x position
	128,																	 // y position
	50,																		 // width
	50,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf36,															 // font pointer
	"+",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
	OnSettingsOption3UpBtnClick												 // on-click function pointer
);
Canvas(
	g_sSettingsOption4Panel,																												   // struct name
	&g_sSettingsContent,																													   // parent widget pointer
	&g_sSettingsLine,																														   // sibling widget pointer
	&g_sSettingsOption4DownBtn,																												   // child widget pointer
	DISPLAY,																																   // display device pointer
	138,																																	   // x position
	184,																																	   // y position
	120,																																	   // width
	50,																																		   // height
	CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_TOP | CANVAS_STYLE_APP_DRAWN, // style
	ClrBlack,																																   // fill color
	ClrWhite,																																   // outline color
	ClrWhite,																																   // text color
	&g_sFontNf10,																															   // font pointer
	"Time Minute",																															   // text
	NULL,																																	   // image pointer
	OnSettingsOption4Paint																													   // on-paint function pointer
);
RectangularButton(
	g_sSettingsOption4DownBtn,												 // struct name
	&g_sSettingsOption4Panel,												 // parent widget pointer
	&g_sSettingsOption4UpBtn,												 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	82,																		 // x position
	184,																	 // y position
	50,																		 // width
	50,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf36,															 // font pointer
	"-",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
	OnSettingsOption4DownBtnClick											 // on-click function pointer
);
RectangularButton(
	g_sSettingsOption4UpBtn,												 // struct name
	&g_sSettingsOption4Panel,												 // parent widget pointer
	NULL,																	 // sibling widget pointer
	NULL,																	 // child widget pointer
	DISPLAY,																 // display device pointer
	264,																	 // x position
	184,																	 // y position
	50,																		 // width
	50,																		 // height
	PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_AUTO_REPEAT, // style
	ClrGray,																 // fill color
	ClrGray,																 // press fill color
	ClrWhite,																 // outline color
	ClrWhite,																 // text color
	&g_sFontNf36,															 // font pointer
	"+",																	 // text
	NULL,																	 // image pointer
	NULL,																	 // press image pointer
	AUTO_REPEAT_DELAY,														 // auto repeat delay
	AUTO_REPEAT_RATE,														 // auto repeat rate
	OnSettingsOption4UpBtnClick												 // on-click function pointer
);
Canvas(
	g_sSettingsLine,	 // struct name
	&g_sSettingsContent, // parent widget pointer
	NULL,				 // sibling widget pointer
	NULL,				 // child widget pointer
	DISPLAY,			 // display device pointer
	6,					 // x position
	6,					 // y position
	308,				 // width
	4,					 // height
	CANVAS_STYLE_FILL,	 // style
	ClrBlue,			 // fill color
	NULL,				 // outline color
	NULL,				 // text color
	NULL,				 // font pointer
	NULL,				 // text
	NULL,				 // image pointer
	NULL				 // on-paint function pointer
);
#pragma endregion

#pragma region Graph panel widget constructors
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
	g_sGraphSpeedChk,				   // struct name
	&g_sGraphPanel,					   // parent widget pointer
	&g_sGraphPowerChk,				   // sibling widget pointer
	NULL,							   // child widget pointer
	DISPLAY,						   // display device pointer
	82,								   // x position
	184,							   // y position
	108,							   // width
	22,								   // height
	CB_STYLE_TEXT | CB_STYLE_SELECTED, // style
	22,								   // box size
	NULL,							   // fill color
	ClrWhite,						   // outline color
	ClrRed,							   // text color
	&g_sFontNf10,					   // font pointer
	" Speed (RPM)",					   // text
	NULL,							   // image pointer
	NULL							   // on-change function pointer
);
CheckBox(
	g_sGraphPowerChk,				   // struct name
	&g_sGraphPanel,					   // parent widget pointer
	&g_sGraphLightChk,				   // sibling widget pointer
	NULL,							   // child widget pointer
	DISPLAY,						   // display device pointer
	82,								   // x position
	212,							   // y position
	108,							   // width
	22,								   // height
	CB_STYLE_TEXT | CB_STYLE_SELECTED, // style
	22,								   // box size
	NULL,							   // fill color
	ClrWhite,						   // outline color
	ClrBlue,						   // text color
	&g_sFontNf10,					   // font pointer
	" Power (W)",					   // text
	NULL,							   // image pointer
	NULL							   // on-change function pointer
);
CheckBox(
	g_sGraphLightChk,  // struct name
	&g_sGraphPanel,	   // parent widget pointer
	&g_sGraphAccelChk, // sibling widget pointer
	NULL,			   // child widget pointer
	DISPLAY,		   // display device pointer
	196,			   // x position
	184,			   // y position
	128,			   // width
	22,				   // height
	CB_STYLE_TEXT,	   // style
	22,				   // box size
	NULL,			   // fill color
	ClrWhite,		   // outline color
	ClrLime,		   // text color
	&g_sFontNf10,	   // font pointer
	" Light (lux)",	   // text
	NULL,			   // image pointer
	NULL			   // on-change function pointer
);
CheckBox(
	g_sGraphAccelChk, // struct name
	&g_sGraphPanel,	  // parent widget pointer
	&g_sGraphContent, // sibling widget pointer
	NULL,			  // child widget pointer
	DISPLAY,		  // display device pointer
	196,			  // x position
	212,			  // y position
	128,			  // width
	22,				  // height
	CB_STYLE_TEXT,	  // style
	22,				  // box size
	NULL,			  // fill color
	ClrWhite,		  // outline color
	ClrYellow,		  // text color
	&g_sFontNf10,	  // font pointer
	" Accel (m/s/s)", // text
	NULL,			  // image pointer
	NULL			  // on-change function pointer
);
Canvas(
	g_sGraphContent,							   // struct name
	&g_sGraphPanel,								   // parent widget pointer
	NULL,										   // sibling widget pointer
	NULL,										   // child widget pointer
	DISPLAY,									   // display device pointer
	6,											   // x position
	6,											   // y position
	308,										   // width
	172,										   // height
	CANVAS_STYLE_OUTLINE | CANVAS_STYLE_APP_DRAWN, // style
	NULL,										   // fill color
	ClrWhite,									   // outline color
	NULL,										   // text color
	NULL,										   // font pointer
	NULL,										   // text
	NULL,										   // image pointer
	OnGraphContentPaint							   // on-paint function pointer
);
#pragma endregion

#pragma region Button click handlers
/**
 * @brief Function to handle the back button click event on the main panel
 *
 * @param pWidget Unused
 */
void OnMainStartBtnClick(tWidget *pWidget) {
	g_bIsRunning = !g_bIsRunning;

	if (g_bIsRunning) {
		PushButtonTextSet(&g_sMainStartBtn, "STOP");
		CanvasTextSet(&g_sMainStatus, "Status: On");
		PushButtonFillColorSet(&g_sMainStartBtn, ClrRed);
		PushButtonFillColorPressedSet(&g_sMainStartBtn, ClrDarkRed);
		CanvasFillColorSet(&g_sSettingsLine, ClrRed);
	} else {
		PushButtonTextSet(&g_sMainStartBtn, "START");
		CanvasTextSet(&g_sMainStatus, "Status: Off");
		PushButtonFillColorSet(&g_sMainStartBtn, ClrBlue);
		PushButtonFillColorPressedSet(&g_sMainStartBtn, ClrDarkBlue);
		CanvasFillColorSet(&g_sSettingsLine, ClrBlue);
	}

	WidgetPaint((tWidget *)&g_sMainStatus);

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
	g_eCurrentPanel = SETTINGS_PANEL;
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
	g_eCurrentPanel = GRAPH_PANEL;
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
	g_eCurrentPanel = MAIN_PANEL;
}

/**
 * @brief Function to handle the option 1 up button click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 */
void OnSettingsOption1UpBtnClick(tWidget *psWidget) {
	g_ui8MaxPower++;
	WidgetPaint((tWidget *)&g_sSettingsOption1Panel);

	GUI_InvokeCallback(GUI_MAX_POWER_CHANGE, g_ui8MaxPower, NULL);
}

/**
 * @brief Function to handle the option 1 down button click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 */
void OnSettingsOption1DownBtnClick(tWidget *psWidget) {
	g_ui8MaxPower--;
	WidgetPaint((tWidget *)&g_sSettingsOption1Panel);

	GUI_InvokeCallback(GUI_MAX_POWER_CHANGE, g_ui8MaxPower, NULL);
}

/**
 * @brief Function to handle the option 2 up button click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 */
void OnSettingsOption2UpBtnClick(tWidget *psWidget) {
	g_ui8MaxAccel++;
	WidgetPaint((tWidget *)&g_sSettingsOption2Panel);

	GUI_InvokeCallback(GUI_MAX_ACCEL_CHANGE, g_ui8MaxAccel, NULL);
}

/**
 * @brief Function to handle the option 2 down button click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 */
void OnSettingsOption2DownBtnClick(tWidget *psWidget) {
	g_ui8MaxAccel--;
	WidgetPaint((tWidget *)&g_sSettingsOption2Panel);

	GUI_InvokeCallback(GUI_MAX_ACCEL_CHANGE, g_ui8MaxAccel, NULL);
}

/**
 * @brief Function to handle the option 3 up button click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 */
void OnSettingsOption3UpBtnClick(tWidget *psWidget) {
	g_ui8TimeHours++;
	if (g_ui8TimeHours > 23)
		g_ui8TimeHours = 0;

	WidgetPaint((tWidget *)&g_sSettingsOption3Panel);

	GUI_InvokeCallback(GUI_SET_TIME_CHANGE, g_ui8TimeHours, g_ui8TimeMinutes);
}

/**
 * @brief Function to handle the option 3 down button click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 */
void OnSettingsOption3DownBtnClick(tWidget *psWidget) {
	g_ui8TimeHours--;
	if (g_ui8TimeHours > 24)
		g_ui8TimeHours = 23;

	g_ui8TimeHours = g_ui8TimeHours % 24;
	WidgetPaint((tWidget *)&g_sSettingsOption3Panel);

	GUI_InvokeCallback(GUI_SET_TIME_CHANGE, g_ui8TimeHours, g_ui8TimeMinutes);
}

/**
 * @brief Function to handle the option 4 up button click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 */
void OnSettingsOption4UpBtnClick(tWidget *psWidget) {
	g_ui8TimeMinutes++;
	if (g_ui8TimeMinutes > 59)
		g_ui8TimeMinutes = 0;
	WidgetPaint((tWidget *)&g_sSettingsOption4Panel);

	GUI_InvokeCallback(GUI_SET_TIME_CHANGE, g_ui8TimeHours, g_ui8TimeMinutes);
}

/**
 * @brief Function to handle the option 4 down button click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 */
void OnSettingsOption4DownBtnClick(tWidget *psWidget) {
	g_ui8TimeMinutes--;
	if (g_ui8TimeMinutes > 60)
		g_ui8TimeMinutes = 59;

	WidgetPaint((tWidget *)&g_sSettingsOption4Panel);

	GUI_InvokeCallback(GUI_SET_TIME_CHANGE, g_ui8TimeHours, g_ui8TimeMinutes);
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
	g_eCurrentPanel = MAIN_PANEL;
}

/**
 * @brief Function to handle the speed checkbox click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 * @param bSelected The state of the checkbox
 */
void OnGraphSpeedChkClick(tWidget *psWidget, uint32_t bSelected) {
	g_bGraphSpeed = bSelected;
}

/**
 * @brief Function to handle the power checkbox click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 * @param bSelected The state of the checkbox
 */
void OnGraphPowerChkClick(tWidget *psWidget, uint32_t bSelected) {
	g_bGraphPower = bSelected;
}

/**
 * @brief Function to handle the light checkbox click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 * @param bSelected The state of the checkbox
 */
void OnGraphLightChkClick(tWidget *psWidget, uint32_t bSelected) {
	g_bGraphLight = bSelected;
}

/**
 * @brief Function to handle the accel checkbox click event on the graph panel
 *
 * @param psWidget The widget that triggered the event
 * @param bSelected The state of the checkbox
 */
void OnGraphAccelChkClick(tWidget *psWidget, uint32_t bSelected) {
	g_bGraphAccel = bSelected;
}
#pragma endregion

#pragma region Widget paint handlers
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
	g_i16CurrentSpeed = GUI_InvokeCallback(GUI_RETURN_RPM, NULL, NULL);

	GrContextForegroundSet(psContext, ClrRed);
	GrContextFontSet(psContext, &g_sFontNf36);
	char text[8];
	snprintf(text, 8, "%03d RPM\0", g_i16CurrentSpeed);
	GrStringDrawCentered(psContext, text, -1, 107, 142, false);
}

/**
 * @brief Function to handle painting the option 1 value on the settings panel
 *
 * @param psWidget The widget that is being painted
 * @param psContext The graphics context
 */
void OnSettingsOption1Paint(tWidget *psWidget, tContext *psContext) {
	GrContextForegroundSet(psContext, ClrRed);
	GrContextFontSet(psContext, &g_sFontNf24);
	char text[4];
	snprintf(text, 4, "%03d\0", g_ui8MaxPower);
	GrStringDrawCentered(psContext, text, -1, 198, 41, false);
}

/**
 * @brief Function to handle painting the option 2 value on the settings panel
 *
 * @param psWidget The widget that is being painted
 * @param psContext The graphics context
 */
void OnSettingsOption2Paint(tWidget *psWidget, tContext *psContext) {
	GrContextForegroundSet(psContext, ClrRed);
	GrContextFontSet(psContext, &g_sFontNf24);
	char text[4];
	snprintf(text, 4, "%03d\0", g_ui8MaxAccel);
	GrStringDrawCentered(psContext, text, -1, 198, 97, false);
}

/**
 * @brief Function to handle painting the option 3 value on the settings panel
 *
 * @param psWidget The widget that is being painted
 * @param psContext The graphics context
 */
void OnSettingsOption3Paint(tWidget *psWidget, tContext *psContext) {
	GrContextForegroundSet(psContext, ClrRed);
	GrContextFontSet(psContext, &g_sFontNf24);
	char text[4];
	snprintf(text, 4, "%02d\0", g_ui8TimeHours);
	GrStringDrawCentered(psContext, text, -1, 198, 153, false);
}

/**
 * @brief Function to handle painting the option 4 value on the settings panel
 *
 * @param psWidget The widget that is being painted
 * @param psContext The graphics context
 */
void OnSettingsOption4Paint(tWidget *psWidget, tContext *psContext) {
	GrContextForegroundSet(psContext, ClrRed);
	GrContextFontSet(psContext, &g_sFontNf24);
	char text[4];
	snprintf(text, 4, "%02d\0", g_ui8TimeMinutes);
	GrStringDrawCentered(psContext, text, -1, 198, 209, false);
}

/**
 * @brief Function to handle painting the graph content
 *
 * @param psWidget The widget that is being painted
 * @param psContext The graphics context
 */
void OnGraphContentPaint(tWidget *psWidget, tContext *psContext) {
	// TODO: Draw graph content
}
#pragma endregion

#pragma region GUI API functions
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

	/* Erase the function callbacks array */
	memset(g_pfnCallbacks, NULL, sizeof(g_pfnCallbacks));
}

/**
 * @brief Internal function to handle updating the GUI
 *
 * @note This function is not intended to be called by the user
 */
void GUI_PulseInternal() {
	if (g_eCurrentPanel == MAIN_PANEL) {
		/* Update current speed */
		WidgetPaint((tWidget *)&g_sMainCurrentSpeed);

		/* Update time and light status */
		uint32_t ui32Time = GUI_InvokeCallback(GUI_RETURN_TIME, NULL, NULL);
		uint32_t ui32Light = GUI_InvokeCallback(GUI_RETURN_LIGHT, NULL, NULL);
		if (ui32Time != g_ui32PrevTime || ui32Light != g_ui32PrevLight) {
			g_ui32PrevTime = ui32Time;
			g_ui32PrevLight = ui32Light;

			if (ui32Light < NIGHT_LIGHT_THRESHOLD)
				snprintf(ga_cTimeText, 20, "Time: %02d:%02d (night)\0", ui32Time / 3600, (ui32Time / 60) % 60);
			else
				snprintf(ga_cTimeText, 20, "Time: %02d:%02d (day)\0", ui32Time / 3600, (ui32Time / 60) % 60);

			CanvasTextSet(&g_sMainTime, ga_cTimeText);
			WidgetPaint((tWidget *)&g_sMainTime);
		}
	}
	if (g_eCurrentPanel == GRAPH_PANEL) {
		/* Update graph content */
		WidgetPaint((tWidget *)&g_sGraphContent);
	}
}

/**
 * @brief Triggers a periodic GUI update
 *
 * @note This function should be called periodically from a clock task
 *
 */
void GUI_Pulse() {
	g_bDoUpdate = true; // VERIFY: Is this sufficient? Does it need to be atomic?
}

/**
 * @brief Handles processing the messages for the widget message queue
 *
 * @note This function does not return and should be called in its own task
 *
 */
void GUI_Handle() {
	while (1) {
		if (g_bDoUpdate) {
			g_bDoUpdate = false;
			GUI_PulseInternal();
		}

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
 * @brief Invokes the callback function for a specific callback
 *
 * @param tCallbackOpt The callback to invoke
 * @param arg1 The first argument to pass to the callback (optional)
 * @param arg2 The second argument to pass to the callback (optional)
 * @return The result of the callback function (optional)
 *
 * @note This function is not intended to be called by the user
 */
uint32_t GUI_InvokeCallback(tGUICallbackOption tCallbackOpt, uint32_t arg1, uint32_t arg2) {
	if (tCallbackOpt >= GUI_CALLBACK_COUNT)
		return 0;
	if (g_pfnCallbacks[tCallbackOpt] == NULL)
		return 0;

	return g_pfnCallbacks[tCallbackOpt](arg1, arg2);
}

/**
 * @brief Starts the GUI
 *
 */
void GUI_Start() {
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sMainPanel);
	WidgetPaint(WIDGET_ROOT);
}
#pragma endregion
