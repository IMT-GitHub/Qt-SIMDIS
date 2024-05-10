#include "DCS.h"

enum
{
  DCSAUX_QUIT,
  DCSAUX_HELP,
  DCSAUX_PRINTTRACKS,

  DCSAUX_UP,
  DCSAUX_DOWN,
  DCSAUX_LEFT,
  DCSAUX_RIGHT,
  DCSAUX_ROLLDEC,
  DCSAUX_ROLLINC,
  DCSAUX_SPEEDDEC,
  DCSAUX_SPEEDINC,
  DCSAUX_LEVEL,

  DCSAUX_BEAMTOGGLE,
  DCSAUX_GATETOGGLE,
  DCSAUX_SAMPLE_DATA,

  DCSAUX_NONE
};

typedef int dcsAuxKeyState;

int dcsAuxGetKeyState(dcsAuxKeyState& keystate, int wait);

bool dcsAuxGetPlatformUpdate(DCSPlatformData& platdata, dcsAuxKeyState& keystate);

bool dcsAuxGetBeamUpdate(DCSBeamData& beamdata, dcsAuxKeyState& keystate);

bool dcsAuxGetGateUpdate(DCSGateData& gatedata, dcsAuxKeyState& keystate);

bool dcsAuxGetBeamEvent(DCSBeamHeader& beamheader, DCSEvent& event, dcsAuxKeyState& keystate);

bool dcsAuxGetGateEvent(DCSGateHeader& gateheader, DCSEvent& event, dcsAuxKeyState& keystate);

int dcsAuxGetCommand(dcsAuxKeyState& keystate);
