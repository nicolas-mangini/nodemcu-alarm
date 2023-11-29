#include <Arduino.h>

static boolean is_between(int currentHour, int startHour, int endHour)
{
    if (endHour > startHour) {
        return currentHour >= startHour && currentHour <= endHour;
    }
    else if (endHour < startHour) {
        return currentHour >= startHour || currentHour <= endHour;
    }
    return false;
}
