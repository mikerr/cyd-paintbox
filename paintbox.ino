
#include <SPI.h>
#include <TFT_eSPI.h>

#include <XPT2046_Touchscreen.h>

TFT_eSPI tft = TFT_eSPI();

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

int width,height;

typedef struct {
  int size;
  int colour;
} pentype;

pentype pen;

int colours[8]= {TFT_BLACK,TFT_BLUE,TFT_RED,TFT_GREEN,TFT_CYAN,TFT_YELLOW,TFT_WHITE};
int menu = 0;
#define mPEN 0
#define mCOLOUR 1

void draw_menubar(int menu){
  int boxsize,b2,pensize;
  boxsize = width/8;
  b2 = boxsize/2;

  if (menu == mPEN){
    pensize = 1;
    
    tft.fillRect(0,height-boxsize,width,boxsize,TFT_WHITE);
    for (int x=0; x <width; x += boxsize){
      tft.drawRect(x,height-boxsize,boxsize,boxsize,TFT_BLACK);
      if (pen.colour == TFT_WHITE) tft.drawCircle(x+ b2,height-b2,pensize,TFT_BLACK);
      else tft.fillCircle(x+ b2,height-b2,pensize,pen.colour);
      pensize +=2;
      }
  }
  if (menu == mCOLOUR){
  
    int colour = 0;
    for (int x=0; x <width; x += boxsize){
      tft.fillRect(x,height-boxsize,boxsize,boxsize,colours[colour]);
      colour++;
      }
   }
}



void setup() {
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);

  touchscreen.setRotation(3);

  tft.init();
  tft.setRotation(3);

  width = tft.width();
  height = tft.height();
  tft.fillScreen(TFT_WHITE);

  menu = mCOLOUR;
  draw_menubar(menu);
}

void loop() {
  int x,y,z;

  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    x = map(p.x, 200, 3700, 1, width);
    y = map(p.y, 240, 3800, 1, height);
    z = p.z;

    x -=20; // calibration -ymmv
    int menubar = height - 40;

    if (y > menubar) { 
      if ( x > width-30) { // change menu
        menu = 1 - menu;
        draw_menubar(menu);
        delay(200);
      }
      if (menu == mPEN){ // pick pensize
          int picked = x / (width / 8);
          pen.size = x / (width / 16);
        }
      if (menu == mCOLOUR)  { //pick colour
          int picked = x / (width / 8);
          pen.colour = colours[picked];
        }
      }
    if (y < menubar) { //just draw
      tft.fillCircle(x,y,pen.size,pen.colour);
      }
  }
}
