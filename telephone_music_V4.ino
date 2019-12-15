#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define pin_stop 7   // interrupteur raccroche
#define pin_pulse 4  // cadran
#define pin_df_tx 6 // dfPlayer TX
#define pin_df_rx 5 // dfPlayer RX

#define PULSE_LOW_DUR 5
#define PULSE_HIGH_DUR 5
#define PULSE_DELAY 500
#define PULSE_FILTRE 100

bool  playing;
bool  beeping;
int pulses;
int pulses_volume;
bool pulse;
unsigned long pulses_end;
unsigned long pulses_filtre;


SoftwareSerial dfSerial(pin_df_tx, pin_df_rx); // RX, TX
DFRobotDFPlayerMini dfPlayer;

//fonction pour surveillance la fin de lecture du son
void printDetail(uint8_t type, int value){
  if(type == DFPlayerPlayFinished)
    playing = false;
}

//initialisation
void setup() {
  pinMode(pin_stop, INPUT_PULLUP);
  pinMode(pin_pulse, INPUT_PULLUP);
  

  pulses = 0;
  playing = false;
  beeping = false;

  //param liaison serie
  dfSerial.begin(9600);
  dfPlayer.begin(dfSerial);

  //volume du dfplayer
  dfPlayer.volume(10);
}


//boucle principale
void loop() {

  //////////Detection telephone racroché///////////////
  if(digitalRead(pin_stop) == HIGH) {

    //Reset
    dfPlayer.stop();
    playing = false;
    beeping = false;
    pulses  = 0;

    //changement du volume
    
    //detection des impulsions du cadran
    if(digitalRead(pin_pulse) == HIGH) {
      if(!pulse && millis() >= pulses_filtre) {
        pulses_volume ++;
        pulse = true;
        pulses_filtre = millis() + PULSE_FILTRE;
      }
    }
    
    //lorsque qu'il y a eu une impulsion on attend l'etat bas
    else if(pulse && digitalRead(pin_pulse) == LOW) {
      pulse       = false;
      pulses_end  = millis() + PULSE_DELAY;
    }
  
    //lorsque'il y a des impulsions de compté et que le temps de fin d'attente est atteint on change le son
    else if(pulses_volume && millis() >= pulses_end) {
      dfPlayer.volume(2*pulses_volume);
      pulses_volume  = 0;
    }
  }
  
  //////////Detection telephone decroché///////////////
  else
  {
  
    //beep au decrochage de combiné, lecture du son 011.mp3 
    if(not beeping) {
      dfPlayer.loop(11);
      beeping = true;
    }
  
    //detection des impulsions du cadran
    if(digitalRead(pin_pulse) == HIGH) {
      if(!pulse && millis() >= pulses_filtre) {
        pulses ++;
        pulse = true;
        pulses_filtre  = millis() + PULSE_FILTRE;
      }
    }
    
    //lorsque qu'il y a eu une impulsion on attend l'etat bas
    else if(pulse && digitalRead(pin_pulse) == LOW) {
      pulse       = false;
      pulses_end  = millis() + PULSE_DELAY;
    }
 
    //lorsque'il y a des impulsions de compté et que le temps de fin d'attente est atteint on lance le son
    else if(pulses && millis() >= pulses_end) {
      dfPlayer.play(pulses);
      playing = true;
      pulses  = 0;
    }
  
    if(dfPlayer.available()){
      printDetail(dfPlayer.readType(), dfPlayer.read());
    }

  }
  //delay pour que la boucle ne tourne pas trop vite. Ne doit plus etre necessaire avec les modifs 
  //delay(1);
}


  
  
 
