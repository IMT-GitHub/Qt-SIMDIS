#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "./SMSSaux.h"

#define PI (3.141592654)
#define D2R(deg) (deg * PI/180) 
#define SQ(x) (pow(x, 2.0))

static void print_commands()
{
  printf("\n\nKeyboard Commands:\n");
  printf("\tq - Quit\n");
  printf("\th - Print help\n");
  printf("\tp - Print tracks\n");
  printf("\t2 - Pitch up\n");
  printf("\t8 - Pitch down\n");
  printf("\t6 - Heading right\n");
  printf("\t4 - Heading left\n");
  printf("\t7 - Roll decrement\n");
  printf("\t9 - Roll increment\n");
  printf("\t1 - Speed decrement\n");
  printf("\t3 - Speed increment\n");
  printf("\t5 - Reset to straight and level\n");
  printf("\tb - Toggle beam\n");
  printf("\tg - Toggle gate\n");
}

int dcsAuxGetKeyState(dcsAuxKeyState& keystate, int wait)
{
  keystate = DCSAUX_NONE;

  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);

  // sleep for wait milliseconds
  struct timeval tv = {wait/1000, (wait%1000)*1000};

  // if no action on stdin
  select(1, &fds, NULL, NULL, &tv);
  if( !FD_ISSET(0, &fds))
    return keystate; // done

  char ch = getchar();
  fflush(stdin);

  switch( ch )
  {
  case 'q'://quit
    keystate = DCSAUX_QUIT;
    break;
  case 'p'://print tracks
    keystate = DCSAUX_PRINTTRACKS;
    break;
  case '2'://up
    keystate = DCSAUX_UP;
    break;
  case '8'://down
    keystate = DCSAUX_DOWN;
    break;
  case '6'://right
    keystate = DCSAUX_RIGHT;
    break;
  case '4'://left
    keystate = DCSAUX_LEFT;
    break;
  case '7'://-roll
    keystate = DCSAUX_ROLLDEC;
    break;
  case '9'://+roll
    keystate = DCSAUX_ROLLINC;
    break;
  case '1'://-speed
    keystate = DCSAUX_SPEEDDEC;
    break;
  case '3'://+speed
    keystate = DCSAUX_SPEEDINC;
    break;
  case '5'://straight and level
    keystate = DCSAUX_LEVEL;
    break;
  case 'b'://toggle beam
    keystate = DCSAUX_BEAMTOGGLE;
    break;
  case 'g'://toggle gate
    keystate = DCSAUX_GATETOGGLE;
    break;
  case 's'://toggle Sample Data
    keystate = DCSAUX_SAMPLE_DATA;
    break;
  case 'h'://print help
    keystate = DCSAUX_HELP;
    print_commands();
    break;
  }

  return keystate;
}

bool dcsAuxGetPlatformUpdate(DCSPlatformData& platdata, dcsAuxKeyState& keystate)
{
  double last_time = platdata.time_;
  double psi = platdata.orientation_.x;
  double theta = platdata.orientation_.y;
  double phi = platdata.orientation_.z;
  double vx = platdata.velocity_.x;
  double vy = platdata.velocity_.y;
  double vz = platdata.velocity_.z;
  double speed = sqrt(SQ(vx) + SQ(vy) + SQ(vz));

  switch (keystate)
  {
  case DCSAUX_UP:
    theta = theta + D2R(3.0);
    if (theta > PI/2.0)
    {
      theta = PI/2.0;
    }
    vx = speed * cos(psi)* sin(PI/2.0 - theta);
    vy = speed * sin(psi)* sin(PI/2.0 - theta);
    vz = -speed * cos(PI/2.0 - theta);
    break;
  case DCSAUX_DOWN:
    theta = theta - D2R(3.0);
    if (theta < (-1 * PI/2.0))
    {
      theta = -1 * PI/2.0;
    }
    vx = speed * cos(psi) * sin(PI/2.0 - theta);
    vy = speed * sin(psi) * sin(PI/2.0 - theta);
    vz = -speed * cos(PI/2.0 - theta);
    break;
  case DCSAUX_RIGHT:
    psi = psi + D2R(3.0);
    if (psi > (2.0*PI))
    {
      psi -= 2.0 * PI;
    }
    vx = speed * cos(psi) * sin(PI/2.0 - theta);
    vy = speed * sin(psi) * sin(PI/2.0 - theta);
    vz = -speed * cos(PI/2.0 - theta);
    break;
  case DCSAUX_LEFT:
    psi = psi - D2R(3.0);
    if (psi < 0)
    {
      psi += 2.0 * PI;
    }
    vx = speed * cos(psi) * sin(PI/2.0 - theta);
    vy = speed * sin(psi) * sin(PI/2.0 - theta);
    vz = -speed * cos(PI/2.0 - theta);
    break;
  case DCSAUX_ROLLDEC:
    phi = phi - D2R(3.0);
    if (phi < 0)
    {
      phi += 2.0 * PI;
    }
    break;
  case DCSAUX_ROLLINC:
    phi = phi + D2R(3.0);
    if (phi > (2.0 * PI))
    {
      phi -= 2.0 * PI;
    }
    break;
  case DCSAUX_SPEEDDEC:
    if (speed < 1.0)
    {
      if (speed < 0.25)
      {
        speed = 0.0;
      }
      else
      {
        speed -= 0.25;
      }
    }
    else
    {
      speed = speed/2.0;
    }
    vx = speed * cos(psi) * sin(PI/2.0 - theta);
    vy = speed * sin(psi) * sin(PI/2.0 - theta);
    vz = -speed * cos(PI/2.0 - theta);
    break;
  case DCSAUX_SPEEDINC:
    if (speed < 1.0)
    {
      speed += 0.25;
    }
    else
    {
      speed = speed * 2.0;
    }
    vx = speed * cos(psi) * sin(PI/2.0 - theta);
    vy = speed * sin(psi) * sin(PI/2.0 - theta);
    vz = -speed*cos(PI/2.0 - theta);
    break;
  case DCSAUX_LEVEL:
    theta = 0.0;
    phi = 0.0;
    vx = speed * cos(psi) * sin(PI/2.0 - theta);
    vy = speed * sin(psi) * sin(PI/2.0 - theta);
    vz = -speed * cos(PI/2.0 - theta);
    break;
  }

  // Calculate new position before applying new orientation
  platdata.time_ = DCS::getyeartime();
  platdata.position_.x += (platdata.time_ - last_time) * platdata.velocity_.x;
  platdata.position_.y += (platdata.time_ - last_time) * platdata.velocity_.y;
  platdata.position_.z += (platdata.time_ - last_time) * platdata.velocity_.z;

  platdata.velocity_.x = vx;
  platdata.velocity_.y = vy;
  platdata.velocity_.z = vz;
  platdata.orientation_.x = psi;
  platdata.orientation_.y = theta;
  platdata.orientation_.z = phi;

  return true;
}

bool dcsAuxGetBeamUpdate(DCSBeamData& beamdata, dcsAuxKeyState& keystate)
{
  // Beams currently stay pointed straight ahead
  beamdata.time_ = DCS::getyeartime();
  return true;
}

bool dcsAuxGetGateUpdate(DCSGateData& gatedata, dcsAuxKeyState& keystate)
{
  // Gates currently stay pointed straight ahead
  gatedata.time_ = DCS::getyeartime();
  return true;
}

bool dcsAuxGetBeamEvent(DCSBeamHeader& beamheader, DCSEvent& event, dcsAuxKeyState& keystate)
{
  if (keystate == DCSAUX_BEAMTOGGLE)
  {
    beamheader.state_ = (beamheader.state_ == DCSSTATE_OFF) ? DCSSTATE_ON : DCSSTATE_OFF;

    event.time_ = DCS::getyeartime();
    event.data_ = beamheader.state_;
    event.eventtype_ = DCSEVENT_STATE;

    return true;
  }
  return false;
}

bool dcsAuxGetGateEvent(DCSGateHeader& gateheader, DCSEvent& event, dcsAuxKeyState& keystate)
{
  if (keystate == DCSAUX_GATETOGGLE)
  {
    gateheader.state_ = (gateheader.state_ == DCSSTATE_OFF) ? DCSSTATE_ON : DCSSTATE_OFF;

    event.time_ = DCS::getyeartime();
    event.data_ = gateheader.state_;
    event.eventtype_ = DCSEVENT_STATE;

    return true;
  }
  return false;
}

int dcsAuxGetCommand(dcsAuxKeyState& keystate)
{
  return keystate;
}
