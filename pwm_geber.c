// pwm_geber
#include "sh_1106.h"
#include "drehgeber.h"
#include "momefilo_flash.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

#define PWM_PIN1 19
#define PWM_PIN2 20
#define PWM_PIN3 21
#define PWM_PIN4 22

uint8_t Auswahl = 1;
uint32_t Werte[] = {0, 0, 0, 0};
bool if_selct = false;

// Zeichnet erstmalig das Menu auf das sh1106-Display
void paint_menu(){
	char buf[10];
	for(int i=0; i<4; i++){
		sprintf(buf, "PWM %d  %.3i", i+1, Werte[i]);
		uint8_t pos[]={0,i};
		sh1106_text12x16(pos, buf, false);
	}
}

// Markiert den selektierte PWM-Kanal im Menu
void mark_pwm(bool up){
	char buf[5];
	sprintf(buf, "PWM %d", Auswahl);
	uint8_t pos[]={0,Auswahl - 1};
	sh1106_text12x16(pos, buf, false);
	if(up && Auswahl < 4) Auswahl++;
	if(!up && Auswahl > 1) Auswahl--;
	sprintf(buf, "PWM %d", Auswahl);
	pos[1] = Auswahl - 1;
	sh1106_text12x16(pos, buf, true);
}

// Markiert den selektierte PWM-Wert im Menu
void select_pwm(bool select){
	char buf[5];
	sprintf(buf, "PWM %d", Auswahl);
	uint8_t pos[]={0,Auswahl - 1};
	sh1106_text12x16(pos, buf, !select);

	sprintf(buf, "  %.3i", Werte[Auswahl - 1]);
	pos[0] = 5;
	sh1106_text12x16(pos, buf, select);
}

// Schreibt den ausgewaehlten PWM-Wert auf den Ausgang
void set_pwmwert(){
	float wert = (256*256-1)/100.0;
	if(Auswahl == 1) { pwm_set_gpio_level(PWM_PIN1, wert * Werte[0]); }
	else if(Auswahl == 2) { pwm_set_gpio_level(PWM_PIN2, wert * Werte[1]); }
	else if(Auswahl == 3) { pwm_set_gpio_level(PWM_PIN3, wert * Werte[2]); }
	else if(Auswahl == 4) { pwm_set_gpio_level(PWM_PIN4, wert * Werte[3]); }
}

// Aendert den ausgewaehlten PWM-Wert und ruft set_pwmwert() auf
void chn_wert(bool up){
	if(up && Werte[Auswahl - 1] < 100) Werte[Auswahl - 1]++;
	if(!up && Werte[Auswahl - 1] > 0) Werte[Auswahl - 1]--;
	char buf[5];
	sprintf(buf, "  %.3i", Werte[Auswahl - 1]);
	uint8_t pos[]={5,Auswahl - 1};
	sh1106_text12x16(pos, buf, select);
	set_pwmwert();
}

// Initialisiert die PWM-Kanaele
void init_pwm(){
	gpio_set_function(PWM_PIN1, GPIO_FUNC_PWM);
	gpio_set_function(PWM_PIN2, GPIO_FUNC_PWM);
	gpio_set_function(PWM_PIN3, GPIO_FUNC_PWM);
	gpio_set_function(PWM_PIN4, GPIO_FUNC_PWM);

	uint slice_num1 = pwm_gpio_to_slice_num(PWM_PIN1);
	uint slice_num2 = pwm_gpio_to_slice_num(PWM_PIN2);
	uint slice_num3 = pwm_gpio_to_slice_num(PWM_PIN3);
	uint slice_num4 = pwm_gpio_to_slice_num(PWM_PIN4);

	pwm_set_enabled(slice_num1, true);
	pwm_set_enabled(slice_num2, true);
	pwm_set_enabled(slice_num3, true);
	pwm_set_enabled(slice_num4, true);

	float wert = (256*256-1)/100.0;
	pwm_set_gpio_level(PWM_PIN1, wert * Werte[0]);
	pwm_set_gpio_level(PWM_PIN2, wert * Werte[1]);
	pwm_set_gpio_level(PWM_PIN3, wert * Werte[2]);
	pwm_set_gpio_level(PWM_PIN4, wert * Werte[3]);

}

int main() {
	stdio_init_all();
	flash_init(10);
	uint32_t *flwerte = flash_getData();
	for(int i=0; i<4; i++) Werte[i] = flwerte[i];
	init_pwm();
	sh1106_init();
	sh1106_clear_screen();
	paint_menu();
	mark_pwm(false);
	while(true){
		uint8_t richtung = drehgeber_get();
		if(richtung == IN_LEFT){
			if(if_selct){chn_wert(false);}
			else mark_pwm(false);
		}
		else if(richtung == IN_RIGHT){
			if(if_selct){chn_wert(true);}
			else mark_pwm(true);
		}
		else if(richtung == IN_PRESS){
			if(!if_selct){
				select_pwm(true);
				if_selct = true;
			}
			else{
				select_pwm(false);
				flash_setDataRow(0, 3, Werte);
				if_selct = false;
			}
		}

	}
}

