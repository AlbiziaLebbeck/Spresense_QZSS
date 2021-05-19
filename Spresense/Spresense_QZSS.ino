#include <GNSS.h>
#include <GNSSPositionData.h>
#include <gpsutils/cxd56_gnss_nmea.h>

SpGnss Gnss;
SpNavData NavData;
unsigned long start_time;
char PositionData[sizeof(GnssPositionData)];

/* output NMEA */
static char nmea_buf[NMEA_SENTENCE_MAX_LEN];

static char *reqbuf(uint16_t size) {
  if (size > sizeof(nmea_buf)) {
    return NULL;
  }
  return nmea_buf;
}

static void freebuf(char *buf) {
  return;
}

static int outbin(char *buf, uint32_t len) {
  return len;
}

uint8_t *data;
unsigned long get_val(int startbit, int bitwidth)
{
  unsigned long val = 0;
  int index = (startbit + bitwidth - 1) / 8;
  int lsb = 7 - (startbit + bitwidth - 1) % 8;

  int i;
  for (i = 0; i < bitwidth; i++, lsb++) {
    if (lsb > 7) {
      index -= 1;
      lsb = 0;
    }
    val |= ((data[index] >> lsb) & 1) << i;
  }
  return val;
}

static int outnmea(char *buf) {
  Serial.print(buf);
  return 0;
}

void setup() {
  // put your setup code here, to run once:
  /* Initialize Serial */
  Serial.begin(115200);
  Serial2.begin(115200);

  /* Initialize GNSS */
  if (Gnss.begin()) {
    Serial.println("begin error!");
  }

  /* select satellite system */
  Gnss.select(GPS); //Gnss.select(GLONASS);
  Gnss.select(QZ_L1CA);
  Gnss.select(QZ_L1S); //Gnss.select(SBAS);

  /* set interval */
  Gnss.setInterval(1);
  if (Gnss.start(COLD_START)) {
    Serial.println("start error!");
  }

  /* use NMEA library */
  NMEA_InitMask();
  NMEA_SetMask(0x4001); // only QZQSM
  NMEA_OUTPUT_CB funcs;
  funcs.bufReq = reqbuf;
  funcs.out = outnmea;
  funcs.outBin = outbin;
  funcs.bufFree = freebuf;
  NMEA_RegistOutputFunc(&funcs);

  start_time = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  /* Check update. */
  if (millis() >= start_time + 15000) {
    Serial2.print("{\"Lat\":");
    Serial2.print(NavData.latitude, 6);
    Serial2.print(",\"Lon\":");
    Serial2.print(NavData.longitude, 6);
    Serial2.print(",\"Satellites\":");
    Serial2.print(NavData.numSatellites);
    Serial2.println("}");
    start_time = millis();
  }
  
  if (Gnss.waitUpdate(1000)) {
    /* Get navigation data. */
    
    Gnss.getNavData(&NavData);
    
    /* Output NMEA */
    Gnss.getPositionData(PositionData);
    NMEA_Output(&(((GnssPositionData*)PositionData)->Data));
    void *handle;
    if (handle = Gnss.getDCReport()) {
      NMEA_DcReport_Output(handle);
      data = ((struct cxd56_gnss_dcreport_data_s*)handle)->sf;
      Serial2.print("{\"Country_ID\":");
      Serial2.print(get_val(32,10));
      Serial2.print(",\"Event\":");
      Serial2.print(get_val(46,3));
      Serial2.print(",\"SubEvent\":");
      Serial2.print(get_val(49,4));
      Serial2.print(",\"Severity\":");
      Serial2.print(get_val(53,2));
      Serial2.print(",\"Target_Lat\":");
      Serial2.print(get_val(85,16)*0.002746624-90);
      Serial2.print("\"Target_Lon\":");
      Serial2.print(get_val(101,17)*0.002746624-180);
      Serial2.println("}");
    }
  }
}
