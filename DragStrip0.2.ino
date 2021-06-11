//arduino pro mini
//version 0.2
//with red light detection, stage sensor and with I2C screen.
//0.4 second delay for pro tree
//0.5 sencond delay for sportsmans tree
//millis() Number of milliseconds passed since the program started
//
/* 
                +--------------------------------+
                |  [GND][GND][VCC][RX][TX][DTR]  |
                |              FTDI              |
                | [ ]1/TX                 RAW[ ] |    
                | [ ]0/RX                 GND[ ] |    
                | [ ]RST        SCL/A5[ ] RST[ ] |   
                | [ ]GND        SDA/A4[ ] VCC[ ] |    
End sense ln1   | [ ]2/INT0    ___      A3/17[ ] |  Stage ln 1
End sense ln2   |~[ ]3/INT1   /   \     A2/16[ ] |  red sense ln2  
white LED ln1   | [ ]4       /PRO  \    A1/15[ ] |  red sense ln1
white LED ln2   |~[ ]5       \ MINI/    A0/14[ ] |  button (pro / sport mode)
Yellow led1     |~[ ]6        \___/    SCK/13[ ] |  Green LED
Yellow led2     | [ ]7          A7[ ] MISO/12[ ] |  button (start)
Yellow led3     | [ ]8          A6[ ] MOSI/11[ ]~|  Red LED ln2
Stage ln 2      |~[ ]9                  SS/10[ ]~|  Red LED ln1
                |           [RST-PB]             |    
                +--------------------------------+  
*/


#define white_ln1   4  // 
#define white_ln2   5  // 
#define red_ln1   10  //
#define red_ln2   11  //
#define yellow1   6  // 
#define yellow2   7  //
#define yellow3   8  //
#define green   13 //  

#define finish_ln1   2 //high when blocked
#define finish_ln2   3 //high when blocked
#define red_sense_ln1   15 //high when blocked
#define red_sense_ln2   16 //high when blocked
#define stage_sense_ln1   17 //high when blocked
#define stage_sense_ln2   9 //high when blocked
#define start_pb   12 //low when pressed
#define pro_pb   14 //

int start_pb_state = 0;
int pro_pb_state = 0;
int fnsh_ln1_state = 0;
int fnsh_ln2_state = 0;
int stage_ln1_state = 0;
int stage_ln2_state = 0;
int red_ln1_state = 0;
int red_ln2_state = 0;


unsigned long time_elapsed1;
unsigned long time_elapsed2;
signed long   time_reaction1;
signed long   time_reaction2;
unsigned long time_ln = 0;  //green light start time
unsigned long time_ln1 = 0; //finish line cross time
unsigned long time_ln2 = 0; //finish line cross time
unsigned long time_red_ln1 = 0; //red light sensor cross time
unsigned long time_red_ln2 = 0; //red light sensor cross time
unsigned long previousMillis = 0;
unsigned long currentMillis; 

int ln1_elapsed_a; //interger
int ln1_elapsed_b; //decimal
int ln1_react_a; //interger
int ln1_react_b; //decimal
int ln2_elapsed_a;
int ln2_elapsed_b;
int ln2_react_a;
int ln2_react_b;

int interval = 0;
int prog_step = 0;
int win_led = 0;
int ledState = LOW;
byte red_lose = 0;        // lane 1 red = 1, lane 2 = 2, 3 = both red
byte red_ln1_passed = 0;
byte red_ln2_passed = 0;
byte finish_passed = 0;   // lane 1 finish = 1, lane 2 = 2, 3 = race over
byte ln1_neg = 0; //negative flag for print
byte ln2_neg = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(white_ln1, OUTPUT);
  pinMode(white_ln2, OUTPUT);
  pinMode(red_ln1, OUTPUT);
  pinMode(red_ln2, OUTPUT);
  pinMode(yellow1, OUTPUT);
  pinMode(yellow2, OUTPUT);
  pinMode(yellow3, OUTPUT);
  pinMode(green, OUTPUT);

  pinMode(finish_ln1, INPUT);
  pinMode(finish_ln2, INPUT);
  pinMode(start_pb, INPUT);
  pinMode(pro_pb, INPUT);
  pinMode(red_sense_ln1, INPUT);
  pinMode(red_sense_ln2, INPUT);
  pinMode(stage_sense_ln1, INPUT);
  pinMode(stage_sense_ln2, INPUT);


  Serial.begin(115200);

}

void loop() { //============================================================

currentMillis = millis();
  
switch (prog_step) {
  case 1:
    staging_lights();
    random_start();
    red_light();
    break;
  case 2:
    staging_lights();
    pro_tree();
    red_light();
    break;
  case 3:
    staging_lights();
    sport_tree();
    red_light();
    break;
  case 4:
    staging_lights();
    waiting_for_finish();
    red_light();
    break;
  case 5:
    staging_lights();
    check_buttons();
    waiting_for_last();
    red_light();
    blink_led();
    break;
  case 6: //race done, print results
    staging_lights();
    check_buttons();
    blink_led();
    print_times();
    break;
  case 7: //race done
    staging_lights();
    check_buttons();
    blink_led();
    red_light();
    break;
  default:
    staging_lights();
    check_buttons();
    break;
}

   
}
//==========================================================


void check_buttons(){ //====================Check start Button===
start_pb_state = digitalRead(start_pb);
pro_pb_state = digitalRead(pro_pb);
stage_ln1_state = digitalRead(stage_sense_ln1);
stage_ln2_state = digitalRead(stage_sense_ln2);

  if (start_pb_state == LOW){
    digitalWrite(white_ln1, HIGH);
    digitalWrite(white_ln2, HIGH);
    digitalWrite(red_ln1, LOW);
    digitalWrite(red_ln2, LOW);
    digitalWrite(green, LOW);
    red_ln1_passed = 0;
    red_ln2_passed = 0;
    red_lose = 0;
    finish_passed = 0;
    interval = (random(100, 3100));
    previousMillis = currentMillis;
    prog_step = 1;
  }

//  if (stage_ln1_state == HIGH) {
//    digitalWrite(white_ln1, HIGH); 
//  } else {
//      digitalWrite(white_ln1, LOW); 
//  }
  
   
//  if (stage_ln2_state == HIGH) {
//    digitalWrite(white_ln2, HIGH);
//  } else {
//      digitalWrite(white_ln2, LOW); 
//  }
   
  
  if (stage_ln1_state == HIGH){   //if staging ln1 & ln2 = high then start the race.
    if (stage_ln2_state == HIGH){
      digitalWrite(red_ln1, LOW);
      digitalWrite(red_ln2, LOW);
      digitalWrite(green, LOW);
      red_ln1_passed = 0;
      red_ln2_passed = 0;
      red_lose = 0;
      finish_passed = 0;
      interval = (random(100, 3100));
      previousMillis = currentMillis;
      prog_step = 1;
    }
  }
}

void staging_lights(){
  
  if (stage_ln1_state == HIGH) {
    digitalWrite(white_ln1, HIGH); 
  } else {
      digitalWrite(white_ln1, LOW); 
  }
  
   
  if (stage_ln2_state == HIGH) {
    digitalWrite(white_ln2, HIGH);
  } else {
      digitalWrite(white_ln2, LOW); 
  }

}

void random_start(){

 if (currentMillis - previousMillis > interval){  //check to see if random interval has expired
  if (pro_pb_state == HIGH) {                     //then go to the start sequence
    previousMillis = currentMillis;
    prog_step = 2; //pro tree
    }
  else {
    previousMillis = currentMillis;
    prog_step = 3; //sport tree
    }
  }
}

 //==============================================================

void red_light(){ //jump start check & reaction timer
  stage_ln1_state = digitalRead(stage_sense_ln1);
  stage_ln2_state = digitalRead(stage_sense_ln2);
  red_ln1_state = digitalRead(red_sense_ln1);
  red_ln2_state = digitalRead(red_sense_ln2);


  if (red_ln1_passed == 0) {
    if (red_ln1_state == HIGH) {
      time_red_ln1 = millis();    //set reaction time
      red_ln1_passed = 1;         //flag to do check only once
        if (prog_step < 4) {      //check program step is after green light function, if not set red
          digitalWrite(red_ln1, HIGH);
          red_lose = red_lose + 1;  //set a bit so this lane doesn't win
        }
    }
  }

  if (red_ln2_passed == 0) {
    if (red_ln2_state == HIGH) {  
      time_red_ln2 = millis();
      red_ln2_passed = 1;
        if (prog_step < 4) {            //check program step is after green light function
          digitalWrite(red_ln2, HIGH);  
          red_lose = red_lose + 2;     //set a bit so this lane doesn't win
        }
    }
  }
  
//  if (stage_ln1_state == HIGH) {        //maybe make seperate function?
//    digitalWrite(white_ln1, HIGH); 
//  } else {
//      digitalWrite(white_ln1, LOW); 
//  }
  
   
//  if (stage_ln2_state == HIGH) {
//    digitalWrite(white_ln2, HIGH);
//  } else {
//      digitalWrite(white_ln2, LOW); 
//  }  



}


void sport_tree(){ //#3

 if (currentMillis - previousMillis >= 0 && currentMillis - previousMillis < 500){
  digitalWrite(yellow1, HIGH);   
  digitalWrite(yellow2, LOW);
  digitalWrite(yellow3, LOW);
  digitalWrite(green, LOW);
  } else if (currentMillis - previousMillis >= 500 && currentMillis - previousMillis < 1000){                       
  digitalWrite(yellow1, LOW);   
  digitalWrite(yellow2, HIGH);
  digitalWrite(yellow3, LOW);
  digitalWrite(green, LOW);
  } else if (currentMillis - previousMillis >= 1000 && currentMillis - previousMillis < 1500){
  digitalWrite(yellow1, LOW);   
  digitalWrite(yellow2, LOW);
  digitalWrite(yellow3, HIGH);
  digitalWrite(green, LOW);
  } else if (currentMillis - previousMillis >= 1500){                       
  digitalWrite(yellow1, LOW);   
  digitalWrite(yellow2, LOW);
  digitalWrite(yellow3, LOW);
  digitalWrite(green, HIGH);
  time_ln = millis();
  prog_step = 4;
  }
}


void pro_tree(){ //#2
 
 if (currentMillis - previousMillis >= 0 && currentMillis - previousMillis < 400){
  digitalWrite(yellow1, HIGH);   
  digitalWrite(yellow2, HIGH);
  digitalWrite(yellow3, HIGH);
  digitalWrite(green, LOW);
  //delay(400);                       
} else if (currentMillis - previousMillis >= 400){
  digitalWrite(yellow1, LOW);   
  digitalWrite(yellow2, LOW);
  digitalWrite(yellow3, LOW);
  digitalWrite(green, HIGH);
  time_ln = millis();
  prog_step = 4;
 
} 

}



void waiting_for_finish(){ //#4  

fnsh_ln1_state = digitalRead(finish_ln1);
fnsh_ln2_state = digitalRead(finish_ln2);

  if (fnsh_ln1_state == HIGH) {
    time_ln1 = millis();
    time_elapsed1 = time_ln1 - time_ln;
    time_reaction1 = time_red_ln1 - time_ln;
    finish_passed = 1;
    win_led = white_ln1; //pin number of win led output lane 1 
    if (red_lose == 1){  //"red_lose" = 1  when lane1 red light is on (= 3 when both red are on)
      win_led = white_ln2; //pin number of output lane 2 
    }
    prog_step = 5;
  } else if (fnsh_ln2_state == HIGH){
    time_ln2 = millis();
    time_elapsed2 = time_ln2 - time_ln;
    time_reaction2 = time_red_ln2 - time_ln;
    finish_passed = 2;
    win_led = white_ln2; //pin number of win led output lane 2 (check "red_lose" is less than 2 to see who wins)
    if (red_lose == 2){  //"red_lose" = 2  when lane2 red light is on (= 3 when both red are on)
      win_led = white_ln1; //pin number of output lane 1 
    }
    prog_step = 5;
  } else {
    prog_step = 4;
  }
 
}


void waiting_for_last(){//#5
fnsh_ln1_state = digitalRead(finish_ln1);
fnsh_ln2_state = digitalRead(finish_ln2);

//wait for losing lane to finish--------
switch (finish_passed) { 
  case 2: //lane 2 won 
    if (fnsh_ln1_state == HIGH) {
    time_ln1 = millis();
    time_elapsed1 = time_ln1 - time_ln;
    time_reaction1 = time_red_ln1 - time_ln;
    prog_step = 6;
  }
    break;
  case 1: //lane 1 won 
    if (fnsh_ln2_state == HIGH) {
    time_ln2 = millis();
    time_elapsed2 = time_ln2 - time_ln;
    time_reaction2 = time_red_ln2 - time_ln;
    prog_step = 6;
  }
    break;
    
}
//--------------------------------------
}



void blink_led(){// blink winning led------------

  if (currentMillis - previousMillis >= 300) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }
  digitalWrite(win_led, ledState);

}//-------------------------------




void print_times(){
  digitalWrite(green, LOW);
  ln1_neg = 0;
  ln2_neg = 0;
  ln1_react_a = time_reaction1 / 1000;                  //reaction time negative when red light
  ln1_react_b = time_reaction1 - ln1_react_a * 1000;
  if (ln1_react_b < 0){
    ln1_react_b = ln1_react_b * -1;
    ln1_react_a = ln1_react_a * -1;
    ln1_neg = 1;//put negative detect flag here?
  }


  ln1_elapsed_a = time_elapsed1 / 1000;
  ln1_elapsed_b = time_elapsed1 - ln1_elapsed_a * 1000;

  ln2_react_a = time_reaction2 / 1000;
  ln2_react_b = time_reaction2 - ln2_react_a * 1000;
  if (ln2_react_b < 0){
    ln2_react_b = ln2_react_b * -1;
    ln2_react_a = ln2_react_a * -1;
    ln2_neg = 1;//put negative detect flag here?
  }

  ln2_elapsed_a = time_elapsed2 / 1000;
  ln2_elapsed_b = time_elapsed2 - ln2_elapsed_a * 1000;

  Serial.print("lane 1 reaction: ");
  if (ln1_neg==1) Serial.print("-"); //to print negative symbol
  Serial.print(ln1_react_a);
  Serial.print(".");
  if (ln1_react_b<100) Serial.print("0"); //to print leading zeros
  if (ln1_react_b<10) Serial.print("0");  //to print leading zeros
  Serial.print(ln1_react_b);
  Serial.println(" sec");

  Serial.print("lane 1 finish:   ");
  Serial.print(ln1_elapsed_a);
  Serial.print(".");
  if (ln1_elapsed_b<100) Serial.print("0"); //to print leading zeros
  if (ln1_elapsed_b<10) Serial.print("0");  //to print leading zeros
  Serial.print(ln1_elapsed_b);
  Serial.println(" sec");

  Serial.print("lane 2 reaction: ");
  if (ln2_neg==1) Serial.print("-"); //to print negative symbol
  Serial.print(ln2_react_a);
  Serial.print(".");
  if (ln2_react_b<100) Serial.print("0"); //to print leading zeros
  if (ln2_react_b<10) Serial.print("0");  //to print leading zeros
  Serial.print(ln2_react_b);
  Serial.println(" sec");

  Serial.print("lane 2 finish:   ");
  Serial.print(ln2_elapsed_a);
  Serial.print(".");
  if (ln2_elapsed_b<100) Serial.print("0"); //to print leading zeros
  if (ln2_elapsed_b<10) Serial.print("0");  //to print leading zeros
  Serial.print(ln2_elapsed_b);
  Serial.println(" sec");
  Serial.println(" ");
  
  Serial.print("lane 1 reaction: ");
  Serial.println(time_reaction1); //lane 1 reaction time
  Serial.print("lane 1 finish:   ");
  Serial.println(time_elapsed1); //lane 1 time
  Serial.print("lane 2 reaction: ");
  Serial.println(time_reaction2); //lane 2 reaction time
  Serial.print("lane 2 finish:   ");
  Serial.println(time_elapsed2); //lane 2 time
  Serial.println("---");

  prog_step = 7;
}




//ln1_e_a = time_reaction1 / 1000;
//ln1_e_b = time_reaction1 - ln1_e_a * 1000;



 // if (buttonState == HIGH) {
 //   digitalWrite(ledPin, HIGH);
 // } else {
 //   digitalWrite(ledPin, LOW);
 // }

//void setup() {
//  Serial.begin(9600);
//}
//void loop() {
//  Serial.print("Time: ");
//  time = millis();

//  Serial.println(time); //prints time since program started
//  delay(1000);          // wait a second so as not to send massive amounts of data
//}

    //digitalWrite(ledPin, ledState);
  
