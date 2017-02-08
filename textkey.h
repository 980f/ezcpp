#ifndef TEXTKEY_H
#define TEXTKEY_H

/** marker for internal tag for text, especially useful in getting 'const' onto the correct place */
typedef const char* TextKey;

/** add 1 to reserve space for null terminator
 * this is a marker for '1+' or '+1'
 */
constexpr unsigned Zguard(unsigned allocation){
  return 1 + allocation;
}


//////////// inline stringy stuff /////////////
#define ERRLOC(moretext) __FILE__ "::" moretext

// some unicode chars frequently used by us mathematical types:
#define Degree "\u00b0"
#define DegreeC Degree "C"
#define Sup2   "\u00B2"
//Alpha was used as a variable name in some sacred code :(
#define AlphaChar "\u03b1"
#define Beta  "\u03b2"
#define Gamma "\u03b3"
#define Delta "\u03b4"

#define Mu    "\u03bc"
#define Sigma "\u03c3"

///** wraps strtod() to regularize use of its 2nd parameter.
// * @param impure if not null gets set to whether the numerical part of the string was followed by more text.*/
//double toDouble(TextKey rawText, bool* impure = nullptr);

#endif // TEXTKEY_H
