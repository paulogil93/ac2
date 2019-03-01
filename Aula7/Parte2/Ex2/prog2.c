void setPWM(unsigned int dutyCycle) {
    // assert dutyCycle >= 0 && dutyCycle <= 100
    if(dutyCycle >= 0 && dutyCycle <= 100) {
        OC1RS = ((PR3+1) * dutyCycle) / 100;
    } else {
        printStr("Invalid duty-cycle value...");
    }
}
