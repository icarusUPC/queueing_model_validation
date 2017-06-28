/*
 * HotSpotList.h
 *
 *  Created on: 25/02/2015
 *      Author: esther
 */

#ifndef HOTSPOTLIST_H_
#define HOTSPOTLIST_H_

#include <iostream>
#include <fstream>
#include <list>

#include "HotSpot.h"

using namespace std;

class HotSpotList {

	list<HotSpot*> hotSpots;

public:
	HotSpotList();
	virtual ~HotSpotList();

	void AddHotSpot(HotSpot* h);
	void DeleteHotSpot(HotSpot* h);
	void PostProcess();
	int WriteTxt(string fileName);

	list<HotSpot*> HotSpots() const	{ return hotSpots; }
	int Size() const {	return hotSpots.size(); }
};

#endif /* HOTSPOTLIST_H_ */
