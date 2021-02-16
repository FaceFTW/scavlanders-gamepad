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


    GPIO_setAsInputPinWithPullDownResistor(botton_port1, botton_A); // setA
    GPIO_setAsInputPinWithPullDownResistor(botton_port1, botton_B); // setB
    GPIO_setAsInputPinWithPullDownResistor(botton_port1, botton_X); // setX
    GPIO_setAsInputPinWithPullDownResistor(botton_port1, botton_Y); // setY
    GPIO_setAsInputPinWithPullDownResistor(botton_port1, botton_U); // setUp
    GPIO_setAsInputPinWithPullDownResistor(botton_port1, botton_D); // setDown
    GPIO_setAsInputPinWithPullDownResistor(botton_port1, botton_R); // setRight
    GPIO_setAsInputPinWithPullDownResistor(botton_port1, botton_L); // setLeft
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN3); // setParing
    GPIO_setAsInputPinWithPullDownResistor(botton_port2, botton_Sel); // setSelect
    GPIO_setAsInputPinWithPullDownResistor(botton_port2, botton_Start); // setStart

}

void get_button(){
    int b_a, b_b, b_x, b_y, b_u, b_d, b_r, b_l, b_p, b_sel, b_start;
    States states = null;// set the state to null

        b_a = GPIO_getInputPinValue(botton_port1, botton_A); // setA
        b_b = GPIO_getInputPinValue(botton_port1, botton_B); // setB
        b_x = GPIO_getInputPinValue(botton_port1, botton_X); // setX
        b_y = GPIO_getInputPinValue(botton_port1, botton_Y); // setY
        b_u = GPIO_getInputPinValue(botton_port1, botton_U); // setUp
        b_d = GPIO_getInputPinValue(botton_port1, botton_D); // setDown
        b_r = GPIO_getInputPinValue(botton_port1, botton_R); // setRight
        b_l = GPIO_getInputPinValue(botton_port1, botton_L); // setLeft
        b_p = GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN7); // setParing
        b_sel = GPIO_getInputPinValue(botton_port2, botton_Sel); // setSelect
        b_start = GPIO_getInputPinValue(botton_port2, botton_Start); // setStart




    if(b_a != 0){
       //print in the console
        states = A;
        printf("button A is pressed");
    }

    else if(b_b != 0){
        states = B;
        printf("button B is pressed");
    }

    else if(b_x != 0 ){
        states = X;
        printf("button X is pressed");
    }

    else if(b_y != 0){
        states = Y;
        printf("button Y is pressed");
    }

    else if(b_u !=0){
        states = U;
        printf("button U is pressed");
    }

    else if( b_d !=0){
        states = D;
        printf("button D is pressed");
    }

    else if(b_r !=0){
        states = R;
        printf("button R is pressed");
    }

    else if(b_l !=0){
        states = L;
        printf("button L is pressed");
    }


    else if(b_p != 0){
        states = P;
        printf("button P is pressed");
    }



    else if(b_sel != 0){
        states = Sel;
        printf("button Sel is pressed");
    }

    else if(b_start != 0){
        states = Start;
        printf("button Start is pressed");

    }



}





