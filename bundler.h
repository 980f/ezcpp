#pragma once

/** a singly linked list class where construction adds an item to the class, destruction removes it.
 Its first use was as a base class of class Grouped, and can make a list of statically created objects.
Once the core works we will try to incorporate lamdas.
*/
template <class Grouped> class Bundler {
  static Bundler *list;//=nullptr;//zero init by nature
  Bundler *next;
public:
  Bundler():next(0){
    //insert into list, presumes that timer service isn't started until all timer objects are constructed.
    if(list == 0) {//if first to be created
      list = this; //start the list
    } else {
      for(Bundler *scan = list; scan ; scan = scan->next) {
        if(scan->next == 0) {
          scan->next = this;
          break;
        }
      }
    }
  }

//can't destruct until we add some  more linker support:
// __aeabi_atexit  __dso_handle
  /** unlink object */
  ~Bundler(){
    if(list==this){
      list=list->next;
    } else {
      for(Bundler *scan = list; scan ; scan = scan->next) {
        if(scan->next == this) {
          scan->next = this->next;
          break;
        }
      }
    }
  }

  typedef  void (Grouped::*groupedfn)(void);

  /** do a grouped action */
  static void forAll( groupedfn fn){
    for(Bundler *scan = list; scan ; scan = scan->next) {
      (static_cast<Grouped *>(scan)->*fn)();
    }
  }

  /** do a grouped action */
  static void forAll(void (*fn)(Grouped &groupee)){
    for(Bundler *scan = list; scan ; scan = scan->next) {
      fn(*scan);
    }
  }

};


template <typename T> Bundler<T>* Bundler<T>::list( 0 );
