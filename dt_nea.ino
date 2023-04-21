#define SCL_PIN A5
#define SCL_PORT PORTD
#define SDA_PIN A3
#define SDA_PORT PORTC

#include <Wire.h>
#include <LCDSoftI2C.h>

// A0 Next
// A1 Prev.
// A2 Select

// =====================================
//  A4 and A5 are reserved for the I2C.
//   D0 and D1 are reserved for Serial.
// =====================================

#define D2 2 // CAFE
#define D3 3 // DOG_WASH
#define D4 4 // DISABLED_TOILET
#define D5 5 // WALKING
#define D6 6 // LIFESTYLE
#define D7 7 // PUPPY
#define D8 8 // FOOD
#define D9 9 // GROOMING

enum class Loc : unsigned short {
  	NONE = 0u,
  	CAFE = 1u,
  	DOG_WASH = 2u,
  	DISABLED_TOILET = 4u,
  	WALKING = 8u,
  	LIFESTYLE = 16u,
  	PUPPY = 32u,
  	FOOD = 64u,
    GROOMING = 128u,
  	RAW_FOOD = FOOD,
  	WET_FOOD = FOOD | PUPPY,
  	DRY_FOOD = FOOD | PUPPY,
  	VEGAN_FOOD = FOOD,
  	TREATS = LIFESTYLE,
  	COLLARS = WALKING,
  	LEADS = WALKING,
  	HARNESSES = WALKING,
  	BEDS = LIFESTYLE,
  	BOWLS = LIFESTYLE,
  	TOYS = LIFESTYLE,
  	ACCESSORIES = LIFESTYLE,
  	CLOTHING = LIFESTYLE,
  	MISCELLANEOUS = LIFESTYLE,
  	
};

typedef struct {
    char *name;
    Loc loc;
} LocPair;

constexpr LocPair locTable[] = {
	  {"Cafe", Loc::CAFE},
    {"Dog Wash *", Loc::DOG_WASH},
    {"Disabled Toilet", Loc::DISABLED_TOILET},
    {"Walkies *", Loc::WALKING},
    {"Lifestyle *", Loc::LIFESTYLE},
    {"Puppy *", Loc::PUPPY},
    {"Food *", Loc::FOOD},
    {"Grooming *", Loc::GROOMING},
    {"Raw Food", Loc::RAW_FOOD},
    {"Wet Food", Loc::WET_FOOD},
    {"Dry Food", Loc::DRY_FOOD},
    {"Vegan Food", Loc::VEGAN_FOOD},
    {"Treats", Loc::TREATS},
    {"Leads", Loc::LEADS},
    {"Harnesses", Loc::HARNESSES},
    {"Beds", Loc::BEDS},
    {"Bowls", Loc::BOWLS},
    {"Toys", Loc::TOYS},
    {"Accessories", Loc::ACCESSORIES},
    {"Clothing", Loc::CLOTHING},
    {"Miscellaneous", Loc::MISCELLANEOUS}
};

bool hasFlag(Loc val1, Loc val2) {
  	return (((unsigned short)val1) & ((unsigned short)val2)) != 0;
}

unsigned short countSetBits(unsigned short n) {
  	unsigned short count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

int *ledsForLoc(Loc loc) {
  	// the number of LEDs to light will be the same as the number of set bits.
  	unsigned short length = countSetBits((unsigned short)loc);
  	// Allocate memory to store the series of LEDs.
  	static int *leds = nullptr;
    if (leds) free(leds);
    leds = (int *)malloc(sizeof(int) * length);   

  	unsigned short index = 0;
  	
  	// Adds the LED address for the corresponding location.
  	if (hasFlag(loc, Loc::CAFE)) {
      	leds[index] = D2;
      	index++;
  	}
  
  	if (hasFlag(loc, Loc::DOG_WASH)) {
      	leds[index] = D3;
      	index++;
  	}
        
    if (hasFlag(loc, Loc::DISABLED_TOILET)) {
      	leds[index] = D4;
      	index++;
  	}
        
    if (hasFlag(loc, Loc::WALKING)) {
      	leds[index] = D5;
      	index++;
  	}
        
    if (hasFlag(loc, Loc::LIFESTYLE)) {
      	leds[index] = D6;
      	index++;
  	}
        
    if (hasFlag(loc, Loc::PUPPY)) {
      	leds[index] = D7;
      	index++;
  	}
        
   	if (hasFlag(loc, Loc::FOOD)) {
      	leds[index] = D8;
      	index++;
  	}

    if (hasFlag(loc, Loc::GROOMING)) {
        leds[index] = D9;
        index++;        
    }

  	return leds;
}

int strlen(char *str) {
    if (str[0] == '\0')
        return 0;
    return 1 + strlen(str + 1);
}

LCDSoftI2C lcd(0x27, 20, 4);

unsigned short length;

int selected = 0;

void updateLcd(void) {
	  //lcd.clear();

    LocPair selectedLoc = locTable[selected % length];

    char *name = selectedLoc.name;
    int nameOffset = (20 - strlen(name)) / 2;
    lcd.setCursor(nameOffset, 1);
    lcd.print(name);

    int selectionLength = 2;
    int displaySelection = selected + 1;
    if (displaySelection > 9) selectionLength++;
    if (length > 9) selectionLength++;

    int selectionOffset = (20 - selectionLength) / 2;
    lcd.setCursor(selectionOffset, 2);
    int containedSelection = displaySelection % length;
    lcd.print(containedSelection == 0 ? 21 : containedSelection);
    lcd.print("/");
    lcd.print(length);
}

void resetLeds() {
    digitalWrite(D2, LOW);
    digitalWrite(D3, LOW);
    digitalWrite(D4, LOW);
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
    digitalWrite(D7, LOW);
    digitalWrite(D8, LOW);
}

void writeLeds() {
    LocPair current = locTable[selected];
    int *leds = ledsForLoc(current.loc);
    for (int i = 0; i < sizeof(leds); i++) {
      digitalWrite(leds[i], HIGH);
    }
}

const int LCD_COLS = 20;
const int LCD_ROWS = 4;

///
/// START PROCEDURE
///
void setup(void) {
    Serial.begin(9600);
    Serial.println("Setup Started.");

    pinMode(A0, INPUT_PULLUP);
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);

    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    pinMode(D8, OUTPUT);

    length = sizeof(locTable) / sizeof(locTable[0]);

    //lcd.init();
    //lcd.backlight();
    Serial.println("Backlight enabled.");

    //updateLcd();
}

int lastNext = HIGH;
int lastPrev = HIGH;
int lastSelect = HIGH;

///
/// UPDATE PROCEDURE
///
void loop(void) {
    int next = digitalRead(A0);
    int prev = digitalRead(A1);
    int select = digitalRead(A2);

    if (next != lastNext && next == LOW) {
        selected++;
        resetLeds();
        //updateLcd();
    }
    if (prev != lastPrev && prev == LOW) {
        selected--;
        resetLeds();
        //updateLcd();
    }
    if (select != lastSelect && select == LOW) {
        writeLeds();
    }

    lastNext = next;
    lastPrev = prev;
    lastSelect = select;
}