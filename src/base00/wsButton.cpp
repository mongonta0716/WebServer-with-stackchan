// ----------------------------<wsButton.cpp>------------------------------------
#include "wsButton.h"

bool KEYLOCK_STATE = false;
int BTN_REQ = 0;

struct box_t
{
  int x;
  int y;
  int w;
  int h;
  int touch_id = -1;

  void setupBox(int x, int y, int w, int h)
  {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
  }
  bool contain(int x, int y)
  {
    return this->x <= x && x < (this->x + this->w) && this->y <= y && y < (this->y + this->h);
  }
};
static box_t BOX_SERVO;
static box_t BOX_STT;

void wsHandleBtn(String arg)
{
  String arg_mode = arg;
  if (arg_mode == "")
    return;

  arg_mode.toUpperCase();
  if (arg_mode == "BTNA")
  {
    BTN_REQ = BtnREQ_A;
  }
  else if (arg_mode == "BTNB")
  {
    BTN_REQ = BtnREQ_B;
  }
  else if (arg_mode == "BTNC")
  {
    BTN_REQ = BtnREQ_C;
  }
  else if (arg_mode == "BOX_SERVO")
  {
    webpage = BoxServoDo();
    return;
  }
  else if (arg_mode == "BOX_STT")
  {
    BTN_REQ = BtnREQ_BOX_STT;
  }
  else
  {
    BTN_REQ = 0;
    return;
  }

  webpage = "Button Pushed : " + arg;
  Serial.println(webpage);
  return;
}

void BtnReqGet()
{
  if (BTN_REQ == 0)
    return;

  Serial.print("BTN_REQ = ");
  Serial.println(BTN_REQ, DEC);

  int req = BTN_REQ;
  BTN_REQ = 0;

  switch (req)
  {
  case BtnREQ_A:
    BtnA_Do();
    break;

  case BtnREQ_B:
    BtnB_Do();
    break;

  case BtnREQ_C:
    BtnC_Do();
    break;

  // case BtnREQ_BOX_SERVO:
  //   BoxServoDo();
  //   break;

  case BtnREQ_BOX_STT:
    BoxSttDo();
    break;

  default:
    break;
  }

  return;
}

void ButtonManage()
{
  BtnReqGet();

  M5.update();

// *** 画面タッチボタン ***
#if defined(ARDUINO_M5STACK_Core2) || defined(ARDUINO_M5STACK_CORES3)
  auto count = M5.Touch.getCount();
  if (count)
  {
    if (!KEYLOCK_STATE)
    {
      auto t = M5.Touch.getDetail();
      if (t.wasPressed())
      {
        if (BOX_SERVO.contain(t.x, t.y))
        {
          BoxServoDo();
        }

        if (BOX_STT.contain(t.x, t.y) && (!mp3->isRunning()))
        {
          BoxSttDo();
        }
      }
    }
  }
#endif

  if (M5.BtnA.wasPressed())
  {
    if (!KEYLOCK_STATE)
      BtnA_Do();
  }

  // ** (BtnB) Timer Start/Stop
  if (M5.BtnB.wasPressed())
  {
    if (!KEYLOCK_STATE)
      BtnB_Do();
  }

  // ** (BtnC) batteryLeve and SysInfoDisp
  if (M5.BtnC.wasPressed())
  {
    if (!KEYLOCK_STATE)
      BtnC_Do();
  }
}



void BtnA_Do()
{
  tone(1);
  if (SYSINFO_DISP_STATE)
    sysInfoDispEnd();

  if (!RANDOM_SPEAK_STATE)
    RANDOM_SPEAK_ON_GET = true;
  else
    RANDOM_SPEAK_OFF_GET = true;
}

void BtnB_Do()
{
  tone(1);
  if (SYSINFO_DISP_STATE)
    sysInfoDispEnd();

  if (!TIMER_STARTED)
  { // ---- Timer 開始 ------
    randomSpeakStop2();
    TIMER_GO_GET = true;
    TIMER_STOP_GET = false;
  }
  else
  { // --- Timer 停止 ------
    TIMER_STOP_GET = true;
    TIMER_GO_GET = false;
  }
}

void BtnC_Do()
{
  tone(1);

  if (!SYSINFO_DISP_STATE)
  {
    randomSpeakStop2();
    timerStop2();
    report_batt_level();
    // sysInfoDispStart(0);
  }
  else
  {
    sysInfoDispEnd();
  }
}

void BoxSttDo()
{
  tone(1);
  if (SYSINFO_DISP_STATE)
    sysInfoDispEnd();

  SST_ChatGPT();
}

void BoxTouchSetup()
{
  BOX_SERVO.setupBox(80, 120, 80, 80);
  BOX_STT.setupBox(0, 0, M5.Display.width(), 60);
}