//
// Created by hamster on 8/6/18.
//

#ifndef DC26_PARTY_BADGE_GODMODE_H
#define DC26_PARTY_BADGE_GODMODE_H

#include "common.h"

void godMode(void);

GODMODE_COMMAND getGodModeCommand(BADGE_ADV badge);
int getGodModeModifier(BADGE_ADV badge);
bool isInGodMode(BADGE_ADV badge);

#ifdef GM

void doGodmode(GODMODE_COMMAND command, int data);

#endif

#endif //DC26_PARTY_BADGE_GODMODE_H
