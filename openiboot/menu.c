#ifndef SMALL
#ifndef NO_STBIMAGE

#include "openiboot.h"
#include "lcd.h"
#include "util.h"
#include "framebuffer.h"
#include "buttons.h"
#include "timer.h"
#include "images/ConsolePNG.h"
#include "images/iPhoneOSPNG.h"
#include "images/AndroidOSPNG.h"
#include "images/ConsoleSelectedPNG.h"
#include "images/iPhoneOSSelectedPNG.h"
#include "images/AndroidOSSelectedPNG.h"
#include "images/HeaderPNG.h"
#include "images.h"
#include "actions.h"
#include "stb_image.h"
#include "pmu.h"
#include "nand.h"
#include "radio.h"
#include "hfs/fs.h"
#include "ftl.h"
#include "scripting.h"

#include "nvram.h"

#include "images/SettingsPNG.h"
#include "images/SettingsSelectedPNG.h"

#include "images/SavePNG.h"
#include "images/SaveSelectedPNG.h"
#include "images/SaveExitPNG.h"
#include "images/SaveExitSelectedPNG.h"
#include "images/SetHeaderPNG.h"

#include "images/ReturnPNG.h"
#include "images/ReturnSelectedPNG.h"
#include "images/RecoveryModePNG.h"
#include "images/RecoveryModeSelectedPNG.h"

#include "images/0PNG.h"
#include "images/1PNG.h"
#include "images/2PNG.h"
#include "images/3PNG.h"
#include "images/4PNG.h"
#include "images/5PNG.h"
#include "images/6PNG.h"
#include "images/7PNG.h"
#include "images/8PNG.h"
#include "images/9PNG.h"
#include "images/10PNG.h"

#include "images/StarPNG.h"
#include "images/RStarPNG.h"

int globalFtlHasBeenRestored = 0; /* global variable to tell wether a ftl_restore has been done*/

static uint32_t FBWidth;
static uint32_t FBHeight;


static uint32_t* imgiPhoneOS;
static int imgiPhoneOSWidth;
static int imgiPhoneOSHeight;
static int imgiPhoneOSX;
static int imgiPhoneOSY;

static uint32_t* imgConsole;
static int imgConsoleWidth;
static int imgConsoleHeight;
static int imgConsoleX;
static int imgConsoleY;

static uint32_t* imgAndroidOS;
static uint32_t* imgAndroidOS_unblended;
static int imgAndroidOSWidth;
static int imgAndroidOSHeight;
static int imgAndroidOSX;
static int imgAndroidOSY;

static uint32_t* imgiPhoneOSSelected;
static uint32_t* imgConsoleSelected;
static uint32_t* imgAndroidOSSelected;
static uint32_t* imgAndroidOSSelected_unblended;

static uint32_t* imgHeader;
static int imgHeaderWidth;
static int imgHeaderHeight;
static int imgHeaderX;
static int imgHeaderY;


static uint32_t* imgSettings;
static uint32_t* imgSettingsSelected;
static int imgSettingsWidth;
static int imgSettingsHeight;
static int imgSettingsX;
static int imgSettingsY;

static uint32_t* imgSetHeader;
static int imgSetHeaderWidth;
static int imgSetHeaderHeight;
static int imgSetHeaderX;
static int imgSetHeaderY;

static uint32_t* imgReturnSelected;
static uint32_t* imgReturn;
static int imgReturnWidth;
static int imgReturnHeight;
static int imgReturnX;
static int imgReturnY;

static uint32_t* imgRecoverySelected;
static uint32_t* imgRecovery;
static int imgRecoveryWidth;
static int imgRecoveryHeight;
static int imgRecoveryX;
static int imgRecoveryY;

static uint32_t* imgSaveSelected;
static uint32_t* imgSave;
static int imgSaveWidth;
static int imgSaveHeight;
static int imgSaveX;
static int imgSaveY;

static uint32_t* imgSaveExitSelected;
static uint32_t* imgSaveExit;
static int imgSaveExitWidth;
static int imgSaveExitHeight;
static int imgSaveExitX;
static int imgSaveExitY;

static uint32_t* img1;
static uint32_t* img2;
static uint32_t* img3;
static uint32_t* img4;
static uint32_t* img5;
static uint32_t* img6;
static uint32_t* img7;
static uint32_t* img8;
static uint32_t* img9;
static uint32_t* img10;
static uint32_t* img0;

static int imgTOX;
static int imgTOY;
static int imgTOWidth;
static int imgTOHeight;

static uint32_t* imgTO;

static uint32_t* imgStar;
static int imgStarWidth;
static int imgStarHeight;
static uint32_t* imgRStar;

static int defaultOS;
static int timeout;
static int tempOS;
static int quickBoot;
static int recovery;
static int srecovery;

typedef enum MenuSelection {
	MenuSelectioniPhoneOS,
	MenuSelectionConsole,
	MenuSelectionAndroidOS,
	MenuSelectionSettings,
	MenuSelectionSave,
	MenuSelectionSaveExit,
	MenuSelectionRecovery,
	MenuSelectionReturn
} MenuSelection;

static MenuSelection Selection;

volatile uint32_t* OtherFramebuffer;

static void drawSelectionBox() {
	volatile uint32_t* oldFB = CurFramebuffer;

	CurFramebuffer = OtherFramebuffer;
	currentWindow->framebuffer.buffer = CurFramebuffer;
	OtherFramebuffer = oldFB;

	if(Selection == MenuSelectioniPhoneOS) {
		framebuffer_draw_image(imgiPhoneOSSelected, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSettings, imgSettingsX, imgSettingsY, imgSettingsWidth, imgSettingsHeight);
	}

	if(Selection == MenuSelectionConsole) {
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsoleSelected, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSettings, imgSettingsX, imgSettingsY, imgSettingsWidth, imgSettingsHeight);
	}

	if(Selection == MenuSelectionAndroidOS) {
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOSSelected, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSettings, imgSettingsX, imgSettingsY, imgSettingsWidth, imgSettingsHeight);
	}
	if(Selection == MenuSelectionSettings) {
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSettingsSelected, imgSettingsX, imgSettingsY, imgSettingsWidth, imgSettingsHeight);
	}

	lcd_window_address(2, (uint32_t) CurFramebuffer);
}

static void toggle(int forward) {
	if(forward)
	{
		if(Selection == MenuSelectioniPhoneOS)
			Selection = MenuSelectionConsole;
		else if(Selection == MenuSelectionConsole)
			Selection = MenuSelectionAndroidOS;
		else if(Selection == MenuSelectionAndroidOS)
			Selection = MenuSelectionSettings;
		else if(Selection == MenuSelectionSettings)
			Selection = MenuSelectioniPhoneOS;
	} else
	{
		if(Selection == MenuSelectioniPhoneOS)
			Selection = MenuSelectionSettings;
		else if(Selection == MenuSelectionSettings)
			Selection = MenuSelectionAndroidOS;
		else if(Selection == MenuSelectionAndroidOS)
			Selection = MenuSelectionConsole;
		else if(Selection == MenuSelectionConsole)
			Selection = MenuSelectioniPhoneOS;
	}
	drawSelectionBox();
}

static void setTOimg(int TO){
	switch(TO){
		case 0:{
			imgTO = img0;
			break;
		}
		case 1:{
			imgTO = img1;
			break;
		}
		case 2:{
			imgTO = img2;
			break;
		}
		case 3:{
			imgTO = img3;
			break;
		}
		case 4:{
			imgTO = img4;
			break;
		}
		case 5:{
			imgTO = img5;
			break;
		}
		case 6:{
			imgTO = img6;
			break;
		}
		case 7:{
			imgTO = img7;
			break;
		}
		case 8:{
			imgTO = img8;
			break;
		}
		case 9:{
			imgTO = img9;
			break;
		}
		case 10:{
			imgTO = img10;
			break;
		}

	}

	
}

static void toggleSettings(int forward) {
	if(forward)
	{
		if(Selection == MenuSelectioniPhoneOS)
			Selection = MenuSelectionConsole;
		else if(Selection == MenuSelectionConsole)
			Selection = MenuSelectionAndroidOS;
		else if(Selection == MenuSelectionAndroidOS)
			Selection = MenuSelectionReturn;
		else if(Selection == MenuSelectionReturn)
			Selection = MenuSelectionRecovery;
		else if(Selection == MenuSelectionRecovery)
			Selection = MenuSelectionSave;
		else if(Selection == MenuSelectionSave)
			Selection = MenuSelectionSaveExit;
		else if(Selection == MenuSelectionSaveExit)
			Selection = MenuSelectioniPhoneOS;
	} else
	{
		if(Selection == MenuSelectioniPhoneOS)
			Selection = MenuSelectionSaveExit;
		else if(Selection == MenuSelectionSaveExit)
			Selection = MenuSelectionSave;
		else if(Selection == MenuSelectionSave)
			Selection = MenuSelectionRecovery;
		else if(Selection == MenuSelectionRecovery)
			Selection = MenuSelectionReturn;
		else if(Selection == MenuSelectionAndroidOS)
			Selection = MenuSelectionAndroidOS;
		else if(Selection == MenuSelectionAndroidOS)
			Selection = MenuSelectionConsole;
		else if(Selection == MenuSelectionConsole)
			Selection = MenuSelectioniPhoneOS;

	}

}

static void drawSetupPage() {
	volatile uint32_t* oldFB = CurFramebuffer;

	CurFramebuffer = OtherFramebuffer;
	currentWindow->framebuffer.buffer = CurFramebuffer;
	OtherFramebuffer = oldFB;

	switch(Selection){
	case MenuSelectioniPhoneOS:
		framebuffer_draw_image(imgiPhoneOSSelected, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSave, imgSaveX, imgSaveY, imgSaveWidth, imgSaveHeight);
		framebuffer_draw_image(imgSaveExit, imgSaveExitX, imgSaveExitY, imgSaveExitWidth, imgSaveExitHeight);
		framebuffer_draw_image(imgRecovery, imgRecoveryX, imgRecoveryY, imgRecoveryWidth, imgRecoveryHeight);
		framebuffer_draw_image(imgReturn, imgReturnX, imgReturnY, imgReturnWidth, imgReturnHeight);
		break;
	case MenuSelectionConsole:
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsoleSelected, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSave, imgSaveX, imgSaveY, imgSaveWidth, imgSaveHeight);
		framebuffer_draw_image(imgSaveExit, imgSaveExitX, imgSaveExitY, imgSaveExitWidth, imgSaveExitHeight);
		framebuffer_draw_image(imgRecovery, imgRecoveryX, imgRecoveryY, imgRecoveryWidth, imgRecoveryHeight);
		framebuffer_draw_image(imgReturn, imgReturnX, imgReturnY, imgReturnWidth, imgReturnHeight);
		break;
	case MenuSelectionAndroidOS:
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOSSelected, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSave, imgSaveX, imgSaveY, imgSaveWidth, imgSaveHeight);
		framebuffer_draw_image(imgSaveExit, imgSaveExitX, imgSaveExitY, imgSaveExitWidth, imgSaveExitHeight);
		framebuffer_draw_image(imgRecovery, imgRecoveryX, imgRecoveryY, imgRecoveryWidth, imgRecoveryHeight);
		framebuffer_draw_image(imgReturn, imgReturnX, imgReturnY, imgReturnWidth, imgReturnHeight);
		break;
	case MenuSelectionRecovery:
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSave, imgSaveX, imgSaveY, imgSaveWidth, imgSaveHeight);
		framebuffer_draw_image(imgSaveExit, imgSaveExitX, imgSaveExitY, imgSaveExitWidth, imgSaveExitHeight);
		framebuffer_draw_image(imgRecoverySelected, imgRecoveryX, imgRecoveryY, imgRecoveryWidth, imgRecoveryHeight);
		framebuffer_draw_image(imgReturn, imgReturnX, imgReturnY, imgReturnWidth, imgReturnHeight);
		break;
	case MenuSelectionReturn:
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSave, imgSaveX, imgSaveY, imgSaveWidth, imgSaveHeight);
		framebuffer_draw_image(imgSaveExit, imgSaveExitX, imgSaveExitY, imgSaveExitWidth, imgSaveExitHeight);
		framebuffer_draw_image(imgRecovery, imgRecoveryX, imgRecoveryY, imgRecoveryWidth, imgRecoveryHeight);
		framebuffer_draw_image(imgReturnSelected, imgReturnX, imgReturnY, imgReturnWidth, imgReturnHeight);
		break;
	case MenuSelectionSave:
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSaveSelected, imgSaveX, imgSaveY, imgSaveWidth, imgSaveHeight);
		framebuffer_draw_image(imgSaveExit, imgSaveExitX, imgSaveExitY, imgSaveExitWidth, imgSaveExitHeight);
		framebuffer_draw_image(imgRecovery, imgRecoveryX, imgRecoveryY, imgRecoveryWidth, imgRecoveryHeight);
		framebuffer_draw_image(imgReturn, imgReturnX, imgReturnY, imgReturnWidth, imgReturnHeight);
		break;
	case MenuSelectionSaveExit:
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSave, imgSaveX, imgSaveY, imgSaveWidth, imgSaveHeight);
		framebuffer_draw_image(imgSaveExitSelected, imgSaveExitX, imgSaveExitY, imgSaveExitWidth, imgSaveExitHeight);
		framebuffer_draw_image(imgRecovery, imgRecoveryX, imgRecoveryY, imgRecoveryWidth, imgRecoveryHeight);
		framebuffer_draw_image(imgReturn, imgReturnX, imgReturnY, imgReturnWidth, imgReturnHeight);
		break;
	default:
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
		framebuffer_draw_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
		framebuffer_draw_image(imgSave, imgSaveX, imgSaveY, imgSaveWidth, imgSaveHeight);
		framebuffer_draw_image(imgSaveExit, imgSaveExitX, imgSaveExitY, imgSaveExitWidth, imgSaveExitHeight);
		framebuffer_draw_image(imgRecovery, imgRecoveryX, imgRecoveryY, imgRecoveryWidth, imgRecoveryHeight);
		framebuffer_draw_image(imgReturn, imgReturnX, imgReturnY, imgReturnWidth, imgReturnHeight);
		break;
	}


/*	if(Selection == MenuSelectioniPhoneOS) {
	}

	if(Selection == MenuSelectionConsole) {
	}

	if(Selection == MenuSelectionAndroidOS) {
	}
	if(Selection == MenuSelectionSave) {
	}
	if(Selection == MenuSelectionSaveExit) {
	}*/
	switch(defaultOS){
		case 0:		
			framebuffer_draw_image(imgStar, imgiPhoneOSX, imgiPhoneOSY, imgStarWidth, imgStarHeight);
			break;
		case 2:		
			framebuffer_draw_image(imgStar, imgConsoleX, imgConsoleY, imgStarWidth, imgStarHeight);
			break;
		case 1:		
			framebuffer_draw_image(imgStar, imgAndroidOSX, imgAndroidOSY, imgStarWidth, imgStarHeight);
			break;
		default:
			break;
		}
	switch(tempOS){
		case 1:		
			framebuffer_draw_image(imgRStar, imgiPhoneOSX, imgiPhoneOSY+40, imgStarWidth, imgStarHeight);
			break;
		case 3:		
			framebuffer_draw_image(imgRStar, imgConsoleX, imgConsoleY+40, imgStarWidth, imgStarHeight);
			break;
		case 2:		
			framebuffer_draw_image(imgRStar, imgAndroidOSX, imgAndroidOSY+40, imgStarWidth, imgStarHeight);
			break;
		default:
			break;
		}
	if(recovery==1){
			framebuffer_draw_image(imgRStar, imgRecoveryX, imgRecoveryY, imgStarWidth, imgStarHeight);		
		}
/*	switch(quickBoot){
		case 1:		
			framebuffer_draw_image(imgRStar, imgiPhoneOSX, imgiPhoneOSY+40, imgStarWidth, imgStarHeight);
			break;
		case 3:		
			framebuffer_draw_image(imgRStar, imgConsoleX, imgConsoleY+40, imgStarWidth, imgStarHeight);
			break;
		case 2:		
			framebuffer_draw_image(imgRStar, imgAndroidOSX, imgAndroidOSY+40, imgStarWidth, imgStarHeight);
			break;
		default:
			break;
		}
*/
	setTOimg(timeout/1000);
	framebuffer_draw_image(imgTO, imgTOX, imgTOY, imgTOWidth, imgTOHeight);
	

	lcd_window_address(2, (uint32_t) CurFramebuffer);
}


static int toggleTO(int forward) {

	if(forward)
	{
//		framebuffer_print_force("Down button\n");	
		switch (timeout)
		{
		case 0:
			//framebuffer_print_force("timeout 10s\n");	
			timeout=10000;
			//draw 10			
			break;
			
		case 1000:
			timeout=0;
			//draw 10
			break;
			
		case 2000:
			timeout=1000;
			//draw 10
			break;
			
		case 4000:
			timeout=2000;
			//draw 10
			break;
			
		case 6000:
			timeout=4000;
			//draw 10
			break;
			
		case 8000:
			timeout=6000;
			//draw 10
			break;
			
		case 10000:
			timeout=8000;
			//draw 10
			break;
			
		default :
			framebuffer_print_force("DEFAULT TimeOut set 10s\n");	
			timeout=10000;
			break;
			
		}
	} else
	{
//		framebuffer_print_force("UP button\n");	
		switch (timeout){
		case 0:
			timeout=1000;
			break;
			
		case 1000:
			timeout=2000;
			break;
			
		case 2000:
			timeout=4000;
			break;
		case 4000 :
			timeout=6000;
			break;
			
		case 6000 :
			timeout=8000;
			break;
			
		case 8000 :
			timeout=10000;
			break;
			
		case 10000 :
			timeout=0;
			break;
			
		default :
			framebuffer_print_force("Uknown timeout encountered, DEFAULT TimeOut set 10s\n");	
			timeout=10000;
			break;
			
		}	
	}
	drawSetupPage();
	return(timeout);
}

int nvram_number(const char* value){
	int iData;
	const char* sData = nvram_getvar(value);
	if(sData)
		iData = parseNumber(sData);
	else
		iData=FALSE;
	return iData;
}

/* ***********************OIB_SETUP*******************************/
int oib_setup(int ttimeout, int ddefaultOS) {
	//framebuffer_print_force("blah\n");
	recovery=0;
	srecovery=0;
	quickBoot = 0;
	timeout = ttimeout;
	defaultOS = ddefaultOS;
	tempOS=0;
	char bufa[5];
	char* bufb;

	//check if recovery mode is on
	const char* sRecovery = nvram_getvar("auto-boot");
	if(sRecovery){
		if(strcmp(sRecovery, "false")==0){
			recovery=1;
		}
	}
	CurFramebuffer = OtherFramebuffer;
	currentWindow->framebuffer.buffer = CurFramebuffer;
	lcd_window_address(2, (uint32_t) CurFramebuffer);

	framebuffer_setdisplaytext(TRUE);
	framebuffer_clear();

	FBWidth = currentWindow->framebuffer.width;
	FBHeight = currentWindow->framebuffer.height;	

	imgiPhoneOS = framebuffer_load_image(dataiPhoneOSPNG, dataiPhoneOSPNG_size, &imgiPhoneOSWidth, &imgiPhoneOSHeight, TRUE);
	imgiPhoneOSSelected = framebuffer_load_image(dataiPhoneOSSelectedPNG, dataiPhoneOSSelectedPNG_size, &imgiPhoneOSWidth, &imgiPhoneOSHeight, TRUE);
	imgConsole = framebuffer_load_image(dataConsolePNG, dataConsolePNG_size, &imgConsoleWidth, &imgConsoleHeight, TRUE);
	imgConsoleSelected = framebuffer_load_image(dataConsoleSelectedPNG, dataConsoleSelectedPNG_size, &imgConsoleWidth, &imgConsoleHeight, TRUE);
	imgAndroidOS_unblended = framebuffer_load_image(dataAndroidOSPNG, dataAndroidOSPNG_size, &imgAndroidOSWidth, &imgAndroidOSHeight, TRUE);
	imgAndroidOSSelected_unblended = framebuffer_load_image(dataAndroidOSSelectedPNG, dataAndroidOSSelectedPNG_size, &imgAndroidOSWidth, &imgAndroidOSHeight, TRUE);
	imgSave = framebuffer_load_image(dataSavePNG, dataSavePNG_size, &imgSaveWidth, &imgSaveHeight, TRUE);
	imgSaveSelected = framebuffer_load_image(dataSaveSelectedPNG, dataSaveSelectedPNG_size, &imgSaveWidth, &imgSaveHeight, TRUE);
	imgSaveExit = framebuffer_load_image(dataSaveExitPNG, dataSavePNG_size, &imgSaveExitWidth, &imgSaveExitHeight, TRUE);
	imgSaveExitSelected = framebuffer_load_image(dataSaveExitSelectedPNG, dataSaveSelectedPNG_size, &imgSaveExitWidth, &imgSaveExitHeight, TRUE);
	imgHeader = framebuffer_load_image(dataHeaderPNG, dataHeaderPNG_size, &imgHeaderWidth, &imgHeaderHeight, TRUE);
	imgSetHeader = framebuffer_load_image(dataSetHeaderPNG, dataSetHeaderPNG_size, &imgSetHeaderWidth, &imgSetHeaderHeight, TRUE);
	imgReturn = framebuffer_load_image(dataReturnPNG, dataReturnPNG_size, &imgReturnWidth, &imgReturnHeight, TRUE);
	imgReturnSelected = framebuffer_load_image(dataReturnSelectedPNG, dataReturnSelectedPNG_size, &imgReturnWidth, &imgReturnHeight, TRUE);
	imgRecovery = framebuffer_load_image(dataRecoveryModePNG, dataRecoveryModePNG_size, &imgRecoveryWidth, &imgRecoveryHeight, TRUE);
	imgRecoverySelected = framebuffer_load_image(dataRecoveryModeSelectedPNG, dataRecoveryModeSelectedPNG_size, &imgRecoveryWidth, &imgRecoveryHeight, TRUE);
	img0 = framebuffer_load_image(data0PNG, data0PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img1 = framebuffer_load_image(data1PNG, data1PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img2 = framebuffer_load_image(data2PNG, data2PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img4 = framebuffer_load_image(data4PNG, data4PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img6 = framebuffer_load_image(data6PNG, data6PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img8 = framebuffer_load_image(data8PNG, data8PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img10 = framebuffer_load_image(data10PNG, data10PNG_size, &imgTOWidth, &imgTOHeight, TRUE);

	imgStar = framebuffer_load_image(dataStarPNG, dataStarPNG_size, &imgStarWidth, &imgStarHeight, TRUE);
	imgRStar = framebuffer_load_image(dataRStarPNG, dataRStarPNG_size, &imgStarWidth, &imgStarHeight, TRUE);

	bufferPrintf("menu: images loaded\r\n");

	imgiPhoneOSX = (FBWidth - imgiPhoneOSWidth) / 2;
	imgiPhoneOSY = 84;
	
	imgTOX = (FBWidth - imgTOWidth);
	imgTOY = 90;

	imgConsoleX = (FBWidth - imgConsoleWidth) / 2;
	imgConsoleY = 207;

	imgAndroidOSX = (FBWidth - imgAndroidOSWidth) / 2;
	imgAndroidOSY = 330;

	imgReturnX = (FBWidth - imgReturnWidth);
	imgReturnY = 170;	
	imgRecoveryX = (FBWidth - imgRecoveryWidth);
	imgRecoveryY = 230;

	imgSaveX = (FBWidth - imgSaveWidth);
	imgSaveY = 300;

	imgSaveExitX = (FBWidth - imgSaveExitWidth);
	imgSaveExitY = 370;

	imgHeaderX = (FBWidth - imgHeaderWidth) / 2;
	imgHeaderY = 17;
	imgSetHeaderX = (FBWidth - imgSetHeaderWidth) / 2;
	imgSetHeaderY = 75;

	framebuffer_setdisplaytext(TRUE);
	framebuffer_clear();

	framebuffer_draw_image(imgHeader, imgHeaderX, imgHeaderY, imgHeaderWidth, imgHeaderHeight);
	framebuffer_draw_image(imgSetHeader, imgSetHeaderX, imgSetHeaderY, imgSetHeaderWidth, imgSetHeaderHeight);

	OtherFramebuffer = CurFramebuffer;

	framebuffer_draw_rect_hgradient(0, 42, 0, 360, FBWidth, (FBHeight - 12) - 360);
	framebuffer_draw_rect_hgradient(0x22, 0x22, 0, FBHeight - 12, FBWidth, 12);

	framebuffer_setloc(0, 47);
	framebuffer_setcolors(COLOR_WHITE, 0x222222);
	framebuffer_print_force("Hold POWER for 10s to shutdown w/o saving\nTap POWER to change selection. Press HOME to select\nVol up/down to change Timeout (1,2,4,6,8,10s)\n");
	framebuffer_print_force(OPENIBOOT_VERSION_STR);
	framebuffer_setcolors(COLOR_WHITE, COLOR_BLACK);
	framebuffer_setloc(0, 0);

	imgAndroidOS = malloc(imgAndroidOSWidth * imgAndroidOSHeight * sizeof(uint32_t));
	imgAndroidOSSelected = malloc(imgAndroidOSWidth * imgAndroidOSHeight * sizeof(uint32_t));

	framebuffer_capture_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
	framebuffer_capture_image(imgAndroidOSSelected, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);

	framebuffer_blend_image(imgAndroidOS, imgAndroidOSWidth, imgAndroidOSHeight, imgAndroidOS_unblended, imgAndroidOSWidth, imgAndroidOSHeight, 0, 0);
	framebuffer_blend_image(imgAndroidOSSelected, imgAndroidOSWidth, imgAndroidOSHeight, imgAndroidOSSelected_unblended, imgAndroidOSWidth, imgAndroidOSHeight, 0, 0);

	Selection = MenuSelectioniPhoneOS;
	switch(defaultOS){
		case 0: {	/*iphone*/
			Selection = MenuSelectioniPhoneOS;
			break;
		}
		case 1: {	/*android*/
			Selection = MenuSelectionAndroidOS;
			break;
		}
		case 2: {	/*console*/
			Selection = MenuSelectionConsole;
			break;
		}
	}


	OtherFramebuffer = CurFramebuffer;
	CurFramebuffer = (volatile uint32_t*) NextFramebuffer;
	drawSetupPage();

	pmu_set_iboot_stage(0);

	memcpy((void*)NextFramebuffer, (void*) CurFramebuffer, NextFramebuffer - (uint32_t)CurFramebuffer);

	uint64_t startTime = timer_get_system_microtime();
	uint64_t powerStartTime = timer_get_system_microtime();

rebump:
	while(TRUE) {
		if(buttons_is_pushed(BUTTONS_HOLD)) {
			if(has_elapsed(powerStartTime, (uint64_t)300 * 1000)) {

			} else if(has_elapsed(powerStartTime, (uint64_t)200 * 1000)) {
				toggleSettings(TRUE);
				drawSetupPage();
			}
			if(has_elapsed(powerStartTime, (uint64_t)10000 * 1000)) {
				pmu_poweroff();
			}
			udelay(200000);
		} else {
			powerStartTime = timer_get_system_microtime();
			udelay(200000);
		}
#ifndef CONFIG_IPOD
		if(!buttons_is_pushed(BUTTONS_VOLUP)) {
			toggleTO(FALSE);
			startTime = timer_get_system_microtime();
			udelay(200000);
		}
		if(!buttons_is_pushed(BUTTONS_VOLDOWN)) {
			toggleTO(TRUE);
			startTime = timer_get_system_microtime();
			udelay(200000);
		}
#endif

		if(buttons_is_pushed(BUTTONS_HOME) && buttons_is_pushed(BUTTONS_HOLD)) {
			switch(Selection){
			case MenuSelectioniPhoneOS:
				tempOS=1;
				quickBoot=1;
				break;			
			case MenuSelectionConsole:
				tempOS=3;
				quickBoot=3;
				break;
			case MenuSelectionAndroidOS:
				tempOS=2;				
				quickBoot=2;
				break;
			default :
				break;
			}	
		}
		
		if(buttons_is_pushed(BUTTONS_HOME)) {
			break;
		}

		udelay(10000);
	}

	if(Selection == MenuSelectioniPhoneOS) {
		defaultOS=0;
		drawSetupPage();
		goto rebump;
	}

	if(Selection == MenuSelectionConsole) {
		defaultOS=2;
		drawSetupPage();
		goto rebump;
	}

	if(Selection == MenuSelectionReturn) {
	}

	if(Selection == MenuSelectionRecovery) {
		srecovery=1;
		if(recovery==1)
			recovery=0;
		else
			recovery=1;
		drawSetupPage();
		goto rebump;
	}

	if(Selection == MenuSelectionSave) {
		// Reset framebuffer back to original if necessary
		if((uint32_t) CurFramebuffer == NextFramebuffer)
		{
			CurFramebuffer = OtherFramebuffer;
			currentWindow->framebuffer.buffer = CurFramebuffer;
			lcd_window_address(2, (uint32_t) CurFramebuffer);
		}
		if(srecovery==1){
			if(recovery==1){
				bufb="false";
			}
			else{
				bufb="true";
			}
			nvram_setvar("auto-boot", bufb);
		}
		framebuffer_setdisplaytext(TRUE);
		framebuffer_clear();
		sprintf(bufa, "%d", defaultOS);
		nvram_setvar("opib-default-os", bufa);
		sprintf(bufa, "%d", timeout);
		nvram_setvar("opib-menu-timeout", bufa);
// dont save the tempOS setting because we are going to use quickboot and skip having to erase tempboot on startup
//		sprintf(bufa, "%d", tempOS);
//		nvram_setvar("opib-temp-os", bufa);
		nvram_save();
		udelay(200000);
		udelay(200000);
	}
	if(Selection == MenuSelectionSaveExit) {
		// Reset framebuffer back to original if necessary
		if((uint32_t) CurFramebuffer == NextFramebuffer)
		{
			CurFramebuffer = OtherFramebuffer;
			currentWindow->framebuffer.buffer = CurFramebuffer;
			lcd_window_address(2, (uint32_t) CurFramebuffer);
		}

		framebuffer_setdisplaytext(TRUE);
		framebuffer_clear();
		if(srecovery==1){
			if(recovery==TRUE){
				bufb="true";
			}
			else{
				bufb="false";
			}
			nvram_setvar("auto-boot", bufb);
		}
		sprintf(bufa, "%d", defaultOS);
		nvram_setvar("opib-default-os", bufa);
		sprintf(bufa, "%d", timeout);
		nvram_setvar("opib-menu-timeout", bufa);
		sprintf(bufa, "%d", tempOS);
		nvram_setvar("opib-temp-os", bufa);
		nvram_save();
		udelay(200000);
		udelay(200000);
		pmu_poweroff();
	}

	if(Selection == MenuSelectionAndroidOS) {
		defaultOS=1;
		drawSetupPage();
		goto rebump;
	}


	return 0;
}


/* ***********************MENU_SETUP*******************************/
int menu_setup(int ttimeout) {
	timeout=ttimeout;	
	const char* sDefaultOS = nvram_getvar("opib-default-os");
	if(sDefaultOS)
		defaultOS = parseNumber(sDefaultOS);
	tempOS=0;
	const char* stempOS = nvram_getvar("opib-temp-os");
	if(!(quickBoot==0)){
		defaultOS=quickBoot-1;
		timeout=1;
	}
	if(stempOS)
			tempOS = parseNumber(stempOS);

	if((!(tempOS==0)) && (quickBoot==0)){
		timeout=1;
		defaultOS=tempOS-1;
		nvram_setvar("opib-temp-os", "0");
		nvram_save();
	}
	FBWidth = currentWindow->framebuffer.width;
	FBHeight = currentWindow->framebuffer.height;	

	imgiPhoneOS = framebuffer_load_image(dataiPhoneOSPNG, dataiPhoneOSPNG_size, &imgiPhoneOSWidth, &imgiPhoneOSHeight, TRUE);
	imgiPhoneOSSelected = framebuffer_load_image(dataiPhoneOSSelectedPNG, dataiPhoneOSSelectedPNG_size, &imgiPhoneOSWidth, &imgiPhoneOSHeight, TRUE);
	imgConsole = framebuffer_load_image(dataConsolePNG, dataConsolePNG_size, &imgConsoleWidth, &imgConsoleHeight, TRUE);
	imgConsoleSelected = framebuffer_load_image(dataConsoleSelectedPNG, dataConsoleSelectedPNG_size, &imgConsoleWidth, &imgConsoleHeight, TRUE);
	imgAndroidOS_unblended = framebuffer_load_image(dataAndroidOSPNG, dataAndroidOSPNG_size, &imgAndroidOSWidth, &imgAndroidOSHeight, TRUE);
	imgAndroidOSSelected_unblended = framebuffer_load_image(dataAndroidOSSelectedPNG, dataAndroidOSSelectedPNG_size, &imgAndroidOSWidth, &imgAndroidOSHeight, TRUE);
	imgHeader = framebuffer_load_image(dataHeaderPNG, dataHeaderPNG_size, &imgHeaderWidth, &imgHeaderHeight, TRUE);

	imgSettings = framebuffer_load_image(dataSettingsPNG, dataSettingsPNG_size, &imgSettingsWidth, &imgSettingsHeight, TRUE);
	imgSettingsSelected = framebuffer_load_image(dataSettingsSelectedPNG, dataSettingsSelectedPNG_size, &imgSettingsWidth, &imgSettingsHeight, TRUE);


	img0 = framebuffer_load_image(data0PNG, data0PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img1 = framebuffer_load_image(data1PNG, data1PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img2 = framebuffer_load_image(data2PNG, data2PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img3 = framebuffer_load_image(data3PNG, data3PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img4 = framebuffer_load_image(data4PNG, data4PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img5 = framebuffer_load_image(data5PNG, data5PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img6 = framebuffer_load_image(data6PNG, data6PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img7 = framebuffer_load_image(data7PNG, data7PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img8 = framebuffer_load_image(data8PNG, data8PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img9 = framebuffer_load_image(data9PNG, data9PNG_size, &imgTOWidth, &imgTOHeight, TRUE);
	img10 = framebuffer_load_image(data10PNG, data10PNG_size, &imgTOWidth, &imgTOHeight, TRUE);

	bufferPrintf("menu: images loaded\r\n");

	imgiPhoneOSX = (FBWidth - imgiPhoneOSWidth) / 2;
	imgiPhoneOSY = 84;

	imgTOX = (FBWidth - imgTOWidth);
	imgTOY = 90;

	imgConsoleX = (FBWidth - imgConsoleWidth) / 2;
	imgConsoleY = 207;

	imgAndroidOSX = (FBWidth - imgAndroidOSWidth) / 2;
	imgAndroidOSY = 330;

	imgHeaderX = (FBWidth - imgHeaderWidth) / 2;
	imgHeaderY = 17;

	imgSettingsX = (FBWidth - imgSettingsWidth);
	imgSettingsY = 370;

	framebuffer_draw_image(imgHeader, imgHeaderX, imgHeaderY, imgHeaderWidth, imgHeaderHeight);

	framebuffer_draw_rect_hgradient(0, 42, 0, 360, FBWidth, (FBHeight - 12) - 360);
	framebuffer_draw_rect_hgradient(0x22, 0x22, 0, FBHeight - 12, FBWidth, 12);

	framebuffer_setloc(0, 47);
	framebuffer_setcolors(COLOR_WHITE, 0x222222);
	framebuffer_print_force("Vol up/down to toggle. Home btn to select. Hold POWER for 4s to shutdown\n");
	char defa[5];
	sprintf(defa, "%d", defaultOS);
	framebuffer_print_force(defa);
	char defb[5];
	sprintf(defb, "%d", timeout);
	framebuffer_print_force(defb);
	framebuffer_print_force(OPENIBOOT_VERSION_STR);
	framebuffer_setcolors(COLOR_WHITE, COLOR_BLACK);
	framebuffer_setloc(0, 0);

	imgAndroidOS = malloc(imgAndroidOSWidth * imgAndroidOSHeight * sizeof(uint32_t));
	imgAndroidOSSelected = malloc(imgAndroidOSWidth * imgAndroidOSHeight * sizeof(uint32_t));

	framebuffer_capture_image(imgAndroidOS, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);
	framebuffer_capture_image(imgAndroidOSSelected, imgAndroidOSX, imgAndroidOSY, imgAndroidOSWidth, imgAndroidOSHeight);

	framebuffer_blend_image(imgAndroidOS, imgAndroidOSWidth, imgAndroidOSHeight, imgAndroidOS_unblended, imgAndroidOSWidth, imgAndroidOSHeight, 0, 0);
	framebuffer_blend_image(imgAndroidOSSelected, imgAndroidOSWidth, imgAndroidOSHeight, imgAndroidOSSelected_unblended, imgAndroidOSWidth, imgAndroidOSHeight, 0, 0);

	Selection = MenuSelectioniPhoneOS;
	switch(defaultOS){
		case 0: 	/*iphone*/
			Selection = MenuSelectioniPhoneOS;
			break;
		case 1: 	/*android*/
			Selection = MenuSelectionAndroidOS;
			break;
		case 2: 	/*console*/
			Selection = MenuSelectionConsole;
			break;
		default:
			break;
	}


	OtherFramebuffer = CurFramebuffer;
	CurFramebuffer = (volatile uint32_t*) NextFramebuffer;

	drawSelectionBox();

	pmu_set_iboot_stage(0);

	memcpy((void*)NextFramebuffer, (void*) CurFramebuffer, NextFramebuffer - (uint32_t)CurFramebuffer);

	uint64_t startTime = timer_get_system_microtime();
	uint64_t powerStartTime = timer_get_system_microtime();

	//timeout = timeout * 1000;
	
	int timeoutLeft = timeout / 1000;
	int timeoutLeftb = timeout / 1000;
	while(TRUE) {
		if(timeout > 0){
			if(has_elapsed(startTime, (uint64_t)(timeout - (timeoutLeft * 1000)) * 1000)){
				timeoutLeft -= 1;
				if(timeoutLeft != timeoutLeftb){
					setTOimg(timeoutLeft + 1);
					framebuffer_draw_image(imgTO, imgTOX, imgTOY, imgTOWidth, imgTOHeight);				
					lcd_window_address(2, (uint32_t) CurFramebuffer);
					OtherFramebuffer = CurFramebuffer;
					timeoutLeftb -= 1;
				}
			}
		}
		// timeout print code here ^^  originally by apocalypse re-done by ZinnX
		if(buttons_is_pushed(BUTTONS_HOLD)) {
			
			if(has_elapsed(powerStartTime, (uint64_t)200 * 1000)) {
				startTime = timer_get_system_microtime();
				timeoutLeft = timeout / 1000;
				timeoutLeftb = timeout / 1000;
				toggle(TRUE);
			}

			if(has_elapsed(powerStartTime, (uint64_t)4000 * 1000)) {
				pmu_poweroff();
			}
			udelay(200000);
		} else {
			powerStartTime = timer_get_system_microtime();

			udelay(200000);
		}
#ifndef CONFIG_IPOD
		if(!buttons_is_pushed(BUTTONS_VOLUP)) {
			toggle(FALSE);
			startTime = timer_get_system_microtime();
			timeoutLeft = timeout / 1000;
			timeoutLeftb = timeout / 1000;
			udelay(200000);
		}
		if(!buttons_is_pushed(BUTTONS_VOLDOWN)) {
			toggle(TRUE);
			startTime = timer_get_system_microtime();
			timeoutLeft = timeout / 1000;
			timeoutLeftb = timeout / 1000;
			udelay(200000);
		}
#endif
		if(buttons_is_pushed(BUTTONS_HOME)) {
			break;
		}
		if(timeout > 0 && has_elapsed(startTime, (uint64_t)timeout * 1000)) {
			bufferPrintf("menu: timed out, selecting current item\r\n");
			break;
		}
		udelay(10000);
	}

	if(Selection == MenuSelectioniPhoneOS) {
		Image* image = images_get(fourcc("ibox"));
		if(image == NULL)
			image = images_get(fourcc("ibot"));
		void* imageData;
		images_read(image, &imageData);
		chainload((uint32_t)imageData);
	}

	if(Selection == MenuSelectionConsole) {
		// Reset framebuffer back to original if necessary
		if((uint32_t) CurFramebuffer == NextFramebuffer)
		{
			CurFramebuffer = OtherFramebuffer;
			currentWindow->framebuffer.buffer = CurFramebuffer;
			lcd_window_address(2, (uint32_t) CurFramebuffer);
		}

		framebuffer_setdisplaytext(TRUE);
		framebuffer_clear();
	}

	if(Selection == MenuSelectionSettings) {
		// Reset framebuffer back to original if necessary
			CurFramebuffer = OtherFramebuffer;
			currentWindow->framebuffer.buffer = CurFramebuffer;
			lcd_window_address(2, (uint32_t) CurFramebuffer);
		framebuffer_setdisplaytext(TRUE);
		framebuffer_clear();
		udelay(10000);
		framebuffer_setdisplaytext(TRUE);
		framebuffer_clear();
		udelay(10000);
		oib_setup(timeout, defaultOS);
//apparently we would like to return...other wise, we would have shutdown....
//clear screen
		framebuffer_setdisplaytext(TRUE);
		framebuffer_clear();

//and re-run menu_setup;
		menu_setup(timeout);
		
	}

	if(Selection == MenuSelectionAndroidOS) {
		// Reset framebuffer back to original if necessary
		if((uint32_t) CurFramebuffer == NextFramebuffer)
		{
			CurFramebuffer = OtherFramebuffer;
			currentWindow->framebuffer.buffer = CurFramebuffer;
			lcd_window_address(2, (uint32_t) CurFramebuffer);
		}

		framebuffer_setdisplaytext(TRUE);
		framebuffer_clear();

#ifndef NO_HFS
		radio_setup();
		nand_setup();
		fs_setup();
		if(globalFtlHasBeenRestored) /* if ftl has been restored, sync it, so kernel doesn't have to do a ftl_restore again */
		{
			if(ftl_sync())
			{
				bufferPrintf("ftl synced successfully");
			}
			else
			{
				bufferPrintf("error syncing ftl");
			}
		}

		pmu_set_iboot_stage(0);
		startScripting("linux"); //start script mode if there is a script file
		boot_linux_from_files();
#endif
	}

	return 0;
}

#endif
#endif
