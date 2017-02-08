#include "cstr.h"
#include "string.h"


Cstr::Cstr() : ptr(nullptr){
  //#nada
}

Cstr::Cstr(TextKey target) : ptr(target){
  //#nada
}

Cstr::~Cstr(){
  //#need explicit instantiation for vtable to get emitted.
}

TextKey Cstr::operator =(TextKey ptr){
  this->ptr = ptr;
  return ptr;
}

TextKey Cstr::c_str() const noexcept {
  return ptr;
}

Cstr::operator const char *() const noexcept {
  return ptr ? ptr : emptyString;//the const in the return type allows us to point to a byte of read-only memory.
}

bool Cstr::isTrivial() const noexcept {
  return ptr==nullptr || *ptr==0;
}

bool Cstr::nonTrivial() const noexcept {
  return ptr != nullptr && *ptr != 0;

}

unsigned Cstr::length() const noexcept {
  return nonTrivial() ? static_cast<unsigned>(strlen(ptr)) : 0;
}

bool Cstr::endsWith(char isit) const noexcept {
  return *this[length()-1]==isit;
}

bool Cstr::is(TextKey other) const noexcept {
    if(ptr == other) {//same object
      return true;
    }
    if(ptr == nullptr) {
      return Cstr(other).isTrivial();
    }
    if(other == nullptr) {
      return *ptr == 0;
    }
    return 0 == strcmp(ptr, other);
}

char Cstr::operator [](const Index &index) const noexcept {
  return (nonTrivial()&&isValid(index)) ? ptr[index]:0;
}

/** attempt to match the reasoning of the @see same() function with respect to comparing null strings and empty strings */
int Cstr::cmp(TextKey rhs) const noexcept {
  if(ptr) {
    if(rhs) {
      return strcmp(ptr,rhs);
    } else {//rhs is nullptr
      return *ptr ? 1 : 0;
    }
  } else {//this wraps nullptr
    return Cstr(rhs).nonTrivial() ? -1 : 0;
  }
} // Zstring::cmp

bool Cstr::startsWith(TextKey other) const noexcept {
  if(ptr == nullptr) {
    return Cstr(other).isTrivial();
  }
  if(ptr == other) {
    return true;
  }
  if(other == nullptr) {//all strings start with nothing, for grep '*' case.
    return true;
  }
  //could do the following with strlen and strncmp, but that is more execution. A variant of strcmp which returns the index of mismatch would be handy.
  const char *s(ptr);
  while(char c = *other++) {
    char m = *s++;
    if(!m) {
      return false;//other is longer than this
    }
    if(c!=m) {
      return false;//mismatch on existing chars
    }
  }
  return true;
} // Cstr::startsWith

Index Cstr::index(char ch) const noexcept {
  if(const char *candidate = chr(ch)) {
    return candidate - ptr;
  } else {
    return BadIndex;
  }
}

Index Cstr::rindex(char ch) const noexcept {
  if(const char *candidate = rchr(ch)) {
    return candidate - ptr;
  } else {
    return BadIndex;
  }
}

const char *Cstr::chr(int chr) const noexcept {
  if(nonTrivial()) {
    return strchr(ptr,chr);
  } else {
    return nullptr;
  }
}

const char *Cstr::rchr(int chr) const noexcept {
  if(nonTrivial()) {
    return strrchr(ptr,chr);
  } else {
    return nullptr;
  }
} // TextPointer::startsWith

void Cstr::clear() noexcept{
  ptr = nullptr;
}
