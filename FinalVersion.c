/* Comments / TO DO list:
I know this is supposed to be a .ino file, but I don't think code your cloud will still highlight keywords then. Just copy/paste - Joe

I think we should use a header file for the structure definitions as it will make the code cleaner. For now, I am implementing the structures here - Varun
Also, I am defining typedefs since it will be more convenient when using the objects later on - Varun

NOTE: I googled 'extern "C"', and it seems that the code language used by the orbit booster pack is a variant of C++ since the 'extern "C"' keyword is used in C++. Based on this information, classes **should** be supported but I am not sure how the microcontroller handles this - Varun
(http://stackoverflow.com/questions/1041866/in-c-source-what-is-the-effect-of-extern-c)

Upon further research, it seems like the Tiva uses a weird variant of C, so above note about classes is irrelevant - Varun

/* Comments / TO DO list:
I know this is supposed to be a .ino file, but I don't think code your cloud will still highlight keywords then. Just copy/paste - Joe

I think we should use a header file for the structure definitions as it will make the code cleaner. For now, I am implementing the structures here - Varun
Also, I am defining typedefs since it will be more convenient when using the objects later on - Varun

NOTE: I googled 'extern "C"', and it seems that the code language used by the orbit booster pack is a variant of C++ since the 'extern "C"' keyword is used in C++. Based on this information, classes **should** be supported but I am not sure how the microcontroller handles this - Varun
(http://stackoverflow.com/questions/1041866/in-c-source-what-is-the-effect-of-extern-c)

Upon further research, it seems like the Tiva uses a weird variant of C, so above note about classes is irrelevant - Varun

*/


/* CODE BEGIN HERE */

/* HEADER FILES */

extern "C" { 
#include <delay.h>
#include <FillPat.h>
#include <I2CEEPROM.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
}

/* TYPEDEFS FOR HARDWARE */
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

/* -----------------------------------------------------STRUCTURE TYPEDEFS------------------------------------------------------*/

typedef struct {  //bullets fired by gun (probably best to leave name as bullet) - Varun
  float x, y;
  int valid;    //x, y pos of the bullet
  int xi,yi;    // Orgiginal Int position
} bullet;

typedef struct {  //attacking sqaures/blocks, can be renamed as desired - Varun
  float x, y;
  int valid;    
  int xi,yi;  
} geese;

typedef struct {
  float x1, y1;
  float x2,y2;  //Top left corner of second block of gun
  // we could implement this in the gun structure - Varun ||Better in struct as only one instance of gun -Mo
  int xi,yi;
} gun;

/* -----------------------------------------------------GLOBAL VARIABLES ------------------------------------------------------*/

extern int xchOledMax; // defined in OrbitOled.c
extern int ychOledMax; // defined in OrbitOled.c
char  chSwtCur;
char  chSwtPrev;
bool  fClearOled;

double gunSpeed;
double geeseSpeed[5]={0.5,0.7,1,1.2,1.5};//Choose speed as pot value changes.
double curGeeseSpeed=0.5;
double bulletSpeed=-2;         //Choose Negative value
const int geeseSize=5;           //we shouldn't implement this in the structures due to multiple copies of one variable - Varun
const int bulletSize=2;
int maxBullet = 4;
const int maxGeese = 3;
const int guns1=3, guns2=7;
const int bulletGap=30;
bool fire=true;
int calibratedAccel;
int bufferAccel=50;
geese ge[maxGeese];
gun g;
bullet b[5];


int lives=4;
int score=0;
int highScore=0;
/* GRAPHICS ARRAYS */     //Can be changed easily if needed
int strtScrnH= 22,strtScrnW=32; //Doesnt have to be multiple of 8, function will trim image
char strtScrnBMP[]={
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x33, 0x73, 0x0B, 0x0F, 0x0F, 0x7F, 0x3F, 0x3F, 0x7F, 
  0x7F, 0x3F, 0x7F, 0x3F, 0x0F, 0x0F, 0x07, 0x73, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0x9F, 0x07, 0x07, 0xC1, 0xC1, 0xC1, 0x00, 0x00, 0x04, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x04, 0x00, 0x00, 0xC1, 0xC1, 0xC1, 0x07, 0x07, 0xAF, 0xFF, 
  0xFF, 0xFD, 0xF8, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFA, 0xE3, 0xE3, 0xE3, 0xF3, 0xF3, 0xFF, 
  0xFF, 0xF7, 0xE3, 0xF3, 0xE3, 0xE3, 0xF3, 0xFC, 0xFC, 0xFD, 0xFF, 0xFF, 0xF8, 0xFC, 0xFE, 0xFF, 
};
char pusheen[]={
  0x00, 0x40, 0x40, 0xC0, 0x80, 0x80, 0x70, 0x9C, 0x86, 0x83, 0x86, 0x98, 0xB0, 0x90, 0xB0, 0x90, 
  0xB0, 0x98, 0x8C, 0x86, 0x83, 0x82, 0x8E, 0x18, 0x10, 0x98, 0xD8, 0x48, 0x18, 0x38, 0x38, 0x38, 
  0x08, 0x10, 0x38, 0x70, 0x70, 0x60, 0x60, 0x40, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x02, 0xC2, 0x7E, 0x03, 0x03, 0x01, 0x07, 0x0F, 0x0F, 0x0F, 
  0x07, 0x47, 0x80, 0xE0, 0x87, 0x47, 0x0F, 0x0F, 0x0F, 0x07, 0x03, 0x01, 0x01, 0x03, 0x02, 0x02, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06, 0x3C, 
  0xE0, 0x80, 0x80, 0xC0, 0x70, 0xF0, 0xF0, 0x40, 0x00, 0x00, 0x00, 0x7F, 0xC4, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xC0, 0x7F, 0x0F, 0x0D, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x0E, 0x18, 0x10, 0x20, 0x60, 0xC0, 0xC0, 0x40, 0x40, 0x40, 0x40, 0xC0, 0xC0, 0x40, 0x40, 
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xC0, 0xC0, 0x40, 0x40, 0x40, 0x40, 0x40, 
  0xC0, 0xC0, 0x60, 0x20, 0x18, 0x08, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};



/* --------------------------------------------------------ESSENTIAL PREMADE FUNCTIONS-------------------------------------------*/

void DeviceInit()   
{
  /*
   * First, Set Up the Clock.
   * Main OSC     -> SYSCTL_OSC_MAIN
   * Runs off 16MHz clock -> SYSCTL_XTAL_16MHZ
   * Use PLL      -> SYSCTL_USE_PLL
   * Divide by 4    -> SYSCTL_SYSDIV_4
   */
  SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_4);

  /*
   * Enable and Power On All GPIO Ports
   */
  //SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOA | SYSCTL_PERIPH_GPIOB | SYSCTL_PERIPH_GPIOC |
  //            SYSCTL_PERIPH_GPIOD | SYSCTL_PERIPH_GPIOE | SYSCTL_PERIPH_GPIOF);

  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOA );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );
  SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
  /*
   * Pad Configure.. Setting as per the Button Pullups on
   * the Launch pad (active low).. changing to pulldowns for Orbit
   */
  GPIOPadConfigSet(SWTPort, SWT1 | SWT2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(BTN1Port, BTN1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
  GPIOPadConfigSet(BTN2Port, BTN2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

  GPIOPadConfigSet(LED1Port, LED1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED2Port, LED2, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED3Port, LED3, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  GPIOPadConfigSet(LED4Port, LED4, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

  /*
   * Initialize Switches as Input
   */
  GPIOPinTypeGPIOInput(SWTPort, SWT1 | SWT2);

  /*
   * Initialize Buttons as Input
   */
  GPIOPinTypeGPIOInput(BTN1Port, BTN1);
  GPIOPinTypeGPIOInput(BTN2Port, BTN2);

  /*
   * Initialize LEDs as Output
   */
  GPIOPinTypeGPIOOutput(LED1Port, LED1);
  GPIOPinTypeGPIOOutput(LED2Port, LED2);
  GPIOPinTypeGPIOOutput(LED3Port, LED3);
  GPIOPinTypeGPIOOutput(LED4Port, LED4);

  /*
   * Enable ADC Periph
   */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

  GPIOPinTypeADC(AINPort, AIN);

  /*
   * Enable ADC with this Sequence
   * 1. ADCSequenceConfigure()
   * 2. ADCSequenceStepConfigure()
   * 3. ADCSequenceEnable()
   * 4. ADCProcessorTrigger();
   * 5. Wait for sample sequence ADCIntStatus();
   * 6. Read From ADC
   */
  ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
  ADCSequenceEnable(ADC0_BASE, 0);

  /*
   * Initialize the OLED
   */
  OrbitOledInit();

  /*
   * Reset flags
   */
  chSwtCur = 0;
  chSwtPrev = 0;
  fClearOled = true;

}
char I2CGenTransmit(char * pbData, int cSize, bool fRW, char bAddr) {

  int     i;
  char *    pbTemp;

  pbTemp = pbData;

  /*Start*/

  /*Send Address High Byte*/
  /* Send Write Block Cmd*/
  I2CMasterSlaveAddrSet(I2C0_BASE, bAddr, WRITE);
  I2CMasterDataPut(I2C0_BASE, *pbTemp);

  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

  DelayMs(1);

  /* Idle wait*/
  while(I2CGenIsNotIdle());

  /* Increment data pointer*/
  pbTemp++;

  /*Execute Read or Write*/

  if(fRW == READ) {

    /* Resend Start condition
  ** Then send new control byte
  ** then begin reading
  */
    I2CMasterSlaveAddrSet(I2C0_BASE, bAddr, READ);

    while(I2CMasterBusy(I2C0_BASE));

    /* Begin Reading*/
    for(i = 0; i < cSize; i++) {

      if(cSize == i + 1 && cSize == 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));
      }
      else if(cSize == i + 1 && cSize > 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));
      }
      else if(i == 0) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));

        /* Idle wait*/
        while(I2CGenIsNotIdle());
      }
      else {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));

        /* Idle wait */
        while(I2CGenIsNotIdle());
      }

      while(I2CMasterBusy(I2C0_BASE));

      /* Read Data */
      *pbTemp = (char)I2CMasterDataGet(I2C0_BASE);

      pbTemp++;

    }

  }
  else if(fRW == WRITE) {

    /*Loop data bytes */
    for(i = 0; i < cSize; i++) {
      /* Send Data */
      I2CMasterDataPut(I2C0_BASE, *pbTemp);

      while(I2CMasterBusy(I2C0_BASE));

      if(i == cSize - 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));
      }
      else {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

        DelayMs(1);

        while(I2CMasterBusy(I2C0_BASE));

        /* Idle wait */
        while(I2CGenIsNotIdle());
      }

      pbTemp++;
    }

  }

  /*Stop*/

  return 0x00;

}

bool I2CGenIsNotIdle() {

  return !I2CMasterBusBusy(I2C0_BASE);

}
int getAccelY(){
  short dataX;
  short dataY;
  short dataZ;
  
  char printVal[10];
  
  char  chPwrCtlReg = 0x2D;
  char  chX0Addr = 0x32;
  char  chY0Addr = 0x34;
  char  chZ0Addr = 0x36;
  
  char  rgchReadAccl[] = {
    0, 0, 0            };
  char  rgchWriteAccl[] = {
    0, 0            };
    
  char rgchReadAccl2[] = {
    0, 0, 0            };
    
    char rgchReadAccl3[] = {
    0, 0, 0            };

  /*int xcoRocketCur = xcoRocketStart;
  int   ycoRocketCur = ycoRocketStart;
  int   xcoExhstCur = xcoExhstStart;
  int   ycoExhstCur = ycoExhstStart;

  int   xDirThreshPos = 50;
  int   xDirThreshNeg = -50;

  bool fDir = true;*/

    /*
     * Enable I2C Peripheral
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    /*
     * Set I2C GPIO pins
     */
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    /*
     * Setup I2C
     */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    /* Initialize the Accelerometer
     *
     */
    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    rgchWriteAccl[0] = chPwrCtlReg;
    rgchWriteAccl[1] = 1 << 3;    // sets Accl in measurement mode
    I2CGenTransmit(rgchWriteAccl, 1, WRITE, ACCLADDR);
  
  
  /*
   * Loop and check for movement until switches
   * change
   */
  

    /*
     * Read the X data register
     */
    rgchReadAccl[0] = chX0Addr;
    rgchReadAccl2[0] = chY0Addr;
    rgchReadAccl3[0] = chZ0Addr;
    
    I2CGenTransmit(rgchReadAccl, 2, READ, ACCLADDR);
    I2CGenTransmit(rgchReadAccl2, 2, READ, ACCLADDR);
    I2CGenTransmit(rgchReadAccl3, 2, READ, ACCLADDR);
    
    dataX = (rgchReadAccl[2] << 8) | rgchReadAccl[1];
    dataY = (rgchReadAccl2[2] << 8) | rgchReadAccl2[1];
    dataZ = (rgchReadAccl3[2] << 8) | rgchReadAccl2[1];
    
    return dataY;
    
}

char CheckSwitches() {

  long  lSwt1;
  long  lSwt2;

  chSwtPrev = chSwtCur;

  lSwt1 = GPIOPinRead(SWT1Port, SWT1);
  lSwt2 = GPIOPinRead(SWT2Port, SWT2);

  chSwtCur = (lSwt1 | lSwt2) >> 6;

  if(chSwtCur != chSwtPrev) {
    fClearOled = true;
  }

  return chSwtCur;

}



/*--------------------------------------------------------------END OF BOILERPLATE CODE-----------------------------------------*/


/*-------------------------------------------------------   FUNCTION PROTOTYPES   -----------------------------------------------------*/
void showStrtScrn();      //Hmm I wonder what this does
void updateLED(int n);         //Lits up n leds from right.Invalid parameter does nothing
int getPot();               //Returns  the potentiometer value [1,3,5,7,9] . We ignored even number as pot is crap and the digit fluctuates
void updateGeese();         //Varun
void updateBullet();        //Carson
void updateGun();           //Khairul
bool hitTestBullet(bullet bull, geese goo);
bool hitTestGun(geese goo, gun theGun);
void globalHitTest();              //Joe
void paintFrame();          //
void gameLoop();
void displayScore();                   //Shows High Score, Pusheen 
void initStuff();
void drawPusheen();

/*-------------------------------------------------------------------------------------------------------------------------------------*/


void initStuff(){
  for (int i = 0; i < maxGeese; i++) {
    ge[i].x = 0.0;
    ge[i].y = i * 8 + (rand() % 8);
    ge[i].valid = 0;
    ge[i].xi = 0;
    ge[i].yi = 0;
  }
  for (int i = 0; i < maxBullet; i++) {
    b[i].x = 127.0;
    b[i].y = 0.0;
    b[i].valid = 0;
    b[i].xi = 127;
    b[i].yi = 0;
  }
  g.x1 = 127.0 - guns1 - guns2;
  g.y1 = 16 - (guns1 / 2);
  g.x2 = 127.0 - guns2;
  g.y2 = 16 - (guns2 / 2);

}


void showStrtScrn(){    //Tested  
  OrbitOledClear();
  OrbitOledMoveTo(48,0);
  OrbitOledPutBmp(strtScrnW,strtScrnH,strtScrnBMP);
  // OrbitOledSetCursor(3,3);
  // OrbitOledPutString("START GAME");
  OrbitOledSetCursor(0,3);
  OrbitOledPutString(" PRESS TO START ");

  
  // for(int t=0,c='3';t<3;t++,c--){   //show timer
  //   OrbitOledSetCursor(14,3);
  //   OrbitOledPutChar(c);
  //   updateLED(4);
  //   delay(400);
  //   updateLED(0);
  //   delay(200);
  // }

  long lBtn1;
  lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  while(lBtn1!=BTN1){
    updateLED(4);
    delay(100);
    updateLED(0);
    delay(100);
    lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  } 
}

void updateLED(int n){       //Tested
  switch(n){
    case 4 : 
            GPIOPinWrite(LED1Port, LED1, LED1);
            GPIOPinWrite(LED2Port, LED2, LED2);
            GPIOPinWrite(LED3Port, LED3, LED3);
            GPIOPinWrite(LED4Port, LED4, LED4);
            break;
    case 3 : 
            GPIOPinWrite(LED1Port, LED1, LED1);
            GPIOPinWrite(LED2Port, LED2, LED2);
            GPIOPinWrite(LED3Port, LED3, LED3);
            GPIOPinWrite(LED4Port, LED4, LOW);
            break;
    case 2 : 
            GPIOPinWrite(LED1Port, LED1, LED1);
            GPIOPinWrite(LED2Port, LED2, LED2);
            GPIOPinWrite(LED3Port, LED3, LOW); 
            GPIOPinWrite(LED4Port, LED4, LOW);
            break;
            
    case 1 : 
            GPIOPinWrite(LED1Port, LED1, LED1);
            GPIOPinWrite(LED2Port, LED2, LOW);
            GPIOPinWrite(LED3Port, LED3, LOW); 
            GPIOPinWrite(LED4Port, LED4, LOW);
            break;
    case 0 : 
            GPIOPinWrite(LED1Port, LED1, LOW);
            GPIOPinWrite(LED2Port, LED2, LOW);
            GPIOPinWrite(LED3Port, LED3, LOW); 
            GPIOPinWrite(LED4Port, LED4, LOW);
  }
}

int getPot(){
  char      val;
  char      cMSB = 0x00;
  char      cMIDB = 0x00;
  char      cLSB = 0x00;
  uint32_t  ulAIN0;
  ADCProcessorTrigger(ADC0_BASE, 0);

  while(!ADCIntStatus(ADC0_BASE, 0, false));
  ADCSequenceDataGet(ADC0_BASE, 0, &ulAIN0);

  cMSB = (0xF00 & ulAIN0) >> 8;
  cMIDB = (0x0F0 & ulAIN0) >> 4;
  cLSB = (0x00F & ulAIN0);

  val = (cMSB > 9) ? 'A' + (cMSB - 10) : '0' + cMSB;
  if(val>='A' && val <='F')return 9; // max pot value
  val=val-'0';
  return val%2 ? val:val+1;


}
void updateGeese() {
  for (int i = 0; i < maxGeese; i++) {
    if (!ge[i].valid) {
      //respawn geese
      ge[i].x = 0;
      ge[i].y = i * 11 + (rand() % (9-geeseSize));
      ge[i].valid = !(rand()%50);
      continue;
    }
    ge[i].xi=(int)ge[i].x;
    ge[i].x += curGeeseSpeed;
    if ((int)ge[i].x >= 127){
      score-=1;
      ge[i].valid=0;
    }
  }
}
int flag=1;
void updateBullet(){

  /*Creating New Bullets*/
  long lBtn2;
      lBtn2 = GPIOPinRead(BTN2Port, BTN2);
  if(lBtn2!=BTN2)flag=1;
  if(lBtn2 == BTN2){          //Executes if button is pressed.
    
    for(int i = 0; i<maxBullet; i++){     
      if( flag && b[i].valid == 0) {    //Executes if array index is unused.
        flag=0;
        b[i].valid = 1;     //Sets array index to used.
        b[i].x = g.x1;        //Sets bullet coordinates equal to gun coordinates. (May need to be tweaked depending on what part of the gun the gun coordinates are referencing). 
        b[i].y = g.y1; 
        break;          //Exits loop because only one array index is needed per bullet.
      }
    } 
  }

  
  /*Handling Displayed Bullets*/
  for(int i = 0; i<maxBullet; i++){
      
    if(b[i].valid ){        //Executes if array index is used.
      b[i].xi=(int)b[i].x;
      b[i].x = b[i].x + bulletSpeed;  //Moves bullet leftwards across the screen.
    
      if(b[i].x <= 0){        //Executes if bullet has exited the confines of the screen.
        b[i].valid = 0;       //Sets array index to unused.
        b[i].xi=0;
        b[i].x=0;
      }       
    }
  } 
}

void updateGun(){
  int accelY=getAccelY();
  if(accelY>(calibratedAccel-bufferAccel/2) && accelY<(calibratedAccel+bufferAccel/2))gunSpeed=0;
  if(accelY<(calibratedAccel-bufferAccel/2))gunSpeed=-1;
  if(accelY>(calibratedAccel+bufferAccel/2))gunSpeed=1;

  g.yi=(int)g.y1;
  g.y1+=(gunSpeed);
  g.y2=g.y1-((guns2-guns1)/2);
  
  if((int)g.y1<((guns2-guns1)/2))g.y1=((guns2-guns1)/2);
  if((int)g.y1>(31-(guns2-guns1)/2)-guns1)g.y1=31-(guns2-guns1)/2-guns1;
}
int incrementScore(){   //tweak this later
  return (int)ceil(curGeeseSpeed)+(6-maxBullet);
}
//Function called by the global hit test
//Don't call this in game loop
bool hitTestBullet(bullet bull, geese goo){
  if(bull.valid == false || goo.valid == false)
    return false;

  //The strictly < and the >= are required the way they are

  //Only need to check left edge - can't clip through b/c of float method
  bool xcond = (bull.x < goo.x + geeseSize) && (bull.x >= goo.x); //May not even need the second condition but whatever
  //For y, need to check both edges
  bool ycondTop = (bull.y <  goo.y + geeseSize) && (bull.y >= goo.y);
  //Pixel we need to check for bottom is bull.y + Size - 1
  bool ycondBottom = (bull.y + bulletSize - 1 < goo.y + geeseSize) && (bull.y + bulletSize - 1 >= goo.y);
  //If either top or bottom edge is in the goose, it is in the range
  bool ycond = ycondTop || ycondBottom;
  return (xcond && ycond);
}

//Function called by the global hit test
//Don't call this in the game loop
bool hitTestGun(geese goo, gun theGun){
  if(goo.valid == false)
    return false;
  //Need to check both right and left edges, since the gun can be moving up down
  //And the right edge of the goose doesn't necessarily intersect first
  
  //Check the small block first

  bool xcondLeft = (goo.x < theGun.x1 + guns1) && (goo.x >= theGun.x1);
  //Pixel we need to check for right is goo.x + Size - 1
  bool xcondRight = (goo.x + geeseSize - 1 < theGun.x1 + guns1) && (goo.x + geeseSize - 1 >= theGun.x1);

  bool xcond = xcondLeft || xcondRight;

  bool ycondTop = (goo.y <  theGun.y1 + guns1) && (goo.y >= theGun.y1);
  //Pixel we need to check for bottom is goo.y + Size - 1
  bool ycondBottom = (goo.y + geeseSize - 1 < theGun.y1 + guns1) && (goo.y + geeseSize - 1 >= theGun.y1);

  bool ycond = ycondTop || ycondBottom;

  if(xcond && ycond)
    return true;

  //Check the large block seconnd
  xcondLeft = (goo.x < theGun.x2 + guns2) && (goo.x >= theGun.x2);
  //Pixel we need to check for right is goo.x + Size - 1
  xcondRight = (goo.x + geeseSize - 1 < theGun.x2 + guns2) && (goo.x + geeseSize - 1 >= theGun.x2);

  xcond = xcondLeft || xcondRight;

  ycondTop = (goo.y <  theGun.y2 + guns2) && (goo.y >= theGun.y2);
  //Pixel we need to check for bottom is goo.y + Size - 1
  ycondBottom = (goo.y + geeseSize - 1 < theGun.y2 + guns2) && (goo.y + geeseSize - 1 >= theGun.y2);

  ycond = ycondTop || ycondBottom;

  return (xcond && ycond);
}

//This is the function the game loop calls
void globalHitTest(){
  //Iterate through array of geese
  for(int i = 0; i < maxGeese; i++){
    //Iterate through array of bullets
    for(int j = 0; j < maxBullet; j++){
      //Perform hit test
      if(hitTestBullet(b[j], ge[i])){
        //Set invalid
        b[j].valid = false;
        ge[i].valid = false;
        //Update score
        score+=incrementScore();
      }
    }
    //Check if goose has hit the gun
    if(hitTestGun(ge[i], g)){
      //Set goose to invalid and lose a life
      ge[i].valid = false;
      lives--;
    }
  }
}
bool frameChanged(){
  if(g.yi!=(int)g.y1)return true;
  for(int i=0;i<maxGeese;i++){
    if(ge[i].valid && (ge[i].xi!=(int)ge[i].x))return true;
  }
  for(int i=0;i<maxBullet;i++){
    if(b[i].valid && (b[i].xi!=(int)b[i].x))return true;
  }
  return false;
}
void paintFrame(){
  if(frameChanged()){
    OrbitOledClear();

    OrbitOledMoveTo((int)(g.x1),(int)g.y1);
    OrbitOledFillRect((int)(g.x1+guns1),(int)(g.y1+guns1));
    // gy2=gy-((gsize2-gsize)/2);
    OrbitOledMoveTo((int)(g.x2),(int)g.y2);
    OrbitOledFillRect((int)(g.x2+guns2),(int)(g.y2+guns2));
    OrbitOledUpdate();
    for(int i=0;i<maxGeese;i++){
      if(ge[i].valid){
        OrbitOledMoveTo((int)ge[i].x,(int)ge[i].y);
        OrbitOledFillRect((int)ge[i].x+geeseSize,(int)ge[i].y+geeseSize);
      }
    }
    OrbitOledUpdate();
    for(int i=0;i<maxBullet;i++){
      if(b[i].valid){
        OrbitOledMoveTo((int)b[i].x,(int)b[i].y);
        OrbitOledFillRect((int)b[i].x+bulletSize,(int)b[i].y+bulletSize);
      }
    }
    OrbitOledUpdate();
  }
}
void drawPusheen(){
  OrbitOledMoveTo(12,0);
  OrbitOledPutBmp(52,32,pusheen);
}
void gameLoop(){
  while(lives>0){
  updateGeese();
  updateBullet();
  updateGun();
  globalHitTest();
  paintFrame();
  updateLED(lives);
  curGeeseSpeed=geeseSpeed[getPot()/2];
  }
}

void displayScore () {
  char scoreStr[8];
  if(score<0)score=0; //Dont wanna mock the user with negative scores
  if (score>highScore) {
    OrbitOledClear();
    drawPusheen (); //Pusheen drawn on the right
    
  //Code for displaying high score on the left (columns 0 - 7)
      OrbitOledMoveTo(92, 0);
    OrbitOledDrawString("NEW");
    OrbitOledMoveTo(88, 8);
    OrbitOledDrawString("HIGH");
    OrbitOledMoveTo(84, 16);
    OrbitOledDrawString("SCORE");
  
    OrbitOledMoveTo(84,24);
    sprintf(scoreStr, "%05d", score);
    OrbitOledDrawString (scoreStr);
    OrbitOledUpdate();
    highScore=score;
  }
  else{
  OrbitOledClear();;
  //Display the current score
  OrbitOledSetCursor (3, 2);
  OrbitOledPutString ("SCORE:");
  OrbitOledSetCursor (10, 2);
  sprintf(scoreStr, "%05d", score);
  OrbitOledPutString (scoreStr);
  }


  long lBtn1;
  lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  while(lBtn1!=BTN1){
    updateLED(4);
    delay(100);
    updateLED(0);
    delay(100);
    lBtn1 = GPIOPinRead(BTN1Port, BTN1);
  } 
}
void setup(){     //run once initialization code here, i.e. OledInit() will probably go here
  DeviceInit();
  score=0;
  showStrtScrn();
  calibratedAccel=getAccelY();
  switch(CheckSwitches()){
    case 0: maxBullet=5;break;
    case 1: maxBullet=4;break;
    case 2:maxBullet=3;break;
    case 3: maxBullet=2;break;
    default: maxBullet=5;
  };
  lives=4;
  updateLED(lives);
  initStuff();
  OrbitOledClear();
  OrbitOledSetFillPattern(OrbitOledGetStdPattern(1));
}


void loop(){ //main game loop
  gameLoop();
  displayScore();
  setup();
}
