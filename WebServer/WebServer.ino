#include <SPI.h>
#include <Ethernet.h>
#include <aWOT.h>
#include <FastLED.h>
#include <string.h>  

/*  Layout  
 * 
 * 
 * S ... VERTICAL ELEMENT 
 * W ... HORIZONTAL ELEMENT 
 * E ... CORNER ELEMENT
 * P ... PLUG ELEMENT (NO LED)
 * = ... WALL ELEMENT
 * F ... FURNITURE ELEMENT
 * U ... UNTER FURNITURE Element
 * 
 * 
 * stair & wall lights 
 * 
 * 
 *                  WWPWWWWPEEPWWWW
 *               WS                SW
 *             WS                    SW  
 *           S                        SW 
 *                                      S
 *      =   ==                           =
 * 
 * 
 * 
 * 
 * 
 * TV wall lights
 *   
 *    UF UF UF 
 * 
 * 
 */
 

#define NUM_LEDS_PIN_1 24 // stair lights     
#define NUM_LEDS_PIN_2 4 // wall lights
#define NUM_LEDS_PIN_3 6 // TV wall lights

#define DATA_PIN_1 3 // stair lights
#define DATA_PIN_2 4 // wall lights
#define DATA_PIN_3 5 // TV wall lights


CRGB leds[NUM_LEDS_PIN_1];
CRGB leds2[NUM_LEDS_PIN_2];
CRGB leds3[NUM_LEDS_PIN_3];
EthernetServer server(80);
WebApp app;
String mode = "tri";
long ledMode = 888; 
long monoC1;
long triC1 = 0x0000FF, triC2 = 0x00FF00, triC3 = 0xffffff; // W S =
long colorArray1[NUM_LEDS_PIN_1], colorArray2[NUM_LEDS_PIN_2], colorArray3[NUM_LEDS_PIN_3];

/* ========================================================================================
 * 
 * Rainbow STUFF
 * 
 * ========================================================================================
 */
#define LED_COUNT 24    //maybe later more than the stairs
int BOTTOM_INDEX = 0;
int TOP_INDEX = int(LED_COUNT/2);
int EVENODD = LED_COUNT%2;
//struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)
 
//int ledMode = 888;         //-START IN DEMO MODE
//int ledMode = 23;            //-MODE TESTING

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR

//---SERIAL/SOFTWARE SERIAL SETUP STUFF
#define SERIAL_BAUDRATE 9600
byte inbyte;                 //-SERIAL INPUT BYTE
int thisarg;                 //-SERIAL INPUT ARG

//---LED FX VARS
int max_bright = 64;         //-SET MAX BRIGHTNESS TO 1/4
//int max_bright = 16;       //-SET MAX BRIGHTNESS FOR TESTING
int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int idx_offset = 0;          //-OFFSET INDEX (BOTTOM LED TO ZERO WHEN LOOP IS TURNED/DOESN'T REALLY WORK)
int ihue = 0;                //-HUE (0-255)
static uint8_t startinghue = 0;
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR








/* ========================================================================================
 * 
 * Setup-function
 * 
 * ========================================================================================
 */
 

void setup() {

  Serial.begin(9600);

  //-------------------------------------------------------------------------------------- IP-Setup
  byte ip[] =  {192, 168, 67, 178};
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());
  


  //-------------------------------------------------------------------------------------- LED-Setup
  FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds, NUM_LEDS_PIN_1);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds2, NUM_LEDS_PIN_2);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_3>(leds3, NUM_LEDS_PIN_3);
  //FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds, LED_COUNT);  // for rainbow  maybe later other pin and leds
  one_color_all(0,0,0); //-CLEAR STRIP
  LEDS.show();


  //-------------------------------------------------------------------------------------- mount the handler to the routes
  app.get("/", &indexCmd);
  app.get("/mono", &monoColor);
  app.get("/tri", &triColor);
  app.get("/multi", &multiColor);
  app.get("/visualizer", &visualizerColor);
  app.get("/rainbow", &rainbowColor);
  app.get("/cats", &queryParams);

  Serial.println("---SETUP COMPLETE---");

  
}

/* ========================================================================================
 * 
 * Loop-function
 * 
 * ========================================================================================
 */
 

void loop(){
  EthernetClient client = server.available();
  if (client){
      app.process(&client);
  }

  showLight();
}

/* ========================================================================================
 * 
 * Define some handler functions which set the different modes and colors 
 *  
 * 
 * ========================================================================================
 */

/* -----------------------------------------------------------------------------------------
 * Index-handler
 * -----------------------------------------------------------------------------------------
 */ 
 
void indexCmd(Request &req, Response &res) {
  // P macro for printing strings from program memory
  P(index) =
    "<html>\n"
    "<head>\n"
    "<title>Hello World!</title>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Greetings middle earth!</h1>\n"
    " <a href=\"http://192.168.67.178/mono?color=0x81F79F\">mono</a> \n"
    " <a href=\"http://192.168.67.178/tri?color1=ff0000&color2=00ff00&color3=DD013A\">tri</a> \n"
    " <a href=\"http://192.168.67.178/rainbow?colorA1=ff0000&colorA2=ff0000&colorA3=ff0000&colorA4=ff0000&colorA5=ff0FF0&colorA6=ff0000&colorA7=ff0FF0&colorA8=ff0000&colorA9=ff0000&colorA10=ff0000&colorA11=ff0000&colorA12=ff0000&colorA13=ff0000&colorA14=ff0000&colorA15=ff0000&colorA16=ff0000&colorA17=ff0000&colorA18=ff0000&colorA19=ff0000&colorA20=ff0000&colorA21=ff0000&colorA22=ff0000&colorA23=ff0000&colorA24=ff0000\">rainbow mode1 >multi</a>\n"
    " <a href=\"http://192.168.67.178/rainbow?mode=1\">rainbow mode1 </a> \n"
    " <a href=\"http://192.168.67.178/rainbow?mode=3\">rainbow mode3 </a> \n"
    " <a href=\"http://192.168.67.178/rainbow?mode=888\">rainbow demo </a> \n"
    "</body>\n"
    "</html>";

  res.success("text/html");
  res.printP(index);

}

/* -----------------------------------------------------------------------------------------
 * monoColor
 * -----------------------------------------------------------------------------------------
 */
 
void monoColor(Request &req, Response &res) {

  String helpMonoC1 = req.query("color");  
  const char *c = helpMonoC1.c_str();
  
  mode ="mono";
  monoC1 = strtoul(c, NULL, 16);
  
  res.success("text/plain");
  res.print(helpMonoC1);
  
}

/* -----------------------------------------------------------------------------------------
 * triColor
 * -----------------------------------------------------------------------------------------
 */
 
void triColor(Request &req, Response &res) {
  
  String helpTriC1 = req.query("color1");  // W
  String helpTriC2 = req.query("color2");  // S
  String helpTriC3 = req.query("color3");  // = 
  String triColors = helpTriC1 + " " + helpTriC2+ " " + helpTriC3; 

  triC1 = strtoul(helpTriC1.c_str(), NULL, 16);
  triC2 = strtoul(helpTriC2.c_str(), NULL, 16);
  triC3 = strtoul(helpTriC3.c_str(), NULL, 16);
  
  mode ="tri";

  
  res.success("text/plain");
  
  res.print(triColors);
  
  
}

/* -----------------------------------------------------------------------------------------
 *  multiColor   
 *  
 *  colorA for colorArray1  
 *  colorB for colorArray2 
 *  colorC for colorArray3 
 * -----------------------------------------------------------------------------------------
 */
 
void multiColor(Request &req, Response &res) {

  for(int i = 0; i < NUM_LEDS_PIN_1; i++)
  {
    String help = "colorA" + String(i);
    String result = req.query(help.c_str());
    colorArray1[i] = strtoul(result.c_str(), NULL, 16);   
  }
  
  for(int i = 0; i < NUM_LEDS_PIN_2; i++)
  {
    String help = "colorB" + String(i);
    String result = req.query(help.c_str());
    colorArray2[i] = strtoul(result.c_str(), NULL, 16);  
  }
  
  for(int i = 0; i < NUM_LEDS_PIN_3; i++)
  {
    String help = "colorC" + String(i);
    String result = req.query(help.c_str());
    colorArray3[i] = strtoul(result.c_str(), NULL, 16);  
  }
  
  mode ="multi";

  res.success("text/plain");
  
  res.print("multi");
}

/* -----------------------------------------------------------------------------------------
 * visualizerColor
 * -----------------------------------------------------------------------------------------
 */
void visualizerColor(Request &req, Response &res) {
  mode ="visualizer";
}

/* -----------------------------------------------------------------------------------------
 * rainbowColor
 * -----------------------------------------------------------------------------------------
 */
void rainbowColor(Request &req, Response &res) {
  

  String helpRainbow = req.query("mode");  
  const char *c = helpRainbow.c_str();
  
  mode ="rainbow";
  ledMode = strtoul(c, NULL, 10);
  

  
}

/* -----------------------------------------------------------------------------------------
 *  freeControlColor 
 *  -----------------------------------------------------------------------------------------
 */
void freeControlColor(Request &req, Response &res) {
  mode ="free";
}

/* -----------------------------------------------------------------------------------------
 * 
 * -----------------------------------------------------------------------------------------
 */

void queryParams(Request &req, Response &res) {
  String type = req.query("type"); // "lolcat"
  String type2 = req.query("type2");
  res.success("text/plain");
  String type3 = type + " " + type2; 
  res.print( type3);
  //res.print(type2);
}



 

/* =============================================================================================
 * 
 * processing the different light modes
 * 
 * =============================================================================================
 */
 
void showLight(){

    if(mode=="mono") //-------------------------------------------------------------------------mono
    {

      for(int i =0; i < NUM_LEDS_PIN_1; i++)
      {
         
        leds[i] = monoC1;  //hex Colorcode
      }  
      
      for(int i =0; i < NUM_LEDS_PIN_2; i++)
      {
          
        leds2[i] = monoC1;
      } 
      
      for(int i =0; i < NUM_LEDS_PIN_3; i++)
      {
         
        leds3[i] = monoC1;
      } 
      
      FastLED.show(); 
    
      Serial.println("monoColor processing");
    } //----------------------------------------------------------------------------------------mono


    /*  Layout   + index
     * 
     *                  5  6  X  7  8  9  10 X 11 12  X 13 14 15  16
     *                  W  W  P  W  W  W  W  P  E  E  P  W  W  W  W
     *     3 4        WS                                           SW     17 18
     *     1 2      WS                                               SW   19 20
     *     0      S                                                    SW 21 22
     *                                                                  S 23
     *      =   ==                                                       =
     * 
     * 
     */

 
    
    if(mode=="tri") //--------------------------------------------------------------------------tri
    {

      //    W triC1          S triC2        =  triC3
  

      /*for(int i =0; i < NUM_LEDS_PIN_1; i++)
      {
        leds[i] = 0x000000;
      }*/
      
      for(int i =0; i < NUM_LEDS_PIN_1; i++)
      {
  
        if( ((i % 2) == 0) && (i < 5) )
        {
          leds[i] = triC2;
        }
        else if( ((i % 2) == 1) && (i < 5))
        {
          leds[i] = triC1;
        }
  
        if( (i >= 5) && (i < 17 ) )
        {
          leds[i] = triC1;
        }
  
        if( ((i % 2) == 1) && (i >=17) )
        {
          leds[i] = triC2;
        }
        else if( ((i % 2) == 0) && (i >=17) )
        {
          leds[i] = triC1;
        } 
         
      } 
  
      for(int i =0; i < NUM_LEDS_PIN_2; i++)
      {
  
        if( (i % 2) == 0 )
        {
          leds2[i] = triC2;
        }
        else{
          leds2[i] = triC1;
        }
        
      } 
  
      for(int i =0; i < NUM_LEDS_PIN_3; i++)
      {
         
        leds3[i] = triC3;
      } 
  
      FastLED.show(); 
  
      Serial.println("triColor processing");
    } //----------------------------------------------------------------------------------------tri
    
    
    
    if(mode=="multi") //------------------------------------------------------------------------multi
    {

      assignMuliColors();
      FastLED.show();
      Serial.println("multiColor processing");
    } //----------------------------------------------------------------------------------------multi
    
    
    
    if(mode=="visualizer") //--------------------------------------------------------------------visualizer
    {
    Serial.println("visualizer processing");
    } //----------------------------------------------------------------------------------------visualizer
    
    
    if(mode=="rainbow") //----------------------------------------------------------------------rainbow
    {
      if (ledMode == 0) {one_color_all(0,0,0);}            //---STRIP OFF
      if (ledMode == 1) {one_color_all(255,255,255);}      //---STRIP SOLID WHITE
      if (ledMode == 2) {rainbow_fade(); }
      if (ledMode == 3) {rainbow_loop(); }
      if (ledMode == 4) {random_burst(); }
      if (ledMode == 5) {color_bounce();}
      if (ledMode == 6) {color_bounceFADE();}
      if (ledMode == 7) {police_lightsONE();}
      if (ledMode == 8) {police_lightsALL();}
      if (ledMode == 9) {flicker();}
      if (ledMode == 10) {pulse_one_color_all();}
      if (ledMode == 11) {pulse_one_color_all_rev();}
      if (ledMode == 12) {fade_vertical();}
      if (ledMode == 13) {rule30();}
      if (ledMode == 14) {random_march();}
      if (ledMode == 15) {rwb_march();}
      if (ledMode == 16) {radiation();}
      if (ledMode == 17) {color_loop_vardelay();}
      if (ledMode == 18) {white_temps();}
      if (ledMode == 19) {sin_bright_wave();}
      if (ledMode == 20) {pop_horizontal();}
      if (ledMode == 21) {quad_bright_curve();}
      if (ledMode == 22) {flame();}
      if (ledMode == 23) {rainbow_vertical();}
      if (ledMode == 24) {pacman();}
      
      if (ledMode == 50) {strip_march_ccw();}
      if (ledMode == 51) {strip_march_cw();}
      
      if (ledMode == 88) {new_rainbow_loop();}
    
      if (ledMode == 101) {one_color_all(255,0,0);}        //---SOLID RED
      if (ledMode == 102) {one_color_all(0,255,0);}        //---SOLID GREEN
      if (ledMode == 103) {one_color_all(0,0,255);}        //---SOLID BLUE
      if (ledMode == 104) {one_color_all(255,255,0);}      //---SOLID YELLOW
      if (ledMode == 105) {one_color_all(0,255,255);}      //---SOLID TEAL?
      if (ledMode == 106) {one_color_all(255,0,255);}      //---SOLID VIOLET?
    
      if (ledMode == 888) {demo_mode();}                   //---DEMO ALL MODE (BLOCKS SERIAL INPUT)
    
     
    
      //---PROCESS HARDWARE SERIAL COMMANDS AND ARGS
      if (Serial.available() > 0) {inbyte = Serial.read();
        if (inbyte == 100) { //---"d" - SET DELAY VAR
          thisarg = Serial.parseInt();
          Serial.print("SET DELAY VARIABLE: ");
          Serial.println(thisarg);
          thisdelay = thisarg;
        }
        if (inbyte == 115) { //---"s" - SET STEP VAR
          thisarg = Serial.parseInt();
          Serial.print("SET STEP VARIABLE: ");
          Serial.println(thisarg);
          thisstep = thisarg;
        }
        if (inbyte == 104) { //---"h" - SET HUE VAR
          thisarg = Serial.parseInt();
          Serial.print("SET HUE VARIABLE: ");
          Serial.println(thisarg);
          thishue = thisarg;
        }
        if (inbyte == 116) { //---"h" - SET SATURATION VAR
          thisarg = Serial.parseInt();
          Serial.print("SET SATURATION VARIABLE: ");
          Serial.println(thisarg);
          thissat = thisarg;
        }    
        if (inbyte == 109) { //---"m" - SET LED FX MODE
          thisarg = Serial.parseInt();      
          Serial.print("SET NEW MODE: ");
          Serial.println(thisarg);
          change_mode(thisarg);
        }    
        if (inbyte == 108) { //---"l" - SET SINGLE LED VALUE
          if (ledMode != 999) {
            one_color_all(0,0,0);
            ledMode = 999;
          }
          int thisindex = Serial.parseInt();
          int thisRED = Serial.parseInt();
          int thisGRN = Serial.parseInt();
          int thisBLU = Serial.parseInt();      
          set_color_led(thisindex, thisRED, thisGRN, thisBLU);
          LEDS.show();
          Serial.print("SET LED COLOR: ");
          Serial.print(thisindex);
          Serial.print(thisRED);
          Serial.print(thisGRN);
          Serial.println(thisBLU);
        }
        if (inbyte == 98) { //---"b" - SET MAX BRIGHTNESS
          max_bright = Serial.parseInt();
          LEDS.setBrightness(max_bright);
          Serial.print("SET MAX BRIGHTNESS: ");
          Serial.println(max_bright);
        }    
      }
     
    Serial.println("rainbow  processing");
    } //----------------------------------------------------------------------------------------rainbow
    
    
    if(mode=="free") //-------------------------------------------------------------------------free
    {
    Serial.println("free processing");
    } //----------------------------------------------------------------------------------------free
 


}



/* =============================================================================================
 * 
 *  Help functions
 * 
 * =============================================================================================
 */


 /* -----------------------------------------------------------------------------------------
 *   set Colors with the colorArrays
 * -----------------------------------------------------------------------------------------
 */

void assignMuliColors(){
  
  for(int i =0; i < NUM_LEDS_PIN_1; i++)
      {
          leds[i] = colorArray1[i];
      } 
  
      for(int i =0; i < NUM_LEDS_PIN_2; i++)
      {
  
        leds2[i] = colorArray2[i];
        
      } 
  
      for(int i =0; i < NUM_LEDS_PIN_3; i++)
      {
         
        leds3[i] = colorArray3[i];
      }     
}

/* -----------------------------------------------------------------------------------------
 *  rainbow functions
 * -----------------------------------------------------------------------------------------
 */

//------------------------------------- UTILITY FXNS --------------------------------------
//---SET THE COLOR OF A SINGLE RGB LED
void set_color_led(int adex, int cred, int cgrn, int cblu) {  
  int bdex;  
  if (idx_offset > 0) {  //-APPLY INDEX OFFSET 
    bdex = (adex + idx_offset) % LED_COUNT;
  }
  else {bdex = adex;}
    leds[bdex].setRGB( cred, cgrn, cblu);
  } 

//---FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
  //-ONLY WORKS WITH INDEX < TOPINDEX
  if (i == BOTTOM_INDEX) {return BOTTOM_INDEX;}
  if (i == TOP_INDEX && EVENODD == 1) {return TOP_INDEX + 1;}
  if (i == TOP_INDEX && EVENODD == 0) {return TOP_INDEX;}
  return LED_COUNT - i;  
}

//---FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {iN = ( i + TOP_INDEX ) % LED_COUNT; }
  return iN;
}

//---FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
  int r;
  if (i < LED_COUNT - 1) {r = i + 1;}
  else {r = 0;}
  return r;
}

//---FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {r = i - 1;}
  else {r = LED_COUNT - 1;}
  return r;
}

void copy_led_array(){
  for(int i = 0; i < LED_COUNT; i++ ) {
    ledsX[i][0] = leds[i].r;
    ledsX[i][1] = leds[i].g;
    ledsX[i][2] = leds[i].b;
  }  
}


//------------------------LED EFFECT FUNCTIONS------------------------
void one_color_all(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
    for(int i = 0 ; i < LED_COUNT; i++ ) {
      set_color_led(i, cred, cgrn, cblu);
    }
    LEDS.show();
}

void rainbow_fade() {                         //-m2-FADE ALL LEDS THROUGH HSV RAINBOW
    ihue++;
    if (ihue > 255) {ihue = 0;}
    for(int idex = 0 ; idex < LED_COUNT; idex++ ) {
      leds[idex] = CHSV(ihue, thissat, 255);
    }
    LEDS.show();    
    delay(thisdelay);
}

void rainbow_loop() {                        //-m3-LOOP HSV RAINBOW
  idex++;
  ihue = ihue + thisstep;
  if (idex >= LED_COUNT) {idex = 0;}
  if (ihue > 255) {ihue = 0;}
  leds[idex] = CHSV(ihue, thissat, 255);
  LEDS.show();
  delay(thisdelay);
}

void random_burst() {                         //-m4-RANDOM INDEX/COLOR
  idex = random(0, LED_COUNT);
  ihue = random(0, 255);  
  leds[idex] = CHSV(ihue, thissat, 255);
  LEDS.show();
  delay(thisdelay);
}

void color_bounce() {                        //-m5-BOUNCE COLOR (SINGLE LED)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == LED_COUNT) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }  
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {leds[i] = CHSV(thishue, thissat, 255);}
    else {leds[i] = CHSV(0, 0, 0);}
  }
  LEDS.show();
  delay(thisdelay);
}

void color_bounceFADE() {                    //-m6-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == LED_COUNT) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  int iL1 = adjacent_cw(idex);
  int iL2 = adjacent_cw(iL1);
  int iL3 = adjacent_cw(iL2);
  int iR1 = adjacent_ccw(idex);
  int iR2 = adjacent_ccw(iR1);
  int iR3 = adjacent_ccw(iR2);
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {leds[i] = CHSV(thishue, thissat, 255);}
    else if (i == iL1) {leds[i] = CHSV(thishue, thissat, 150);}
    else if (i == iL2) {leds[i] = CHSV(thishue, thissat, 80);}
    else if (i == iL3) {leds[i] = CHSV(thishue, thissat, 20);}        
    else if (i == iR1) {leds[i] = CHSV(thishue, thissat, 150);}
    else if (i == iR2) {leds[i] = CHSV(thishue, thissat, 80);}
    else if (i == iR3) {leds[i] = CHSV(thishue, thissat, 20);}    
    else {leds[i] = CHSV(0, 0, 0);}
  }
  LEDS.show();
  delay(thisdelay);
}

void police_lightsONE() {                    //-m7-POLICE LIGHTS (TWO COLOR SINGLE LED)
  idex++;
  if (idex >= LED_COUNT) {idex = 0;}
  int idexR = idex;
  int idexB = antipodal_index(idexR);
  int thathue = (thishue + 160) % 255;  
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idexR) {leds[i] = CHSV(thishue, thissat, 255);}
    else if (i == idexB) {leds[i] = CHSV(thathue, thissat, 255);}    
    else {leds[i] = CHSV(0, 0, 0);}
  }
  LEDS.show();  
  delay(thisdelay);
}

void police_lightsALL() {                  //-m8-POLICE LIGHTS (TWO COLOR SOLID)
  idex++;
  if (idex >= LED_COUNT) {idex = 0;}
  int idexR = idex;
  int idexB = antipodal_index(idexR);
  int thathue = (thishue + 160) % 255;
  leds[idexR] = CHSV(thishue, thissat, 255);
  leds[idexB] = CHSV(thathue, thissat, 255);  
  LEDS.show();  
  delay(thisdelay);
}

void flicker() {                          //-m9-FLICKER EFFECT
  int random_bright = random(0,255);
  int random_delay = random(10,100);
  int random_bool = random(0,random_bright);
  if (random_bool < 10) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = CHSV(thishue, thissat, random_bright);
    }
    LEDS.show();
    delay(random_delay);
  }
}

void pulse_one_color_all() {              //-m10-PULSE BRIGHTNESS ON ALL LEDS TO ONE COLOR 
  if (bouncedirection == 0) {
    ibright++;
    if (ibright >= 255) {bouncedirection = 1;}
  }
  if (bouncedirection == 1) {
    ibright = ibright - 1;
    if (ibright <= 1) {bouncedirection = 0;}         
  }  
    for(int idex = 0 ; idex < LED_COUNT; idex++ ) {
      leds[idex] = CHSV(thishue, thissat, ibright);
    }
    LEDS.show();    
    delay(thisdelay);
}

void pulse_one_color_all_rev() {           //-m11-PULSE SATURATION ON ALL LEDS TO ONE COLOR 
  if (bouncedirection == 0) {
    isat++;
    if (isat >= 255) {bouncedirection = 1;}
  }
  if (bouncedirection == 1) {
    isat = isat - 1;
    if (isat <= 1) {bouncedirection = 0;}         
  }  
    for(int idex = 0 ; idex < LED_COUNT; idex++ ) {
      leds[idex] = CHSV(thishue, isat, 255);
    }
    LEDS.show();
    delay(thisdelay);
}

void fade_vertical() {                    //-m12-FADE 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {idex = 0;}  
  int idexA = idex;
  int idexB = horizontal_index(idexA);
  ibright = ibright + 10;
  if (ibright > 255) {ibright = 0;}
  leds[idexA] = CHSV(thishue, thissat, ibright);
  leds[idexB] = CHSV(thishue, thissat, ibright);
  LEDS.show();
  delay(thisdelay);
}

void random_red() {                       //QUICK 'N DIRTY RANDOMIZE TO GET CELL AUTOMATA STARTED  
  int temprand;
  for(int i = 0; i < LED_COUNT; i++ ) {
    temprand = random(0,100);
    if (temprand > 50) {leds[i].r = 255;}
    if (temprand <= 50) {leds[i].r = 0;}
    leds[i].b = 0; leds[i].g = 0;
  }
  LEDS.show();  
}

void rule30() {                          //-m13-1D CELLULAR AUTOMATA - RULE 30 (RED FOR NOW)
  if (bouncedirection == 0) {
    random_red();
    bouncedirection = 1;
  }
  copy_led_array();
  int iCW;
  int iCCW;
  int y = 100;  
  for(int i = 0; i < LED_COUNT; i++ ) {
    iCW = adjacent_cw(i);
    iCCW = adjacent_ccw(i);
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] > y) {leds[i].r = 0;}
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {leds[i].r = 0;}
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {leds[i].r = 0;}
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] > y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {leds[i].r = 0;}
  }
  LEDS.show();  
  delay(thisdelay);
}

void random_march() {                   //-m14-RANDOM MARCH CCW
  copy_led_array();
  int iCCW;
  leds[0] = CHSV(random(0,255), 255, 255);
  for(int idex = 1; idex < LED_COUNT ; idex++ ) {
    iCCW = adjacent_ccw(idex);
    leds[idex].r = ledsX[iCCW][0];
    leds[idex].g = ledsX[iCCW][1];
    leds[idex].b = ledsX[iCCW][2];
  }
  LEDS.show();  
  delay(thisdelay);
}

void rwb_march() {                    //-m15-R,W,B MARCH CCW
  copy_led_array();
  int iCCW;
  idex++;
  if (idex > 2) {idex = 0;}
  switch (idex) {
    case 0:
      leds[0].r = 255;
      leds[0].g = 0;
      leds[0].b = 0;
    break;
    case 1:
      leds[0].r = 255;
      leds[0].g = 255;
      leds[0].b = 255;
    break;
    case 2:
      leds[0].r = 0;
      leds[0].g = 0;
      leds[0].b = 255;
    break;
  }
    for(int i = 1; i < LED_COUNT; i++ ) {
      iCCW = adjacent_ccw(i);
      leds[i].r = ledsX[iCCW][0];
      leds[i].g = ledsX[iCCW][1];
      leds[i].b = ledsX[iCCW][2];    
    }
  LEDS.show();  
  delay(thisdelay);
}

void radiation() {                   //-m16-SORT OF RADIATION SYMBOLISH- 
  int N3  = int(LED_COUNT/3);
  int N6  = int(LED_COUNT/6);  
  int N12 = int(LED_COUNT/12);  
  for(int i = 0; i < N6; i++ ) {     //-HACKY, I KNOW...
    tcount = tcount + .02;
    if (tcount > 3.14) {tcount = 0.0;}
    ibright = int(sin(tcount)*255);    
    int j0 = (i + LED_COUNT - N12) % LED_COUNT;
    int j1 = (j0+N3) % LED_COUNT;
    int j2 = (j1+N3) % LED_COUNT;    
    leds[j0] = CHSV(thishue, thissat, ibright);
    leds[j1] = CHSV(thishue, thissat, ibright);
    leds[j2] = CHSV(thishue, thissat, ibright);    
  }    
    LEDS.show();    
    delay(thisdelay);    
}

void color_loop_vardelay() {                    //-m17-COLOR LOOP (SINGLE LED) w/ VARIABLE DELAY
  idex++;
  if (idex > LED_COUNT) {idex = 0;}
  int di = abs(TOP_INDEX - idex);
  int t = constrain((10/di)*10, 10, 500);
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {
      leds[i] = CHSV(0, thissat, 255);
    }
    else {
      leds[i].r = 0; leds[i].g = 0; leds[i].b = 0;
    }
  }
  LEDS.show();  
  delay(t);
}

void white_temps() {                            //-m18-SHOW A SAMPLE OF BLACK BODY RADIATION COLOR TEMPERATURES
  int N9 = int(LED_COUNT/9);
  for (int i = 0; i < LED_COUNT; i++ ) {
    if (i >= 0 && i < N9) {leds[i].r = 255; leds[i].g = 147; leds[i].b = 41;}         //-CANDLE - 1900
    if (i >= N9 && i < N9*2) {leds[i].r = 255; leds[i].g = 197; leds[i].b = 143;}     //-40W TUNG - 2600
    if (i >= N9*2 && i < N9*3) {leds[i].r = 255; leds[i].g = 214; leds[i].b = 170;}   //-100W TUNG - 2850
    if (i >= N9*3 && i < N9*4) {leds[i].r = 255; leds[i].g = 241; leds[i].b = 224;}   //-HALOGEN - 3200
    if (i >= N9*4 && i < N9*5) {leds[i].r = 255; leds[i].g = 250; leds[i].b = 244;}   //-CARBON ARC - 5200
    if (i >= N9*5 && i < N9*6) {leds[i].r = 255; leds[i].g = 255; leds[i].b = 251;}   //-HIGH NOON SUN - 5400
    if (i >= N9*6 && i < N9*7) {leds[i].r = 255; leds[i].g = 255; leds[i].b = 255;}   //-DIRECT SUN - 6000
    if (i >= N9*7 && i < N9*8) {leds[i].r = 201; leds[i].g = 226; leds[i].b = 255;}   //-OVERCAST SKY - 7000
    if (i >= N9*8 && i < LED_COUNT) {leds[i].r = 64; leds[i].g = 156; leds[i].b = 255;}//-CLEAR BLUE SKY - 20000  
  }
  LEDS.show();  
  delay(100);
}

void sin_bright_wave() {        //-m19-BRIGHTNESS SINE WAVE
  for(int i = 0; i < LED_COUNT; i++ ) {
    tcount = tcount + .1;
    if (tcount > 3.14) {tcount = 0.0;}
    ibright = int(sin(tcount)*255);
    leds[i] = CHSV(thishue, thissat, ibright);
    LEDS.show();    
    delay(thisdelay);
  }
}

void pop_horizontal() {        //-m20-POP FROM LEFT TO RIGHT UP THE RING
  int ix;
  if (bouncedirection == 0) {
    bouncedirection = 1;
    ix = idex;  
  }
  else if (bouncedirection == 1) {
    bouncedirection = 0;
    ix = horizontal_index(idex);
    idex++;
    if (idex > TOP_INDEX) {idex = 0;}      
  }
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == ix) {
      leds[i] = CHSV(thishue, thissat, 255);
    }
    else {
      leds[i].r = 0; leds[i].g = 0; leds[i].b = 0;
    }
  }
  LEDS.show();
  delay(thisdelay);    
}

void quad_bright_curve() {      //-m21-QUADRATIC BRIGHTNESS CURVER
  int ax;    
  for(int x = 0; x < LED_COUNT; x++ ) {
    if (x <= TOP_INDEX) {ax = x;}
    else if (x > TOP_INDEX) {ax = LED_COUNT-x;}
    int a = 1; int b = 1; int c = 0;
    int iquad = -(ax*ax*a)+(ax*b)+c; //-ax2+bx+c
    int hquad = -(TOP_INDEX*TOP_INDEX*a)+(TOP_INDEX*b)+c;
    ibright = int((float(iquad)/float(hquad))*255);
    leds[x] = CHSV(thishue, thissat, ibright);
  }
  LEDS.show();  
  delay(thisdelay);
}

void flame() {                                    //-m22-FLAMEISH EFFECT
  int idelay = random(0,35);
  float hmin = 0.1; float hmax = 45.0;
  float hdif = hmax-hmin;
  int randtemp = random(0,3);
  float hinc = (hdif/float(TOP_INDEX))+randtemp;
  int ihue = hmin;
  for(int i = 0; i <= TOP_INDEX; i++ ) {
    ihue = ihue + hinc;
    leds[i] = CHSV(ihue, thissat, 255);
    int ih = horizontal_index(i);    
    leds[ih] = CHSV(ihue, thissat, 255);    
    leds[TOP_INDEX].r = 255; leds[TOP_INDEX].g = 255; leds[TOP_INDEX].b = 255;    
    LEDS.show();    
    delay(idelay);
  }
}

void rainbow_vertical() {                        //-m23-RAINBOW 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {idex = 0;}  
  ihue = ihue + thisstep;
  if (ihue > 255) {ihue = 0;}
  int idexA = idex;
  int idexB = horizontal_index(idexA);
  leds[idexA] = CHSV(ihue, thissat, 255);
  leds[idexB] = CHSV(ihue, thissat, 255);
  LEDS.show();  
  delay(thisdelay);
}

void pacman() {                                  //-m24-REALLY TERRIBLE PACMAN CHOMPING EFFECT
  int s = int(LED_COUNT/4);
  lcount++;
  if (lcount > 5) {lcount = 0;}
  if (lcount == 0) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {set_color_led(i, 255, 255, 0);}
    }
  if (lcount == 1 || lcount == 5) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {set_color_led(i, 255, 255, 0);}
    leds[s].r = 0; leds[s].g = 0; leds[s].b = 0;}
  if (lcount == 2 || lcount == 4) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {set_color_led(i, 255, 255, 0);}
    leds[s-1].r = 0; leds[s-1].g = 0; leds[s-1].b = 0;  
    leds[s].r = 0; leds[s].g = 0; leds[s].b = 0;
    leds[s+1].r = 0; leds[s+1].g = 0; leds[s+1].b = 0;}        
  if (lcount == 3) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {set_color_led(i, 255, 255, 0);}
    leds[s-2].r = 0; leds[s-2].g = 0; leds[s-2].b = 0;  
    leds[s-1].r = 0; leds[s-1].g = 0; leds[s-1].b = 0;  
    leds[s].r = 0; leds[s].g = 0; leds[s].b = 0;
    leds[s+1].r = 0; leds[s+1].g = 0; leds[s+1].b = 0;
    leds[s+2].r = 0; leds[s+2].g = 0; leds[s+2].b = 0;}
  LEDS.show();  
  delay(thisdelay);
}

void strip_march_cw() {                        //-m50-MARCH STRIP CW
  copy_led_array();
  int iCCW;  
  for(int i = 0; i < LED_COUNT; i++ ) {
    iCCW = adjacent_ccw(i);
    leds[i].r = ledsX[iCCW][0];
    leds[i].g = ledsX[iCCW][1];
    leds[i].b = ledsX[iCCW][2];    
  }
  LEDS.show();  
  delay(thisdelay);
}

void strip_march_ccw() {                        //-m51-MARCH STRIP CCW 
  copy_led_array();
  int iCW;  
  for(int i = 0; i < LED_COUNT; i++ ) {
    iCW = adjacent_cw(i);
    leds[i].r = ledsX[iCW][0];
    leds[i].g = ledsX[iCW][1];
    leds[i].b = ledsX[iCW][2];    
  }
  LEDS.show();  
  delay(thisdelay);
}

void new_rainbow_loop(){                       //-m88-RAINBOW FADE FROM FAST_SPI2
  ihue -= 1;
  fill_rainbow( leds, LED_COUNT, ihue );
  LEDS.show();
  delay(thisdelay);
}

void demo_mode(){
  int r = 10;
  thisdelay = 20; thisstep = 10; thishue = 0; thissat = 255;
  for(int i=0; i<r*3; i++) {one_color_all(255,255,255);}
  for(int i=0; i<r*25; i++) {rainbow_fade();}
  for(int i=0; i<r*20; i++) {rainbow_loop();}
  for(int i=0; i<r*20; i++) {random_burst();}
  for(int i=0; i<r*12; i++) {color_bounce();}
  thisdelay = 40;
  for(int i=0; i<r*12; i++) {color_bounceFADE();}
  for(int i=0; i<r*6; i++) {police_lightsONE();}
  for(int i=0; i<r*5; i++) {police_lightsALL();}
  thishue = 160; thissat = 50;  
  for(int i=0; i<r*40; i++) {flicker();}
  one_color_all(0,0,0);
  thisdelay = 15; thishue = 0; thissat = 255;
  for(int i=0; i<r*50; i++) {pulse_one_color_all();}
  for(int i=0; i<r*40; i++) {pulse_one_color_all_rev();}
  thisdelay = 60; thishue = 180; 
  for(int i=0; i<r*5; i++) {fade_vertical();}
  random_red();
  thisdelay = 100;
  for(int i=0; i<r*5; i++) {rule30();}
  thisdelay = 40;
  for(int i=0; i<r*8; i++) {random_march();}
  thisdelay = 80;
  for(int i=0; i<r*5; i++) {rwb_march();}
  one_color_all(0,0,0);
  thisdelay = 60; thishue = 95; 
  for(int i=0; i<r*15; i++) {radiation();}
  for(int i=0; i<r*15; i++) {color_loop_vardelay();}
  for(int i=0; i<r*5; i++) {white_temps();}
  thisdelay = 35; thishue = 180; 
  for(int i=0; i<r; i++) {sin_bright_wave();}
  thisdelay = 100; thishue = 0; 
  for(int i=0; i<r*5; i++) {pop_horizontal();}
  thisdelay = 100; thishue = 180; 
  for(int i=0; i<r*4; i++) {quad_bright_curve();}
  one_color_all(0,0,0);
  for(int i=0; i<r*3; i++) {flame();}
  thisdelay = 50;
  for(int i=0; i<r*10; i++) {pacman();}
  thisdelay = 50; thisstep = 15;
  for(int i=0; i<r*15; i++) {rainbow_vertical();}
  thisdelay = 100;
  for(int i=0; i<r*3; i++) {strip_march_ccw();}
  for(int i=0; i<r*3; i++) {strip_march_cw();}  
  for(int i=0; i<r*3; i++) {one_color_all(255,0,0); delay(35);}
  for(int i=0; i<r*3; i++) {one_color_all(0,255,0); delay(35);}
  for(int i=0; i<r*3; i++) {one_color_all(0,0,255); delay(35);}
  for(int i=0; i<r*3; i++) {one_color_all(255,255,0); delay(35);}
  for(int i=0; i<r*3; i++) {one_color_all(0,255,255); delay(35);}
  for(int i=0; i<r*3; i++) {one_color_all(255,0,255); delay(35);}
  thisdelay = 5;
  for(int i=0; i<r*120; i++) {new_rainbow_loop();}  
}

void change_mode(int newmode){
  thissat = 255;
  if (newmode == 2) {thisdelay = 20;}                      //---STRIP RAINBOW FADE
  if (newmode == 3) {thisdelay = 20; thisstep = 10;}       //---RAINBOW LOOP
  if (newmode == 4) {thisdelay = 20;}                      //---RANDOM BURST
  if (newmode == 5) {thisdelay = 20; thishue = 0;}         //---CYLON v1
  if (newmode == 6) {thisdelay = 40; thishue = 0;}         //---CYLON v2
  if (newmode == 7) {thisdelay = 40; thishue = 0;}         //---POLICE LIGHTS SINGLE
  if (newmode == 8) {thisdelay = 40; thishue = 0;}         //---POLICE LIGHTS SOLID
  if (newmode == 9) {thishue = 160; thissat = 50;}         //---STRIP FLICKER
  if (newmode == 10) {thisdelay = 15; thishue = 0;}        //---PULSE COLOR BRIGHTNESS
  if (newmode == 11) {thisdelay = 15; thishue = 0;}        //---PULSE COLOR SATURATION
  if (newmode == 12) {thisdelay = 60; thishue = 180;}      //---VERTICAL SOMETHING
  if (newmode == 13) {thisdelay = 100;}                    //---CELL AUTO - RULE 30 (RED)
  if (newmode == 14) {thisdelay = 40;}                     //---MARCH RANDOM COLORS
  if (newmode == 15) {thisdelay = 80;}                     //---MARCH RWB COLORS
  if (newmode == 16) {thisdelay = 60; thishue = 95;}       //---RADIATION SYMBOL
  //---PLACEHOLDER FOR COLOR LOOP VAR DELAY VARS
  if (newmode == 19) {thisdelay = 35; thishue = 180;}      //---SIN WAVE BRIGHTNESS
  if (newmode == 20) {thisdelay = 100; thishue = 0;}       //---POP LEFT/RIGHT
  if (newmode == 21) {thisdelay = 100; thishue = 180;}     //---QUADRATIC BRIGHTNESS CURVE
  //---PLACEHOLDER FOR FLAME VARS
  if (newmode == 23) {thisdelay = 50; thisstep = 15;}      //---VERITCAL RAINBOW
  if (newmode == 24) {thisdelay = 50;}                     //---PACMAN
  if (newmode == 50) {thisdelay = 100;}                    //---MARCH STRIP NOW CCW
  if (newmode == 51) {thisdelay = 100;}                    //---MARCH STRIP NOW CCW  
  if (newmode == 88) {thisdelay = 5;}                      //---NEW RAINBOW LOOP
  bouncedirection = 0;
  one_color_all(0,0,0);
  ledMode = newmode;
}



 

 

