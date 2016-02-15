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
 

#define NUM_LEDS_PIN_1 1 // stair lights     
#define NUM_LEDS_PIN_2 4 // wall lights
#define NUM_LEDS_PIN_3 6 // TV wall lights

#define DATA_PIN_1 3 // stair lights
#define DATA_PIN_2 4 // wall lights
#define DATA_PIN_3 5 // TV wall lights


CRGB leds1[NUM_LEDS_PIN_1];
CRGB leds2[NUM_LEDS_PIN_2];
CRGB leds3[NUM_LEDS_PIN_3];
EthernetServer server(80);
WebApp app;
String mode;

long monoC1;
long triC1, triC2, triC3; // W S =
long colorArray1[NUM_LEDS_PIN_1], colorArray2[NUM_LEDS_PIN_2], colorArray3[NUM_LEDS_PIN_3];



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
  FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds1, NUM_LEDS_PIN_1);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds2, NUM_LEDS_PIN_2);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_3>(leds3, NUM_LEDS_PIN_3);



  //-------------------------------------------------------------------------------------- mount the handler to the routes
  app.get("/", &indexCmd);
  app.get("/mono", &monoColor);
  app.get("/tri", &triColor);
  app.get("/multi", &multiColor);
  app.get("/equalizer", &equalizerColor);
  app.get("/rainbow", &rainbowColor);
  app.get("/cats", &queryParams);

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
 * equalizerColor
 * -----------------------------------------------------------------------------------------
 */
void equalizerColor(Request &req, Response &res) {
  mode ="equalizer";
}

/* -----------------------------------------------------------------------------------------
 * rainbowColor
 * -----------------------------------------------------------------------------------------
 */
void rainbowColor(Request &req, Response &res) {
  mode ="rainbow";
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
         
        leds1[i] = monoC1;  //hex Colorcode
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
  
     
      for(int i =0; i < NUM_LEDS_PIN_1; i++)
      {
  
        if( ((i % 2) == 0) && (i < 5) )
        {
          leds1[i] = triC2;
        }
        else
        {
          leds1[i] = triC1;
        }
  
        if( (i >= 5) && (i < 17 ) )
        {
          leds1[i] = triC1;
        }
  
        if( ((i % 2) == 1) && (i >=17) )
        {
          leds1[i] = triC2;
        }
        else
        {
          leds1[i] = triC1;
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
    
    
    
    if(mode=="equalizer") //--------------------------------------------------------------------equalizer
    {
    Serial.println("equalizer processing");
    } //----------------------------------------------------------------------------------------equalizer
    
    
    if(mode=="rainbow") //----------------------------------------------------------------------rainbow
    {
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
          leds1[i] = colorArray1[i];
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
 * 
 * -----------------------------------------------------------------------------------------
 */

 

