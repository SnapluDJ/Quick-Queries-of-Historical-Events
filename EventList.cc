//
// EVENTLIST.CC
// Skeleton code for your EventList collection type.
//

#include <iostream>
#include <cstdlib>
#include <iomanip>   // for dump() function

#include "EventList.h"

using namespace std;

int maxUsedHeight;

////////////////////////////////////////////////////////////////////
// Here's a suitable geometric random number generator for choosing
// pillar heights.  We treat the middle bit of each random number
// generated as a random coin flip.
////////////////////////////////////////////////////////////////////

int EventList::randomHeight()
{
  int v = 1;
  while ((rand() >> 16) & 0x01) { v++; }
  return v;
}

////////////////////////////////////////////////////////////////////
// Methods for the Lab
////////////////////////////////////////////////////////////////////

//
// EventPillar constructor
//
EventPillar::EventPillar(Event *e, int height)
{
	record = e;
	pillarHeight = height;
	next = new EventPillar* [height];
}

//
// EventPillar destructor
//
EventPillar::~EventPillar()
{
	delete[] next;
}

//

//
// EventList constructor
//
EventList::EventList()
{
	initHead = new Event(-9999,"noDescription");
	initTail = new Event(9999, "noDescription");

	head = new EventPillar(initHead, 1);
	tail = new EventPillar(initTail, 1);

	head->next[0] = tail;
	tail->next[0] = nullptr;
	maxUsedHeight = 1;
}


//
// EventList destructor
//
EventList::~EventList()
{
	EventPillar *p, *q;

	p = head;
	q = p->next[0];

	while (q != tail){
		p = q;
		q = q->next[0];
		delete p;
	}

	delete head;
	delete tail;
	delete initHead;
	delete initTail;
}

//
//Enlarge the height of head and tail
//
void EventList::enlargeHeight()
{
	EventPillar *newHead, *newTail;
	int level;
	EventPillar *p;

	level = head->pillarHeight-1;
	p = head;
	newHead = new EventPillar(initHead, 2*head->pillarHeight);
	newTail = new EventPillar(initTail, 2*tail->pillarHeight);

	//Let last pillar at every level link to newTail
	while (level>=0){
		while (p->next[level] != tail)
			p = p->next[level];
		p->next[level] = newTail;
		newTail->next[level] = nullptr;
		--level;
		p = head;
	}

	//Copy head to newHead
	for (int i=0; i<head->pillarHeight; i++)
		newHead->next[i] = head->next[i];

	//Link newHead and newTail
	for (int i = head->pillarHeight; i<2*head->pillarHeight; i++){
		newHead->next[i] = newTail;
		newTail->next[i] = nullptr;
	}

	delete head;
	delete tail;
	head = newHead;
	tail = newTail;

	return;
}


//
// Add an Event to the list.
//
void EventList::insert(Event *e)
{
	/////////////////////////
	// One pillar per event//
	//      O(logn)        //
	/////////////////////////
	int h;
	int level;
	EventPillar *p, *q;

	h = randomHeight();
	EventPillar* newRecord = new EventPillar(e, h);

	if (h > maxUsedHeight)
		maxUsedHeight = h;
	while (head->pillarHeight < maxUsedHeight)
		enlargeHeight();

	level = head->pillarHeight-1;
	p = head;

	while(level >= 0){
		q = p->next[level];
		if (q->record->year > newRecord->record->year){
			if (level < newRecord->pillarHeight){
				p->next[level] = newRecord;
				newRecord->next[level] = q;
			}
			--level;
		}
		else p = q;
	}

	return;
}


//
// Remove all Events in the list with the specified year.
//
void EventList::remove(int year)
{
	////////////////////
	//    O(m+logn)   //
	////////////////////
	int level;
	EventPillar *p, *q, *temp;

	level = head->pillarHeight-1;
	p = head;

	// Reason for "level >=1" : at level 0, we need to delete
	// the pillar from memory
	while (level >= 1){
		q = p->next[level];
		if (q->record->year == year){
			while (q->record->year == year)
				q = q->next[level];
			p->next[level] = q;
		}
		else if (q->record->year < year)
			p = q;
		else
			--level;
	}

	//Find the right location at level 0
	q = p->next[level]; // Reset q
	while (q->record->year < year){
		p = q;
		q = p->next[level];
	}

	//Delete from the memory
	while (q->record->year == year){
		temp = q;
		q = q->next[level];
		delete temp;
	}
	p->next[level] = q;

	return;
}


//
// Find all events with greatest year <= input year.
//
vector<Event *> *EventList::findMostRecent(int year)
{
	////////////////////
	//    O(m+logn)   //
	////////////////////
	int level;
	int tempYear;
	EventPillar *p, *q;
	vector<Event*>* results = new vector<Event*>;

	//No event
	if (year < head->next[0]->record->year)
		return nullptr;

	level = head->pillarHeight-1;
	p = head;

	while (level >= 0){
		q = p->next[level];
		if (q->record->year < year)
			p = q;
		else
			--level;
	}
	tempYear = p->record->year;

	if (q->record->year == year) //year is in the skipList
		while (q->record->year <= year){
				results->push_back(q->record);
				q = q->next[0];
		}
	else{ //Do while loop again with tempYear since tempYear is in the skipList
		level = head->pillarHeight-1;
		p = head;

		while (level >= 0){
			q = p->next[level];
			if (q->record->year < tempYear)
				p = q;
			else
				--level;
		}

		while (q->record->year < year){
			results->push_back(q->record);
			q = q->next[0];
		}
	}

	return results;
}


//
// Find all Events within the specific range of years (inclusive).
//
vector<Event *> *EventList::findRange(int first, int last)
{
	////////////////////
	//    O(m+logn)   //
	////////////////////
	int level;
	EventPillar *p,*q;
	vector<Event*>* results = new vector<Event*>;

	level = head->pillarHeight-1;
	p = head;

	while (level >= 0){
		q = p->next[level];
		if (q->record->year < first)
			p = q;
		else
			--level;
	}

	//No event
	if (q->record->year > last)
		return nullptr;

	while (q->record->year <= last){
		results->push_back(q->record);
		q = q->next[0];
	}

	return results;
}


//
// Utility function to print list
// Usage (in Lab3.cc): list->dump(cout);
//
void EventList::dump(ostream& os)
{

  // Your code may look different depending on how you implement
  //   the EventPillar object.  Use below code as an example, 
  //   but feel free to modify it to suit your needs.

//  os << "Event list: maxUsedHeight: " << maxUsedHeight << endl;
//  EventPillar *p = head;
//  while(p)
//  {
//    int key = (p->record) ? p->record->year : numeric_limits<int>::max();
//    os << "Pillar " << p << " key "
//      << key << ": ";
//    for(int i=0; i < p->pillarHeight; ++i)
//      os << setw(10) << p->next[i];
//    os << endl;
//    p = p->next[0];
//  }
//  cout<<endl;
//
//  return;

}
