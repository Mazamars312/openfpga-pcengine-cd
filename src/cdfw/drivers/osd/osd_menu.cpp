/*
 * Copyright 2022 Murray Aickin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "osd_menu.h"

static uint32_t timer_wait_hold;
static uint32_t timer_wait_value;
char s[64];

uint8_t nstore1;
uint8_t nstore2;
uint8_t nstore3;
uint8_t frame_counter = 0;

void osd_display_error_apf(){
  
};

void osd_display_info(){
  if ((error_system == 0) && (AFP_REGISTOR(2) == 1)){
    sprintf (s, "Delay: %d", timer_wait_value);
    OsdWrite(s);
    OsdUpdate(1);
    OsdClear();
    sprintf (s, "T M:S : %d %d %d", nstore1,  nstore2 , nstore3);
    OsdWrite(s);
    OsdUpdate(0);
    OsdClear();
    if (frame_counter != 0) {
      InfoEnable(10, 10, 16, 2, 291, 31744); // Had to use the info as some of the PCE outputs are smaller
      timer_wait_hold  = RISCGetTimer2(200);
      frame_counter = frame_counter - 1;
    }
    else {
      InfoEnable(10, 10, 16, 2, 291, 65535); // Had to use the info as some of the PCE outputs are smaller
      timer_wait_value = 0;
    }
  }
  else {
    OsdDisable();
  }
}

void osd_display_info_update(int timer_wait){
  if ((error_system == 0) && (AFP_REGISTOR(2) == 1)){
      timer_wait_value = timer_wait_value + timer_wait;
      if (timer_wait > 0) frame_counter = 2;
  }
}

void osd_display_timing (uint8_t n1, uint8_t n2, uint8_t n3){
  if ((error_system == 0) && (AFP_REGISTOR(2) == 1)){
    nstore1 = n1;
    nstore2 = n2;
    nstore3 = n3;
  }
}

void osd_display_error_dataslot (int data_error){
  switch(data_error)
  {
  case 6:
    OsdWrite("-----------Core Error----------");
    OsdUpdate(0);
    OsdClear();
    OsdWrite("This function is not added yet");
    OsdUpdate(1);
    OsdClear();
    OsdWrite("Please quit the core and reload");
    OsdUpdate(2);
    OsdClear();
    OsdWrite("the game that you want to play");
    OsdUpdate(3);
    OsdClear();
    OsdWrite("      upcoming function");
    OsdUpdate(4);
    OsdClear();
    OsdWrite("Now Im just wasting codespace" );
    OsdUpdate(5);
    OsdClear();
    OsdWrite("ULTRAFP64 is a pain in the ass");
    OsdUpdate(6);
    OsdClear();
    OsdWrite("Im also one too, but I drink");
    OsdUpdate(7);
    OsdClear();
    OsdWrite("more. Will Fix later YAY - MAZA  ");
    OsdUpdate(8);
    OsdClear();
    OsdWrite("ULTRAFP64 COMING SOON");
    OsdUpdate(9);
    OsdClear();
    OsdWrite("------------------------------");
    OsdUpdate(10);
    OsdClear();
    OsdWrite("I hate that guy Maza - UFP64");
    OsdUpdate(11);
    OsdClear();
    InfoEnable(5, 80, 31, 12, 192, 65535); // Had to use the info as some of the PCE outputs are smaller
    break;

  case 5:
    OsdWrite ("----------Core Error--------");
    OsdUpdate(0);
    OsdClear();
    OsdWrite("The Core did something weird");
    OsdUpdate(1);
    OsdClear();
    OsdWrite("the MPU received a reset ");
    OsdUpdate(2);
    OsdClear();
    OsdWrite("command from the core. ");
    OsdUpdate(3);
    OsdClear();
    OsdWrite("If this is a hacked CD ISO  ");
    OsdUpdate(4);
    OsdClear();
    OsdWrite("This could be a unallighed" );
    OsdUpdate(5);
    OsdClear();
    OsdWrite("Data location for the TOC   ");
    OsdUpdate(6);
    OsdClear();
    OsdWrite("Pain in the ass error to fix");
    OsdUpdate(7);
    OsdClear();
    OsdWrite("Will Fix later YAY - MAZA  ");
    OsdUpdate(8);
    OsdClear();
    OsdWrite("ULTRAFP64 COMING SOON");
    OsdUpdate(9);
    OsdClear();
    OsdWrite("------------------------------");
    OsdUpdate(10);
    OsdClear();
    InfoEnable(10, 80, 30, 11, 192, 65535); // Had to use the info as some of the PCE outputs are smaller
    break;

  default:
    sprintf (s, "-------DataFile Error---- %d", data_error);
    OsdWrite(s);
    OsdUpdate(0);
    OsdClear();
    OsdWrite("     Please check the files  ");
    OsdUpdate(1);
    OsdClear();
    OsdWrite(" are in the correct location");
    OsdUpdate(2);
    OsdClear();
    OsdWrite(" or both the File and folder ");
    OsdUpdate(3);
    OsdClear();
    OsdWrite(" name is below 255 Charrators");
    OsdUpdate(4);

    OsdClear();
    OsdWrite("         Directory");
    OsdUpdate(5);

    OsdClear();
    snprintf (s, 30," %s", aft.aft_filepath);
    OsdWrite(s);
    OsdUpdate(6);
    
    OsdClear();
    snprintf (s, 30," %s", aft.aft_filepath + 29);
    OsdWrite(s);
    OsdUpdate(8);

    OsdClear();
    snprintf (s, 30," %s", aft.aft_filepath + 58);
    OsdWrite(s);
    OsdUpdate(9);

    OsdClear();
    OsdWrite("Trying to find the File Name  ");
    OsdUpdate(10);
    
    OsdClear();
    snprintf (s, 30," %s", aft.aft_filename);
    OsdWrite(s);
    OsdUpdate(11);
    
    OsdClear();
    snprintf (s, 30," %s", aft.aft_filename + 29);
    OsdWrite(s);
    OsdUpdate(12);

    OsdClear();
    snprintf (s, 30," %s", aft.aft_filename + 58);
    OsdWrite(s);
    OsdUpdate(13);

    OsdClear();
    OsdWrite("------------------------------");
    OsdUpdate(14);
    OsdClear();
    InfoEnable(10, 80, 30, 15, 192, 65535); // Had to use the info as some of the PCE outputs are smaller
    break;
  }



}
