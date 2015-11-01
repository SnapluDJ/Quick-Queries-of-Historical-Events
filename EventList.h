//
// EVENTLIST.H
// Interface for your EventList collection type.
//

#ifndef __EVENTLIST_H
#define __EVENTLIST_H

#include <vector>
#include <string>

#include "Event.h"

//
// Single pillar object
//
class EventPillar  {
public:
  EventPillar(Event* e, int height);
  ~EventPillar();

  int pillarHeight;
  Event *record;
  EventPillar ** next;



};

//
// Skiplist object
//
class EventList {
public:
  EventList();
  ~EventList();
  
  EventPillar *head, *tail;
  Event *initHead, *initTail;

  void enlargeHeight();

  //
  // Add an Event to the list.
  //
  void insert(Event *e);
  
  //
  // Remove all Events in the list with the specified year.
  //
  void remove(int year);
  
  //
  // Find all events with greatest year <= input year
  //
  std::vector<Event *> *findMostRecent(int year);
  
  //
  // Find all Events within the specific range of years (inclusive).
  //
  std::vector<Event *> *findRange(int first, int last);
  
  ///////////////////////////////////////////////////////////////////

  // Print entire list (for testing/debugging)
  void dump(std::ostream& os);
  
private:
  
  int randomHeight();
};

#endif
