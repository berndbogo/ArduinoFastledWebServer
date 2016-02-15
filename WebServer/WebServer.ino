#include <SPI.h>
#include <Ethernet.h>
#include <aWOT.h>
#include <FastLED.h>


#define NUM_LEDS 1
#define DATA_PIN_1 3
#define DATA_PIN_2 4
#define DATA_PIN_3 5


CRGB leds[NUM_LEDS];
EthernetServer server(80);
WebApp app;
String mode;

String monoC1,c1,c2,c3,c4,c5,c6;
String triC1, triC2, triC3;




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
  FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds, NUM_LEDS);



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
  mode ="mono";
  String color = req.query("color"); // "lolcat"
  res.success("text/plain");
  res.print(color);
  monoC1=color;
   
}

/* -----------------------------------------------------------------------------------------
 * triColor
 * -----------------------------------------------------------------------------------------
 */
void triColor(Request &req, Response &res) {
  mode ="tri";
  String color1 = req.query("color1"); // "lolcat"
  String color2 = req.query("color2");
  String color3 = req.query("color3");
  triC1=color1;
  triC2=color2;
  triC3=color3;
  res.success("text/plain");
  String colors = color1 + " " + color2+ " " + color3; 
  res.print( colors);
  //res.print(type2);
  
}

/* -----------------------------------------------------------------------------------------
 * 
 * -----------------------------------------------------------------------------------------
 */
void multiColor(Request &req, Response &res) {
  mode ="multi";
}

/* -----------------------------------------------------------------------------------------
 * 
 * -----------------------------------------------------------------------------------------
 */
void equalizerColor(Request &req, Response &res) {
  mode ="equalizer";
}

/* -----------------------------------------------------------------------------------------
 * 
 * -----------------------------------------------------------------------------------------
 */
void rainbowColor(Request &req, Response &res) {
  mode ="rainbow";
}

/* -----------------------------------------------------------------------------------------
 *    
 *  
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

    for(int i =0; i < NUM_LEDS; i++)
    {
      const char * c = monoC1.c_str();
      leds[i] = strtoul(c, NULL, 16);//0xFF44DD;  //hex Colorcode
    }  
    
    FastLED.show(); 
    
    Serial.println("monoColor processing");
    } //----------------------------------------------------------------------------------------mono
    
    
    if(mode=="tri") //--------------------------------------------------------------------------tri
    {
    Serial.println("triColor processing");
    } //----------------------------------------------------------------------------------------tri
    
    
    
    if(mode=="multi") //------------------------------------------------------------------------multi
    {
    Serial.println("multiColor processing");
    } //----------------------------------------------------------------------------------------multi
    
    
    
    if(mode=="equalizer") //--------------------------------------------------------------------equalizer
    {
    Serial.println("triColor processing");
    } //----------------------------------------------------------------------------------------equalizer
    
    
    if(mode=="rainbow") //----------------------------------------------------------------------rainbow
    {
    Serial.println("monoColor processing");
    } //----------------------------------------------------------------------------------------rainbow
    
    
    if(mode=="free") //-------------------------------------------------------------------------free
    {
    Serial.println("triColor processing");
    } //----------------------------------------------------------------------------------------free
 


}

