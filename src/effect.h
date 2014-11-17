#ifndef EFFECT_H
#define EFFECT_H

#include "alure2.h"

namespace alure {

class ALContext;

class ALEffect : public Effect {
    ALContext *const mContext;
    ALuint mId;

public:
    ALEffect(ALContext *context, ALuint id) : mContext(context), mId(id)
    { }
    virtual ~ALEffect() { }

    virtual void setReverbProperties(const EFXEAXREVERBPROPERTIES *props) final;

    ALContext *getContext() const { return mContext; }
    ALuint getId() const { return mId; }

    void cleanup();
};

} // namespace alure

#endif /* EFFECT_H */