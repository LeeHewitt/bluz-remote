#define UP D1
#define LEFT D0
#define DOWN D3
#define RIGHT D2
#define START D5
#define SELECT D4
#define B D6
#define A A7
#define BUTTON_COUNT 8

#define DEBOUNCE_TIMEOUT 250
#define INACTIVE_TIMEOUT 3600000

BLE_ADV_NAME("REMOTE");

bool send_event = false;
long debounce = 0;
long time_since_last_activity = 0;
int last_button_index = 0;

typedef struct button_commands {
    int button;
    String command;
    
} button_commands;

button_commands buttons[BUTTON_COUNT]={
    {UP, "UP"},
    {LEFT, "LEFT"},
    {DOWN, "DOWN" },
    {RIGHT, "RIGHT" },
    {START, "START" },
    {SELECT, "SELECT" },
    {B, "B" },
    {A, "A" }
};

void button_pressed()
{
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (digitalRead(buttons[i].button) == LOW && millis() - debounce >  DEBOUNCE_TIMEOUT) {
            send_event = true;
            debounce = millis();
            time_since_last_activity = millis();
            last_button_index = i;
        }
    }
}


void setup() {
    Wire.end();
    
    //reduce tx power to reduce battery consumption
    BLE.setTxPower(-8);
    
    // setup each button
    for (int i = 0; i < BUTTON_COUNT; i++) {
        pinMode(buttons[i].button, INPUT_PULLUP);
    }
}

void loop() {
    System.sleep(SLEEP_MODE_CPU);
    
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (digitalRead(buttons[i].button) == LOW && millis() - debounce >  DEBOUNCE_TIMEOUT) {
            send_event = true;
            debounce = millis();
            time_since_last_activity = millis();
            // last_button_index = i;
            Particle.publish("Bluz Remote", buttons[i].command, PRIVATE);
        }
    }
    
    //if no one has pressed a button in a while, power off to save the battery
    if (millis() - time_since_last_activity > INACTIVE_TIMEOUT) {
        System.sleep(SLEEP_MODE_DEEP);
    }
}
