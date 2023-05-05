#define OSC_MATCH_ENABLE_2STARS 1
#define OSC_MATCH_ENABLE_NSTARS 1

#include <string.h>
#include "OSCMatch.h"

static int osc_match_star(const char *pattern, const char *address);
static int osc_match_star_r(const char *pattern, const char *address);
static int osc_match_single_char(const char *pattern, const char *address);
static int osc_match_bracket(const char *pattern, const char *address);
static int osc_match_curly_brace(const char *pattern, const char *address);

int osc_match(const char *pattern, const char *address, int *pattern_offset, int *address_offset)
{
	if(!strcmp(pattern, address)){
		*pattern_offset = strlen(pattern);
		*address_offset = strlen(address);
		return OSC_MATCH_ADDRESS_COMPLETE | OSC_MATCH_PATTERN_COMPLETE;
	}
	
	const char *pattern_start;
	const char *address_start;
	
	pattern_start = pattern;
	address_start = address;
	
	*pattern_offset = 0;
	*address_offset = 0;
	
	while(*address != '\0' && *pattern != '\0'){
		if(*pattern == '*'){
			if(!osc_match_star(pattern, address)){
				return 0;
			}
			while(*pattern != '/' && *pattern != '\0'){
				pattern++;
			}
			while(*address != '/' && *address != '\0'){
				address++;
			}
		}else if(*address == '*'){
			while(*pattern != '/' && *pattern != '\0'){
				pattern++;
			}
			while(*address != '/' && *address != '\0'){
				address++;
			}
		}else{
			int n = 0;
			if(!(n = osc_match_single_char(pattern, address))){
				return 0;
			}
			if(*pattern == '['){
				while(*pattern != ']'){
					pattern++;
				}
				pattern++;
				address++;
			}else if(*pattern == '{'){
				while(*pattern != '}'){
					pattern++;
				}
				pattern++;
				address += n;
			}else{
				pattern++;
				address++;
			}
		}
	}
	
	*pattern_offset = pattern - pattern_start;
	*address_offset = address - address_start;
	
	int r = 0;
	
	if(*address == '\0') {
		r |= OSC_MATCH_ADDRESS_COMPLETE;
	}
	
	if(*pattern == '\0') {
		r |= OSC_MATCH_PATTERN_COMPLETE;
	}
	
	return r;
}

static int osc_match_star(const char *pattern, const char *address)
{
	const char *address_start = address;
	const char *pattern_start = pattern;
	int num_stars = 0;
	if(*address == '\0') { return 0; }
	while(*address != '/' && *address != '\0'){
		address++;
	}
	while(*pattern != '/' && *pattern != '\0'){
		if(*pattern == '*'){
			num_stars++;
		}
		pattern++;
	}
	pattern--;
	address--;
	switch(num_stars){
		case 1:
		{
			const char *pp = pattern, *aa = address;
			while(*pp != '*'){
				if(!(osc_match_single_char(pp, aa))){
					return 0;
				}
				if(*pp == ']' || *pp == '}'){
					while(*pp != '[' && *pp != '{'){
						pp--;
					}
				}
				pp--;
				aa--;
			}
		}
			break;
		case 2:
#if (OSC_MATCH_ENABLE_2STARS == 1)
		{
			const char *pp = pattern, *aa = address;
			while(*pp != '*'){
				if(!(osc_match_single_char(pp, aa))){
					return 0;
				}
				if(*pp == ']' || *pp == '}'){
					while(*pp != '[' && *pp != '{'){
						pp--;
					}
				}
				pp--;
				aa--;
			}
			aa++; // we want to start one character forward to allow the star to match nothing
			const char *star2 = pp;
			const char *test = aa;
			int i = 0;
			while(test > address_start){
				pp = star2 - 1;
				aa = test - 1;
				i++;
				while(*pp != '*'){
					if(!osc_match_single_char(pp, aa)){
						break;
					}
					if(*pp == ']' || *pp == '}'){
						while(*pp != '[' && *pp != '{'){
							pp--;
						}
					}
					pp--;
					aa--;
				}
				if(pp == pattern_start){
					return 1;
				}
				test--;
			}
			return 0;
		}
			break;
#else
			return 0;
#endif
		default:
#if (OSC_MATCH_ENABLE_NSTARS == 1)
			return osc_match_star_r(pattern_start, address_start);
			break;
#else
			return 0;
#endif
	}
	return 1;
}

#if (OSC_MATCH_ENABLE_NSTARS == 1)
static int osc_match_star_r(const char *pattern, const char *address)
{
	if(*address == '/' || *address == '\0'){
		if(*pattern == '/' || *pattern == '\0' || (*pattern == '*' && ((*(pattern + 1) == '/') || *(pattern + 1) == '\0'))){
			return 1;
		}else{
			return 0;
		}
	}
	if(*pattern == '*'){
		if(osc_match_star_r(pattern + 1, address)){
			return 1;
		}else{
			return osc_match_star_r(pattern, address + 1);
		}
	}else{
		if(!osc_match_single_char(pattern, address)){
			return 0;
		}
		if(*pattern == '[' || *pattern == '{'){
			while(*pattern != ']' && *pattern != '}'){
				pattern++;
			}
		}
		return osc_match_star_r(pattern + 1, address + 1);
	}
}
#endif

static int osc_match_single_char(const char *pattern, const char *address)
{
	switch(*pattern){
		case '[':
			return osc_match_bracket(pattern, address);
		case ']':
			while(*pattern != '['){
				pattern--;
			}
			return osc_match_bracket(pattern, address);
		case '{':
			return osc_match_curly_brace(pattern, address);
		case '}':
			while(*pattern != '{'){
				pattern--;
			}
			return osc_match_curly_brace(pattern, address);
		case '?':
			return 1;
		default:
			if(*pattern == *address){
				return 1;
			}else{
				return 0;
			}
	}
	return 0;
}

static int osc_match_bracket(const char *pattern, const char *address)
{
	pattern++;
	int val = 1;
	if(*pattern == '!'){
		pattern++;
		val = 0;
	}
	int matched = !val;
	while(*pattern != ']' && *pattern != '\0'){
		// the character we're on now is the beginning of a range
		if(*(pattern + 1) == '-'){
			if(*address >= *pattern && *address <= *(pattern + 2)){
				matched = val;
				break;
			}else{
				pattern += 3;
			}
		}else{
			// just test the character
			if(*pattern == *address){
				matched = val;
				break;
			}
			pattern++;
		}
	}
	return matched;
}

static int osc_match_curly_brace(const char *pattern, const char *address)
{
	pattern++;
	const char *ptr = pattern;
	while(*ptr != '}' && *ptr != '\0' && *ptr != '/'){
		while(*ptr != '}' && *ptr != '\0' && *ptr != '/' && *ptr != ','){
			ptr++;
		}
		int n = ptr - pattern;
		if(!strncmp(pattern, address, n)){
			return n;
		}else{
			ptr++;
			pattern = ptr;
		}
	}
	return 0;
}