create table COMPETITION
(
    name 			VARCHAR(80) PRIMARY KEY,
    place			VARCHAR(80),
    wheel_radius	FLOAT
);

create table RACE
(
    id 			INTEGER PRIMARY KEY AUTOINCREMENT,
    num 		INTEGER,
    date 		DATETIME,
    ref_compet 	VARCHAR(80),

    FOREIGN KEY (ref_compet) REFERENCES COMPETITION(name) ON DELETE CASCADE
);

create table LAP
(
    num 		INTEGER,
    start_time 	TIME,
    end_time 	TIME,
    distance 	FLOAT,
    ref_race 	INTEGER,

    FOREIGN KEY (ref_race) REFERENCES RACE(id) ON DELETE CASCADE,
    PRIMARY KEY (num, ref_race)
);

create table POSITION
(
    id 				INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp 		TIME,
    latitude 		FLOAT,
    longitude 		FLOAT,
    altitude 		FLOAT,
    eval_speed 		FLOAT,
    ref_lap_num		INTEGER,
    ref_lap_race	INTEGER,

    FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race) ON DELETE CASCADE
);

create table SECTOR
(
    id			INTEGER PRIMARY KEY AUTOINCREMENT,
    num 		INTEGER,
    ref_compet 	VARCHAR(80),
    min_speed 	REAL DEFAULT 0 CHECK(min_speed <= max_speed),
    max_speed 	REAL DEFAULT 0,
    start_pos 	INTEGER,
    end_pos 	INTEGER,

    FOREIGN KEY (ref_compet) REFERENCES COMPETITION(name) ON DELETE CASCADE,
    FOREIGN KEY (start_pos) REFERENCES POSITION(id) ON DELETE CASCADE,
    FOREIGN KEY (end_pos) REFERENCES POSITION(id) ON DELETE CASCADE
);

create table SPEED
(
    id 				INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp 		TIME,
    value 			FLOAT,
    ref_lap_num 	INTEGER,
    ref_lap_race	INTEGER,

    FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race) ON DELETE CASCADE
);

create table ACCELERATION
(
    id 				INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp 		TIME,
    g_long 			FLOAT,
    g_lat 			FLOAT,
    ref_lap_num		INTEGER,
    ref_lap_race	INTEGER,

    FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race) ON DELETE CASCADE
);

create table MEGASQUIRT
(
    timestamp 		TIME,    	-- Temps depuis le début du tour
    ref_lap_num 	INTEGER,	-- reférence du tour
    ref_lap_race	INTEGER,	-- référence de la course

    -- Données d'une trame Megasquirt

    seconds 		DOUBLE,
    pulseWidth1		DOUBLE,
    pulseWidth2		DOUBLE,
    rpm 			DOUBLE,
    advance			DOUBLE,
    squirt			DOUBLE,
    engine			DOUBLE,
    afrtgt1			DOUBLE,
    afrtgt2			DOUBLE,
    wbo2_en1		DOUBLE,
    wbo2_en2		DOUBLE,
    barometer		DOUBLE,
    map 			DOUBLE,
    mat 			DOUBLE,
    coolant 		DOUBLE,
    tps 			DOUBLE,
    batteryVoltage	DOUBLE,
    afr1 			DOUBLE,
    afr2 			DOUBLE,
    knock 			DOUBLE,
    egoCorrection1	DOUBLE,
    egoCorrection2	DOUBLE,
    airCorrection	DOUBLE,
    warmupEnrich	DOUBLE,
    accelEnrich		DOUBLE,
    tpsfuelcut		DOUBLE,
    baroCorrection	DOUBLE,
    gammaEnrich		DOUBLE,
    veCurr1			DOUBLE,
    veCurr2			DOUBLE,
    iacstep			DOUBLE,
    idleDC			DOUBLE,
    coldAdvDeg		DOUBLE,
    tpsDOT			DOUBLE,
    mapDOT			DOUBLE,
    dwell			DOUBLE,
    mafmap			DOUBLE,
    fuelload 		DOUBLE,
    fuelCorrection	DOUBLE,
    portStatus		DOUBLE,
    knockRetard		DOUBLE,
    EAEFuelCorr1	DOUBLE,
    egoV 			DOUBLE,
    egoV2 			DOUBLE,
    status1 		DOUBLE,
    status2 		DOUBLE,
    status3 		DOUBLE,
    status4 		DOUBLE,
    looptime		DOUBLE,
    status5			DOUBLE,
    tpsADC			DOUBLE,
    fuelload2		DOUBLE,
    ignload 		DOUBLE,
    ignload2 		DOUBLE,
    synccnt 		DOUBLE,
    timing_err 		DOUBLE,
    deltaT 			DOUBLE,
    wallfuel1		DOUBLE,
    gpioadc0		DOUBLE,
    gpioadc1		DOUBLE,
    gpioadc2		DOUBLE,
    gpioadc3		DOUBLE,
    gpioadc4		DOUBLE,
    gpioadc5		DOUBLE,
    gpioadc6		DOUBLE,
    gpioadc7		DOUBLE,
    gpiopwmin0		DOUBLE,
    gpiopwmin1		DOUBLE,
    gpiopwmin2		DOUBLE,
    gpiopwmin3		DOUBLE,
    adc6			DOUBLE,
    adc7			DOUBLE,
    wallfuel2		DOUBLE,
    EAEFuelCorr2	DOUBLE,
    boostduty		DOUBLE,
    syncreason		DOUBLE,
    user0			DOUBLE,
    inj_adv1		DOUBLE,
    inj_adv2		DOUBLE,
    pulseWidth3		DOUBLE,
    pulseWidth4		DOUBLE,
    vetrim1curr		DOUBLE,
    vetrim2curr		DOUBLE,
    vetrim3curr		DOUBLE,
    vetrim4curr		DOUBLE,
    maf 			DOUBLE,
    eaeload1		DOUBLE,
    afrload1		DOUBLE,
    rpmdot			DOUBLE,
    gpioport0		DOUBLE,
    gpioport1		DOUBLE,
    gpioport2		DOUBLE,

    FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race) ON DELETE CASCADE,
    PRIMARY KEY (timestamp, ref_lap_num, ref_lap_race)
);
