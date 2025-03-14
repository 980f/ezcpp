#pragma once

bool isPresent(const char *flags, char flag);

class Char {
  public:
    char raw;

  public:
    Char(char raw = 0): raw(raw) { }

    Char&operator =(char newraw) {
      raw = newraw;
      return *this;
    }

    /** compare byte.
       @returns whether this is the same value as @param ch. */
    bool is(int ch) const noexcept {
      return raw == char(ch);
    }

    /** @returns @see is() */
    bool operator ==(int ch) const noexcept {
      return is(ch);
    }

    operator char(void) const  noexcept { //cuts down on compiler complaints, except in switches.
      return raw;
    }

    /** @returns whether this is allowed in numeric constant or enum name */
    bool numAlpha() const noexcept;

    /** @returns whether this is first char of an identifier, per JSON and C++ rules. This is pretty much anything that isn't a number, punctuation or a control char */
    bool startsName() const noexcept;
    /** @returns whether this is exactly an upper case letter */
    bool isUpper() const noexcept ;
    /** @returns whether this is exactly a lower case letter */
    bool isLower() const noexcept;
    /** @return as lower case */
    char asLower() const noexcept;
    /** @return as upper case */
    char asUpper() const noexcept;
    /** convert to lower case and @return whether that caused a change */
    bool toLower() noexcept;
    /** convert to upper case and @return whether that caused a change */
    bool toUpper() noexcept;

    /** @returns char conditionally toUpper'd or toLower'd or unmodified */
    bool toCase(int which) const {
      if (which > 0 && isLower()) {
        return raw & ~ 0x20;
      }
      if (which < 0 && isUpper()) {
        return raw | 0x20;
      }
      return raw;
    }

    /** @returns whether this is a decimal digit */
    bool isDigit() const noexcept;

    bool isControl() const noexcept;
    /** @returns whether this is first char of an number image, per JSON and C++ rules */
    bool startsNumber() const noexcept;

    /** @returns whether this is non-initial char of a number. Doesn't retain state so defective numbers will still get a true here*/
    bool isInNumber() const noexcept;

    /** @returns whether this is considered whitespace */
    bool isWhite() const noexcept;

    /** @returns whether char is in the @param tokens list */
    bool in(const char *tokens) const noexcept;

    /** @returns whether this is a legal hex digit per C's rules */
    bool isHexDigit() const noexcept;

    /** @returns the math value of this which is presumed to be a hexdigit, wild (but repeatable) trash if not.*/
    unsigned hexDigit() const noexcept;

    /** append this as hex digit to @param uch unicode receiver. Presumes caller has already checked isHexDigit() */
    template <typename Intish> void hexDigit(Intish &uch) const noexcept {
      uch <<= 4;
      uch |= hexDigit();
    }

    /** if this is a hexadecimal digit then apply it to @param accumulator, @returns whether it did so */
    template <typename Intish> bool appliedNibble(Intish&accumulator)const noexcept {
      if (isHexDigit()) {
        hexDigit(accumulator);
        return true;
      } else {
        return false;
      }
    }

    /** when you are really sure that this is a decimal digit then call this guy, else call @see appliedDigit */
    template <typename Intish> void applyTo(Intish&accumulator)const noexcept {
      int digit = raw - '0';
      accumulator *= 10;
      accumulator += digit;
    }

    /** if this is a decimal digit then apply it to @param accumulator, @returns whether it did so */
    template <typename Intish> bool appliedDigit(Intish&accumulator)const noexcept {
      if (isDigit()) {
        applyTo(accumulator);
        return true;
      } else {
        return false;
      }
    }

    /** hex digit character for the 0th==low or 1th=high nibble of this char.
       we could use a boolean for the nibble select, but want this to look like the unicode variation of this class.*/
    char hexNibble(unsigned sb) const noexcept;

    /** @returns whether this char needs a slash preceding it */
    bool needsSlash() const noexcept;

    /** @returns the c-escape partner of this. 'n' goes to newline, a newline goes to 'n' */
    char slashee()const noexcept;
};
