#pragma once

#include "daisy_core.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * Return code for osc_match() that indicates that the entire address was successfully matched
   */
#define OSC_MATCH_ADDRESS_COMPLETE 1

  /**
   * Return code for osc_match() that indicates that the entire pattern was successfully matched
   */
#define OSC_MATCH_PATTERN_COMPLETE 2

  /**
   * Match a pattern against an address.  In the case of a partial match, pattern_offset
   * and address_offset will contain the number of bytes into their respective strings
   * where the match failed.
   *
   * @param pattern The pattern to match
   * @param address The address to match
   * @param pattern_offset The number of bytes into the pattern that were matched successfully
   * @param address_offset The number of bytes into the address that were matched successfully
   * @return 0 if the match failed altogether, or an or'd combination of OSC_MATCH_ADDRESS_COMPLETE and
   * OSC_MATCH_PATTERN_COMPLETE.
   */
  int osc_match(const char *pattern, const char *address, int *pattern_offset, int *address_offset);

#ifdef __cplusplus
}
#endif