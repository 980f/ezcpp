#pragma once

/** a singly linked list class where construction adds an item to the class, destruction removes it.
 Its first use was as a base class of class Grouped, and can make a list of statically created objects.
Once the core works we will try to incorporate lamdas.
*/
template <class Grouped> class Bundler {
  static Bundler *list;//=nullptr;//zero init by nature
  /** a private link keeps caller from slipping in a null on us and orphaning the tail */
  Bundler *next;
public:
  Bundler():next(0){
    //insert into list
    if(list == nullptr) {//if first to be created
      list = this; //start the list
    } else {
      for(Bundler *scan = list; scan ; scan = scan->next) {
        if(scan->next == nullptr) {
          scan->next = this;
          break;
        }
      }
    }
  }

  /** unlink object, this does NOT free them, freeing these calls this. */
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

  /** call a member function on all members of the group */
  static void forAll( groupedfn fn){
    for(Bundler *scan = list; scan ; scan = scan->next) {
      (static_cast<Grouped *>(scan)->*fn)();
    }
  }

  /** for each member of the group call a function that takes a member as an argument. */
  static void forAll(void (*fn)(Grouped &groupee)){
    for(Bundler *scan = list; scan ; scan = scan->next) {
      fn(*scan);
    }
  }

  /** if this bundle is the sole owner of pointers you need some means to destroy the entities */
  static void freeAll(){
    while(list){
      delete list;//this has a side effect of altering list.
    }
  }

};

/** the following seems a bit suspicious, might generate multiple instances */
template <typename T> Bundler<T>* Bundler<T>::list( 0 );
