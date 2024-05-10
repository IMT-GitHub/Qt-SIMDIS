/* -*- mode: c++ -*- */
/****************************************************************************
 *****                                                                  *****
 *****                   Classification: UNCLASSIFIED                   *****
 *****                    Classified By:                                *****
 *****                    Declassify On:                                *****
 *****                                                                  *****
 ****************************************************************************
 *
 *
 * Developed by: Naval Research Laboratory, Tactical Electronic Warfare Div. 
 *               EW Modeling and Simulation, Code 5770
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * For more information please send email to simdis@enews.nrl.navy.mil
 *
 * January 23, 2012 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#include <string>
#include "VSIAssert.h"
#include "iostreamc"
#include "timec.h"
#include "net.h"

namespace
{

// Returns 0 on success
int writeString(SockMulticast* sock, const std::string& text)
{
  return (sock->write(text.c_str(), 1) == 1) ? 0 : 1;
}

// char is intentional here; bool = expected value was found (true = yes)
bool readExpectedValue(SockMulticast* sock, const char expectedValue)
{
  if (!sock->readReady(0, 0))
  {
    // Return out if there's no read ready so we don't block the socket
    return expectedValue == '\0';
  }
  char data[1024];
  // Set [0] to known bad value
  data[0] = '\0';
  int rv = sock->read(data, sizeof(data));
  // If there was an error reading, return success ONLY if we expected failure
  if (rv <= 0)
    return expectedValue == '\0';
  // If we expected an error reading, we didn't get it!  Error out here
  if (expectedValue == '\0')
    return false;
  // Return true only if the first character matches
  return data[0] == expectedValue;
}

// Tests SIM-1867 to ensure that groups are properly filtered
int testGroup()
{
  static const uint16_t MC_PORT = 10436; // random port
  static const std::string MC_ADDR = "225.0.6.144"; // random address for MC
  static const std::string MC_DIFF_ADDR = "225.0.6.155"; // Another address
  int rv = 0;
  SockMulticast sender(MC_PORT, MC_ADDR.c_str(), "");
  rv += VSI_ASSERT(sender.isValid());
  SockMulticast recv1(MC_PORT, MC_ADDR.c_str(), "");
  rv += VSI_ASSERT(recv1.isValid());
  SockMulticast recv2(MC_PORT, MC_ADDR.c_str(), "");
  rv += VSI_ASSERT(recv2.isValid());
  // construct another multicast with only a different group
  SockMulticast diffGroup(MC_PORT, MC_DIFF_ADDR.c_str(), "");
  rv += VSI_ASSERT(diffGroup.isValid());
  // Write out data, then sleep to give OS time to process
  rv += VSI_ASSERT(0 == writeString(&sender, "5"));
  Sleep(50);
  // Both multicast should receive the data, and the different group should not receive it
  rv += VSI_ASSERT(recv1.readReady(0,0));
  rv += VSI_ASSERT(recv2.readReady(0,0));
  // readReady is unable to distinguish between groups...
  //rv += VSI_ASSERT(!diffGroup.readReady(0,0));
  rv += VSI_ASSERT(readExpectedValue(&recv1, '5'));
  rv += VSI_ASSERT(readExpectedValue(&recv2, '5'));
  rv += VSI_ASSERT(readExpectedValue(&diffGroup, '\0'));

  return rv;
}

}


int MulticastTest(int argc, char* argv[])
{
  initializeSocket();
  int rv = 0;
  rv += testGroup();
  finalizeSocket();
  return rv;
}
