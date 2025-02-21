#pragma once
#define TABLE_OF_POINTERS 1
/* sometimes you want a bunch of items to be processed in a group, without some place having to know of the existence of all those members.
 * IE the members can all be private to their own modules, they don't need to be known by some special module like the one with main().
 * "Bundler" was a class that you could add as a base class and that would put all members of that class into a list.
 * That however required that all objects be mutable as the mechanism involved a linkedlist that is modified during construction,
 * which in turn required code to do that construction on every program start.
 *
 * This set of macro's lets you build a table in rom of pointers to members of the list, the objects can themselves be const (insome variants of the mechanism).
 * There are a few variants on the theme depending upon whether the table is const objects, pointers to objects.
 *
 * you will have to add a *(KEEP(.table.*)) to your linker script ROM section.
 * (The addition of the 'used' attribute below may have made the KEEP moot)
*/

//Table of pointers, the pointers are const, the object pointed to is not.
#define TableTag(ClassT,prior) const __attribute((used, section(".table." #ClassT "." #prior )))

//puts a thing in the list
#define RegisterAt(ClassX,varble,priority) ClassX * TableTag(ClassX,priority) ClassX##varble (&varble)
#define Register(ClassY,varble) RegisterAt(ClassY,varble,5)


//tagging the table, conveniently using the pointers to begin and end as the markers for the extent of the table
//the extreme tag numbers below are what define the range of priorites, They look like numbers but are text.
//this approach consumes two memory locations in the rom, instead of having to edit the linker script with a section for each table.

#define MakeTable(ClassM) \
ClassM * TableTag(ClassM,0) begin##ClassM##Table(nullptr);\
ClassM * TableTag(ClassM,9999999) end##ClassM##Table(nullptr)

#define ForTable(ClassK) \
for(ClassK * const *it=&begin##ClassK##Table;*(++it);)

//////////////////////////////
/* When all the objects are of the same concrete class the objects themselves can be in rom and the table is the objects, not pointers.
*/
#define ObjectTag(Classy,prior) __attribute((used, section(".table." #Classy "." #prior )))

//creates a thing in the list
#define MakeObjectAt(Classy,varble,priority,...) const Classy ObjectTag(Classy,priority) varble (__VA_ARGS__)
#define MakeObject(Classy,varble,...) MakeObjectAt(Classy,varble,5,__VA_ARGS__)

//to use this macro your class must not do anything "live" on construction as you will make two objects which get ignored
#define MakeObjectTable(Classy,...) \
const Classy ObjectTag(Classy,0) begin##ClassM##Table{__VA_ARGS__};\
const Classy ObjectTag(Classy,9999999) end##ClassM##Table{__VA_ARGS__}

//for start and count iteration, see ForObjects comment
#define ObjectTableStart(Classy) (&begin##ClassM##Table) + 1
#define ObjectTableSize(Classy) ((&end##ClassM##Table - &begin##ClassM##Table)/sizeof(Classy)) -1

/* iteration must ignore dummy first and last entities as they are used for markers, created by MakeObjectTable and the names are forced. */
#define ForObjects(Classy) \
for(const Classy *it=begin##Classy##sTable;++it<end##Classy##sTable;)


//tagging the table, conveniently using the pointers to begin and end as the markers for the extent of the table
//the extreme tag numbers below are what define the range of priorities, They look like numbers but are text.
//note that the values in the table delimiters are irrelevant, but they are handy for debug
#define MakeConstTable(Classy) \
const Classy * TableTag(Classy,0) begin##Classy##sTable(reinterpret_cast<const Classy *const >(&begin##Classy##sTable+1));\
const Classy * TableTag(Classy,9999999) end##Classy##sTable(reinterpret_cast<const Classy *const >(&end##Classy##sTable))

#define ForConstTableObjects(Classy) \
for(const Classy *it=begin##Classy##sTable;it<end##Classy##sTable;++it)

/////////////////////////////
/* needed this variant for a table of function pointers, which have an inherent constness that can't be explicitly declared.
*/
#define MakeRefAt(Classy,varble,priority) const Classy ObjectTag(Classy,priority) RefTo##varble (varble)
#define MakeRef(Classy,varble) MakeRefAt(Classy,varble,5)

#define MakeRefTable(Classy) \
Classy ObjectTag(Classy,0) begin##Classy##sTable(nullptr);\
Classy ObjectTag(Classy,9999999) end##Classy##sTable(nullptr)

#define ForRefs(Classy) \
for(Classy *it=&begin##Classy##sTable;**(++it);)
