#ifndef TABLEOFPOINTERS_H
#define TABLEOFPOINTERS_H

/* sometimes you want a bunch of items to be processed in a group, without them having to know of the existence of that group.
 * "Bunder" was a class that you could add as a base class and that would put all members of that class into a list.
 * That however required that all objects be mutable as the mechanism involved a linkedlist that is modified during construction.
 *
 * This set of macro's lets you build a table in rom of pointers to members of the list, the objects can themselves be const (mostly).
 * Pointers are used so that polymorphic assemblages can be made.
*/

//the table is of pointers, the pointers are const, the object pointed to is not.
#define TableTag(PolledTimer,prior) const __attribute((used, section(".table." #PolledTimer "." #prior )))

//puts a timer in the list
#define RegisterAt(Polledtimer,varble,priority) PolledTimer * TableTag(PolledTimer,priority) PolledTimer##varble (&varble)
#define Register(PolledTimer,varble) RegisterAt(PolledTimer,varble,5)


//tagging the table, conveniently using the pointers to begin and end as the markers for the extent of the table
//the extreme tag numbers below are what define the range of priorites, They look like numbers but are text.

#define MakeTable(PolledTimer) \
PolledTimer * TableTag(PolledTimer,0) begin##PolledTimer##Table(nullptr);\
PolledTimer * TableTag(PolledTimer,9999999) end##PolledTimer##Table(nullptr);

#define ForTable(PolledTimer) \
for(PolledTimer * const *it=&begin##PolledTimer##Table;*(++it);)

//////////////////////////////
/* When all the objects are of the same concrete class the objects themselves can be in rom and the table is the objects, not pointers.
*/
#define ObjectTag(Classy,prior) __attribute((used, section(".table." #Classy "." #prior )))

//puts a timer in the list
#define MakeObjectAt(Classy,varble,priority,...) const Classy ObjectTag(Classy,priority) varble {__VA_ARGS__}
#define MakeObject(Classy,varble,...) MakeObjectAt(Classy,varble,5,__VA_ARGS__)

//tagging the table, conveniently using the pointers to begin and end as the markers for the extent of the table
//the extreme tag numbers below are what define the range of priorites, They look like numbers but are text.
//note that the values in the table delimiters are irrelevent, they are handy for debug
#define MakeConstTable(Classy) \
const Classy * TableTag(Classy,0) begin##Classy##sTable(reinterpret_cast<const Classy *const >(&begin##Classy##sTable+1));\
const Classy * TableTag(Classy,9999999) end##Classy##sTable(reinterpret_cast<const Classy *const >(&end##Classy##sTable))

#define ForObjects(Classy) \
for(const Classy *it=begin##Classy##sTable;it<end##Classy##sTable;++it)


#endif // TABLEOFPOINTERS_H
