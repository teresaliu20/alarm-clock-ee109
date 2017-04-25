void init_clock();
void init_timer1(unsigned short m);
void init_timer0(unsigned short m);
void updateTime();

char hoursTens;
char hoursOnes;
char minsTens;
char minsOnes;
char secsTens;
char secsOnes;

char alarmHoursTens;
char alarmHoursOnes;
char alarmMinsTens;
char alarmMinsOnes;

char daysIndex;

char* days[7];

// state 0: clock
// state 1: day of the week
// state 2: hour
// state 3: minute
// state 4: alarm hour
// state 4: alarm min
volatile int state;

int buzzCounter;