void radioSetup() {                                       // настройка радио 
    radio.begin();                                        // Инициируем работу nRF24L01+
    radio.setChannel(Channel);                            
    radio.setDataRate(SIG_SPEED);
    radio.setPALevel(SIG_POWER);
    radio.openWritingPipe(0x1234567890LL);
    radio.stopListening();                                // не слушаем радиоэфир, мы передатчик
}
void Radio() {
    transmit_data[0] = dispTemp;
    transmit_data[1] = dispHum;
    transmit_data[2] = dispPres;
    transmit_data[3] = outTemp;
    transmit_data[4] = outHum;
    transmit_data[5] = wind;

    radio.write(&transmit_data, sizeof(transmit_data));     // отправляем данные и указываем сколько байт пакет
}
