/* 
 * File:   OverlayID.h
 * Author: mfbari
 *
 * Created on November 26, 2012, 1:10 AM
 */

#ifndef OVERLAYID_H
#define	OVERLAYID_H

class OverlayID {
    unsigned int overlay_id;
    int prefix_length;
    int MAX_LENGTH;
public:

    OverlayID() {
    }

    OverlayID(unsigned int overlay_id, int prefix_lenght, int max_length) {
        this->overlay_id = overlay_id;
        this->prefix_length = prefix_lenght;
        this->MAX_LENGTH = max_length;
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

    int GetBitAtPosition(int n) const {
        return (((this->overlay_id & (1 << n)) >> n) & 0x00000001);
    }

    OverlayID ToggleBitAtPosition(int n) {
        OverlayID id(this->overlay_id, this->prefix_length, MAX_LENGTH);
        id.overlay_id ^= (1 << n);
        return id;
    }

    int GetMatchedPrefixLength(const OverlayID &id) const {
        int position = MAX_LENGTH - 1;
        for (; position > 0; position--) {
            if (this->GetBitAtPosition(position) != id.GetBitAtPosition(position)) {
                break;
            }
        }
        return MAX_LENGTH - position - 1;
    }

    bool operator==(const OverlayID &id) const {
        if (this->prefix_length != id.prefix_length)
            return false;
        if (this->GetMatchedPrefixLength(id) < this->prefix_length)
            return false;
        return true;
    }

//    bool operator<=(const OverlayID &id) {
//        return this->overlay_id <= id.overlay_id;
//    }

    bool operator<(const OverlayID &id) const {
        return this->overlay_id < id.overlay_id;
    }
};
#endif	/* OVERLAYID_H */

