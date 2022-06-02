#include "contact_insertion.h"

#define CONTACT_MOVE_TOLERNACE  0.1f

void contactInsert(struct ContactManifold* contactState, struct EpaResult* epaResult) {
    int shouldReplace = 1;
    int replacementIndex = 0;
    float smallestOverlap = -10000.0f;

    int insertIndex;

    for (insertIndex = 0; insertIndex < contactState->contactCount && insertIndex < MAX_CONTACT_COUNT; ++insertIndex) {
        struct ContactPoint* contactPoint = &contactState->contacts[insertIndex];

        if (contactPoint->id == epaResult->id) {
            // if the existing contact is close enough then keep it
            // if (vector3DistSqrd(&contactPoint->contactALocal, &epaResult->contactA) < CONTACT_MOVE_TOLERNACE &&
            //     vector3DistSqrd(&contactPoint->contactBLocal, &epaResult->contactB) < CONTACT_MOVE_TOLERNACE) {
            //     return;
            // }
            break;
        }

        if (contactPoint->penetration > smallestOverlap) {
            replacementIndex = insertIndex;
            smallestOverlap = contactPoint->penetration;
        }

        // if an existing point already exists for the faces this point covers
        // and that point contacts other faces it is a better choice than 
        // the current point
        if ((contactPoint->id & epaResult->id) == epaResult->id && contactPoint->id > epaResult->id) {
            shouldReplace = 0;
        }
    }

    if (contactState->contactCount == 0) {
        contactState->normal = epaResult->normal;
        vector3Perp(&contactState->normal, &contactState->tangentVectors[0]);
        vector3Normalize(&contactState->tangentVectors[0], &contactState->tangentVectors[0]);
        vector3Cross(&contactState->normal, &contactState->tangentVectors[0], &contactState->tangentVectors[1]);
    }

    if (insertIndex == MAX_CONTACT_COUNT) {
        if (!shouldReplace) {
            return;
        }

        insertIndex = replacementIndex;
    } else {
        shouldReplace = 0;
    }

    struct ContactPoint* contactPoint = &contactState->contacts[insertIndex];

    contactPoint->id = epaResult->id;
    contactPoint->contactALocal = epaResult->contactA;
    contactPoint->contactBLocal = epaResult->contactB;
    contactPoint->penetration = epaResult->penetration;

    if (insertIndex == contactState->contactCount) {
        contactState->contactCount = insertIndex + 1;
    }

    if (shouldReplace) {
        contactPoint->normalImpulse = 0.0f;
        contactPoint->tangentImpulse[0] = 0.0f;
        contactPoint->tangentImpulse[1] = 0.0f;
        contactPoint->bias = 0.0f;
        contactPoint->normalMass = 0.0f;
        contactPoint->tangentMass[0] = 0.0f;
        contactPoint->tangentMass[1] = 0.0f;
    }
}