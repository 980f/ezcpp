//"(C) Andrew L. Heilveil, 2017"
#include "char.h"
#include <cctype>
#include "index.h"  //for escaping
#include "cstr.h"   //for escaping

bool isPresent(const char *flags, char flag){
  unsigned badStringLimiter = 26; //in case string pointer is garbage we don't want to read all of ram

  if(flags) {
    char probe;
    while((probe = *flags++) && badStringLimiter-- > 0) {
      if(flag == probe) {
        return true;
      }
    }
  }
  return false;
} /* isPresent */

/** In the string below there are pairs of (escape code: escaped char) e.g. n\n
   it looks confusing as some of the chars being escaped are unchanged by the process.
   when removing a slash you search for the char and pass back the odd member of the pair
   when adding slashes you find the char and emit a slash and the even member of the pair
*/
static const char *SeaScapes = "a\a" "b\b" "c\x03" "f\f" "n\n" "r\r" "t\t" "v\v" "\\\\" "//" "''" "??" "\"\"";


bool Char::needsSlash()const noexcept {
  return Index(Cstr(SeaScapes).index(raw)).isOdd();//isOdd includes checking for valid.
}

/** while named for one usage this works symmetrically */
char Char::slashee() const noexcept {
  Index present = Cstr(SeaScapes).index(raw);
  if (present.isValid()) {
    return SeaScapes[1 ^ present]; //xor with 1 swaps even and odd.
  } else {
    return raw;
  }
}

////////////////////////////////////

bool Char::numAlpha() const noexcept {
  return isalnum(raw) || isPresent("+-.", raw);
}

bool Char::startsName() const noexcept {
  return isalpha(raw);
}

bool Char::isDigit() const noexcept {
  return isdigit(raw);
}

bool Char::isLower() const noexcept {
  return 'a' <= raw && raw <= 'z';
}

char Char::asLower() const noexcept {
  return static_cast<char>(tolower(raw));
}

bool Char::isUpper() const noexcept {
  return 'A' <= raw && raw <= 'Z';
}

char Char::asUpper() const noexcept {
  return static_cast<char>(toupper(raw));
}

bool Char::isControl() const noexcept {
  return iscntrl(raw);
}

/** convert to lower case and @return whether that caused a change */
bool Char::toLower() noexcept {
  if (isUpper()) {
    raw |= 0x20;
    return true;
  } else {
    return false;
  }
}

/** convert to lower case and @return whether that caused a change */
bool Char::toUpper() noexcept {
  if (isLower()) {
    raw &= ~0x20;
    return true;
  } else {
    return false;
  }
}

bool Char::isInNumber() const noexcept {
  return isdigit(raw) || in("+-.Ee");
}

bool Char::isWhite() const noexcept {
  return isspace(raw);
}

bool Char::in(const char *tokens) const noexcept {
  return isPresent(tokens, raw);
}

bool Char::isHexDigit() const noexcept {
  return isxdigit(raw);
}

//#include "ignoresignwarnings.h"
unsigned Char::hexDigit() const noexcept {
  unsigned trusting = (raw | 0x20) - '0'; //tolowerthen subtract char for zero.
  if ((trusting > 9)) {
    trusting-=39;// ~ 'a' -'0' + 10
  }
  return trusting; //'A'-'0' = 17, want 10 for that
}

char Char::hexNibble(unsigned sb) const noexcept {
  unsigned char nib = 15 & (raw >> (sb * 4)); //push to low nib
  return nib > 9 ? 'A' + nib - 10 : '0' + nib;
}

bool Char::startsNumber() const noexcept {
  return isDigit()||is('-')||is('+');//todo: ?how did this disappear
}
