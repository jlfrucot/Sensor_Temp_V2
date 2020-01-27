/* Projet ESP32_Sensor_Temp_V2
  *Une petite lib pour lire la température d'un capteur DHT11 ou DHT22
  *
  * @author 2020 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @see The GNU Public License (GPL)
  *
  * this program is free software you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program. If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef JLF_TEMP_DEVICE
#define JLF_TEMP_DEVICE

#include <Arduino.h>
#include <DHTesp.h>
class JLF_Temp_Device : public DHTesp
{

public:
    /*
        On construit un objet "Température"
        * @param unsigned int pin : la broche à lire
        * @param DHTesp::DHT_MODEL_t model : DHT11, DHT22
    */
    JLF_Temp_Device(unsigned int pin, DHTesp::DHT_MODEL_t);
    ~JLF_Temp_Device();
    /*
    Retourne la température courante lue par update()
    */
    float getCurrentTemp();
    /*
        Retourne la température moyenne et réinitialise le calcul
    */
    float getAverageTemp();
    /*
    Retourne l'humidité courante lue par update()
    */
    float getCurrentHumidity();

    /*
        Lit la tension sur la broche m_pin et la convertit en °C
    */
    void update();

private:
    /*
        Calcule la température moyenne
        float newTemp : la nouvelle température lue
    */
    void calculateAverageTemp(float newTemp);

    const int m_pin;                   // La broche du capteur
    const DHTesp::DHT_MODEL_t m_model; //Type du capteur

    unsigned int m_nbValeurs;        // Nombre d'appels valides à update pour le calcul de la moyenne
    float        m_currentTemp;      // La dernière température lue
    float        m_currentHumidity;         // Dernière valeur du taux d'humidité
    float        m_averageTemp;      // La température moyenne depuis le dernier appel à getAverageTemp()
};
#endif