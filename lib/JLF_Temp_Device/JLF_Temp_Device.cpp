

#include "JLF_Temp_Device.h"

#define OUTPUT_SERIAL_PRINT 0 // 0 pas de sortie sur le moniteur série, 1 sortie sur le moniteur série

JLF_Temp_Device::JLF_Temp_Device(unsigned int pin, DHTesp::DHT_MODEL_t model) : m_pin(pin),
                                                                                m_model(model),
                                                                                m_nbValeurs(0)
{
    this->setup(m_pin, m_model);
}
JLF_Temp_Device::~JLF_Temp_Device()
{
}

float JLF_Temp_Device::getCurrentTemp()
{
    m_currentTemp = this->getTemperature();
#if OUTPUT_SERIAL_PRINT
    // Affichage de la valeur sur le moniteur série
    Serial.print(" Temperature :   ");
    Serial.print(m_currentTemp);
    Serial.println(" degrés C");
#endif
    // On renvoie la valeur sous forme de float
    return m_currentTemp;
}

float JLF_Temp_Device::getAverageTemp()
{
    m_nbValeurs = 0; // On remet les compteurs à zéro pour le calcul de la moyenne
    return m_averageTemp;
}

void JLF_Temp_Device::update()
{
    m_currentTemp = getCurrentTemp();
    if (!isnan(m_currentTemp))
    {
        m_nbValeurs++;                       // On a une mesure de plus à compter dans la moyenne
        calculateAverageTemp(m_currentTemp); // On met à jour la température moyenne
    }
}
void JLF_Temp_Device::calculateAverageTemp(float newTemp)
{
    m_averageTemp = ((m_averageTemp * (m_nbValeurs - 1)) + newTemp) / m_nbValeurs; //héhé
#if OUTPUT_SERIAL_PRINT
    Serial.println(m_averageTemp);
#endif
}
