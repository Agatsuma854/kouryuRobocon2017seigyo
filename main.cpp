#include "mbed/mbed.h"

Serial ch1(p13,p14); //T送R受
DigitalOut debugG(p6),debugY(p7),debugR(p8),frontRightMot(P2_6),frontLeftMot(P2_7),rearRightMot(P2_8),rearLeftMot(P2_10);
PwmOut frontRightSpeed(p26),frontLeftSpeed(p25),rearRightSpeed(p24),rearLeftSpeed(p23);


//unsigned char InData[11];
int sum,InData[11];

void setUp(){
	ch1.baud(38400);
	debugR = debugY = debugG = frontRightMot = frontLeftMot =  rearRightMot = rearLeftMot = 0;

	frontRightSpeed.write(0);
	frontLeftSpeed.write(0);
	rearRightSpeed.write(0);
	rearLeftSpeed.write(0);
}

/*Assign    7~8byteは格bitにボタンのon/offが入る(他は可変抵抗)
Serial
|-------|---------------|----------------------
| Byte  |     Data      | Information
|-------|---------------|----------------------
| 0Byte |     0xAF      | Start Germany:Anfang
| 1Byte | unsigned char | LStick X
| 2Byte | unsigned char | LStick Y
| 3Byte | unsigned char | RStick X
| 4Byte | unsigned char | RStick Y
| 5Byte | unsigned char | L2
| 6Byte | unsigned char | R2
| 7Byte | unsigned char | ButtonAssign1
| 8Byte | unsigned char | ButtonAssign2
| 9Byte | unsigned char | CheckSUM
|10Byte |     0xED      | End Germany:Ende
|-------|---------------|----------------------

7Byte ButtonAssign
|-------|--------|-------------
|  Bit  |  Data  | Information
|-------|--------|-------------
| 0Bit  |  i/o   |  TRIANGLE
| 1Bit  |  i/o   |  CIRCLE
| 2Bit  |  i/o   |  CROSS
| 3Bit  |  i/o   |  SQUARE
| 4Bit  |  i/o   |  UP
| 5Bit  |  i/o   |  RIGHT
| 6Bit  |  i/o   |  DOWN
| 7Bit  |  i/o   |  LEFT
|-------|--------|-------------

8Byte ButtonAssign2 == BAS
|-------|--------|-------------
|  Bit  |  Data  | Information
|-------|--------|-------------
| 0Bit  |  i/o   |  L1
| 1Bit  |  i/o   |  L3
| 2Bit  |  i/o   |  R1
| 3Bit  |  i/o   |  R3
| 4Bit  |  i/o   |  SELECT
| 5Bit  |  i/o   |  START
| 6Bit  |  i/o   |  PS
| 7Bit  |  i/o   |  -
|-------|--------|-------------
*/

void stop(){
	debugG = debugY = 0;
	debugR = 1;
}


int hash(){
	sum = (InData[1] + InData[2] + InData[3] + InData[4] +
		   InData[5] + InData[6] + InData[7] + InData[8]);

	if((sum == InData[9])&&(0xED == InData[10])){
		return 1;
	}

return 0;
}


int serial(){

	if(ch1.readable()){
		int numNot,i = 0;

		for(numNot = 0;numNot < 12;numNot++){
			InData[0] = ch1.getc();

			if(InData[0] == 0xAF){

				for(i = 1;i < 11;i++){
					InData[i] = ch1.getc();
				}

				if(hash()){
					return 1;
				}

			}
		}

	}
return 0;
}

/*モーターの出力とドライバーの制御

frontRightSpeed.write(0);
frontRightMot = 1;

frontLeftSpeed.write(0);
frontLeftMot = 1

rearRightSpeed.write(0);
rearRightMot = 1

rearLeftSpeed.write(0);
rearLeftMot = 1;
*/

void motor(){//InData[1]X //InData[2]Y << leftStic
	if(InData[1] < 127){

			frontRightSpeed.write((127 - InData[1])/(127));
			frontRightMot = 1;

			frontLeftSpeed.write((127 - InData[1])/(127));
			frontLeftMot = 1;

			rearRightSpeed.write((127 - InData[1])/(127));
			rearRightMot = 1;

			rearLeftSpeed.write((127 - InData[1])/(127));
			rearLeftMot = 1;
	}
}

void action(){
	debugY = debugG = debugR = 0;
	debugG = 1; //通信成功_緑ELD点灯


}

void move(){
	motor();
	action();
}



void loop(){
	while(1){
		if(serial()){
		 move();
		}else{
			stop();
		}
	}
}

int main(void){
	setUp();
	while(1){
		loop();
	}
return 0;
}
