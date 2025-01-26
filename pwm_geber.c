// pwm_geber by momefilo Desing

/* In dieser Header-Datei muessen die Pins und der I2C-Kanal angepasst werden
 * Standart sda=4, sdc=5, kanal=0 */
#include "sh_1106.h"

/* In dieser Header-Datei muessen die Pins des KY-040 angepasst werden
 * Standart CKL=16, DT=17, SW=18 */
#include "drehgeber.h"

#include "momefilo_flash.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

// Die GPIO-Belegung
#define PWM_PIN1 19
#define PWM_PIN2 20
#define PWM_PIN3 21
#define PWM_PIN4 22

/* Der selektierte PWM-Kanal 1-4
 * Wird von mark_pwm(bool up) geaendert */
uint8_t Auswahl = 1;

/* Das Werte-Area der Kanaele zur Laufzeit
 * wird von chn_wert(bool up) geaendert */
uint32_t Werte[] = {0, 0, 0, 0};

/* Wird mit einem Druck auf den Drehgeber true
 * und mit widerholtem Druck false */
bool if_selct = false;

/* Zeichnet das Menu auf das sh1106-Display
 * wird einmalig von main() aufgerufen */
void paint_menu(){

	// Der Textspeicher der geschrieben wird
	char buf[10];

	// Vier Kanaele werden geschrieben
	for(int i=0; i<4; i++){

		// der angezeigte Text pro Kanal
		sprintf(buf, "PWM %d  %.3i", i+1, Werte[i]);

		// die Schreibfunktion des Display erwartet ein uint8_t array {x, y} in Zeichen
		uint8_t pos[]={0,i};

		// der letze bool-Parameter legt eine invertierte darstellung des Text fest wenn true
		sh1106_text12x16(pos, buf, false);
	}
}

/* Markiert den selektierte PWM-Kanal im Menu
 * wird aus der Dauerschleife in main() aufgerufen */
void mark_pwm(bool up){
	char buf[5];

	// erst wird der bisher Markierte text demarkiert
	sprintf(buf, "PWM %d", Auswahl);
	uint8_t pos[]={0,Auswahl - 1};
	sh1106_text12x16(pos, buf, false);

	// dann die Auswahl ensprechend geaendert
	if(up && Auswahl < 4) Auswahl++;
	if(!up && Auswahl > 1) Auswahl--;
	sprintf(buf, "PWM %d", Auswahl);

	// dann der aktuelle Kanal markiert
	pos[1] = Auswahl - 1;
	sh1106_text12x16(pos, buf, true);
}

/* Markiert den selektierte PWM-Wert im Menu
 * und demarkiert den selektierte PWM-Kanal im Menu
 * wird aus der Dauerschleife in main() aufgerufen */
void select_pwm(bool select){
	char buf[5];

	// PWM-Kanal demarkieren
	sprintf(buf, "PWM %d", Auswahl);
	uint8_t pos[]={0,Auswahl - 1};
	sh1106_text12x16(pos, buf, !select);

	// PWM-Wert markieren
	sprintf(buf, "  %.3i", Werte[Auswahl - 1]);
	pos[0] = 5;
	sh1106_text12x16(pos, buf, select);
}

/* Schreibt den ausgewaehlten PWM-Wert auf den Ausgang
 * wird von chn_wert(bool up) aufgerufen */
void set_pwmwert(){

	// Hoechster PWM-Wert ist 256*256-1 und wird auf 100% skaliert
	float wert = (256*256-1)/100.0;

	// Je nach aktueller Auswahl wird der Wert auf den Ausgang geschrieben
	if(Auswahl == 1) { pwm_set_gpio_level(PWM_PIN1, wert * Werte[0]); }
	else if(Auswahl == 2) { pwm_set_gpio_level(PWM_PIN2, wert * Werte[1]); }
	else if(Auswahl == 3) { pwm_set_gpio_level(PWM_PIN3, wert * Werte[2]); }
	else if(Auswahl == 4) { pwm_set_gpio_level(PWM_PIN4, wert * Werte[3]); }
}

/* Aendert den ausgewaehlten PWM-Wert
 * wird aus der Dauerschleife in main() aufgerufen */
void chn_wert(bool up){

	// Aendern des PWM-Wertes entsprechend der aktuellen Auswahl
	if(up && Werte[Auswahl - 1] < 100) Werte[Auswahl - 1]++;
	if(!up && Werte[Auswahl - 1] > 0) Werte[Auswahl - 1]--;
	char buf[5];

	 // Den geaenderten Wert auf das Display schreiben
	sprintf(buf, "  %.3i", Werte[Auswahl - 1]);
	uint8_t pos[]={5,Auswahl - 1};
	sh1106_text12x16(pos, buf, true);

	 // Den geaenderten Wert in den Ausgang schreiben
	set_pwmwert();
}

/* Initialisiert die PWM-Kanaele
 * wird einmalig aus main() aufgerufen */
void init_pwm(){

	// PWM-Funktion auf den entsprechenden GPIOs aktivieren
	gpio_set_function(PWM_PIN1, GPIO_FUNC_PWM);
	gpio_set_function(PWM_PIN2, GPIO_FUNC_PWM);
	gpio_set_function(PWM_PIN3, GPIO_FUNC_PWM);
	gpio_set_function(PWM_PIN4, GPIO_FUNC_PWM);
	pwm_set_enabled(pwm_gpio_to_slice_num(PWM_PIN1), true);
	pwm_set_enabled(pwm_gpio_to_slice_num(PWM_PIN2), true);
	pwm_set_enabled(pwm_gpio_to_slice_num(PWM_PIN3), true);
	pwm_set_enabled(pwm_gpio_to_slice_num(PWM_PIN4), true);

	// Die gespeicherten Werte in die Ausgaenge schreiben
	float wert = (256*256-1)/100.0;
	pwm_set_gpio_level(PWM_PIN1, wert * Werte[0]);
	pwm_set_gpio_level(PWM_PIN2, wert * Werte[1]);
	pwm_set_gpio_level(PWM_PIN3, wert * Werte[2]);
	pwm_set_gpio_level(PWM_PIN4, wert * Werte[3]);

}

int main() {
	/* Initialisiert den 10ten 4kb Sektor vom oberen Ende des Flash */
	flash_init(10);

	/* Liest die gespeicherten PWM-Werte aus dem Flash */
	uint32_t *flwerte = flash_getData();

	/* und speichert diese im Werte-Arrea */
	for(int i=0; i<4; i++) Werte[i] = flwerte[i];

	init_pwm();// Initialisiert die PWM-Ausgaenge
	sh1106_init();// Initialisiert das Display
	sh1106_clear_screen();
	paint_menu();// Zeichnet das gesamte Menu auf das Display
	mark_pwm(false);// Markiert PWM-Kanal 1
	while(true){
		/* Staendig wird der Drehgeber abgefragt */
		uint8_t richtung = drehgeber_get();
		if(richtung == IN_LEFT){// ist die Drehung linksherum?
			if(if_selct){chn_wert(false);}// Der Knopf ist zum ersten mal gedrueckt
			else mark_pwm(false);// Der Knopf ist nicht oder zum zweiten mal gedrueckt
		}
		else if(richtung == IN_RIGHT){
			if(if_selct){chn_wert(true);}// aendere den Wert des ausgewaehlten Kanal
			else mark_pwm(true);// markiere den nacsten Kanal
		}
		else if(richtung == IN_PRESS){// ist der Knopf gedrueckt?
			if(!if_selct){//ist der Knopf zum ersten mal gedrueckt
				select_pwm(true);// Kanal auswaehlen
				if_selct = true;
			}
			else{//ist der Knopf zum zweiten mal gedrueckt
				select_pwm(false);// Kanal abwaehlen
				// Speichere PWM-Werte im Flashspeicher
				flash_setDataRow(0, 3, Werte);
				if_selct = false;
			}
		}

	}
}

