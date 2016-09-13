#include "ps2controller.h"

int ps2_txrx(short int command){
  short int i; 
  _delay_us(1);
  short int data = 0x00;                             // clear data variable to save setting low bits later.

  for(i=0; i<8; i++)
  {
    if(command & (1 << i))
    { 
      PS2_PIN_OUTSET(PS2COMMAND);       // bit bang "command" out on PScommand wire.
    } 
    else 
    {
      PS2_PIN_OUTCLR(PS2COMMAND);
    }


    PS2_PIN_OUTCLR(PS2CLOCK);                             // CLOCK LOW
    _delay_us(10);                                    // wait for output to stabilise


    if(PS2_PIN_GET(PS2DATA)) 
    {
      data |= (1 << i);                               // read PSdata pin and store
    }

    PS2_PIN_OUTSET(PS2CLOCK);                             // CLOCK HIGH
  }

  PS2_PIN_OUTSET(PS2COMMAND);        
  _delay_us(20);                                                   // wait for ACK to pass.
  return(data);
}

void select(){
  PS2_PIN_OUTSET(PS2COMMAND);   
  PS2_PIN_OUTSET(PS2CLOCK);   
  PS2_PIN_OUTCLR(PS2ATTENTION);  
}

/**DESELECT**/
//Might need adjustment
void deselect(){
  PS2_PIN_OUTSET(PS2COMMAND);   
  _delay_ms(1);
  PS2_PIN_OUTSET(PS2ATTENTION);  
  _delay_ms(1);
}

void reselect(){
  deselect();
  select();
}

uint8_t  ps2_init_analog_mode()
{
  PS2_PIN_DIRSET(PS2COMMAND);
  PS2_PIN_DIRSET(PS2CLOCK);
  PS2_PIN_DIRSET(PS2ATTENTION);

  uint8_t mode = 0;
  select();  
    ps2_txrx(0x01);
    ps2_txrx(0x43);
    ps2_txrx(0x00);
    ps2_txrx(0x01);
    ps2_txrx(0x00);
  reselect();
    ps2_txrx(0x01);
    ps2_txrx(0x44);
    ps2_txrx(0x00);
    ps2_txrx(0x01);
    ps2_txrx(0x03);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
  reselect();
    ps2_txrx(0x01);
    ps2_txrx(0x4F);
    ps2_txrx(0x00);
    ps2_txrx(0xFF);
    ps2_txrx(0xFF);
    ps2_txrx(0x03);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
  reselect();
    ps2_txrx(0x01);
    ps2_txrx(0x43);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
    ps2_txrx(0x5A);
    ps2_txrx(0x5A);
    ps2_txrx(0x5A);
    ps2_txrx(0x5A);
    ps2_txrx(0x5A);
  reselect();
    ps2_txrx(0x01);
    mode = ps2_txrx(0x42);            // the 2nd byte to be returned from the controller should = 0x73 for "red" analouge controller.
    ps2_txrx(0x00);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
    ps2_txrx(0x00);
  deselect();
  return mode;

}


uint8_t ps2_read_controller(ps2_controller* controller){
    select();
    ps2_txrx(0x01);
    controller->mode = ps2_txrx(0x42);     
    ps2_txrx(0x00);       // the 2nd byte to be returned from the controller should = 0x73 for "red" analouge controller.
    uint16_t oldbuttons = controller->buttons;
    controller->buttons = 0;
    controller->buttons |= ps2_txrx(0x00);
    controller->buttons |= ps2_txrx(0x00) << 8;
    controller->buttons_change = oldbuttons ^ controller->buttons;
    controller->rx = ps2_txrx(0x00);
    controller->ry = ps2_txrx(0x00);
    controller->lx = ps2_txrx(0x00);
    controller->ly = ps2_txrx(0x00);
    for (int i = 0; i < 12; ++i)
    {
      controller->buttons_pressure = ps2_txrx(0x00);
    }
  deselect();
  return 0;
}