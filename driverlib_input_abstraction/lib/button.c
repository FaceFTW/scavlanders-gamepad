/**********************************************************************
 * \headerfile: button.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Code related to parsing and encoding button inputs
 * \version: Feb 11, 2021
 ***********************************************************************/
#include "button.h"
#include"ti/devices/msp432p4xx/driverlib/driverlib.h"

/***********************************************************************
 * 							FUNCTION DECLARATIONS					   *
***********************************************************************/
void init_button(){
    GPIO_setAsinputPin(); // setA
    GPIO_setAsinputPin(); // setB
    GPIO_setAsinputPin(); // setX
    GPIO_setAsinputPin(); // setY
    GPIO_setAsinputPin(); // setUp
    GPIO_setAsinputPin(); // setDown
    GPIO_setAsinputPin(); // setRight
    GPIO_setAsinputPin(); // setLeft
    GPIO_setAsinputPin(); // setParing
    GPIO_setAsinputPin(); // setSelect
    GPIO_setAsinputPin(); // setStart

}

void get_button(){
    States states = null;
    if(GPIO_getInputPinValue(botton_A)){
       //print in the console
        states = A;
    }

    else if(GPIO_getInputPinValue(botton_B)){
        states = B;
    }

    else if(GPIO_getInputPinValue(botton_X)){
        states = X;
    }

    else if(GPIO_getInputPinValue(botton_Y)){
        states = Y;
    }

    else if(GPIO_getInputPinValue(botton_U)){
        states = U;
    }

    else if(GPIO_getInputPinValue(botton_D)){
        states = D;
    }

    else if(GPIO_getInputPinValue(botton_R)){
        states = R;
    }

    else if(GPIO_getInputPinValue(botton_L)){
        states = L;
    }

    else if(GPIO_getInputPinValue(botton_P)){
        states = P;
    }

    else if(GPIO_getInputPinValue(botton_Sel) == 0){
        states = Sel;
    }

    else if(GPIO_getInputPinValue(botton_Start) == 0){
        states = Start;
    }


}
void print_botton(States states){
    if(states == null){
        printf();
    }
    else if (states == A){
        printf();
    }
    else if(states == B){
        printf();
    }
    else if(states == X){
            printf();
        }
    else if(states == Y){
            printf();
        }
    else if(states == U){
            printf();
        }
    else if(states == D){
            printf();
        }
    else if(states == R){
            printf();
        }
    else if(states == L){
            printf();
        }
    else if(states == P){
            printf();
        }
    else if(states == Sel){
            printf();
        }
    else if(states == Start){
            printf();
        }


}

