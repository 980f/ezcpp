#ifndef TABLEOFPOINTERS_H
#define TABLEOFPOINTERS_H

//the table is of pointers, the pointers are const, the object pointed to is not.
#define TableTag(PolledTimer,prior) const __attribute((used, section(".table.##PolledTimer##." #prior )))

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

#endif // TABLEOFPOINTERS_H
