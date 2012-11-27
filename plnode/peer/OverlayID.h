/* 
 * File:   OverlayID.h
 * Author: mfbari
 *
 * Created on November 26, 2012, 1:10 AM
 */

#ifndef OVERLAYID_H
#define	OVERLAYID_H

//#define MAX_LENGTH 16

class OverlayID {
    unsigned int overlay_id;
    int prefix_length;
public:
    static int MAX_LENGTH;
    OverlayID(){}
    OverlayID(unsigned int overlay_id, int prefix_lenght) {
        this->overlay_id = overlay_id;
        this->prefix_length = prefix_lenght;
        MAX_LENGTH = 32;
    }

    void SetPrefix_length(int prefix_length) {
        this->prefix_length = prefix_length;
    }

    int GetPrefix_length() const {
        return prefix_length;
    }

    void SetOverlay_id(int overlay_id) {
        this->overlay_id = overlay_id;
    }

    int GetOverlay_id() const {
        return overlay_id;
    }

    int GetBitAtPosition(int n) {
        return (((this->overlay_id & (1 << n)) >> n) & 0x00000001);
    }

    OverlayID ToggleBitAtPosition(int n) {
        OverlayID id(this->overlay_id, this->prefix_length);
        id.overlay_id ^= (1 << n);
        return id;
    }

    int GetMatchedPrefixLength(OverlayID id) {
        int position = MAX_LENGTH - 1;
        for (; position > 0; position--) {
            if (this->GetBitAtPosition(position) != id.GetBitAtPosition(position)) {
                break;
            }
        }
        return MAX_LENGTH - position - 1;
    }
};


#endif	/* OVERLAYID_H */

