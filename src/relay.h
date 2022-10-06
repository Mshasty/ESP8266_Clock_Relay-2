void relayState(int pin, bool state, String id){
    static bool oldState = false;
    if(state != oldState){
        digitalWrite(pin, state);
        if(id != "") jee.var(id, state? "true" : "false");
        oldState = state;
    } 
}

void relay(){
  static unsigned long i;
  static bool st = false;
  if(!st){
    st = true;
    pinMode(RELAY, OUTPUT);
  }
  if(i + 1000 > millis()) return;
  i = millis();
  tem = ds_tem;
  if (mode == "TEM"){
    if (tem > p_tem + ( h_tem / 2)){
      relayState(RELAY, false, "relay");
    }
    if (tem < p_tem - ( h_tem / 2)){
      relayState(RELAY, true, "relay");
    }
  }
}

