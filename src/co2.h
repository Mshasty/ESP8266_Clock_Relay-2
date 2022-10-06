
/*
#define CO2_PIN 5

void ICACHE_RAM_ATTR pwm_in_interrupt();

void co2_handle(){
    static bool st = false;
    if(!st){
        st = true;
        attachInterrupt(5, pwm_in_interrupt, CHANGE);
    }
}

void pwm_in_interrupt() {
  static int t_last = 0;
  int t = micros();
  if (digitalRead(CO2_PIN) == LOW) {
    co2 = (t - t_last - 2) * 4 / 1000;
  }
  t_last = t;
} */